/* 
 * Copyright (c) 2018, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glew.h>
#  if defined( _WIN32 )
#  include <GL/wglew.h>
#  include <GL/freeglut.h>
#  else
#  include <GL/glut.h>
#  endif
#endif

#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_math_stream_namespace.h>

#include <sutil.h>
#include "common.h"
#include "random.h"
#include <Arcball.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <math.h>
#include <stdint.h>

using namespace optix;

#define DEBUG

const char* const SAMPLE_NAME = "wystrachRecreation";
const char* const SAMPLE_NAME_OTHER = "Wystrach Experimental Recreation Again";
const int BILLBOARD_COUNT = 10;
const float DEWAR_CYLINDER_RADIUS_MM = 61.5f;
const float OFSTAD_CYLINDER_RADIUS_MM = 122.23;
const float CYLINDER_RADIUS_MM = OFSTAD_CYLINDER_RADIUS_MM;
const float OFSTAD_SAFE_RADIUS_MM = 90.0f; // Guestimate, assuming the heated metal circle was 
const float CYLINDER_HEIGHT_MM = 96.0f;
enum Experiment {Ofstad, NaturalPanorama, Natural};
const float3 DEFAULT_ERROR_COLOUR = make_float3(1.0f, 0.0f, 0.0f);
const float CAMERA_SEPARATION_MM = 3.51f; // How far apart each camera position is.
const float CAMERA_HEIGHT = 2.0f;
const float CAMERA_DEBUG_SPHERE_RADIUS = 1.0f;

const int CAMERA_POS_MATRIX_SIZE = (int)(CYLINDER_RADIUS_MM*2 / CAMERA_SEPARATION_MM);
float3 cameraPositions[CAMERA_POS_MATRIX_SIZE][CAMERA_POS_MATRIX_SIZE];// Stores the experimental camera positions
float3 extraCameraPositions[5];// Stores the target camera positions
bool legalPosition[CAMERA_POS_MATRIX_SIZE][CAMERA_POS_MATRIX_SIZE];// 2D bool array that states whether it's a legal positon to be in.

static float rand_range(float min, float max)
{
    static unsigned int seed = 0u;
    return min + (max - min) * rnd(seed);
}


//------------------------------------------------------------------------------
//
// Globals
//
//------------------------------------------------------------------------------

Context      context;
uint32_t     width  = 720u;
uint32_t     height = 240u;
bool         use_pbo = true;

std::string  texture_path;
const char*  environment_ptx;
int          tutorial_number = 4;
Experiment   experiment = Ofstad;//NaturalPanorama;
bool         renderModeOn = false;
bool         deformationOn = false;
float        cylinderRadiusMM = CYLINDER_RADIUS_MM;
int          cameraPosition = 0; // Which camera position is the camera in

// Camera state
float3       camera_up;
float3       camera_lookat;
float3       camera_eye;
Matrix4x4    camera_rotate;
sutil::Arcball arcball;

//float2 threeSixtyRange = make_float2(2.0f * M_PIf, M_PIf);
float2 threeSixtyRange = make_float2(1.0f * M_PIf, M_PIf);

// Mouse state
int2       mouse_prev_pos;
int        mouse_button;


//------------------------------------------------------------------------------
//
// Forward decls
//
//------------------------------------------------------------------------------

Buffer getOutputBuffer();
void destroyContext();
void registerExitHandler();
void generateCameraPositions();
void createContext();
void createGeometry();
void setupCamera();
void setupLights();
void updateCameraNoRender();
void updateCameraRender();
void glutInitialize( int* argc, char** argv );
void glutRun();

void glutDisplay();
void glutKeyboardPress( unsigned char k, int x, int y );
void glutMousePress( int button, int state, int x, int y );
void glutMouseMotion( int x, int y);
void glutResize( int w, int h );


//------------------------------------------------------------------------------
//
//  Helper functions
//
//------------------------------------------------------------------------------

Buffer getOutputBuffer()
{
    return context[ "output_buffer" ]->getBuffer();
}


void destroyContext()
{
    if( context )
    {
        context->destroy();
        context = 0;
    }
}


void registerExitHandler()
{
    // register shutdown handler
#ifdef _WIN32
    glutCloseFunc( destroyContext );  // this function is freeglut-only
#else
    atexit( destroyContext );
#endif
}


void generateCameraPositions()
{
  // Function that creates the positions that the camera will be in.

  float halfCameraFieldWidth = ((CAMERA_POS_MATRIX_SIZE - 1) * CAMERA_SEPARATION_MM)/2;
  float x,y,angle;
  int i,o;
  // Generate the regular test positions
  for(i = 0; i < CAMERA_POS_MATRIX_SIZE; i++)
    for(o = 0; o < CAMERA_POS_MATRIX_SIZE; o++)
    {
      x = -halfCameraFieldWidth + i*CAMERA_SEPARATION_MM;
      y = -halfCameraFieldWidth + o*CAMERA_SEPARATION_MM;
      cameraPositions[i][o] = make_float3(x, CAMERA_HEIGHT,y);
      legalPosition[i][o] = sqrt(x*x+y*y) < CYLINDER_RADIUS_MM;
    }
  // Generate the target positions
  const float halfRadius = CYLINDER_RADIUS_MM/2.0f;
  for(i = 0; i<4; i++)
  {
    angle = (i/4.0f) *2* M_PIf - M_PIf/4.0f;
    x = cos(angle) * halfRadius;
    y = sin(angle) * halfRadius;
    printf("angle: %f  (%f,%f)\n", angle, x, y);
    extraCameraPositions[i] = make_float3(x, CAMERA_HEIGHT, y);
  }
  extraCameraPositions[4] = make_float3(0.0f, CAMERA_HEIGHT, 0.0f);
}

void createContext()
{
    // Set up context
    context = Context::create();
    context->setRayTypeCount( 2 ); // Sets how many types of rays there are - presumably a miss and normal ray
    context->setEntryPointCount( 1 );
    context->setStackSize( 4640 ); // For use in the RTX execution strategy "rtContextSetMaxTraceDepth" and "rtContextSetMaxCallableDepth" should be used to control stack size.
    context->setMaxTraceDepth( 5 ); // How many recursions of the raytracer we'll allow. Only supports RTX execution mode.

    // Note: high max depth for reflection and refraction through glass
    context["max_depth"]->setInt( 100 );
    context["scene_epsilon"]->setFloat( 1.e-4f );
    context["importance_cutoff"]->setFloat( 0.01f );
    context["ambient_light_color"]->setFloat( 0.31f, 0.33f, 0.28f );
    //context["ambient_light_color"]->setFloat( 0.0f, 0.0f, 0.0f );

    // Output buffer
    // First allocate the memory for the GL buffer, then attach it to OptiX.
    GLuint vbo = 0;
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, 4 * width * height, 0, GL_STREAM_DRAW);
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    Buffer buffer = sutil::createOutputBuffer( context, RT_FORMAT_UNSIGNED_BYTE4, width, height, use_pbo );
    context["output_buffer"]->set( buffer );


    // Ray generation program
    std::string camera_name;
    if(renderModeOn)
    {
      // Do 360 rendering stuff
      camera_name = "threeSixtyExperimentCamera";
    }else{
      // Do debug rendering stuff
      //camera_name = "threeSixtyExperimentCamera";
      if(deformationOn)
        camera_name = "deformed_camera";
      else
        camera_name = "pinhole_camera";//threeSixty_camera";
      // Configure the view range of the 360 camera (This should probably be in a struct/object)
      //if(threeSixtyRange.x ==  0.f && threeSixtyRange.y ==  0.f)
      //{
      //  // Set the viewRange to be the default view range of 360 degrees.
      //  printf("Rendering in default 360 mode...\n");
      //  context["viewRange"]->setFloat(2.0f *M_PIf, M_PIf);
      //}else{
      //  printf("Rendering with custom 360 parameters...\n");
      //  context["viewRange"]->setFloat(threeSixtyRange.x, threeSixtyRange.y);
      //}
    }
    Program ray_gen_program = context->createProgramFromPTXString( environment_ptx, camera_name );
    context->setRayGenerationProgram( 0, ray_gen_program );

    // Exception program
    Program exception_program = context->createProgramFromPTXString( environment_ptx, "exception" );
    context->setExceptionProgram( 0, exception_program );
    context["bad_color"]->setFloat(DEFAULT_ERROR_COLOUR);

    // Miss program
    std::string miss_name = "miss";
    context->setMissProgram( 0, context->createProgramFromPTXString( environment_ptx, miss_name ) );
    const float3 default_color = make_float3(1.0f, 1.0f, 1.0f);
    const std::string texpath = texture_path + "/" + std::string( "environment.hdr" );
    //context["envmap"]->setTextureSampler( sutil::loadTexture( context, texpath, default_color) );
#ifdef DEBUG
    context["bg_color"]->setFloat( make_float3( 0.34f, 0.55f, 0.85f ) );
#else
    context["bg_color"]->setFloat( make_float3(1.0f));// Make the sky white.
#endif

    // 3D solid noise buffer, 1 float channel, all entries in the range [0.0, 1.0].
    //const int tex_width  = 64;
    //const int tex_height = 64;
    //const int tex_depth  = 64;
    //Buffer noiseBuffer = context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT, tex_width, tex_height, tex_depth);
    //float *tex_data = (float *) noiseBuffer->map();

    //// Random noise in range [0, 1]
    //for (int i = tex_width * tex_height * tex_depth;  i > 0; i--) {
    //    // One channel 3D noise in [0.0, 1.0] range.
    //    *tex_data++ = rand_range(0.0f, 1.0f);
    //}
    //noiseBuffer->unmap(); 
}

float4 make_plane( float3 n, float3 p )
{
    n = normalize(n);
    float d = -dot(n, p);
    return make_float4( n, d );
}

void setBillboardNormalAndOrigin(Geometry& billboard, float3 normal, float3 origin)
{
  billboard["plane_normal"]->setFloat(normalize(normal));
  billboard["plane_origin"]->setFloat(origin);

  // Calculate the coordinate-space vectors of the plane (This should be done externally ideally)
  float3 upVector = make_float3(0.0f, 1.0f, 0.0f);
  float3 xAxis;
  if(normal.x == upVector.x && normal.y == upVector.y && normal.z == upVector.z) // equality check.
    xAxis = make_float3(1.0f, 0.0f, 0.0f); // If the normal is pointed the same way as the upVector, solve gimble lock.
  else
    xAxis = normalize(cross(upVector, normal));// If not, form the xAxis
  billboard["precalc_xAxis"]->setFloat(xAxis);
  billboard["precalc_yAxis"]->setFloat(normalize(cross(normal,xAxis)));// Form the yAxis
}

void createGeometry()
{
    int i;
    // Create GIs for each piece of geometry
    std::vector<GeometryInstance> gis;

    const char *ptx = sutil::getPtxString( SAMPLE_NAME, "box.cu" );
    Program box_bounds    = context->createProgramFromPTXString( ptx, "box_bounds" );
    Program box_intersect = context->createProgramFromPTXString( ptx, "box_intersect" );

    // Circular Base geometry
    Geometry circleBase = context->createGeometry();
    circleBase->setPrimitiveCount(1u);
    ptx = sutil::getPtxString(SAMPLE_NAME, "billboard.cu");
    circleBase->setBoundingBoxProgram(context->createProgramFromPTXString(ptx, "bounds"));
    circleBase->setIntersectionProgram(context->createProgramFromPTXString(ptx, "intersect"));
    setBillboardNormalAndOrigin(circleBase, make_float3(0.0f, 1.0f, 0.0f), make_float3(0.0f));
    circleBase["radius"]->setFloat(cylinderRadiusMM);
    circleBase["circular"]->setInt(1);

    // Test Cylinder geometry
    Geometry testCylinder = context->createGeometry();
    testCylinder->setPrimitiveCount(1u);
    ptx = sutil::getPtxString(SAMPLE_NAME, "verticalCylinder.cu");
    testCylinder->setBoundingBoxProgram(context->createProgramFromPTXString(ptx, "bounds"));
    testCylinder->setIntersectionProgram(context->createProgramFromPTXString(ptx, "intersect"));
    testCylinder["origin"] -> setFloat(make_float3(0.0f));
    testCylinder["radius"] -> setFloat(cylinderRadiusMM);
    testCylinder["height"] -> setFloat(CYLINDER_HEIGHT_MM);

    // Billboards geometry
    if(experiment == NaturalPanorama || experiment == Natural)
    {
      Geometry billboards[BILLBOARD_COUNT];
      Material billboardMaterials[BILLBOARD_COUNT];
      float distance = 165.0f;
      float scale = 10.0f;
      ptx = sutil::getPtxString(SAMPLE_NAME, "billboard.cu");
      for(i = 0; i<BILLBOARD_COUNT; i++)
      {
        // Geometry
        billboards[i] = context->createGeometry();
        billboards[i]->setPrimitiveCount(1u);
        billboards[i]->setBoundingBoxProgram(context->createProgramFromPTXString(ptx, "bounds"));
        billboards[i]->setIntersectionProgram(context->createProgramFromPTXString(ptx, "intersect"));
        float angle = M_PIf * 2 * (float)i/(float)BILLBOARD_COUNT;
        float x = cos(angle);
        float y = sin(angle);
        setBillboardNormalAndOrigin(billboards[i], make_float3(-x, 0.0f, -y), make_float3(x*distance,scale,y*distance));
        billboards[i]["radius"]->setFloat(scale);
        billboards[i]["circular"]->setInt(0);

        // Material
        //billboardMaterials[i] = context->createMaterial();
        //Program billboard_ch = context->createProgramFromPTXString( environment_ptx, "closest_hit_radiance3");
        //billboardMaterials[i]->setClosestHitProgram(0, billboard_ch);
        //billboardMaterials[i]["Ka"]->setFloat(make_float3(0.5f));
        //billboardMaterials[i]["Kd"]->setFloat(make_float3(0.5f, 0.35f, 0.2f));
        //billboardMaterials[i]["Ks"]->setFloat(make_float3(0.4f));
        //billboardMaterials[i]["phone_exp"]-> setFloat(88);

        // Tree Material
        billboardMaterials[i] = context->createMaterial();
        Program billboard_ch = context->createProgramFromPTXString( environment_ptx, "treeBillboard");
        billboardMaterials[i]->setClosestHitProgram(0, billboard_ch);
        //const float3 default_color = make_float3(1.0f, 0.0f, 0.0f);
        const std::string texpath = std::string( "/home/blayze/Software/Optix-6.0.0/NVIDIA-OptiX-SDK-6.0.0-linux64/SDK/wystrachRecreation/foliage/trees/200x200.ppm" );
        billboardMaterials[i]["treemap"]->setTextureSampler( sutil::loadTexture( context, texpath, DEFAULT_ERROR_COLOUR) );

        // Finally, add this billboard to the GIs list:
        gis.push_back(context->createGeometryInstance( billboards[i], &billboardMaterials[i], &billboardMaterials[i]+1));
      }
    }

    // View sphere placement
#ifdef DEBUG
    Geometry cameraSpheres[CAMERA_POS_MATRIX_SIZE * CAMERA_POS_MATRIX_SIZE];
    Material cameraSphereMaterials[CAMERA_POS_MATRIX_SIZE * CAMERA_POS_MATRIX_SIZE];
    ptx = sutil::getPtxString(SAMPLE_NAME, "sphere.cu");
    for(int i = 0; i < CAMERA_POS_MATRIX_SIZE; i++)
      for(int o = 0; o < CAMERA_POS_MATRIX_SIZE; o++)
      {
        int index = i*CAMERA_POS_MATRIX_SIZE + o;

        // Geometry
        cameraSpheres[index] = context->createGeometry();
        cameraSpheres[index]->setPrimitiveCount(1u);
        cameraSpheres[index]->setBoundingBoxProgram(context->createProgramFromPTXString(ptx, "bounds"));
        cameraSpheres[index]->setIntersectionProgram(context->createProgramFromPTXString(ptx, "intersect"));
        cameraSpheres[index]["origin"] -> setFloat(cameraPositions[i][o]);
        cameraSpheres[index]["radius"] -> setFloat(CAMERA_DEBUG_SPHERE_RADIUS);

        // Materials
        cameraSphereMaterials[index] = context->createMaterial();
        Program sphere_ch = context->createProgramFromPTXString(environment_ptx, "closest_hit_radiance3");
        cameraSphereMaterials[index]->setClosestHitProgram(0, sphere_ch);
        if(legalPosition[i][o])
          cameraSphereMaterials[index]["Ka"]->setFloat( 0.1f, 0.8f, 0.1f );
        else
          cameraSphereMaterials[index]["Ka"]->setFloat( 0.8f, 0.1f, 0.1f );
        cameraSphereMaterials[index]["Kd"]->setFloat( 194/255.f*.6f, 186/255.f*.6f, 151/255.f*.6f );
        cameraSphereMaterials[index]["Ks"]->setFloat( 0.4f, 0.4f, 0.4f );
        cameraSphereMaterials[index]["phong_exp"]->setFloat( 88 );

        // Add to GIs list
        gis.push_back(context->createGeometryInstance(cameraSpheres[index],
                      &cameraSphereMaterials[index],
                      &cameraSphereMaterials[index]+1));
      }

    // Generate goal spheres.
    Geometry goalSpheres[5];
    Material goalSphereMaterials[5];
    for(int i = 0; i<5; i++)
    {
      // Geometry
      goalSpheres[i] = context->createGeometry();
      goalSpheres[i]->setPrimitiveCount(1u);
      goalSpheres[i]->setBoundingBoxProgram(context->createProgramFromPTXString(ptx, "bounds"));
      goalSpheres[i]->setIntersectionProgram(context->createProgramFromPTXString(ptx, "intersect"));
      goalSpheres[i]["origin"] -> setFloat(extraCameraPositions[i]);
      goalSpheres[i]["radius"] -> setFloat(CAMERA_DEBUG_SPHERE_RADIUS * 1.5f);

      // Material
      goalSphereMaterials[i] = context->createMaterial();
      Program goalSphere_ch = context->createProgramFromPTXString( environment_ptx, "closest_hit_radiance3");
      goalSphereMaterials[i]->setClosestHitProgram(0, goalSphere_ch);
      goalSphereMaterials[i]["Ka"]->setFloat(make_float3(1.0f, 0.5f, 0.0f));
      goalSphereMaterials[i]["Kd"]->setFloat( 194/255.f*.6f, 186/255.f*.6f, 151/255.f*.6f );
      goalSphereMaterials[i]["Ks"]->setFloat(make_float3(0.4f));
      goalSphereMaterials[i]["phone_exp"]-> setFloat(88);

      // Add to GIs list
      gis.push_back(context->createGeometryInstance(goalSpheres[i],
                    &goalSphereMaterials[i],
                    &goalSphereMaterials[i]+1));
    }

#endif

    //// Materials

    // CircleBase material
#ifdef DEBUG
    Material circle_base_matl = context->createMaterial();
    Program circle_base_ch = context->createProgramFromPTXString( environment_ptx, "closest_hit_radiance3" );
    circle_base_matl->setClosestHitProgram( 0, circle_base_ch );
    // Shadows
        Program floor_ah = context->createProgramFromPTXString( environment_ptx, "any_hit_shadow" );
        circle_base_matl->setAnyHitProgram( 1, floor_ah );
    circle_base_matl["Ka"]->setFloat( make_float3(0.7f));//, 0.3f, 0.1f );
    circle_base_matl["Kd"]->setFloat( make_float3(194/255.f*.6f));//, 186/255.f*.6f, 151/255.f*.6f );
    //circle_base_matl["Kd"]->setFloat( 194/255.f*.6f, 186/255.f*.6f, 151/255.f*.6f );
    circle_base_matl["Ks"]->setFloat( 0.4f, 0.4f, 0.4f );
    circle_base_matl["phong_exp"]->setFloat( 88 );
#else
    Material circle_base_matl = context->createMaterial();
    Program circle_base_ch = context->createProgramFromPTXString( environment_ptx, "solid_color" );
    circle_base_matl->setClosestHitProgram( 0, circle_base_ch );
    circle_base_matl["ambient_light_color"]->setFloat( make_float3(0.0f));
#endif

    // Test cylinder material
    Material testCylinder_matl = context->createMaterial();

    Program testCylinder_ch;
    switch(experiment)
    {
      case NaturalPanorama:
        testCylinder_ch = context->createProgramFromPTXString(environment_ptx, "dewarNaturalPanorama");
        testCylinder_matl->setClosestHitProgram(0, testCylinder_ch);
        testCylinder_matl["virtualCameraPosition"]->setFloat(make_float3(0.0f,CAMERA_HEIGHT,0.0f));
#ifdef DEBUG
        testCylinder_matl["tint"]->setInt(1);
#endif
      break;
      case Ofstad:
        testCylinder_ch = context->createProgramFromPTXString(environment_ptx, "ofstad_cylinder");
        testCylinder_matl->setClosestHitProgram(0, testCylinder_ch);
      break;
      default:
        testCylinder_ch = context->createProgramFromPTXString(environment_ptx, "solid_color");
        testCylinder_matl->setClosestHitProgram(0, testCylinder_ch);
        testCylinder_matl["ambient_light_color"]->setFloat(DEFAULT_ERROR_COLOUR);
    }
        


    //gis.push_back( context->createGeometryInstance( floor, &new_floor_matl, &new_floor_matl+1) );
    gis.push_back( context->createGeometryInstance( circleBase, &circle_base_matl, &circle_base_matl+1));
    gis.push_back( context->createGeometryInstance( testCylinder, &testCylinder_matl, &testCylinder_matl+1));

    // Place all in group
    GeometryGroup geometrygroup = context->createGeometryGroup();
    geometrygroup->setChildCount( static_cast<unsigned int>(gis.size()) );
    for(i = 0; i<gis.size(); i++)
    {
      geometrygroup->setChild(i, gis[i]);
    }

    geometrygroup->setAcceleration( context->createAcceleration("Trbvh") );
    //geometrygroup->setAcceleration( context->createAcceleration("NoAccel") );

    context["top_object"]->set( geometrygroup );
    context["top_shadower"]->set( geometrygroup );

}


void setupCamera()
{
    camera_eye    = make_float3( 7.0f, 9.2f, -6.0f );
    camera_lookat = make_float3( 0.0f, 4.0f,  0.0f );
    camera_up     = make_float3( 0.0f, 1.0f,  0.0f );

    camera_rotate  = Matrix4x4::identity();
}


void setupLights()
{

    BasicLight lights[] = { 
        { make_float3( -5.0f, 60.0f, -16.0f ), make_float3( 1.0f, 1.0f, 1.0f ), 1 }/*,
        { make_float3( 0.0f, -5.0f, 0.0f ), make_float3( 0.5f, 1.0f, 0.5f ), 1 }*/
    };

    Buffer light_buffer = context->createBuffer( RT_BUFFER_INPUT );
    light_buffer->setFormat( RT_FORMAT_USER );
    light_buffer->setElementSize( sizeof( BasicLight ) );
    light_buffer->setSize( sizeof(lights)/sizeof(lights[0]) );
    memcpy(light_buffer->map(), lights, sizeof(lights));
    light_buffer->unmap();

    context[ "lights" ]->set( light_buffer );
}

void updateCameraRender()
{
  const int totalTestPositions = CAMERA_POS_MATRIX_SIZE*CAMERA_POS_MATRIX_SIZE;
  float3 position;
  int legal;
  if(cameraPosition < totalTestPositions)
  {
    position = cameraPositions[(int)(cameraPosition/CAMERA_POS_MATRIX_SIZE)][(int)(cameraPosition%CAMERA_POS_MATRIX_SIZE)];
    legal = legalPosition[(int)(cameraPosition/CAMERA_POS_MATRIX_SIZE)][(int)(cameraPosition%CAMERA_POS_MATRIX_SIZE)];
  }else{
    position = extraCameraPositions[cameraPosition-totalTestPositions];
    legal = 1;
  }
  context["renderPosition"]->setFloat(position);
  context["legal"]->setInt(legal);

  //cameraPosition ++;
  cameraPosition = (cameraPosition+1)%(totalTestPositions + 5);
}

void updateCameraNoRender()
{
    const float vfov = 60.0f;
    const float aspect_ratio = static_cast<float>(width) /
                               static_cast<float>(height);

    float3 camera_u, camera_v, camera_w;
    sutil::calculateCameraVariables(
            camera_eye, camera_lookat, camera_up, vfov, aspect_ratio,
            camera_u, camera_v, camera_w, true );

    const Matrix4x4 frame = Matrix4x4::fromBasis(
            normalize( camera_u ),
            normalize( camera_v ),
            normalize( -camera_w ),
            camera_lookat);
    const Matrix4x4 frame_inv = frame.inverse();
    // Apply camera rotation twice to match old SDK behavior
    const Matrix4x4 trans   = frame*camera_rotate*camera_rotate*frame_inv;

    camera_eye    = make_float3( trans*make_float4( camera_eye,    1.0f ) );
    camera_lookat = make_float3( trans*make_float4( camera_lookat, 1.0f ) );
    camera_up     = make_float3( trans*make_float4( camera_up,     0.0f ) );

    sutil::calculateCameraVariables(
            camera_eye, camera_lookat, camera_up, vfov, aspect_ratio,
            camera_u, camera_v, camera_w, true );

    camera_rotate = Matrix4x4::identity();

    context["eye"]->setFloat( camera_eye );
    context["U"  ]->setFloat( camera_u );
    context["V"  ]->setFloat( camera_v );
    context["W"  ]->setFloat( camera_w );
}


void glutInitialize( int* argc, char** argv )
{
    glutInit( argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_ALPHA | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowSize( width, height );
    glutInitWindowPosition( 100, 100 );
    glutCreateWindow( SAMPLE_NAME_OTHER );
    glutHideWindow();
}


void glutRun()
{
    // Initialize GL state
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, -1, 1 );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glViewport(0, 0, width, height);

    glutShowWindow();
    glutReshapeWindow( width, height);

    // register glut callbacks
    glutDisplayFunc( glutDisplay );
    glutIdleFunc( glutDisplay );
    glutReshapeFunc( glutResize );
    glutKeyboardFunc( glutKeyboardPress );
    glutMouseFunc( glutMousePress );
    glutMotionFunc( glutMouseMotion );

    registerExitHandler();

    glutMainLoop();
}


//------------------------------------------------------------------------------
//
//  GLUT callbacks
//
//------------------------------------------------------------------------------

void glutDisplay()
{
    if(renderModeOn)
      updateCameraRender();
    else
      updateCameraNoRender();

    context->launch( 0, width, height );

    Buffer buffer = getOutputBuffer();
    sutil::displayBufferGL( getOutputBuffer() );

    {
        static unsigned frame_count = 0;
        sutil::displayFps( frame_count++ );
    }

    glutSwapBuffers();
}


void glutKeyboardPress( unsigned char k, int x, int y )
{

    switch( k )
    {
        case( 'q' ):
        case( 27 ): // ESC
        {
            destroyContext();
            exit(0);
        }
        case( 's' ):
        {
            const std::string outputImage = std::string(SAMPLE_NAME) + ".ppm";
            std::cerr << "Saving current frame to '" << outputImage << "'\n";
            sutil::displayBufferPPM( outputImage.c_str(), getOutputBuffer() );
            break;
        }
    }
}


void glutMousePress( int button, int state, int x, int y )
{
    if( state == GLUT_DOWN )
    {
        mouse_button = button;
        mouse_prev_pos = make_int2( x, y );
    }
    else
    {
        // nothing
    }
}


void glutMouseMotion( int x, int y)
{
    if( mouse_button == GLUT_RIGHT_BUTTON )
    {
        const float dx = static_cast<float>( x - mouse_prev_pos.x ) /
                         static_cast<float>( width );
        const float dy = static_cast<float>( y - mouse_prev_pos.y ) /
                         static_cast<float>( height );
        const float dmax = fabsf( dx ) > fabs( dy ) ? dx : dy;
        const float scale = fminf( dmax, 0.9f );
        camera_eye = camera_eye + (camera_lookat - camera_eye)*scale;
    }
    else if( mouse_button == GLUT_LEFT_BUTTON )
    {
        const float2 from = { static_cast<float>(mouse_prev_pos.x),
                              static_cast<float>(mouse_prev_pos.y) };
        const float2 to   = { static_cast<float>(x),
                              static_cast<float>(y) };

        const float2 a = { from.x / width, from.y / height };
        const float2 b = { to.x   / width, to.y   / height };

        camera_rotate = arcball.rotate( b, a );
    }

    mouse_prev_pos = make_int2( x, y );
}


void glutResize( int w, int h )
{
    if ( w == (int)width && h == (int)height ) return;

    width  = w;
    height = h;
    sutil::ensureMinimumSize(width, height);

    sutil::resizeBuffer( getOutputBuffer(), width, height );

    glViewport(0, 0, width, height);

    glutPostRedisplay();
}


//------------------------------------------------------------------------------
//
// Main
//
//------------------------------------------------------------------------------

void printUsageAndExit( const std::string& argv0 )
{
    std::cerr << "\nUsage: " << argv0 << " [options]\n";
    std::cerr <<
        "App Options:\n"
        "  -h | --help         Print this usage message and exit.\n"
        "  -f | --file         Save single frame to file and exit. If used with -r, this should be the location of a folder (with no trailing'/')\n"
        "  -n | --nopbo        Disable GL interop for display buffer.\n"
        "  -T | --tutorial-number <num>                         Specify tutorial number\n"
        "  -t | --texture-path <path>                           Specify path to texture directory\n"
        "  -r | --render-images                                 Render all experimental positions\n"
        "  -e | --experiment <ofstad|natural-panorama|natural>  Which experiment to run (Ofstad by default\n"
        //" -ar | --arena-radius <ofstad|dewar>                   Which arena size to use\n"
        "App Keystrokes:\n"
        "  q  Quit\n"
        "  s  Save image to '" << SAMPLE_NAME << ".ppm'\n"
        << std::endl;

    exit(1);
}

int main( int argc, char** argv )
{
    std::string out_file;
    for( int i=1; i<argc; ++i )
    {
        const std::string arg( argv[i] );
        //std::cout << arg << "\n";

        if( arg == "-h" || arg == "--help" )
        {
            printUsageAndExit( argv[0] );
        }
        else if ( arg == "-f" || arg == "--file" )
        {
            if( i == argc-1 )
            {
                std::cerr << "Option '" << arg << "' requires additional argument.\n";
                printUsageAndExit( argv[0] );
            }
            out_file = argv[++i];
        } 
        else if( arg == "-n" || arg == "--nopbo"  )
        {
            use_pbo = false;
        }
        else if ( arg == "-t" || arg == "--texture-path" )
        {
            if ( i == argc-1 ) {
                std::cerr << "Option '" << arg << "' requires additional argument.\n";
                printUsageAndExit( argv[0] );
            }
            texture_path = argv[++i];
        }
        else if ( arg == "-T" || arg == "--tutorial-number" )
        {
            if ( i == argc-1 ) {
                printUsageAndExit( argv[0] );
            }
            tutorial_number = atoi(argv[++i]);
            if ( tutorial_number < 0 || tutorial_number > 11 ) {
                std::cerr << "Tutorial number (" << tutorial_number << ") is out of range [0..11]\n";
                printUsageAndExit( argv[0] );
            }
        }else if (arg == "-r" || arg == "--render-images")
        {
          //if(i==argc-1)
          //  printUsageAndExit(argv[0]);
          renderModeOn = true;
        }else if (arg == "--deformation")
        {
          deformationOn = true;
        }else if (arg == "-e" || arg == "--experiment")
        {
          if(i==argc-1)
            printUsageAndExit(argv[0]);
          std::string experimentName = argv[++i];
          if(experimentName == "ofstad")
            experiment = Ofstad;
          else if(experimentName == "natural-panorama")
            experiment = NaturalPanorama;
          else if(experimentName == "natural")
            experiment = Natural;
          else{
              std::cerr << "Option '" << arg << "' requires additional correct argument.\n";
              printUsageAndExit(argv[0]);
          }
        }else{
            std::cerr << "Unknown option '" << arg << "'\n";
            printUsageAndExit( argv[0] );
        }
    }

    if( texture_path.empty() ) {
        texture_path = std::string( sutil::samplesDir() ) + "/data";
    }

    try
    {
        glutInitialize( &argc, argv );

#ifndef __APPLE__
        glewInit();
#endif

        // Load the PTX source code for the experiments
        //std::stringstream ss;
        //ss << "tutorial" << tutorial_number << ".cu";
        environment_ptx = sutil::getPtxString( SAMPLE_NAME, std::string("wystrachExperimentsEnv.cu").c_str() );

        generateCameraPositions();
        createContext();
        createGeometry();
        setupCamera();
        setupLights();

        context->validate();

        if ( out_file.empty() )
        {
            glutRun();
        }
        else
        {
            if(!renderModeOn)
            {
              updateCameraNoRender();
              context->launch( 0, width, height );
              sutil::displayBufferPPM( out_file.c_str(), getOutputBuffer() );
              destroyContext();
            }else{
              int i, o;
              for(i = 0; i<CAMERA_POS_MATRIX_SIZE; i++)
              {
                for(o = 0; o<CAMERA_POS_MATRIX_SIZE; o++)
                {
                  updateCameraRender();
                  context->launch(0, width, height);
                  std::stringstream ss;
                  ss << out_file << "/render-" << i << "-" << o << ".ppm";
                  sutil::displayBufferPPM(ss.str().c_str(), getOutputBuffer());
                  //glutSwapBuffers();
                }
              }
              for(i = 0; i<5; i++)
              {
                  int idx = cameraPosition-CAMERA_POS_MATRIX_SIZE*CAMERA_POS_MATRIX_SIZE;
                  float3 goalLoc = extraCameraPositions[idx];
                  printf("Camera is at goal location %i@(%f, %f, %f)\n", idx, goalLoc.x, goalLoc.y, goalLoc.z);
                  updateCameraRender();
                  context->launch(0, width, height);
                  std::stringstream ss;
                  ss << out_file << "/render-goal-" << i << ".ppm";
                  sutil::displayBufferPPM(ss.str().c_str(), getOutputBuffer());
              }
              //out
              //destroyContext();
              destroyContext();
            }
        }
        return 0;
    }
    SUTIL_CATCH( context->get() )
}


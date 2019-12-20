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
//#include "random.h"
#include <Arcball.h>

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <math.h>
#include <stdint.h>

#include <iostream>
#include <thread>

#include "EyeGenerator.h"

using namespace std;
using namespace optix;

#define DEBUG

//// STATIC GLOBALS
const float3 DEFAULT_ERROR_COLOUR = make_float3(1.0f, 0.0f, 0.0f);
const char* DIRECTORY_NAME = "eyeVisualiser";
const int OMMATIDIAL_COUNT = 500;

//// Global Variables
Context      context;
uint32_t     width  = 720u;
uint32_t     height = 240u;
bool         use_pbo = true;
EyeGenerator eg(OMMATIDIAL_COUNT);
thread* eyeGeneratorThreadPtr;
//StaticCoordinate renderRays[OMMATIDIAL_COUNT];

const char* environment_ptx;

// Camera state
float3       camera_up;
float3       camera_lookat;
float3       camera_eye;
Matrix4x4    camera_rotate;
sutil::Arcball arcball;

// Mouse state
int2       mouse_prev_pos;
int        mouse_button;


//// Forward Declarations
Buffer getOutputBuffer();
void registerExitHandler();
void updateCameraRender();
void updateOmmatidialRays();
void glutInitialize(int* argc, char** argv);
void destroyContext();
void createContext();

void createGeometry();
void setBillboardNormalAndOrigin(Geometry& billboard, float3 normal, float3 origin);
void setupCamera();
//void setupRenderRays();

void glutDisplay();
void glutKeyboardPress( unsigned char k, int x, int y );
void glutMousePress( int button, int state, int x, int y );
void glutMouseMotion( int x, int y);
void glutResize( int w, int h );


//// Helper Functions
Buffer getOutputBuffer()
{
    return context[ "output_buffer" ]->getBuffer();
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

void createContext()
{
    // Set up context
    context = Context::create();
    context->setRayTypeCount( 2 ); // Sets how many types of rays there are - presumably a miss and normal ray
    context->setEntryPointCount( 1 );
    context->setStackSize( 4640 ); // For use in the RTX execution strategy "rtContextSetMaxTraceDepth" and "rtContextSetMaxCallableDepth" should be used to control stack size.
    context->setMaxTraceDepth( 5 ); // How many recursions of the raytracer we'll allow. Only supports RTX execution mode.

    // Note: low max depth because there's little reflection and refraction through glass
    context["max_depth"]->setInt( 10 );
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
    std::string camera_name = "pinhole_camera";
    Program ray_gen_program = context->createProgramFromPTXString( environment_ptx, camera_name );
    context->setRayGenerationProgram( 0, ray_gen_program );

    // Exception program
    Program exception_program = context->createProgramFromPTXString( environment_ptx, "exception" );
    context->setExceptionProgram( 0, exception_program );
    context["bad_color"]->setFloat(DEFAULT_ERROR_COLOUR);

    // Miss program
    context->setMissProgram( 0, context->createProgramFromPTXString( environment_ptx, "miss" ) );
    context["bg_color"]->setFloat( make_float3(1.0f));// Make the background white
}

void destroyContext()
{
    eg.stop();
    if(eyeGeneratorThreadPtr->joinable())
    {
      cout << ">Awaiting thread closure..." << endl;
      eyeGeneratorThreadPtr->join();
      cout << ">Thread closed, proceeding to close glut context." << endl;
    }

    if( context )
    {
        context->destroy();
        context = 0;
    }
}

//// Glut handling
void glutInitialize( int* argc, char** argv )
{
    glutInit( argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_ALPHA | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowSize( width, height );
    glutInitWindowPosition( 100, 100 );
    glutCreateWindow( "Eye Visualiser" );
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

/// Glut Callbacks below
void glutDisplay()
{
    // Call the actual renderer
    updateCameraRender();

    // Update the line positions
    updateOmmatidialRays();

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

GeometryGroup gg;
Geometry line;
Geometry ommatidialRays[OMMATIDIAL_COUNT];
//// Geometry
void createGeometry()
{
  int i;
  // Create Geom Instances for each piece of geometry (Note: This might need to be made global for the the ommatidial bits)
  std::vector<GeometryInstance> gis;

  const char *cylinderPtx = sutil::getPtxString(DIRECTORY_NAME, "cylinder.cu");

  // Ommatidial rays
  for(i = 0; i<OMMATIDIAL_COUNT; i++)
  {
    ommatidialRays[i] = context->createGeometry();
    ommatidialRays[i]->setPrimitiveCount(1u);
    //ptx = sutil::getPtxString(DIRECTORY_NAME, "cylinder.cu");
    ommatidialRays[i]->setBoundingBoxProgram(context->createProgramFromPTXString(cylinderPtx, "bounds"));
    ommatidialRays[i]->setIntersectionProgram(context->createProgramFromPTXString(cylinderPtx, "intersect"));
    ommatidialRays[i]["origin"]->setFloat(make_float3(0.0f,0.0f,0.0f));
    ommatidialRays[i]["direction"]->setFloat(normalize(make_float3(0.0f,0.0f,1.0f)));
    ommatidialRays[i]["cylinderLength"]->setFloat(0.5f);
    ommatidialRays[i]["radius"]->setFloat(0.01f);

    Material ray_matl = context->createMaterial();
    Program ommatidial_ray_ch = context->createProgramFromPTXString(environment_ptx, "solid_color");
    ray_matl->setClosestHitProgram(0, ommatidial_ray_ch);
    ray_matl["ambient_light_color"]->setFloat(make_float3(0.0f));

    gis.push_back(context->createGeometryInstance(ommatidialRays[i], &ray_matl, &ray_matl+1));
  }

  // Assembling the geometry group
  gg = context->createGeometryGroup();
  gg->setChildCount(static_cast<unsigned int>(gis.size()));
  for(i = 0; i<gis.size(); i++)
  {
    gg->setChild(i, gis[i]);
  }

  gg->setAcceleration(context->createAcceleration("Trbvh"));

  context["top_object"]->set(gg);
  context["top_shadower"]->set(gg);
}

// The below functions, as well as the relevant cuda code should be separated out, as the eye generator has.
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

//// Camera Work

void setupCamera()
{
    camera_eye    = make_float3( 7.0f, 9.2f, -6.0f );
    camera_lookat = make_float3( 0.0f, 0.0f,  0.0f );
    camera_up     = make_float3( 0.0f, 1.0f,  0.0f );

    camera_rotate  = Matrix4x4::identity();
}

void updateOmmatidialRays()
{
  StaticCoordinate sc;
  for(int i = 0; i<OMMATIDIAL_COUNT; i++)
  {
    sc = eg.getCoordinateInfo(i);
    ommatidialRays[i]["direction"]->setFloat(sc.direction);
    ommatidialRays[i]["origin"]->setFloat(sc.position);
  }
}
//float count = 0.0f;
void updateCameraRender()
{
  //line["direction"]->setFloat(normalize(make_float3(cos(count),1.0f,sin(count))));
  //count += 0.01f;
  gg->getAcceleration()->markDirty();

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

int main(int argc, char** argv)
{
  //SphericalCoordinate* sc = new SphericalCoordinate(1);
  //SphericalCoordinate sc(4);
  //cout << "hi, world." << endl;
  //cout << sc.getId() << endl;

  srand(42);
  eg.generateSphericalCoordinates();
  eg.stepSize = 0.001f;

  cout << "Starting thread...";
  //thread eyeGeneratorThread(EyeGenerator::basicIterator, &eg);
  thread eyeGeneratorThread(EyeGenerator::rieszSEnergyIterator, &eg);
  eyeGeneratorThreadPtr = &eyeGeneratorThread;

  try
  {
    glutInitialize(&argc, argv);

#ifndef __APPLE__
    glewInit();
#endif

    environment_ptx = sutil::getPtxString( DIRECTORY_NAME, std::string("environment.cu").c_str() );

    createContext();
    createGeometry();
    setupCamera();
    //setupRenderRays();

    context->validate();

    glutRun();

    if(eyeGeneratorThread.joinable())
      eyeGeneratorThread.join();
    return 0;
  }
  SUTIL_CATCH( context->get() )
  if(eyeGeneratorThread.joinable())
    eyeGeneratorThread.join();
}

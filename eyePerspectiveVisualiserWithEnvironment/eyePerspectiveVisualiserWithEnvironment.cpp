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
#include <Arcball.h>
#include <OptiXMesh.h>

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
const char* DIRECTORY_NAME = "eyePerspectiveVisualiserWithEnvironment";
const char* PRIMITIVES_DIRECTORY_NAME = "commonPrimitives";
const int OMMATIDIAL_COUNT = 1000;//38;//1000;

//// Global Variables
Context      context;
uint32_t     width  = 720u;
uint32_t     height = 240u;
bool         use_pbo = true;
EyeGenerator eg(OMMATIDIAL_COUNT);
thread* eyeGeneratorThreadPtr;

const char* environment_ptx;

//// Boot Variables
bool renderEnv = true;

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
void setupLights(); // TODO: Remove this/do something else with the lights.
void setupRenderRays();

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
    std::string camera_name = "ommatidial_camera";
    Program ray_gen_program = context->createProgramFromPTXString( environment_ptx, camera_name );
    context->setRayGenerationProgram( 0, ray_gen_program );
    ray_gen_program["renderPosition"]->setFloat(make_float3(0.0f));

    // Exception program
    Program exception_program = context->createProgramFromPTXString( environment_ptx, "exception" );
    context->setExceptionProgram( 0, exception_program );
    context["bad_color"]->setFloat(DEFAULT_ERROR_COLOUR);

    // Miss program
    if(renderEnv)
    {
      context->setMissProgram( 0, context->createProgramFromPTXString( environment_ptx, "miss_env" ) );
      const std::string texpath = "/home/blayze/Software/Optix-6.0.0/studies/data/environment.hdr";
      const float3 default_color = make_float3(0.0f, 1.0f, 1.0f);
      context["envmap"]->setTextureSampler( sutil::loadTexture( context, texpath, default_color) );
    }else{
      context->setMissProgram( 0, context->createProgramFromPTXString( environment_ptx, "miss" ) );
      const std::string texpath = "/home/blayze/Software/Optix-6.0.0/studies/data/environment.hdr";
      const float3 color = make_float3(1.0f, 1.0f, 1.0f);
      context["bg_color"]->setFloat( color);
    }
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

//// Geometry
void createGeometry()
{
  //int i;
  //// Create Geom Instances for each piece of geometry (Note: This might need to be made global for the the ommatidial bits)
  //std::vector<GeometryInstance> gis;

  //// Assembling the geometry group
  //gg = context->createGeometryGroup();
  //gg->setChildCount(static_cast<unsigned int>(gis.size()));
  //for(i = 0; i<gis.size(); i++)
  //{
  //  gg->setChild(i, gis[i]);
  //}

  //// Mesh rendering
  OptiXMesh mesh;
  mesh.context = context;
  mesh.use_tri_api = true;//use_tri_api;
  mesh.ignore_mats = false;//ignore_mats;

  // Material configuration
  Material cow_matl = context->createMaterial();
  const char* meshShadersPTX = sutil::getPtxString( DIRECTORY_NAME, std::string("meshMaterial.cu").c_str() );
  Program cow_ch = context->createProgramFromPTXString(meshShadersPTX, "basic_shaded_solid_color");
  cow_matl->setClosestHitProgram(0, cow_ch);
  cow_matl["ambient_illumination"]->setFloat(make_float3(0.01f));
  cow_matl["base_color"] -> setFloat(make_float3(0.9f, 0.9f, 0.9f));
  cow_matl["sun_color"] -> setFloat(make_float3(1.0f));
  cow_matl["sun_direction"] -> setFloat(make_float3(0.0f, 1.0f, 0.0f));
  mesh.material = cow_matl;

  std::string filename = "/home/blayze/Software/Optix-6.0.0/studies/data/cow.obj";
  loadMesh( filename, mesh ); 
  GeometryGroup tri_gg = context->createGeometryGroup();
  tri_gg->addChild(mesh.geom_instance);
  tri_gg->setAcceleration(context->createAcceleration("Trbvh"));

  // SCALE WISDOM COW.
  Transform robotInDisguise = context->createTransform();
  optix::Matrix4x4 matrix = optix::Matrix4x4::translate(make_float3(0.0f,-0.5f,1.0f)) * optix::Matrix4x4::scale(make_float3(20.0f));
  robotInDisguise->setMatrix(0, matrix.getData(), matrix.inverse().getData());
  robotInDisguise->setChild(tri_gg);

  // Assembling the top group.
  Group topGroup = context->createGroup();
  topGroup->setAcceleration(context->createAcceleration("Trbvh"));
  topGroup->addChild(robotInDisguise);

  context["top_object"]->set(topGroup);
  context["top_shadower"]->set(topGroup);
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
void setupLights()
{
    // No lights
    Buffer light_buffer = context->createBuffer( RT_BUFFER_INPUT );
    light_buffer->setFormat( RT_FORMAT_USER );
    light_buffer->setElementSize( 32);
    light_buffer->setSize(0);

    context[ "lights" ]->set( light_buffer );
}

Buffer ommatidialBuffer;
float3 sphericalPositions[OMMATIDIAL_COUNT];
void updateOmmatidialRays()
{
  // This bit is for the visualiser, not the perspecetive visualiser.
  //StaticCoordinate sc;
  //for(int i = 0; i<OMMATIDIAL_COUNT; i++)
  //{
  //  sc = eg.getCoordinateInfo(i);
  //  ommatidialRays[i]["direction"]->setFloat(sc.direction);
  //  ommatidialRays[i]["origin"]->setFloat(sc.position);
  //}

  // This bit is for the perspectiv visualiser.
  Buffer b = context["ommatidia"]->getBuffer();

  float3 newdata[OMMATIDIAL_COUNT];
  for(int i = 0; i<OMMATIDIAL_COUNT; i++)
    newdata[i] = eg.getCoordinateInfo(i).direction;

  b->setFormat(RT_FORMAT_USER);
  memcpy(b->map(), newdata, sizeof(newdata));
  b->unmap();
}
//float count = 0.0f;
void updateCameraRender()
{
  //gg->getAcceleration()->markDirty();

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

void setupRenderRays()
{
  StaticCoordinate sc;
  for(int i = 0; i<OMMATIDIAL_COUNT; i++)
  {
    sc = eg.getCoordinateInfo(i);
    sphericalPositions[i] = sc.direction;
  }

  Buffer ommatidialBuffer;
  ommatidialBuffer = context->createBuffer(RT_BUFFER_INPUT);
  ommatidialBuffer->setFormat(RT_FORMAT_USER);
  ommatidialBuffer->setElementSize(sizeof(float3));
  ommatidialBuffer->setSize(sizeof(sphericalPositions)/sizeof(sphericalPositions[0]));
  memcpy(ommatidialBuffer->map(), sphericalPositions, sizeof(sphericalPositions));
  ommatidialBuffer->unmap();

  context["ommatidia"]->set(ommatidialBuffer);
}

int main(int argc, char** argv)
{
  // Process arguments
  for(int i=1; i<argc; i++)
  {
    const std::string arg(argv[i]);
    if(arg == "-nobg")
      renderEnv = false;
  }

  // Run the code
  srand(42);
  eg.generateSphericalCoordinates();
  eg.stepSize = 0.001f;
  eg.coordinateProximityCount = 10;

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
    setupLights();
    setupRenderRays();

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

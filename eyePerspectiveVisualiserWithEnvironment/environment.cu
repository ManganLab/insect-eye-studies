#include "tutorial.h"

rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, ); 
rtDeclareVariable(float3, shading_normal,   attribute shading_normal, ); 
rtDeclareVariable(float2, uv_coords,        attribute uv_coords, );

rtDeclareVariable(PerRayData_radiance, prd_radiance, rtPayload, );
rtDeclareVariable(PerRayData_shadow,   prd_shadow,   rtPayload, );

rtDeclareVariable(optix::Ray, ray,          rtCurrentRay, );
rtDeclareVariable(float,      t_hit,        rtIntersectionDistance, );
rtDeclareVariable(uint2,      launch_index, rtLaunchIndex, );

rtDeclareVariable(float,        scene_epsilon, , );
rtDeclareVariable(rtObject,     top_object, , );

///// OMMATIDIAL POSITION STORAGE
rtBuffer<float3> ommatidia;


///// CAMERA SHADERS

rtDeclareVariable(float3,        eye, , );
rtDeclareVariable(float3,        U, , );
rtDeclareVariable(float3,        V, , );
rtDeclareVariable(float3,        W, , );
rtDeclareVariable(float3,        bad_color, , );
rtBuffer<uchar4, 2>              output_buffer;
rtDeclareVariable(float3,        renderPosition, , );// The position of the ommatidial camera.

const float ONEHUNDREDTWENTY_DEGREES = 120.0f/180.0f * M_PIf;
const float THIRTY_DEGREES = 30.0f/180.0f * M_PIf;


// A simple pinhole camera
RT_PROGRAM void pinhole_camera()
{
  size_t2 screen = output_buffer.size();

  float2 d = make_float2(launch_index) / make_float2(screen) * 2.f - 1.f;
  float3 ray_origin = eye;
  float3 ray_direction = normalize(d.x*U + d.y*V + W);

  optix::Ray ray(ray_origin, ray_direction, RADIANCE_RAY_TYPE, scene_epsilon );

  PerRayData_radiance prd;
  prd.importance = 1.f;
  prd.depth = 0;

  rtTrace(top_object, ray, prd);

  output_buffer[launch_index] = make_color( prd.result );
}

// A camera that only outputs green
RT_PROGRAM void green_camera()
{
  output_buffer[launch_index] = make_color(make_float3(0.0f,1.0f,0.0f));
}

//float radialDistancce = 
// A camera that finds the closest ommatidium to each point as a point on a sphere and renders from it's perspective
RT_PROGRAM void ommatidial_camera()
{
  size_t2 screen = output_buffer.size();
  // Full panorama:
  float2 d = make_float2(launch_index) / make_float2(screen) * make_float2(2.0f * M_PIf, M_PIf) - make_float2(M_PIf, M_PIf/2.0f);
  // 30 degree from the floor (vertical range of -30 deg to 90 deg):
  //float2 d = make_float2(launch_index) / make_float2(screen) * make_float2(2.0f * M_PIf, ONEHUNDREDTWENTY_DEGREES) - make_float2(M_PIf, THIRTY_DEGREES);
  float3 ray_direction = make_float3(sin(d.x)*cos(d.y), sin(d.y), cos(d.x)*cos(d.y));// This should already be normalized, as it is a sphere.

  // ray_direction now acts as a point on the sphere to render to, the closest ommatidium must be rendered now (TODO: in the future, pre-render each and do this bit in a separate shader)

  float closestDistance = acos(dot(ommatidia[0], ray_direction));
  int closestIndex = 0;
  float dist;
  int i;
  for(i = 1; i<ommatidia.size(); i++)
  {
    dist = acos(dot(ommatidia[i], ray_direction));
    if(dist < closestDistance)
    {
      closestDistance = dist;
      closestIndex = i;
    }
  }

  /// DEBUG
  //if(threadIdx.x == 10)
  //{
  //  //printf("Launch index: (%f, %f), screen: (%f, %f), d: (%f, %f)\n", lidx.x, lidx.y, scrn.x, scrn.y, d.x, d.y);
  //  printf("RAY LISTING....\n");
  //  printf("%f\n", closestDistance);
  //  for(i = 1; i<ommatidia.size(); i++)
  //  {
  //    printf("[%i] (%.2f, %.2f, %.2f)\n", i, ommatidia[i].x, ommatidia[i].y, ommatidia[i].z);
  //  }
  //}
#ifdef DEBUG
  if(closestDistance < 0.01f)
  {
    output_buffer[launch_index] = make_color(make_float3(1.0f,0.0f,0.0f));
  }else{
    optix::Ray ray(renderPosition, ray_direction, RADIANCE_RAY_TYPE, scene_epsilon);

    PerRayData_radiance prd;
    prd.importance = 1.0f;
    prd.depth = 0;

    rtTrace(top_object, ray, prd);

    output_buffer[launch_index] = make_color(prd.result);
  }
#else
  optix::Ray ray(renderPosition, ommatidia[closestIndex], RADIANCE_RAY_TYPE, scene_epsilon);
  PerRayData_radiance prd;
  prd.importance = 1.0f;
  prd.depth = 0;

  rtTrace(top_object, ray, prd);

  output_buffer[launch_index] = make_color(prd.result);
#endif
}

///// MISS SHADERS

rtDeclareVariable(float3, bg_color, , );
// The default miss program
RT_PROGRAM void miss()
{
  prd_radiance.result = bg_color;
}

rtTextureSampler<float4, 2> envmap;
RT_PROGRAM void miss_env()
{
  float theta = atan2f(ray.direction.x, ray.direction.z);
  float phi = M_PIf * 0.5f - acosf(ray.direction.y);
  float u = (theta + M_PIf) * (0.5f * M_1_PIf);
  float v = 0.5f * (1.0f + sin(phi));
  prd_radiance.result = make_float3(tex2D(envmap, u, v));
  //prd_radiance.result = make_float3(1.0f, 0.0f, 0.0f);
}

//
// Terminates and fully attenuates ray after any hit
//
RT_PROGRAM void any_hit_shadow()
{
  // this material is opaque, so it fully attenuates all shadow rays
  prd_shadow.attenuation = make_float3(0);

  rtTerminateRay();
}
  

///// SURFACE SHADERS


////// THERE CAN ONLY BE ONNNNE!!!
rtDeclareVariable(float3,   ambient_light_color, , ); // Stores the ambient light colour of anyting
RT_PROGRAM void solid_color()
{
  prd_radiance.result = ambient_light_color;
}

//
// Set pixel to solid color upon failure
//
RT_PROGRAM void exception()
{
  output_buffer[launch_index] = make_color( bad_color );
}

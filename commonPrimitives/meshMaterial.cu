#include "tutorial.h"

//// Required Variables
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

rtDeclareVariable(float3, base_color, , ); // Stores the base colour of this object
rtDeclareVariable(float3, ambient_illumination, , ); // Stores the ambient colour of the environment
rtDeclareVariable(float3, sun_color, , );// The colour of the sun above the object
rtDeclareVariable(float3, sun_direction, , );// The direction of the sun from the object
RT_PROGRAM void basic_shaded_solid_color()
{
  float3 world_shade_normal = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, shading_normal));
  float3 color = base_color * ambient_illumination;

  color += sun_color * base_color * fmaxf(0.0f, dot(sun_direction, world_shade_normal));
  prd_radiance.result = color;
}

rtTextureSampler<float4, 2> Kd_map;
rtDeclareVariable(float3, texcoord, attribute texcoord, ); 

RT_PROGRAM void basic_shaded_texture()
{
  float3 world_shading_normal   = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, shading_normal ) );
  float3 world_geometric_normal = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, geometric_normal ) );
  
  float3 ffnormal = faceforward( world_shading_normal, -ray.direction, world_geometric_normal );

  const float3 Kd_val = make_float3( tex2D( Kd_map, texcoord.x, texcoord.y ) );

  // At this point Kd_val is treated as the base color, ffnormal is the world_shade_normal (it's been modified by the vertex data)
  float3 color = Kd_val * ambient_illumination;
  color += sun_color * Kd_val * fmaxf(0.0f, dot(sun_direction, ffnormal));
  prd_radiance.result = color;
}

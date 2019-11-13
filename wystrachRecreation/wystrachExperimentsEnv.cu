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


///// CAMERA SHADERS

rtDeclareVariable(float3,        eye, , );
rtDeclareVariable(float3,        U, , );
rtDeclareVariable(float3,        V, , );
rtDeclareVariable(float3,        W, , );
rtDeclareVariable(float2,        viewRange, , );// The range of viewing for the 360-degree eye, (0-360, 0-180)
rtDeclareVariable(float3,        bad_color, , );
rtDeclareVariable(float3,        renderPosition, , );// The position of the 360-degree-experimental camera.
rtBuffer<uchar4, 2>              output_buffer;

const float ONEHUNDREDTWENTY_DEGREES = 120.0f/180.0f * M_PIf;
const float THIRTY_DEGREES = 30.0f/180.0f * M_PIf;

// A camera to perform the 360 degree FOV renders.
rtDeclareVariable(int, legal, , );
RT_PROGRAM void threeSixtyExperimentCamera()
{
  if(!legal)
  {
    output_buffer[launch_index] = make_color(bad_color);
    return;
  }
  size_t2 screen = output_buffer.size();
  // Full panorama:
  //float2 d = make_float2(launch_index) / make_float2(screen) * make_float2(2.0f * M_PIf, M_PIf) - make_float2(M_PIf, M_PIf/2.0f);
  // 30 degree from the floor (vertical range of -30 deg to 90 deg):
  float2 d = make_float2(launch_index) / make_float2(screen) * make_float2(2.0f * M_PIf, ONEHUNDREDTWENTY_DEGREES) - make_float2(M_PIf, THIRTY_DEGREES);
  float3 ray_origin = renderPosition;
  float3 ray_direction = make_float3(sin(d.x)*cos(d.y), sin(d.y), cos(d.x)*cos(d.y));// This should already be normalized, as it is a sphere.

  optix::Ray ray(ray_origin, ray_direction, RADIANCE_RAY_TYPE, scene_epsilon);

  PerRayData_radiance prd;
  prd.importance = 1.0f;
  prd.depth = 0;

  rtTrace(top_object, ray, prd);

  output_buffer[launch_index] = make_color(prd.result);
}
RT_PROGRAM void threeSixty_camera()
{
  size_t2 screen = output_buffer.size();

  //// ORIGINAL:
  //float2 d = make_float2(launch_index) / make_float2(screen) * make_float2(2.0f * M_PIf , M_PIf) + make_float2(M_PIf, 0);

  float2 d = make_float2(launch_index) / make_float2(screen) * make_float2(1.f * M_PIf , M_PIf) + make_float2(M_PIf, 0)
                                                             + make_float2(1.f * M_PIf, 0);
  float3 angle = make_float3(cos(d.x) * sin(d.y), -cos(d.y), sin(d.x) * sin(d.y));
  float3 ray_origin = eye;
  float3 ray_direction = normalize(angle.x*normalize(U) + angle.y*normalize(V) + angle.z*normalize(W));

  ////float2 d = make_float2(launch_index)/make_float2(screen) * viewRange + (viewRange * make_float2(0.75f, 0.f));
  //float2 d = make_float2(launch_index)/make_float2(screen) * make_float2(2.f, 2.f) - make_float2(screen);//viewRange + (viewRange * make_float2(0.75f, 0.f));
  ////if(viewRange == 0.0f)
  ////{
  ////  d = d * make_float2(2.0f * M_PIf , M_PIf) + make_float2(M_PIf, 0.f);// Default 360 degree configuration
  ////}else{
  ////  d = d * make_float2(
  ////}
  ////float2 d = make_float2(launch_index) / make_float2(screen) * make_float2(2.0f * M_PIf , M_PIf/2.0f) + make_float2(M_PIf, M_PIf/4.0f);
  //float3 angle = make_float3(cos(d.x) * sin(d.y), -cos(d.y), sin(d.x) * sin(d.y));
  //float3 ray_origin = eye;
  //float3 ray_direction = normalize(angle.x*normalize(U) + angle.y*normalize(V) + angle.z*normalize(W));


  //if(threadIdx.x == 10)
  //{
  //  printf("Launch index: (%f, %f), screen: (%f, %f), d: (%f, %f)\n", lidx.x, lidx.y, scrn.x, scrn.y, d.x, d.y);
  //}
  // Apply camera rotation using U, V & W rotation information.
  //float3 ray_direction = normalize(d.x*make_float3(1,0,0) + d.y*make_float3(0,1,0) + make_float3(0,0,1)); //normalize(d.x*U + d.y*V + W);

  optix::Ray ray(ray_origin, ray_direction, RADIANCE_RAY_TYPE, scene_epsilon );

  PerRayData_radiance prd;
  prd.importance = 1.f;
  prd.depth = 0;

  rtTrace(top_object, ray, prd);

  output_buffer[launch_index] = make_color( prd.result );
}


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

RT_PROGRAM void deformed_camera()
{
  size_t2 screen = output_buffer.size();

  float2 d = make_float2(launch_index) / make_float2(screen) * 2.f - 1.f;
  float3 offset = normalize(make_float3(d.x,d.y, 0.f));
  float3 ray_origin = eye + 2.f*normalize(offset.x*U + offset.y*V + W);
  //float3 ray_origin = normalize(offset.x*U + offset.y*V + W);
  //float3 ray_origin = eye + normalize(make_float3(d.x,d.y, 0.f));
  float3 ray_direction = normalize(d.x*U + d.y*V + W);

  optix::Ray ray(ray_origin, ray_direction, RADIANCE_RAY_TYPE, scene_epsilon );

  PerRayData_radiance prd;
  prd.importance = 1.f;
  prd.depth = 0;

  rtTrace(top_object, ray, prd);

  output_buffer[launch_index] = make_color( prd.result );
}


///// MISS SHADERS

rtDeclareVariable(float3, bg_color, , );
// The default miss program
RT_PROGRAM void miss()
{
  prd_radiance.result = bg_color;
}
// Utilises an ENV map
rtTextureSampler<float4, 2> envmap;
RT_PROGRAM void missEnv()
{
  float theta = atan2f(ray.direction.x, ray.direction.z);
  float phi = M_PIf * 0.5f - acosf(ray.direction.y);
  float u = (theta + M_PIf) * (0.5f * M_1_PIf);
  float v = 0.5f * (1.0f + sin(phi));
  prd_radiance.result = make_float3(tex2D(envmap, u, v));
}
// First Ofstad miss attempt
RT_PROGRAM void missOfstad()
{
  float theta = atan2f(ray.direction.x, ray.direction.z);
  float phi = M_PIf * 0.5f - acosf(ray.direction.y);
  float u = (theta + M_PIf) * (0.5f * M_1_PIf);
  float v = 0.5f * (1.0f + sin(phi));
  //prd_radiance.result = make_float3(tex2D(envmap, u, v));
  //prd_radiance.result = bg_color;
  if(theta < M_PIf/4.f)
  {
    if ((int)(theta*3)%2 == 0)
    {
      prd_radiance.result = make_float3(0,0,0);
    }else{
      prd_radiance.result = make_float3(1,1,1);
    }
  }else{
    if ((int)(phi*4)%2 == 0)
    {
      prd_radiance.result = make_float3(0,0,0);
    }else{
      prd_radiance.result = make_float3(1,1,1);
    }
  }
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

rtDeclareVariable(float3,   ambient_light_color, , ); // Stores the ambient light colour of anyting
RT_PROGRAM void solid_color()
{
  prd_radiance.result = ambient_light_color;
}

rtBuffer<BasicLight>        lights;
rtDeclareVariable(float, base_light_mult, , );// Sometimes called K_a
rtDeclareVariable(float3,   Kd, , );
RT_PROGRAM void shaded_solid_color()
{
  float3 world_shade_normal = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, shading_normal));
  float3 hit_point = ray.origin + t_hit * ray.direction;
  float3 color = base_light_mult * ambient_light_color;

  // For each light, calculate it's affect on the ambient light color.
  for(int i = 0; i < lights.size(); i++)
  {
    BasicLight light = lights[i];
    float3 dirToLight = normalize(light.pos - hit_point);
    float lightScale = dot(dirToLight, world_shade_normal); // (a.b)/(|a||b|) = cos(@), but here |a| and |b| have been made to equal 1.
    color += light.color * lightScale * Kd;
  }
  prd_radiance.result = color;
}

rtDeclareVariable(float2, textureCoords, attribute textureCoords , );
const float ONE_THIRD = 1.0f/3.0f;
const float TWO_THIRDS = 2.0f/3.0f;
RT_PROGRAM void ofstad_cylinder()
{
  //printf("texture: (%f, %f)\n", textureCoords.x, textureCoords.y);
  float3 colour = make_float3(textureCoords.x,textureCoords.y,0.0f);// Default colour displays coordinates.
  if(textureCoords.x <= ONE_THIRD)
  {
    // Vertical Bars
    colour = make_float3((int)floor(textureCoords.x/ONE_THIRD*8)%2 == 0);
  }else if(textureCoords.x <= TWO_THIRDS){
    // Horizontal Bars
    colour = make_float3((int)floor(textureCoords.y/ONE_THIRD)%2 == 0);
  }else{
    // Diagonal Bars
    colour = make_float3((int)floor(textureCoords.y/ONE_THIRD+ (textureCoords.x-TWO_THIRDS)/ONE_THIRD*8)%2 == 1);
  }
  prd_radiance.result = colour;
}

rtDeclareVariable(float3, virtualCameraPosition, , );
rtDeclareVariable(int, tint, , );
RT_PROGRAM void dewarNaturalPanorama()
{
  PerRayData_radiance returned_prd;
  returned_prd.importance = 1.0f;
  returned_prd.depth = prd_radiance.depth+1;

  float3 hitPoint = ray.origin + t_hit * ray.direction;
  //float3 spreadRayDir = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, -geometric_normal) );
  float3 spreadRayDir = normalize(hitPoint - virtualCameraPosition);
  hitPoint = hitPoint + 0.001f * spreadRayDir;
  optix::Ray spreadRay(hitPoint, spreadRayDir, RADIANCE_RAY_TYPE, scene_epsilon);
  rtTrace(top_object, spreadRay, returned_prd);
  
  float3 colour = returned_prd.result;
  if(tint == 1)
    colour += make_float3(0.0f,0.1f,0.0f);

  prd_radiance.result = colour;
}

rtTextureSampler<float4, 2> treemap;
RT_PROGRAM void treeBillboard()
{
  float3 textureColour = make_float3(tex2D(treemap, uv_coords.x, uv_coords.y));

  PerRayData_radiance returned_prd;
  returned_prd.importance = 1.0f;
  returned_prd.depth = prd_radiance.depth+1;
  float3 hitPoint = ray.origin + t_hit * ray.direction;
  optix::Ray continuedRay(hitPoint, ray.direction, RADIANCE_RAY_TYPE, scene_epsilon);
  rtTrace(top_object, continuedRay, returned_prd);
  textureColour = returned_prd.result * textureColour;

  prd_radiance.result = textureColour;
}


//
// Phong surface shading with shadows 
//
rtDeclareVariable(float3,   Ka, , );
rtDeclareVariable(float3,   Ks, , );
rtDeclareVariable(float,    phong_exp, , );
rtDeclareVariable(rtObject, top_shadower, , );

RT_PROGRAM void closest_hit_radiance3()
{
  float3 world_geo_normal   = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, geometric_normal ) );
  float3 world_shade_normal = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, shading_normal ) );
  float3 ffnormal     = faceforward( world_shade_normal, -ray.direction, world_geo_normal );
  float3 color = Ka * ambient_light_color;

  float3 hit_point = ray.origin + t_hit * ray.direction;

  for(int i = 0; i < lights.size(); ++i) {
    BasicLight light = lights[i];
    float3 L = normalize(light.pos - hit_point);
    float nDl = dot( ffnormal, L);

    if( nDl > 0.0f ){
      // cast shadow ray
      PerRayData_shadow shadow_prd;
      shadow_prd.attenuation = make_float3(1.0f);
      float Ldist = length(light.pos - hit_point);
      optix::Ray shadow_ray( hit_point, L, SHADOW_RAY_TYPE, scene_epsilon, Ldist );
      rtTrace(top_shadower, shadow_ray, shadow_prd);
      float3 light_attenuation = shadow_prd.attenuation;

      if( fmaxf(light_attenuation) > 0.0f ){
        float3 Lc = light.color * light_attenuation;
        color += Kd * nDl * Lc;

        float3 H = normalize(L - ray.direction);
        float nDh = dot( ffnormal, H );
        if(nDh > 0)
          color += Ks * Lc * pow(nDh, phong_exp);
      }

    }
  }
  prd_radiance.result = color;
}
  

//
// (NEW)
// Phong surface shading with shadows and reflections
//
rtDeclareVariable(float3, reflectivity, , );
rtDeclareVariable(float, importance_cutoff, , );
rtDeclareVariable(int, max_depth, , );

RT_PROGRAM void floor_closest_hit_radiance4()
{
  float3 world_geo_normal   = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, geometric_normal ) );
  float3 world_shade_normal = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, shading_normal ) );
  float3 ffnormal     = faceforward( world_shade_normal, -ray.direction, world_geo_normal );
  float3 color = Ka * ambient_light_color;

  float3 hit_point = ray.origin + t_hit * ray.direction;

  float3 wobble;
  float mult = 1.0f;
  wobble.x = sin(hit_point.x * mult);
  wobble.y = cos(hit_point.z * mult);
  wobble.z = sin(hit_point.z * mult);
  world_shade_normal = world_shade_normal + wobble;
  world_shade_normal = world_geo_normal + wobble;

  float3 true_point;
  true_point.x = hit_point.x;
  //true_point.y = hit_point.y + ((cos(hit_point.x) * sin(hit_point.z) + 1)/2 );
  true_point.y = hit_point.y + (wobble.x * wobble.y + mult * mult)/(mult/2);
  true_point.z = hit_point.z;

  for(int i = 0; i < lights.size(); ++i) {
    BasicLight light = lights[i];
    float3 L = normalize(light.pos - true_point);
    float nDl = dot( ffnormal, L);

    if( nDl > 0.0f ){
      // cast shadow ray
      PerRayData_shadow shadow_prd;
      shadow_prd.attenuation = make_float3(1.0f);
      float Ldist = length(light.pos - true_point);
      optix::Ray shadow_ray( true_point, L, SHADOW_RAY_TYPE, scene_epsilon, Ldist );
      rtTrace(top_shadower, shadow_ray, shadow_prd);
      float3 light_attenuation = shadow_prd.attenuation;

      if( fmaxf(light_attenuation) > 0.0f ){
        float3 Lc = light.color * light_attenuation;
        color += Kd * nDl * Lc;

        float3 H = normalize(L - ray.direction);
        float nDh = dot( ffnormal, H );
        if(nDh > 0)
          color += Ks * Lc * pow(nDh, phong_exp);
      }

    }
  }

  float importance = prd_radiance.importance * optix::luminance( reflectivity );


  //wobble = normalize(wobble);
  //wobble.x = (wobble.x + 1)/2;
  //wobble.y = (wobble.y + 1)/2;
  //wobble.z = (wobble.z + 1)/2;
  // reflection ray
  if( importance > importance_cutoff && prd_radiance.depth < max_depth) {
    PerRayData_radiance refl_prd;
    refl_prd.importance = importance;
    refl_prd.depth = prd_radiance.depth+1;
    float3 R = reflect( ray.direction, ffnormal );
    /// Zeroed vector
    //float3 zeroed;
    //zeroed.x = 1.0f;
    //zeroed.y = 0.0f;
    //zeroed.z = 1.0f;
    //R = R * zeroed;
    /// All reflect toward the box
    //float3 boxPos;
    //boxPos.x = 0.0f;
    //boxPos.y = 3.5f;
    //boxPos.z = 0.0f;
    //float3 R = normalize(boxPos-hit_point);
    /// Sinusoidal surface wobble
    R += wobble * 0.1;
    optix::Ray refl_ray( hit_point, R, RADIANCE_RAY_TYPE, scene_epsilon );
    rtTrace(top_object, refl_ray, refl_prd);
    //color += reflectivity * refl_prd.result;
    color = refl_prd.result; // Full reflectivity
  }

  prd_radiance.result = color;
  //prd_radiance.result = wobble;
}
  

//
// Set pixel to solid color upon failure
//
RT_PROGRAM void exception()
{
  output_buffer[launch_index] = make_color( bad_color );
}

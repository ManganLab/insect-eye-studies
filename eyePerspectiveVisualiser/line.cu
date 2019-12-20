#include <optix_world.h> // Include access to the C++ API in the optix namespace

using namespace optix;

// Configuration variables
rtDeclareVariable(float3, origin, , ); // The origin of this line
rtDeclareVariable(float3, direction, , ); // The direction of this line
rtDeclareVariable(float, lineLength, , ); // The length of this line
rtDeclareVariable(float, radius, , ); // The thickness of this line

// Recuired variables
rtDeclareVariable(int, lgt_instance, , ) = {0};
rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, ); 
rtDeclareVariable(float3, shading_normal, attribute shading_normal, ); 
rtDeclareVariable(int, lgt_idx, attribute lgt_idx, ); 
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );

RT_PROGRAM void intersect(int)
{
  //The minimum distance between two lines in 3D is equal to the distance between two planes containing those lines
  float3 normal = cross(ray.direction, direction); // Form the plane normals
  float distance = dot(ray.origin - origin, normal)/dot(normal, normal);// Distance between the two planes.

  if(distance > radius)
    return; // This ray missed.

  /// Now we know the ray gets within the tube of this line, we need to work out where the collision is.
  /// Project the ray direction and origin down to a 2D plane formed with the line direction as the normal
  // Generate x and y axis of 2D line-space.
  float3 xAxis = normalize(cross(direction, make_float3(0.0f,1.0f,0.0f)));
  float3 yAxis = normalize(cross(xAxis, direction));
  // Project the direction and origin (as in, the origin from the new coordinate system)
  float2 projectedRayDirection = make_float2(dot(ray.direction, xAxis), dot(ray.direction, yAxis));
  float3 relativeOrigin = ray.origin - origin;
  float2 projectedRelativeRayOrigin = make_float2(dot(relativeOrigin, xAxis), dot(relativeOrigin, yAxis));
  /// calculate the distance from the ray origin to the hit point in 2D (where the ray is exactly 'radius' away from the line)
  // The length of the projected relative ray origin^2 - d^2 = length to hit point in 2D space
  // Note that here we're not using length() because that would return a sqrt'd value that we'd have to square again, so we calculate the squared length of projectedRelativeRayOrigin manually.
  float distanceTo2Dhit = sqrt((projectedRelativeRayOrigin.x*projectedRelativeRayOrigin.x + projectedRelativeRayOrigin.y*projectedRelativeRayOrigin.y) - radius*radius);
  /// calculate the scale factor (distance to the 3D hit) for the projected ray origin to get it to the hit point, this is the distance.
  float distanceTo3Dhit = distanceTo2Dhit/length(projectedRayDirection);

  //if(rtPotentialIntersection(distanceTo3Dhit))
  if(rtPotentialIntersection(0.001f))
  {
    shading_normal = geometric_normal = make_float3(0.0f, 1.0f, 0.0f); // For now.
    lgt_idx = lgt_instance;
    rtReportIntersection(0);// Render.
  }
}
RT_PROGRAM void bounds(int, float result[6])
{
  optix::Aabb* aabb = (optix::Aabb*)result;
  float3 end = origin + direction * lineLength;
  float3 tipRadius = make_float3(radius);
  aabb->set(fminf(origin, end) - tipRadius, fmaxf(origin, end) + tipRadius);
}

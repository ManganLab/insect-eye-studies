#include <optix_world.h> // Include access to the C++ API in the optix namespace

using namespace optix;

// Configuration variables
rtDeclareVariable(float3, origin, , ); // The origin of this sphere.
rtDeclareVariable(float, radius, , ); // The radius of this sphere.

// Precalculated variables

// Recuired variables
rtDeclareVariable(int, lgt_instance, , ) = {0};
rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, ); 
rtDeclareVariable(float3, shading_normal, attribute shading_normal, ); 
rtDeclareVariable(int, lgt_idx, attribute lgt_idx, ); 
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );

RT_PROGRAM void intersect(int)
{
  //if(threadIdx.x == 10)
  //  printf("Hello from the sphere.\n");
  float3 rayToOrigin = ray.origin - origin;// - ray.origin;
  float a = dot(ray.direction, ray.direction);
  if(a == 0)
    return; // Return if a would induce a div 0.
  else
    a *= 2;// Pre-multiply a by 2 to avoid potential double-calculation later.
  float b = -2 * dot(ray.direction, rayToOrigin);// Pre-multiply b by -1 to avoid potential double-calculation later.
  float c = dot(rayToOrigin, rayToOrigin) - radius*radius;

  // Calculate intersection distances:
  float discriminator = b*b - 2*a*c;
  if(discriminator <= 0)
    return; // Return if the ray doesn't intersect (or it only grazes the outside, it's not worth calculating that for this.

  // Calculate the first intersection
  discriminator = sqrt(discriminator);// Perform sqrt.
  float distance;
  if(discriminator < 0)// If the disc. is < 0, then the closer variant (smaller distance) is found from adding the srt to b, then dividing.
    distance = (b + discriminator)/a;
  else
    distance = (b - discriminator)/a;// If not, then subtract.
  float3 intersectPoint = ray.origin + distance * ray.direction;

  if(rtPotentialIntersection(distance))
  {
    shading_normal = geometric_normal = normalize(intersectPoint - origin);// The normal radiates from the centre of the sphere.
    lgt_idx = lgt_instance;
    rtReportIntersection(0);// Render
  }
}
RT_PROGRAM void bounds(int, float result[6])
{
  //printf("Hello from sphere bounds\n");
  optix::Aabb* aabb = (optix::Aabb*)result;
  float3 maxPoint = origin + make_float3(radius);
  float3 minPoint = origin - make_float3(radius);
  //printf("  | maxPoint: (%f, %f, %f)\n", maxPoint.x, maxPoint.y, maxPoint.z);
  //printf("  | minPoint: (%f, %f, %f)\n", minPoint.x, minPoint.y, minPoint.z);
  aabb->set(minPoint, maxPoint);
}

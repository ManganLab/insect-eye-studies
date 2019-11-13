#include <optix_world.h> // Include access to the C++ API in the optix namespaces.

using namespace optix;

// Configuration variables
rtDeclareVariable(float3, origin, , );// The origin of this vertical cylinder (in the x,z plane)
rtDeclareVariable(float, radius, , );// The radius of this vertical cylinder
rtDeclareVariable(float, height, , );
rtDeclareVariable(float2, textureCoords, attribute textureCoords, );
rtDeclareVariable(float, cameraPositionFromGround, , );

// Precalculated variables

// Required variables
rtDeclareVariable(int, lgt_instance, , ) = {0};
rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, ); 
rtDeclareVariable(float3, shading_normal, attribute shading_normal, ); 
rtDeclareVariable(int, lgt_idx, attribute lgt_idx, ); 
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );

static __device__ float2 to2D(float3 v)
{
  return(make_float2(v.x, v.z));
}
RT_PROGRAM void intersect(int)
{
  //// Calculate the intersection of the ray with the circle in 2D:

  float2 rayOrigin2D = to2D(ray.origin);//make_float2(ray.origin.x, ray.origin.z);
  float2 rayDir2D = to2D(ray.direction);//make_float2(ray.direction.x, ray.direction.z);
  float2 origin2D = to2D(origin);
  float2 cylinderToRay = origin2D - rayOrigin2D;
  float a = dot(rayDir2D, rayDir2D);
  if(a == 0.0f)
    return; // The ray does not intersect the cylinder.
  else
    a *= 2;// Multiply a by two to turn it into the denominator
  float b = -2*dot(rayDir2D, cylinderToRay); // Multiply by -1 so that it doesn't need to be later (disc uses b^2)
  float c = dot(cylinderToRay, cylinderToRay) - radius * radius;

  float disc = (b*b)-(2*a*c);

  //if(threadIdx.x == 10)
  //  printf("%f\n", disc);

  if(disc < 0 || disc == 0.0f)
    return; // The ray does not intersect the cylinder or the ray intersects but only at the edges, which we don't care about.

  //if(threadIdx.x == 10)
  //  printf("2\n");

  // The ray intersects the cylinder two times.
  // Now we can caluclate the two points on the circle. As we only care about rays coming
  // from the inside out, we can calculate each variation of the root and take the first
  // one that dots positively with the surface normal, meaning that it's viewing from
  // the inside.
  
  // Derive the correct non-backface intersect point:
  float sqrtDisc = sqrt(disc);
  float distance = (-b + sqrtDisc)/a;
  float2 intersectPoint2D = rayOrigin2D + distance * rayDir2D;
  float2 normal2D = normalize(origin2D - intersectPoint2D);
  float cullTest = dot((rayOrigin2D - intersectPoint2D), normal2D);
  if(cullTest < 0)// If the point is a back-face, then we'll have to calculate using the other point...
  {
    distance = (-b - sqrtDisc)/a;
    intersectPoint2D = rayOrigin2D + distance * rayDir2D;
    normal2D = normalize(origin2D - intersectPoint2D);
  }
  
  // Convert the 2D coordinates to 3D ones:
  float3 intersectPoint = ray.origin + distance * ray.direction;
  float3 normal = make_float3(normal2D.x, 0.0f, normal2D.y);

  // Render the object if the intersect point is within the accepted vertical space:
  if(intersectPoint.y >= origin.y && intersectPoint.y <= origin.y + height && rtPotentialIntersection(distance))
  {
    shading_normal = geometric_normal = normal;
    // Calculate the texture coordinates
    textureCoords = make_float2((atan2f(intersectPoint2D.x-origin2D.x, intersectPoint2D.y-origin2D.y)+M_PIf)/M_PIf/2.0f, (intersectPoint.y - origin.y)/height);
    lgt_idx = lgt_instance;
    rtReportIntersection(0);// Render.
  }
}

RT_PROGRAM void bounds(int, float result[6])
{
  optix::Aabb* aabb = (optix::Aabb*)result;
  printf("Hello from verticalCylinder Bounds\n");
  float3 maxArea = origin + make_float3(radius, height, radius);
  float3 minArea = origin - make_float3(radius, 0.0f, radius);
  printf("  | maxArea: (%f, %f, %f)\n", maxArea.x, maxArea.y, maxArea.z);
  printf("  | minArea: (%f, %f, %f)\n", minArea.x, minArea.y, minArea.z);
  aabb->set(minArea, maxArea);
}

#include "SphericalCoordinate.h"
#include <stdlib.h>

using namespace std;

// Inlines
inline float randRange(float min, float max)
{
  return static_cast<float>(rand()/static_cast<float>(RAND_MAX)) * (max-min) + min;
}

SphericalCoordinate::SphericalCoordinate(int idin)//void)
{
  state = normalize(make_float3(randRange(-1.0f,1.0f),randRange(-1.0f,1.0f),randRange(-1.0f,1.0f)));
  id = idin + 1;
  //std::cout << "Spherical Coordinate object created" << std::endl;
}
SphericalCoordinate::~SphericalCoordinate(void)
{
  //std::cout << "Spherical Coordinate object destroyed" << std::endl;
}

void SphericalCoordinate::randomMove(float scale)
{
  oldState = state;
  // Take a step at `stepAngle` degrees to north with `scale` distance.
  float stepAngle = randRange(0,M_PI);
  // Rotate 
  float3 
  // Do nothing.
}

inline float SphericalCoordinate::getClosestDistance(NonPlanarCoordinate* others[], int count)
{
  return (radius * getCloasestDistanceFast(others, count));
}
// Returns distance as an angle as it's faster, the slow version will just scale it properly.
float SphericalCoordinate::getCloasestDistanceFast(NonPlanarCoordinate* others[], int count)
{
  int i;
  //cout << "Vector " << this->getId() << ": (" <<  this->state.x << ", " << this->state.y << ", " << this->state.z << ")" << endl;

  float closestDistance = std::numeric_limits<float>::max();
  for(i = 0; i<count; i++)
  {
    SphericalCoordinate* sc = (SphericalCoordinate*)others[i];
    if(sc == this)
      continue; // Skip comparing with itself

    // Claculate the distance to each of them
    float distance = this->getFastDistanceTo(sc);
    if(distance <= closestDistance)
      closestDistance = distance;
  }
    
  return(closestDistance);
}

void SphericalCoordinate::backtrack()
{
  // Do nothing.
  std::cout << "Backtracking..." << std::endl;
}

int SphericalCoordinate::getId()
{
  return id;
}

// Returns the distance, but not the actual distance around the sphere
inline float SphericalCoordinate::getFastDistanceTo(SphericalCoordinate* other)
{
  return(acos(dot(this->state, other->state)));
}

float SphericalCoordinate::radius = 5;

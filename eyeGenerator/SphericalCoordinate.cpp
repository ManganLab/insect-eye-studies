#include "SphericalCoordinate.h"

//class NonPlanarCoordinate;

SphericalCoordinate::SphericalCoordinate(void)
{
  std::cout << "Spherical Coordinate object created" << std::endl;
}
SphericalCoordinate::~SphericalCoordinate(void)
{
  std::cout << "Spherical Coordinate object destroyed" << std::endl;
}

void SphericalCoordinate::randomMove(float scale)
{
  // Do nothing.
}

float SphericalCoordinate::getClosestDistance(SphericalCoordinate* others)
//float SphericalCoordinate::getClosestDistance(NonPlanarCoordinate* others)
{
  // Do nothing.
  return(0.0f);
}

void SphericalCoordinate::backtrack()
{
  // Do nothing.
}

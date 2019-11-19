#include "SphericalCoordinate.h"

//class NonPlanarCoordinate;

SphericalCoordinate::SphericalCoordinate(void)
{
  std::cout << "Spherical Coordinate object created" << std::endl;
  test = 24;
}
SphericalCoordinate::~SphericalCoordinate(void)
{
  std::cout << "Spherical Coordinate object destroyed" << std::endl;
}

int SphericalCoordinate::run(void)
{
  return(test);
}

void SphericalCoordinate::randomMove(float scale)
{
  // Do nothing.
}

float SphericalCoordinate::getClosestDistance(SphericalCoordinate* others)
{
  // Do nothing.
  return(0.0f);
}

void SphericalCoordinate::backtrack()
{
  // Do nothing.
}

int SphericalCoordinate::printNumber()
{
  std::cout << "42" << std::endl;
  return(42);
}

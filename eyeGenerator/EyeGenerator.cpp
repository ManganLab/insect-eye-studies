//using namespace optix;

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include <algorithm>

#include "EyeGenerator.h"

#include "SphericalCoordinate.h"

static int COORDINATE_COUNT = 1000;
static float STEP_SIZE = 5.0f;// 10 works well too.


EyeGenerator::EyeGenerator(int coordinateCount)
{
  this->coordinateCount = coordinateCount;
  //coordinates = new NonPlanarCoordinate[coordinateCount];
  coordinates = (NonPlanarCoordinate**) calloc(coordinateCount, sizeof(NonPlanarCoordinate*));
  if(coordinates == NULL)
  {
    std::cout << "ERROR: Insufficient memory to store ommatidial coordinates." << std::endl;
    exit(0);
  }
}
EyeGenerator::~EyeGenerator()
{
  for(int i = 0; i<coordinateCount; i++)
    delete coordinates[i]; // Remove from heap
  free(coordinates);
}
void EyeGenerator::generateSphericalCoordinates()
{
  for(int i = 0; i<coordinateCount; i++)
  {
    NonPlanarCoordinate* sc = new SphericalCoordinate(i);
    coordinates[i] = sc;
  }
}
void EyeGenerator::test()
{
  std::cout << "testing.." << std::endl;
  for(int i = 0; i<coordinateCount; i++)
    std::cout << ((SphericalCoordinate*)coordinates[i])->getId() << std::endl;
}
StaticCoordinate EyeGenerator::getCoordinateInfo(int i)
{
  return coordinates[i]->getStaticCoord();
}

void EyeGenerator::basicIterator(EyeGenerator* eg)
{
  // This can reference anything within eg because it's the same class.
  std::cout << "Hi. " << eg->coordinateCount << ". We're in a thread." << std::endl;
  std::cout << "Running with " << eg->coordinateCount << " coordinates and step size " << STEP_SIZE << "..." << std::endl;
  int i;

  // Sort some line stuff:
  std::cout << std::endl << std::endl << std::endl << "[3A";

  // Iterate N times, morphing the positions of the coordinates...
  float avgFastDistance, variance;
  float fastDistances[eg->coordinateCount];
  int iteration = 0;
  do {
    std::cout << "[s" << "Iteration " << ++iteration << ":" << std::endl;
    std::random_shuffle(&(eg->coordinates)[0], &(eg->coordinates)[3]); // Shuffle
    
    //// Calculate statistics about the current state
    avgFastDistance = 0;
    for(i = 0; i<eg->coordinateCount; i++)
    {
      float closestDistance = (eg->coordinates)[i]->getCloasestDistanceFast((eg->coordinates), eg->coordinateCount);
      avgFastDistance += closestDistance;
      fastDistances[i] = closestDistance;
    }
    avgFastDistance /= eg->coordinateCount;//Average Closest Distance
    variance = 0;
    for(i = 0; i<eg->coordinateCount; i++)
      variance += (fastDistances[i] - avgFastDistance) * (fastDistances[i] - avgFastDistance);
    variance /= eg->coordinateCount;//Variance Computed

    //// Move the coordinates
    std::cout << "  Average [fast] distance: " << avgFastDistance << std::endl;
    std::cout << "  Variance: " << variance << "8";

    // Now take each coordinate and move it a step, reverting it if the step has taken it closer to any coordinate.
    for(i = 0; i<eg->coordinateCount; i++)
    {
      //fastDistances[i]; // The pre-move closest distance.
      (eg->coordinates)[i]->randomMove(STEP_SIZE * variance);//Move the coordinate by a random amount scaled by the variance.
      if((eg->coordinates)[i]->getCloasestDistanceFast((eg->coordinates), eg->coordinateCount) < fastDistances[i])
        (eg->coordinates)[i]->backtrack(); // Backtrack if the step was a bad one.
    }
  }while(variance > 0.00001 && iteration < 100001);

  // Some more line stuff:
  std::cout << "[3B";
}

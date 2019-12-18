//using namespace optix;

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include <algorithm>

#include "SphericalCoordinate.h"

static int COORDINATE_COUNT = 1000;
static float STEP_SIZE = 5.0f;// 10 works well too.

using namespace std;


void iterate(NonPlanarCoordinate* coordinates[])
{
  std::cout << "Running with " << COORDINATE_COUNT << " coordinates and step size " << STEP_SIZE << "..." << std::endl;
  int i;

  // Sort some line stuff:
  cout << endl << endl << endl << "[3A";

  // Iterate N times, morphing the positions of the coordinates...
  float avgFastDistance, variance;
  float fastDistances[COORDINATE_COUNT];
  int iteration = 0;
  do {
    cout << "[s" << "Iteration " << ++iteration << ":" << endl;//"8";
    random_shuffle(&coordinates[0], &coordinates[3]); // Shuffle
    
    //// Calculate statistics about the current state
    avgFastDistance = 0;
    for(i = 0; i<COORDINATE_COUNT; i++)
    {
      float closestDistance = coordinates[i]->getCloasestDistanceFast(coordinates, COORDINATE_COUNT);
      avgFastDistance += closestDistance;
      fastDistances[i] = closestDistance;
    }
    avgFastDistance /= COORDINATE_COUNT;//Average Closest Distance
    variance = 0;
    for(i = 0; i<COORDINATE_COUNT; i++)
      variance += (fastDistances[i] - avgFastDistance) * (fastDistances[i] - avgFastDistance);
    variance /= COORDINATE_COUNT;//Variance Computed

    //// Move the coordinates
    cout << "  Average [fast] distance: " << avgFastDistance << endl;
    cout << "  Variance: " << variance << "8";

    // Now take each coordinate and move it a step, reverting it if the step has taken it closer to any coordinate.
    for(i = 0; i<COORDINATE_COUNT; i++)
    {
      //fastDistances[i]; // The pre-move closest distance.
      coordinates[i]->randomMove(STEP_SIZE * variance);//Move the coordinate by a random amount scaled by the variance.
      if(coordinates[i]->getCloasestDistanceFast(coordinates, COORDINATE_COUNT) < fastDistances[i])
        coordinates[i]->backtrack(); // Backtrack if the step was a bad one.
    }
  }while(variance > 0.0001 && iteration < 100001);

  // Some more line stuff:
  cout << "[3B";

  //// Free the memory
  //for(i = 0; i<COORDINATE_COUNT; i++)
  //  delete coordinates[i];

  //exit(1);
}

void freeList(NonPlanarCoordinate* coordinates[])
{
  for(int i = 0; i<COORDINATE_COUNT; i++)
    delete coordinates[i]; // Remove from heap
}

int main(int argc, char** argv)
{
  srand(42);

  SphericalCoordinate::radius = 2;
  // Construct the coordinates on the heap
  NonPlanarCoordinate* coordinates[COORDINATE_COUNT];

  for(int i = 0; i<COORDINATE_COUNT; i++)
    coordinates[i] = new SphericalCoordinate(i);

  iterate(coordinates);

  // Free the memory
  freeList(coordinates);

  exit(1);
}

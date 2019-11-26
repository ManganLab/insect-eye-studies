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
static float STEP_SIZE = 5.0f;

using namespace std;

/// TODO: This will ultimately be a class with a type parameter for the subclass of NonPlanarCoordinate that it will use.

int main(int argc, char** argv)
{
  std::cout << "Running with " << COORDINATE_COUNT << " coordinates..." << std::endl;
  srand(42);
  int i;

  SphericalCoordinate::radius = 2;

  // Construct the coordinates on the heap
  NonPlanarCoordinate* coordinates[COORDINATE_COUNT];
  float avgFastDistance, variance;
  for(i = 0; i<COORDINATE_COUNT; i++)
    coordinates[i] = new SphericalCoordinate(i);

  // Iterate N times, morphing the positions of the coordinates...
  float fastDistances[COORDINATE_COUNT];
  for(int iteration = 0; iteration<10000; iteration++)
  {
    cout << "Iteration " << (iteration+1) << ":" << endl;
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
    cout << "  Variance: " << variance << endl;

    // Now take each coordinate and move it a step, reverting it if the step has taken it closer to any coordinate.
    for(i = 0; i<COORDINATE_COUNT; i++)
    {
      //fastDistances[i]; // The pre-move closest distance.
      coordinates[i]->randomMove(STEP_SIZE * variance);//Move the coordinate by a random amount scaled by the variance.
      if(coordinates[i]->getCloasestDistanceFast(coordinates, COORDINATE_COUNT) < fastDistances[i])
        coordinates[i]->backtrack(); // Backtrack if the step was a bad one.
    }
  }

  //// Free the memory
  //for(i = 0; i<COORDINATE_COUNT; i++)
  //  delete coordinates[i];

  // Experimental area
  //for (i = 0; i< 100; i++)
  //{
  //  SphericalCoordinate sc = SphericalCoordinate(42);
  //  SphericalCoordinate scClone = SphericalCoordinate(43);
  //  cout << "Test SC created: " << sc.getId() << endl;
  //  cout << "Clone SC created: " << scClone.getId() << endl;

  //  sc.cloneTo(&scClone);

  //  cout << "Distance between sc and clone: " << sc.getFastDistanceTo(&scClone) << endl;
  //  cout << endl<< "Manipulating clone by 0.0f ..." << endl;

  //  scClone.randomMove(0.0f);
  //  cout << "Distance between sc and clone: " << sc.getFastDistanceTo(&scClone) << endl;

  //  cout << "Manipulating clone by 1.0f..." << endl;

  //  scClone.randomMove(1.0f);
  //  cout << "Distance between sc and clone: " << sc.getFastDistanceTo(&scClone) << endl;
  //}

  exit(1);
}

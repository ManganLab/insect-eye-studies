//using namespace optix;

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <math.h>
#include <stdint.h>

//#include "TestClass.h"
//#include "TestClass.h"
#include "SphericalCoordinate.h"

static int COORDINATE_COUNT = 3;

/// TODO: This will ultimately be a class with a type parameter for the subclass of NonPlanarCoordinate that it will use.

int main(int argc, char** argv)
{
  //std::cout << "Running...\n";
  ////SphericalCoordinate sc();
  ////std::cout << sc.run() << endl;
  ////TestClass test = TestClass();
  //SphericalCoordinate test = SphericalCoordinate();
  //std::cout << test.run() << std::endl;

  //NonPlanarCoordinate* test2 = &test;
  //std::cout << (*test2).printNumber() << std::endl;

  //exit(1);

  //NonPlanarCoordinate coordinates[2];

  std::cout << "Running...\n";
  int i;

  // Construct the coordinates on the heap
  NonPlanarCoordinate<SphericalCoordinate>* coordinates[COORDINATE_COUNT];
  //SphericalCoordinate* coordinates[COORDINATE_COUNT];
  for(i = 0; i<COORDINATE_COUNT; i++)
    coordinates[i] = new SphericalCoordinate();

  //for(i = 0; i<COORDINATE_COUNT; i++)
  //{
  //  std::cout << "--" << std::endl;
  //  coordinates[i]->printNumber();
  //  std::cout << ((SphericalCoordinate*)coordinates[i])->run() << std::endl;
  //}

  int count = sizeof(coordinates)/sizeof(SphericalCoordinate*);
  std::cout << count << std::endl;

  for(i = 0; i<COORDINATE_COUNT; i++)
    delete coordinates[i];


  exit(1);
}

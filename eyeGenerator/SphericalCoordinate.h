#ifndef SPHERICALCOORDINATE_H
#define SPHERICALCOORDINATE_H

#include "NonPlanarCoordinate.h"

#include <iostream>

//using namespace std;

class SphericalCoordinate : public NonPlanarCoordinate {
  public:
    SphericalCoordinate();
    int run();

    // Virtual Overriders
    void randomMove(float scale);
    float getClosestDistance(NonPlanarCoordinate* others);
    void backtrack();
  private:
    int test;
};

#endif

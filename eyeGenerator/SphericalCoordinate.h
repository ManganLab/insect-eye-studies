#ifndef SPHERICALCOORDINATE_H
#define SPHERICALCOORDINATE_H

#include "NonPlanarCoordinate.h"

#include <iostream>

//using namespace std;

class SphericalCoordinate : public NonPlanarCoordinate<SphericalCoordinate> {
  public:
    SphericalCoordinate();
    ~SphericalCoordinate();
    int run();

    // Virtual Overriders
    void randomMove(float scale);
    float getClosestDistance(SphericalCoordinate* others);
    void backtrack();
  private:
    float logitude, latitude;
};

#endif

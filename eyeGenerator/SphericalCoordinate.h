#ifndef SPHERICALCOORDINATE_H
#define SPHERICALCOORDINATE_H

#include "NonPlanarCoordinate.h"

#include <iostream>
#include <limits>
#include <cmath>

#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_math_stream_namespace.h>

using namespace optix;

class SphericalCoordinate : public NonPlanarCoordinate {
  public:
    // static info for this setup
    static float radius;

    // Constructor/destructor
    SphericalCoordinate(int idin);
    ~SphericalCoordinate();

    // Virtual Overriders
    void randomMove(float scale);
    float getClosestDistance(NonPlanarCoordinate* others[], int count);
    float getCloasestDistanceFast(NonPlanarCoordinate* others[], int count);
    void backtrack();

    // Unique functions
    int getId();

  private:
    //float getDistanceTo(SphericalCoordinate* other);
    float getFastDistanceTo(SphericalCoordinate* other);
    void setLatLong(float lat, float lon);

    int id;
    float3 state;// Stores the actual directional state of this object
    float3 oldState;// Stores the last directional state for backtracking
};

#endif

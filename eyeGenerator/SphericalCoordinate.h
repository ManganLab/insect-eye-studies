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
    float getEnergy(NonPlanarCoordinate* others[], int count, int proximity);
    float getClosestDistance(NonPlanarCoordinate* others[], int count);
    float getCloasestDistanceFast(NonPlanarCoordinate* others[], int count);
    void backtrack();
    StaticCoordinate getStaticCoord();

    // Unique functions
    int getId();

    void cloneTo(SphericalCoordinate* clone);
    float getFastDistanceTo(SphericalCoordinate* other);

  private:
    static const float3 VERTICAL;// = make_float(0.0f,0.0f,1.0f);

    int id;
    float3 state;// Stores the actual directional state of this object
    float3 oldState;// Stores the last directional state for backtracking

    //float getDistanceTo(SphericalCoordinate* other);
    void setLatLong(float lat, float lon);
};

#endif

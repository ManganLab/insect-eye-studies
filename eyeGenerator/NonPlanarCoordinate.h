#ifndef NONPLANARCOORDINATE_H
#define NONPLANARCOORDINATE_H

#include <iostream>

#include <optixu/optixu_vector_types.h>

struct StaticCoordinate
{
#if defined(__cplusplus)
  typedef optix::float3 float3;
#endif
  float3 position;
  float3 direction;
  int padding[2];// padding so this structure is 32 bytes.
};

class NonPlanarCoordinate {
  public:
    // Constructor and destructor
    NonPlanarCoordinate();
    virtual ~NonPlanarCoordinate();
    // Takes a step in a random direction and distance (Scaled by `scale`)
    virtual void randomMove(float scale) = 0;
    // Calculates the energy (a function of how close other coordinates are to this coordinate)
    virtual float getEnergy(NonPlanarCoordinate* others[], int count, int proximity) = 0;
    // Returns the distance to the closest other NonPlanarCoordinate object (which usually has to be of the same class type)
    virtual float getClosestDistance(NonPlanarCoordinate* others[], int count) = 0;
    virtual float getCloasestDistanceFast(NonPlanarCoordinate* others[], int count) = 0;
    virtual void backtrack() = 0; // Backtracks the last step
    virtual StaticCoordinate getStaticCoord() = 0; // Returns this coordinate as a static coorinate.
  //protected:
  //  Some internal state that tracks position
};

#endif

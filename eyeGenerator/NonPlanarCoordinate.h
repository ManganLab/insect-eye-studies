#ifndef NONPLANARCOORDINATE_H
#define NONPLANARCOORDINATE_H

#include <iostream>

class NonPlanarCoordinate {
  public:
    // Constructor and destructor
    NonPlanarCoordinate();
    virtual ~NonPlanarCoordinate();
    // Takes a step in a random direction and distance (Scaled by `scale`)
    virtual void randomMove(float scale) = 0;
    // Returns the distance to the closest other NonPlanarCoordinate object (which usually has to be of the same class type)
    virtual float getClosestDistance(NonPlanarCoordinate* others[], int count) = 0;
    virtual float getCloasestDistanceFast(NonPlanarCoordinate* others[], int count) = 0;
    virtual void backtrack() = 0; // Backtracks the last step
  //protected:
  //  Some internal state that tracks position
};

#endif

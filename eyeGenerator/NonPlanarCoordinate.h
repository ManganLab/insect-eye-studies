#ifndef NONPLANARCOORDINATE_H
#define NONPLANARCOORDINATE_H

#include <iostream>

// Uses the Curiously Recurring Template Pattern
template <class T>
class NonPlanarCoordinate {
  public:
    // Constructor and destructor
    NonPlanarCoordinate();
    virtual ~NonPlanarCoordinate();
    // Takes a step in a random direction and distance (Scaled by `scale`)
    virtual void randomMove(float scale) = 0;
    // Returns the distance to the closest other NonPlanarCoordinate object (which usually has to be of the same class type)
    virtual float getClosestDistance(T* others) = 0; // Not even sure if this is needed, might be enough to cast?
    virtual void backtrack() = 0; // Backtracks the last step
  //protected:
  //  Some internal state that tracks position
};

#endif

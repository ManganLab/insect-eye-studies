#ifndef NONPLANARCOORDINATE_H
#define NONPLANARCOORDINATE_H

class NonPlanarCoordinate {
  public:
    virtual void randomMove(float scale) = 0; // Takes a step in a random direction and distance (Scaled by `scale`)
    virtual float getClosestDistance(NonPlanarCoordinate* others) = 0;
    virtual void backtrack() = 0; // Backtracks the last step
  //protected:
    //float x, y; // oordinates of the 
    // Some internal state that tracks position
};

#endif

#ifndef EYE_GENERATOR_H
#define EYE_GENERATOR_H

//class NonPlanarCoordinate;// Forward-declare the coordinate class.
#include "NonPlanarCoordinate.h"// Nope, actually include it for the StaticCoordinate type too.

class EyeGenerator {
  public:
    static void basicIterator(EyeGenerator* eg);

    int coordinateCount;
    
    EyeGenerator(int coordinateCount);
    ~EyeGenerator();

    void generateSphericalCoordinates();
    void test();
    StaticCoordinate getCoordinateInfo(int i);
  
  private:
    NonPlanarCoordinate** coordinates;
    
};

#endif

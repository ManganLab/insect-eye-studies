//using namespace optix;

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <math.h>
#include <stdint.h>

#include "SphericalCoordinate.h"

int main(int argc, char** argv)
{
  std::cout << "Running...\n";
  SphericalCoordinate sc();
  std::cout << sc.run() << endl;
  exit(1);
}

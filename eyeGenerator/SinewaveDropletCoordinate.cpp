#include "SinewaveDropletCoordinate.h"
#include <stdlib.h>

using namespace std;

SinewaveDropletCoordinate::SinewaveDropletCoordinate()
{
  position = make_float2(randRange(-0.5f, 0.5f), randRange(-0.5f, 0.5f));
  oldPosition.x = position.x;
  oldPosition.y = position.y;
}
SinewaveDropletCoordinate::~SinewaveDropletCoordinate(void) { }

void SinewaveDropletCoordinate::randomMove(float scale)
{
  //// Save the old state
  //oldPosition.x = position.x;
  //oldPosition.y = position.y;
  //// Makes a random move in a random direction
  //float angle = randRange(0, M_PI*2.0f);
  //float distance = randRange(0, scale);
  //position.x += cos(angle) * distance;
  //position.y += sin(angle) * distance;
  //position = fminf(fmaxf(position, -bounds), bounds);
}
void SinewaveDropletCoordinate::backtrack()
{
  // Backtracks
  position.x = oldPosition.x;
  position.y = oldPosition.y;
}
StaticCoordinate SinewaveDropletCoordinate::getStaticCoord()
{
  StaticCoordinate sc;
  // Really here is where we should transform it into 3D with wave mechanics.
  sc.position.x = position.x * scale;
  sc.position.z = position.y * scale;
  sc.position.y = 0.0f;
  sc.direction = TRUE_VERTICAL;
  return sc;
}
float SinewaveDropletCoordinate::getFastDistanceTo(NonPlanarCoordinate* other)
{
  return 0.0f;
}
float SinewaveDropletCoordinate::getDistanceTo(NonPlanarCoordinate* other)
{
  return 0.0f;
}

float SinewaveDropletCoordinate::scale = 1.0f;
float2 SinewaveDropletCoordinate::bounds = make_float2(0.5f, 0.5f);

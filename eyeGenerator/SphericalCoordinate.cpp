#include "SphericalCoordinate.h"
#include <stdlib.h>

using namespace std;

// Inlines
inline float randRange(float min, float max)
{
  return static_cast<float>(rand()/static_cast<float>(RAND_MAX)) * (max-min) + min;
}

SphericalCoordinate::SphericalCoordinate(int idin)//void)
{
  state = normalize(make_float3(randRange(-1.0f,1.0f),randRange(-1.0f,1.0f),randRange(-1.0f,1.0f)));
  id = idin + 1;
  //std::cout << "Spherical Coordinate object created" << std::endl;
}
SphericalCoordinate::~SphericalCoordinate(void)
{
  //std::cout << "Spherical Coordinate object destroyed" << std::endl;
}

inline float toDegrees(float rads)
{
  return((180*rads)/M_PI);
}

void SphericalCoordinate::randomMove(float scale)
{
  oldState = make_float3(state.x, state.y, state.z);
  /// Take a step at `stepAngle` degrees to north with `scale` distance.
  float stepAngle = randRange(0,2*M_PI);
  /// Formulate the vector offset from the Vertical
  float3 offset = normalize(make_float3(cos(stepAngle)*scale, sin(stepAngle)*scale, 1.0f));// TODO: Note that this is a planar projection onto a sphere.
  //float3 offset = make_float3(0.0f,0.0f,1.0f);// offset points directly up (like vertical).

  // TODO: the offset vector should probably be formed by rotating the up unit vector forward by scale, then around z by stepAngle.
  // TODO: This vector can then be subject to rotation toward the ommatidial position.
  /// First generate the position of the current ommatidium coordinate

  float verticalAngle = -acos(dot(state, VERTICAL));// The vertical angle of the current state (multiplied by -1 because the upcoming rotation is ccw)

  //cout << endl << "> state vertical angle : " << toDegrees(verticalAngle) << endl;
  //cout << "> offset vertical angle: " << toDegrees(acos(dot(offset,VERTICAL))) << endl;
  //cout << "> rotating offset..."<<endl;

  float horizontalAngle;
  if (state.x == 0.0f && state.y == 0.0f)
    horizontalAngle = 0.0f;
  else
    horizontalAngle = atan2(state.y, state.x);

  // TODO: The below rotation needs to be converted into a single matrix operation.
  float3 oldOffset = make_float3(offset.x, offset.y, offset.z);
  /// Then rotate the offset vector to the position of the current ommatidium coordinate
  // Rotate about the y-axis
  offset.x = cos(verticalAngle) * oldOffset.x - sin(verticalAngle) * oldOffset.z;
  offset.z = sin(verticalAngle) * oldOffset.x + cos(verticalAngle) * oldOffset.z;
  
  // Update oldOffset for the next rotation.
  oldOffset.x = offset.x;
  oldOffset.y = offset.y;
  oldOffset.z = offset.z;

  // Rotate about the z-axis
  offset.x = cos(horizontalAngle) * oldOffset.x - sin(horizontalAngle) * oldOffset.y;
  offset.y = sin(horizontalAngle) * oldOffset.x + cos(horizontalAngle) * oldOffset.y;


  // Update the state to reflect the new offset state.
  state.x = offset.x;
  state.y = offset.y;
  state.z = offset.z;
}

inline float SphericalCoordinate::getClosestDistance(NonPlanarCoordinate* others[], int count)
{
  return (radius * getCloasestDistanceFast(others, count));
}
// Returns distance as an angle as it's faster, the slow version will just scale it properly.
float SphericalCoordinate::getCloasestDistanceFast(NonPlanarCoordinate* others[], int count)
{
  int i;
  //cout << "Vector " << this->getId() << ": (" <<  this->state.x << ", " << this->state.y << ", " << this->state.z << ")" << endl;

  float closestDistance = std::numeric_limits<float>::max();
  for(i = 0; i<count; i++)
  {
    SphericalCoordinate* sc = (SphericalCoordinate*)others[i];
    if(sc == this)
      continue; // Skip comparing with itself

    // Claculate the distance to each of them
    float distance = this->getFastDistanceTo(sc);
    if(distance <= closestDistance)
      closestDistance = distance;
  }
    
  return(closestDistance);
}

void SphericalCoordinate::backtrack()
{
  //std::cout << "Backtracking..." << std::endl;
  state.x = oldState.x;
  state.y = oldState.y;
  state.z = oldState.z;
}

int SphericalCoordinate::getId()
{
  return id;
}

// Returns the distance, but not the actual distance around the sphere
float SphericalCoordinate::getFastDistanceTo(SphericalCoordinate* other)
{
  float angularDistance = acos(dot(this->state, other->state));
  if(isnan(angularDistance))
    return (0.0f);// Return zero as default behaviour
  return(angularDistance);
}

void SphericalCoordinate::cloneTo(SphericalCoordinate* clone)
{
  clone->state.x = state.x;
  clone->state.y = state.y;
  clone->state.z = state.z;
}

float SphericalCoordinate::radius = 5;
const float3 SphericalCoordinate::VERTICAL = make_float3(0.0f, 0.0f, 1.0f);

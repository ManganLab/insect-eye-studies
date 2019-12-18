

macro(add_eye_generator relative_path sources)
  include_directories(${relative_path})
  set(eye_generator_sources
      ${relative_path}/EyeGenerator.cpp
      ${relative_path}/EyeGenerator.h
      ${relative_path}/NonPlanarCoordinate.cpp
      ${relative_path}/NonPlanarCoordinate.h
      ${relative_path}/SphericalCoordinate.cpp
      ${relative_path}/SphericalCoordinate.h
     )
  list(APPEND ${sources} ${eye_generator_sources})
endmacro()


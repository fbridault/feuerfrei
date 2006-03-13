/* Source.cpp: implementation of the Source class. */
#include "source.hpp"

/* Operators */
Source & Source::operator= (const Source & S)
{
  lightIntensity = S.lightIntensity;
  position = new Point (*(S.position));
  direction = new Vector (*(S.direction));
  aperture = S.aperture;
  lightCategory = S.lightCategory;
  sampling = S.sampling;
  return *this;
}

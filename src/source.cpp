/* Source.cpp: implementation of the Source class. */
#include "source.hpp"

/* Operators */
CSource & CSource::operator= (const CSource & S)
{
  lightIntensity = S.lightIntensity;
  position = new CPoint (*(S.position));
  direction = new CVector (*(S.direction));
  aperture = S.aperture;
  lightCategory = S.lightCategory;
  sampling = S.sampling;
  return *this;
}

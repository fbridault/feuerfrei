#include "leadSkeleton.hpp"

#include <stdlib.h>
#include <math.h>

#include "../scene/graphicsFn.hpp"
#include "../solvers/solver.hpp"

LeadSkeleton::LeadSkeleton (Solver * const s, const Point& position, const Point& rootMoveFactor,  uint *pls) :
  Skeleton (s, position, rootMoveFactor, pls)
{
}

LeadSkeleton::~LeadSkeleton ()
{
}

void LeadSkeleton::drawParticle (Particle * const particle)
{
  glColor4f (0.1, 0.1, 0.1, 0.8);
  glPushMatrix ();
  glTranslatef (particle->x, particle->y, particle->z);
  GraphicsFn::SolidSphere (0.01, 10, 10);
  glPopMatrix ();
}

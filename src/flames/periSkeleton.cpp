#include "periSkeleton.hpp"

#include <stdlib.h>
#include <math.h>

#include "../scene/graphicsFn.hpp"
#include "../solvers/solver.hpp"

PeriSkeleton::PeriSkeleton (Solver * const s, const Point& position, const Point& rootMoveFactor, 
			    LeadSkeleton * guide, uint *pls) :
  Skeleton (s, position, rootMoveFactor, pls)
{
  this->guide = guide;
}

PeriSkeleton::~PeriSkeleton ()
{
}

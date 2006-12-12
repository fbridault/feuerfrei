#include "periSkeleton.hpp"

#include <stdlib.h>
#include <math.h>

#include "../scene/graphicsFn.hpp"
#include "../solvers/solver3D.hpp"

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE PERISKELETON **************************************/
/**********************************************************************************************************************/
PeriSkeleton::PeriSkeleton (Field3D * const s, const Point& position, const Point& rootMoveFactor, 
			    LeadSkeleton *leadSkeleton, uint *pls) :
  Skeleton (s, position, rootMoveFactor, pls)
{
  this->m_lead = leadSkeleton;
}

PeriSkeleton::~PeriSkeleton ()
{
}

FreePeriSkeleton* PeriSkeleton::split (uint splitHeight, FreeLeadSkeleton *leadSkeleton)
{
  FreePeriSkeleton *skel = new FreePeriSkeleton(this, leadSkeleton, splitHeight);
  
  m_headIndex = splitHeight;
  
  return( skel );
}

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FREEPERISKELETON **********************************/
/**********************************************************************************************************************/
FreePeriSkeleton::FreePeriSkeleton(const PeriSkeleton* const src, FreeLeadSkeleton* const leadSkeleton, uint splitHeight) :
  FreeSkeleton(src, splitHeight)
{
  m_lead = leadSkeleton;
}

FreePeriSkeleton::FreePeriSkeleton(uint size, Field3D* s, FreeLeadSkeleton* const leadSkeleton) :
  FreeSkeleton(size, s)
{
  m_lead = leadSkeleton;
}

FreePeriSkeleton::~FreePeriSkeleton ()
{
}

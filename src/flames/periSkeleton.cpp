#include "periSkeleton.hpp"

#include <stdlib.h>
#include <math.h>

#include "../scene/graphicsFn.hpp"
#include "../solvers/solver3D.hpp"

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE PERISKELETON **************************************/
/**********************************************************************************************************************/
PeriSkeleton::PeriSkeleton (Field3D * const s, const Point& position, const Point& rootMoveFactor, 
			    LeadSkeleton *leadSkeleton, FlameConfig *flameConfig) :
  Skeleton (s, position, rootMoveFactor, flameConfig)
{
  this->m_lead = leadSkeleton;
  addParticle(&m_root);
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

void PeriSkeleton::addParticle(const Point* const pt)
{
  if(m_headIndex >= NB_PARTICLES_MAX-1){
    puts("Erreur : trop de particules");
    return;
  }
  m_headIndex++;
  
  m_queue[m_headIndex] = *pt;
  m_queue[m_headIndex].birth(m_flameConfig->periLifeSpan);
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

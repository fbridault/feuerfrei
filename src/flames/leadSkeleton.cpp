#include "leadSkeleton.hpp"

#include <stdlib.h>
#include <math.h>

#include "periSkeleton.hpp"
#include "../scene/graphicsFn.hpp"
#include "../solvers/solver3D.hpp"

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE LEADSKELETON **************************************/
/**********************************************************************************************************************/
LeadSkeleton::LeadSkeleton (Field3D * const s, const Point& position, const Point& rootMoveFactor,  uint *pls) :
  Skeleton (s, position, rootMoveFactor, pls)
{
}

LeadSkeleton::~LeadSkeleton ()
{
}

void LeadSkeleton::drawParticle (Particle * const particle)
{
  glColor4f (0.1, 1.0, 0.1, 0.8);
  glPushMatrix ();
  glTranslatef (particle->x, particle->y, particle->z);
  GraphicsFn::SolidSphere (0.01, 10, 10);
  glPopMatrix ();
}

void LeadSkeleton::drawRoot ()
{
  glColor4f (0.0, 0.4, 0.0, 0.8);
  glPushMatrix ();
  glTranslatef (m_root.x, m_root.y, m_root.z);
  GraphicsFn::SolidSphere (0.01, 10, 10);
  glPopMatrix ();
}

FreeLeadSkeleton* LeadSkeleton::split (uint splitHeight)
{ 
  FreeLeadSkeleton *skel = new FreeLeadSkeleton(this, splitHeight);
  
  m_headIndex = splitHeight;
  
  return( skel );
}


/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FREELEADSKELETON **********************************/
/**********************************************************************************************************************/
FreeLeadSkeleton::FreeLeadSkeleton(const LeadSkeleton* const src, uint splitHeight) :
  FreeSkeleton(src, splitHeight)
{
}

FreeLeadSkeleton::~FreeLeadSkeleton ()
{
}

FreePeriSkeleton* FreeLeadSkeleton::dup(Point& offset)
{
  FreePeriSkeleton *copy = new FreePeriSkeleton(getSize (), m_solver, this);
  
  for (uint i = 0; i < getSize (); i++){
    copy->m_queue[i] = m_queue[i] + offset;
    copy->m_queue[i].m_lifespan += m_queue[i].m_lifespan;
  }
  
  copy->m_headIndex = m_headIndex;
  return copy;
}

void FreeLeadSkeleton::drawParticle (Particle * const particle)
{
  glColor4f (0.1, 1.0, 0.1, 0.8);
  glPushMatrix ();
  glTranslatef (particle->x, particle->y, particle->z);
  GraphicsFn::SolidSphere (0.01, 10, 10);
  glPopMatrix ();
}

#include "periSkeleton.hpp"

#include <stdlib.h>
#include <math.h>

#include "../scene/graphicsFn.hpp"
#include "../solvers/solver3D.hpp"
#include "leadSkeleton.hpp"


/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE PERISKELETON **************************************/
/**********************************************************************************************************************/
PeriSkeleton::PeriSkeleton (Field3D * const s, const Point& position, const Point& rootMoveFactor, 
			    LeadSkeleton *leadSkeleton, const FlameConfig* const flameConfig) :
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

void PeriSkeleton::addForces ()
{
  m_solver->addVsrc( m_root, m_lead->getLastAppliedForce(), m_selfVelocity);  
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

bool PeriSkeleton::moveParticle (Particle * const pos)
{
  if (pos->isDead ())
    return false;
  
  /* Déplacement de la particule */
  *pos += m_solver->getUVW(*pos, m_lead->getSelfVelocity()/1.1);
  
  /* Si la particule sort de la grille, elle est éliminée */
  if (   pos->x < 0 || pos->x > m_solver->getDimX()
      || pos->y < 0 || pos->y > m_solver->getDimY()
      || pos->z < 0 || pos->z > m_solver->getDimZ())
    return false;
  
  return true;
}

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FREEPERISKELETON **********************************/
/**********************************************************************************************************************/
FreePeriSkeleton::FreePeriSkeleton(const PeriSkeleton* const src, FreeLeadSkeleton* const leadSkeleton, uint splitHeight) :
  FreeSkeleton(src, splitHeight)
{
  m_lead = leadSkeleton;
}

FreePeriSkeleton::FreePeriSkeleton(uint size, Field3D* const s, FreeLeadSkeleton* const leadSkeleton) :
  FreeSkeleton(size, s)
{
  m_lead = leadSkeleton;
}

FreePeriSkeleton::~FreePeriSkeleton ()
{
}

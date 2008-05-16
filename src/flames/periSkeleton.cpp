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
			    LeadSkeleton *leadSkeleton, uint pls) :
  Skeleton (s, position, rootMoveFactor, pls)
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
  float dummy;

  /* Applique une force à la base des squelettes (pour les bougies). Ceci n'a
   * aucun effet pour les FakeFields, qui comptent eux sur la selfVelocity du
   * squelette relatif pour transmettre cette force (voir moveParticle() plus
   * bas). */
  m_solver->addVsrc( m_root, m_lead->getLastAppliedForce(), dummy); }

void PeriSkeleton::addParticle(const Point* const pt)
{
  if(m_headIndex >= NB_PARTICLES_MAX-1){
    puts("(EE) Too many particles in PeriSkeleton::addParticle() !!!");
    return;
  }
  m_headIndex++;
  
  m_queue[m_headIndex] = *pt;
  m_queue[m_headIndex].birth(m_lifeSpan);
}

bool PeriSkeleton::moveParticle (Particle * const particle)
{
  if (particle->isDead ())
    return false;
  
  /* Déplacement de la particule. Dans le cas présent, on utilise la
   * selfVelocity du squelette relatif pour ajouter une force propre au
   * squelette dans le cas des FakeFields. */
  m_solver->moveParticle(*particle, m_lead->getSelfVelocity());
  
  /* Si la particule sort de la grille, elle est éliminée */
  if (   particle->x < 0.0f || particle->x > m_solver->getDimX()
      || particle->y < 0.0f || particle->y > m_solver->getDimY()
      || particle->z < 0.0f || particle->z > m_solver->getDimZ())
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

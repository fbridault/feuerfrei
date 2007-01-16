#include "leadSkeleton.hpp"

#include <stdlib.h>
#include <math.h>

#include "periSkeleton.hpp"
#include "../scene/graphicsFn.hpp"
#include "../solvers/solver3D.hpp"

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE LEADSKELETON **************************************/
/**********************************************************************************************************************/
LeadSkeleton::LeadSkeleton (Field3D * const s, const Point& position, const Point& rootMoveFactor, FlameConfig *flameConfig,
			    double u, float noiseIncrement, float noiseMin, float noiseMax) :
  Skeleton (s, position, rootMoveFactor, flameConfig),
  m_noiseGenerator(noiseIncrement, noiseMin, noiseMax)
{
  m_u = u;
  m_perturbateCount=0;
  addParticle(&m_root);
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

void LeadSkeleton::addForces ()
{ 
  /* Recopie pour améliorer la lisibilité du code - optimisé par le compilateur de toute façon... */
  double innerForce = m_flameConfig->innerForce;
  char perturbate = m_flameConfig->flickering;
  int fdf = m_flameConfig->fdf;
  
  switch(fdf){
  case FDF_LINEAR :
    m_solver->addVsrc (m_root, innerForce * (m_u + 1));
    break;
  case FDF_BILINEAR :
    m_solver->addVsrc (m_root, innerForce * m_u * m_u );
    break;
  case FDF_EXPONENTIAL :
    m_solver->addVsrc (m_root, .1 * exp(innerForce * 14 * m_u));
    break;
  case FDF_GAUSS:
    m_solver->addVsrc (m_root, innerForce*exp(innerForce * 30 -(m_u) * m_u)/(9.0));
    break;
  case FDF_RANDOM:
    m_solver->addVsrc (m_root, innerForce * rand()/((double)RAND_MAX));
    break;
  }
  
  switch(perturbate){
  case FLICKERING_VERTICAL :
    if (m_perturbateCount >= 2)
      {
	m_solver->addVsrc (m_root, innerForce*5);
	m_perturbateCount = 0;
      }
    else
      m_perturbateCount++;
    break;
    
  case FLICKERING_RIGHT :
    //     if(m_perturbateCount>=24)
    //       m_perturbateCount = 0;
    //     else{
    //       if(m_perturbateCount>=20){
    // 	for (uint i = -m_solver->getZRes() / 4 - 1; i <= m_solver->getZRes () / 4 + 1; i++)
    // 	  for (uint j = -2 * m_solver->getYRes() / 4; j < -m_solver->getYRes() / 4; j++)
    // 	    m_solver->setUsrc (m_solver->getXRes(),
    // 			       ((uint) (ceil (m_solver->getYRes() / 2.0))) + j,
    // 			       ((uint) (ceil (m_solver->getZRes() / 2.0))) + i, -.1);
    //       }
    //       m_perturbateCount++;
    //    }
    break;
  case FLICKERING_RANDOM1 :
    m_solver->addVsrc (m_root, rand()/((double)RAND_MAX)-.5);
    break;
  case FLICKERING_RANDOM2 :
    m_solver->addVsrc(m_root, rand()/(10*(double)RAND_MAX));
    m_solver->addVsrc(m_root+Point(.1,0, 0), rand()/(10*(double)RAND_MAX));
    m_solver->addVsrc(m_root+Point(-.1,0,0), rand()/(10*(double)RAND_MAX));
    m_solver->addVsrc(m_root+Point(0,0, .1), rand()/(10*(double)RAND_MAX));
    m_solver->addVsrc(m_root+Point(0,0,-.1), rand()/(10*(double)RAND_MAX));
    break;
  case FLICKERING_NOISE :
    m_solver->addVsrc (m_root, m_noiseGenerator.getNextValue());
    break;
  }
}

void LeadSkeleton::addParticle(const Point* const pt)
{
  if(m_headIndex >= NB_PARTICLES_MAX-1){
    puts("Erreur : trop de particules");
    return;
  }
  m_headIndex++;
  
  m_queue[m_headIndex] = *pt;
  m_queue[m_headIndex].birth(m_flameConfig->leadLifeSpan);
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

FreePeriSkeleton* FreeLeadSkeleton::dup(const Point& offset)
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

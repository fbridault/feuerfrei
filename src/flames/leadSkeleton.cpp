#include "leadSkeleton.hpp"

#include <stdlib.h>
#include <math.h>
#include <engine/graphicsFn.hpp>

#include "periSkeleton.hpp"
#include "../solvers/solver3D.hpp"

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE LEADSKELETON **************************************/
/**********************************************************************************************************************/
LeadSkeleton::LeadSkeleton (Field3D * const s, const CPoint& position, const CPoint& rootMoveFactor,
			    uint pls, float u, float noiseIncrement, float noiseMin, float noiseMax) :
  Skeleton (s, position, rootMoveFactor,pls),
  m_noiseGenerator(noiseIncrement, noiseMin, noiseMax)
{
  m_u = u;
  m_perturbateCount=0;
  addParticle(&m_root);
}

LeadSkeleton::~LeadSkeleton ()
{
}

void LeadSkeleton::drawParticle (Particle * const particle) const
{
  glColor4f (0.1f, 1.0f, 0.1f, 0.8f);
  glPushMatrix ();
  glTranslatef (particle->x, particle->y, particle->z);
  CUGraphicsFn::SolidSphere (0.01f, 10, 10);
  glPopMatrix ();
}

void LeadSkeleton::drawRoot () const
{
  glColor4f (0.0f, 0.4f, 0.0f, 0.8f);
  glPushMatrix ();
  glTranslatef (m_root.x, m_root.y, m_root.z);
  CUGraphicsFn::SolidSphere (0.01f, 10, 10);
  glPopMatrix ();
}

void LeadSkeleton::addForces (int fdf, float innerForce, char perturbate)
{
  m_selfVelocity = 0.0f;

  /* Applications des FDF */
  switch(fdf){
  case FDF_LINEAR :
    m_lastAppliedForce = innerForce * (m_u + 1.0f);
    break;
  case FDF_BILINEAR :
    m_lastAppliedForce = innerForce * m_u * m_u;
    break;
  case FDF_EXPONENTIAL :
    m_lastAppliedForce = .1f * innerForce * 10 * exp(m_u);
    break;
  case FDF_GAUSS:
    m_lastAppliedForce = innerForce * 10 * exp(-m_u * m_u)/(9.0f);
    break;
  case FDF_RANDOM:
    m_lastAppliedForce = innerForce * rand()/((float)RAND_MAX);
    break;
  default:
    cerr << "(EE) FDF type error !!!" << endl;
  }

  switch(perturbate){
  case FLICKERING_VERTICAL :
    if (m_perturbateCount >= 2)
      {
	m_lastAppliedForce += innerForce*5.0f;
	m_perturbateCount = 0;
      }
    else
      m_perturbateCount++;
    break;
  case FLICKERING_RANDOM1 :
    m_lastAppliedForce += (rand()/((float)RAND_MAX))/2.0f - .25f;
    break;
  case FLICKERING_RANDOM2 :
    m_lastAppliedForce += rand()/(10.0f*(float)RAND_MAX);
    break;
  case FLICKERING_NOISE :
    m_lastAppliedForce += m_noiseGenerator.getNextValue();
    break;
  case FLICKERING_NONE :
    break;
  default:
    cerr << "(EE) Flickering type error !!!" << endl;
  }
  m_solver->addVsrc( m_root, m_lastAppliedForce, m_selfVelocity);
}

void LeadSkeleton::addParticle(const CPoint* const pt)
{
  if(m_headIndex >= NB_PARTICLES_MAX-1){
    puts("(EE) Too many particles in LeadSkeleton::addParticle() !!!");
    return;
  }
  m_headIndex++;

  m_queue[m_headIndex] = *pt;
  m_queue[m_headIndex].birth(m_lifeSpan);
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

FreePeriSkeleton* FreeLeadSkeleton::dup(const CPoint& offset)
{
  FreePeriSkeleton *copy = new FreePeriSkeleton(getInternalSize (), m_solver, this);

  for (uint i = 0; i < getInternalSize (); i++){
    copy->m_queue[i] = m_queue[i] + offset;
    copy->m_queue[i].m_lifespan += m_queue[i].m_lifespan;
  }

  copy->m_headIndex = m_headIndex;
  copy->m_selfVelocity = m_selfVelocity;
  return copy;
}

void FreeLeadSkeleton::drawParticle (Particle * const particle) const
{
  glColor4f (0.1f, 1.0f, 0.1f, 0.8f);
  glPushMatrix ();
  glTranslatef (particle->x, particle->y, particle->z);
  CUGraphicsFn::SolidSphere (0.01f, 10, 10);
  glPopMatrix ();
}

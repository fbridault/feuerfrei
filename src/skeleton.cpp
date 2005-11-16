#include "skeleton.hpp"

#include <stdlib.h>
#include <math.h>

#include "graphicsFn.hpp"


Skeleton::Skeleton(Solver* const s, const CPoint& position, const CPoint& pt, 
		   const CPoint& rootMoveFactor, int pls) : m_rootMoveFactor(rootMoveFactor)
{
  m_solver = s;
  m_flamePos = position;
  m_queue = new Particle[NB_PARTICULES];
  m_headIndex = -1;
  
  m_root = m_rootSave = pt;
  m_particleLifespan = pls;
  addParticle(&m_root);
}

Skeleton::~Skeleton()
{
  delete [] m_queue;
}

void Skeleton::addParticle(const CPoint* const pt)
{
  if(m_headIndex >= NB_PARTICULES-1){
    puts("Erreur : trop de particules");
    return;
  }
  m_headIndex++;
  
  m_queue[m_headIndex] = *pt;
  m_queue[m_headIndex].birth(m_particleLifespan);
}

void Skeleton::removeParticle(int n)
{
  int i;
  
  for( i=n; i<m_headIndex ; i++)
    m_queue[i] = m_queue[i+1];
  
  m_headIndex--;
  if(m_headIndex==-1){
    //puts("Erreur : file vide");
    addParticle(&m_root);
    return;
  }
}

void Skeleton::updateParticle(int i, const CPoint* const pt)
{
  m_queue[i] = *pt;
  m_queue[i].decreaseLife();
}

void Skeleton::swap(int i, int j)
{
  Particle tmp(m_queue[i]);
  
  m_queue[i] = m_queue[j];
  
  m_queue[j] = tmp;
}

void Skeleton::draw ()
{
  glDisable (GL_LIGHTING);

  drawRoot();
  for (int i = 0; i < getSize (); i++)
    drawParticle( getParticle (i) ) ;

  glEnable (GL_LIGHTING);
}

void Skeleton::drawRoot ()
{
  glColor4f (1.0, 0.0, 0.25, 0.8);
  glPushMatrix ();
  glTranslatef (m_root.x, m_root.y, m_root.z);
  GraphicsFn::SolidSphere (0.01, 10, 10);
  glPopMatrix ();
}

void Skeleton::drawParticle (Particle * const particle)
{
  glColor4f (1.0, 1.0, 0.25, 0.8);
  glPushMatrix ();
  glTranslatef (particle->x, particle->y, particle->z);
  GraphicsFn::SolidSphere (0.01, 10, 10);
  glPopMatrix ();
}

#include "skeleton.hpp"

#include <stdlib.h>
#include <math.h>

#include "../scene/graphicsFn.hpp"
#include "../solvers/solver3D.hpp"

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FREESKELETON **************************************/
/**********************************************************************************************************************/
FreeSkeleton::FreeSkeleton(uint size, Field3D* const s)
{
  m_solver = s;
  m_queue = new Particle[size];
  m_headIndex = -1;
}

FreeSkeleton::FreeSkeleton(const FreeSkeleton* const src, uint splitHeight)
{
  uint i;
  
  m_solver = src->m_solver;
  m_queue = new Particle[splitHeight+1];
  
  /* Recopie des particules en fonction de la hauteur de coupe */
  for( i=0; i <= splitHeight; i++){
    m_queue[i] = src->m_queue[i];
    m_queue[i].m_lifespan +=5;
  }
  m_headIndex = splitHeight;
  m_selfVelocity = src->m_selfVelocity;
}

FreeSkeleton::~FreeSkeleton()
{
  delete [] m_queue;
}

void FreeSkeleton::removeParticle(uint n)
{
  int i;
  
  for( i=n; i<m_headIndex ; i++)
    m_queue[i] = m_queue[i+1];
  
  m_headIndex--;
//   if(m_headIndex==-1){
//     //puts("Erreur : file vide");
//     addParticle(&m_root);
//     return;
//   }
}

void FreeSkeleton::swap(uint i, uint j)
{
  Particle tmp(m_queue[i]);
  
  m_queue[i] = m_queue[j];  
  m_queue[j] = tmp;
}

void FreeSkeleton::move ()
{
  Particle *tmp;
  uint i;
  
  /* Déplacement des particules */
  /* Boucle de parcours : du haut vers le bas */
  for (i = 0; i < getSize (); i++)
    {
      tmp = getParticle (i);

      if (moveParticle (tmp))
	  updateParticle (i, tmp);
      else
	{
	  removeParticle (i);
	  if (i < getSize () - 1)
	    i--;
	}
    }	/* for */
  
}

bool FreeSkeleton::moveParticle (Particle * const particle)
{
  if (particle->isDead ())
    return false;
  
  /* Si la particule sort de la grille, elle prend la vélocité du bord */
  if ( particle->x >= m_solver->getDimX() )
    particle->x = m_solver->getDimX() - EPSILON;
  if ( particle->y >= m_solver->getDimY() )
    particle->y = m_solver->getDimY() - EPSILON;
  if ( particle->z >= m_solver->getDimZ() )
    particle->z = m_solver->getDimZ() - EPSILON;
  
  /* Calculer la nouvelle position */
  /* Intégration d'Euler */
  m_solver->moveParticle(*particle, m_selfVelocity);
  
  return true;
}

void FreeSkeleton::draw () const
{
  for (uint i = 0; i < getSize (); i++)
    drawParticle( getParticle (i) ) ;
  glBegin(GL_LINE_STRIP);
  for (uint i = 0; i < getSize (); i++)
    glVertex3f(m_queue[i].x, m_queue[i].y, m_queue[i].z);
  glEnd();    
}

void FreeSkeleton::drawParticle (Particle * const particle) const
{
  glColor4f (1.0, 1.0, 0.25, 0.8);
  glPushMatrix ();
  glTranslatef (particle->x, particle->y, particle->z);
  GraphicsFn::SolidSphere (0.01, 10, 10);
  glPopMatrix ();
}


/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE SKELETON ******************************************/
/**********************************************************************************************************************/

Skeleton::Skeleton(Field3D* const s, const Point& position, const Point& rootMoveFactor, const FlameConfig* const flameConfig) : 
  FreeSkeleton(NB_PARTICLES_MAX, s),
  m_rootMoveFactor(rootMoveFactor)
{  
  m_root = m_rootSave = position;
  m_flameConfig = flameConfig;
  m_selfVelocity=0;
}

void Skeleton::draw () const
{
  drawRoot();
  for (uint i = 0; i < getSize (); i++)
    drawParticle( getParticle (i) ) ;
  glBegin(GL_LINE_STRIP);
  for (uint i = 0; i < getSize (); i++)
    glVertex3f(m_queue[i].x, m_queue[i].y, m_queue[i].z);
  glVertex3f(m_root.x, m_root.y, m_root.z);
  glEnd();
}

void Skeleton::drawRoot () const
{
  glColor4f (1.0, 0.0, 0.25, 0.8);
  glPushMatrix ();
  glTranslatef (m_root.x, m_root.y, m_root.z);
  GraphicsFn::SolidSphere (0.01, 10, 10);
  glPopMatrix ();
}

void Skeleton::moveRoot ()
{
  /* Calculer la nouvelle position */
  /* Intégration d'Euler */
  m_root = m_rootSave + m_rootMoveFactor * m_solver->getUVW (m_rootSave, m_selfVelocity);
  
  return;
}

void Skeleton::move ()
{
  Particle *tmp;
  uint i;
  
  moveRoot ();
  
  if (getSize () < NB_PARTICLES_MAX - 1)
      addParticle (&m_root);
  
  /* Déplacement des particules */
  /* Boucle de parcours : du haut vers le bas */
  for (i = 0; i < getSize (); i++)
    {
      tmp = getParticle (i);

      if (moveParticle (tmp))
	  updateParticle (i, tmp);
      else
	{
	  removeParticle (i);
	  if (i < getSize () - 1)
	    i--;
	}
    }	/* for */
}

bool Skeleton::moveParticle (Particle * const particle)
{
  if (particle->isDead ())
    return false;
  
  m_solver->moveParticle(*particle, m_selfVelocity);
  
  /* Si la particule sort de la grille, elle est éliminée */
  if (   particle->x < 0 || particle->x > m_solver->getDimX()
      || particle->y < 0 || particle->y > m_solver->getDimY()
      || particle->z < 0 || particle->z > m_solver->getDimZ())
    return false;
  
  return true;
}

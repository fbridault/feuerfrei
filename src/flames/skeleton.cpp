#include "skeleton.hpp"

#include <stdlib.h>
#include <math.h>

#include "../scene/graphicsFn.hpp"
#include "../solvers/solver3D.hpp"

#define LIFE_EXTEND 5

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FREESKELETON **************************************/
/**********************************************************************************************************************/
FreeSkeleton::FreeSkeleton(uint size, Field3D* const s)
{
  assert (s != NULL);
  m_solver = s;
  m_queue = new Particle[size];
  m_headIndex = -1;
}

FreeSkeleton::FreeSkeleton(const FreeSkeleton* const src, uint splitHeight)
{
  uint i;

  assert (src != NULL);

  m_solver = src->m_solver;
  m_queue = new Particle[splitHeight+1];
  
  /* Recopie des particules en fonction de la hauteur de coupe */
  for( i=0; i <= splitHeight; i++){
    m_queue[i] = src->m_queue[i];
    /* Extension de la durée de vie !! */
    m_queue[i].m_lifespan +=LIFE_EXTEND;
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
  assert(m_headIndex>=0);
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
  for (i = 0; i < getInternalSize (); i++)
    {
      tmp = getInternalParticle (i);

      if (moveParticle (tmp))
	  updateParticle (i, tmp);
      else
	{
	  removeParticle (i);
	  if (i < getInternalSize () - 1)
	    i--;
	}
    }
}

bool FreeSkeleton::moveParticle (Particle * const particle)
{
  Particle copy(*particle), copy2;

  assert (particle != NULL);

  if (particle->isDead ())
    return false;
  
  /* Si la particule sort de la grille, elle prend la vélocité du bord */
  if ( particle->x >= m_solver->getDimX() )
    particle->x = m_solver->getDimX() - EPSILON;
  if ( particle->y >= m_solver->getDimY() )
    particle->y = m_solver->getDimY() - EPSILON;
  if ( particle->z >= m_solver->getDimZ() )
    particle->z = m_solver->getDimZ() - EPSILON;
  if ( particle->x < 0.0f )
    particle->x = EPSILON;
  if ( particle->z < 0.0f )
    particle->z = EPSILON;
  /* Cas particulier en y, on supprime la particule si elle passe SOUS la grille
   * (évite les problèmes de "stagnation" de la flamme) */
  if ( particle->y < 0.0f )
    return false;

  copy2 = *particle;
  /* Calculer la nouvelle position ( Intégration d'Euler, on prend juste la dérivée première ) */
  m_solver->moveParticle(*particle, m_selfVelocity);
  
  *particle = *particle - copy2 + copy;
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
  glColor4f (1.0f, 1.0f, 0.25f, 0.8f);
  glPushMatrix ();
  glTranslatef (particle->x, particle->y, particle->z);
  GraphicsFn::SolidSphere (0.01f, 10, 10);
  glPopMatrix ();
}


/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE SKELETON ******************************************/
/**********************************************************************************************************************/

Skeleton::Skeleton(Field3D* const s, const Point& position, const Point& rootMoveFactor, uint pls) : 
  FreeSkeleton(NB_PARTICLES_MAX, s),
  m_rootMoveFactor(rootMoveFactor)
{  
  m_root = m_rootSave = position;
  m_selfVelocity=0.0f;
  m_lod=FULL_SKELETON;
  m_lifeSpan = pls;
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
  glColor4f (1.0f, 0.0f, 0.25f, 0.8f);
  glPushMatrix ();
  glTranslatef (m_root.x, m_root.y, m_root.z);
  GraphicsFn::SolidSphere (0.01f, 10, 10);
  glPopMatrix ();
}

void Skeleton::moveRoot ()
{
  /* Calculer la nouvelle position ( Intégration d'Euler, on prend juste la dérivée première ) */
  m_root = m_rootSave + m_rootMoveFactor * m_solver->getUVW (m_rootSave, m_selfVelocity);
  
  /* Si l'origine sort de la grille, on la replace */
  if ( m_root.x >= m_solver->getDimX() )
    m_root.x = m_solver->getDimX() - EPSILON;
  if ( m_root.y >= m_solver->getDimY() )
    m_root.y = m_solver->getDimY() - EPSILON;
  if ( m_root.z >= m_solver->getDimZ() )
    m_root.z = m_solver->getDimZ() - EPSILON;
  if ( m_root.x < 0.0f )
    m_root.x = EPSILON;
  if ( m_root.y < 0.0f )
    m_root.y = EPSILON;
  if ( m_root.z < 0.0f )
    m_root.z = EPSILON;
}

void Skeleton::move ()
{
  Particle *tmp;
  uint i;
  
  moveRoot ();
  
  if (getInternalSize () < NB_PARTICLES_MAX - 1)
      addParticle (&m_root);

  FreeSkeleton::move ();
}

bool Skeleton::moveParticle (Particle * const particle)
{
  assert (particle != NULL);
  
  if (particle->isDead ())
    return false;
  
  m_solver->moveParticle(*particle, m_selfVelocity);
  
  /* Si la particule sort de la grille, elle est éliminée */
  if (   particle->x < 0.0f || particle->x > m_solver->getDimX()
      || particle->y < 0.0f || particle->y > m_solver->getDimY()
      || particle->z < 0.0f || particle->z > m_solver->getDimZ() )
    return false;
  
  return true;
}

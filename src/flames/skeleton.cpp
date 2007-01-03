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

void FreeSkeleton::updateParticle(uint i, const Point* const pt)
{
  m_queue[i] = *pt;
  m_queue[i].decreaseLife();
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

bool FreeSkeleton::moveParticle (Particle * const pos)
{
  uint i, j, k;

  if (pos->isDead ())
    return 0;

  /* Retrouver les quatres cellules adjacentes autour de la particule */
  m_solver->findPointPosition(*pos, i, j, k);
  
  /* Si la particule sort de la grille, elle prend la vélocité du bord */
  if ( i >= m_solver->getXRes()  )
    i = m_solver->getXRes()-1;
  if ( j >= m_solver->getYRes()  )
    j = m_solver->getYRes()-1;
  if ( k >= m_solver->getZRes()  )
    k = m_solver->getZRes()-1;
  
  /* Calculer la nouvelle position */
  /* Intégration d'Euler */
  *pos += m_solver->getUVW (i, j, k);
  
  return 1;
}

void FreeSkeleton::draw ()
{
  for (uint i = 0; i < getSize (); i++)
    drawParticle( getParticle (i) ) ;
  glBegin(GL_LINE_STRIP);
  for (uint i = 0; i < getSize (); i++)
    glVertex3f(m_queue[i].x, m_queue[i].y, m_queue[i].z);
  glEnd();    
}

void FreeSkeleton::drawParticle (Particle * const particle)
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

Skeleton::Skeleton(Field3D* const s, const Point& position, const Point& rootMoveFactor, uint *pls) : 
  FreeSkeleton(NB_PARTICLES_MAX, s),
  m_rootMoveFactor(rootMoveFactor)
{  
  m_root = m_rootSave = position;
  m_particleLifespan = pls;
  addParticle(&m_root);
}

void Skeleton::addParticle(const Point* const pt)
{
  if(m_headIndex >= NB_PARTICLES_MAX-1){
    puts("Erreur : trop de particules");
    return;
  }
  m_headIndex++;
  
  m_queue[m_headIndex] = *pt;
  m_queue[m_headIndex].birth(*m_particleLifespan);
}

void Skeleton::draw ()
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

void Skeleton::drawRoot ()
{
  glColor4f (1.0, 0.0, 0.25, 0.8);
  glPushMatrix ();
  glTranslatef (m_root.x, m_root.y, m_root.z);
  GraphicsFn::SolidSphere (0.01, 10, 10);
  glPopMatrix ();
}

void Skeleton::moveRoot ()
{
  uint i, j, k;
  
  m_solver->findPointPosition(m_root, i, j, k);

  /* Calculer la nouvelle position */
  /* Intégration d'Euler */
  m_root = m_rootSave + m_rootMoveFactor * m_solver->getUVW (i, j, k);

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


bool Skeleton::moveParticle (Particle * const pos)
{
  uint i, j, k;

  if (pos->isDead ())
    return false;

  /* Retrouver les quatres cellules adjacentes autour de la particule */
  m_solver->findPointPosition(*pos, i, j, k);
  
  /* Calculer la nouvelle position */
  /* Intégration d'Euler */
  *pos += m_solver->getUVW (i, j, k);

  if (pos->x < 0 || pos->x > 1
      || pos->y < 0 || pos->y > 1
      || pos->z < 0 || pos->z > 1)
    return false;
  
  return true;
}

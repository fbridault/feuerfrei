#include "periSkeleton.hpp"

#include <stdlib.h>
#include <math.h>

#include "graphicsFn.hpp"
#include "solvers/solver.hpp"

PeriSkeleton::PeriSkeleton (Solver * const s, const CPoint& position, const CPoint& pt,
			    const CPoint& rootMoveFactor, LeadSkeleton * guide, int pls) :
  Skeleton (s, position, pt, rootMoveFactor, pls)
{
  this->guide = guide;
}

PeriSkeleton::~PeriSkeleton ()
{
}

int
PeriSkeleton::moveRoot ()
{
  int i, j, k;
  CPoint tmp;
  double distx = 10 * m_solver->getDimX() / (double) m_solver->getXRes ();
  double distz = m_solver->getDimZ() / (double) m_solver->getZRes ();
  
  m_solver->findPointPosition(m_root, i, j, k);

  /* Calculer la nouvelle position */
  /* Int�gration d'Euler */

  // cout << pos->x << " " << pos->y << " " << pos->z << "     ";
  //   cout << i << " " << j << " " << k << endl;

  //tmp.setX( m_rootSave.x + 4*(m_solver->getU(i,j,k) + m_solver->getU(i-1,j,k) + m_solver->getU(i+1,j,k) + m_solver->getU(i,j-1,k) + m_solver->getU(i,j+1,k)+ m_solver->getU(i,j,k-1) + m_solver->getU(i,j,k+1))/7.0 );
  //tmp.setY( m_rootSave.y + 0.75*(m_solver->getV(i,j,k) + m_solver->getV(i-1,j,k) + m_solver->getV(i+1,j,k) + m_solver->getV(i,j-1,k) + m_solver->getV(i,j+1,k)+ m_solver->getV(i,j,k-1) + m_solver->getV(i,j,k+1))/7.0 );
  //tmp.setZ( m_rootSave.z + 4*(m_solver->getW(i,j,k) + m_solver->getW(i-1,j,k) + m_solver->getW(i+1,j,k) + m_solver->getW(i,j-1,k) + m_solver->getW(i,j+1,k)+ m_solver->getW(i,j,k-1) + m_solver->getW(i,j,k+1))/7.0 );

  tmp.x = m_rootSave.x + m_rootMoveFactor.x * m_solver->getU (i, j, k);
  tmp.y = m_rootSave.y + m_rootMoveFactor.y * m_solver->getV (i, j, k);
  tmp.z = m_rootSave.z + m_rootMoveFactor.z * m_solver->getW (i, j, k);

  if (tmp.x < (m_rootSave.x - distx)
      || tmp.x > (m_rootSave.x + distx)
      || tmp.z < (m_rootSave.z - distz)
      || tmp.z > (m_rootSave.z + distz))
    return 0;

  m_root = tmp;

  return 1;
}

int
PeriSkeleton::moveParticle (Particle * const pos, int n)
{
  int i, j, k;
  //  int light;

  if (pos->isDead ())
    return 0;
  
  m_solver->findPointPosition(*pos, i, j, k);

  /* Calculer la nouvelle position */
  /* Intégration d'Euler */

  // cout << pos->x << " " << pos->y << " " << pos->z << "     ";
  //   cout << i << " " << j << " " << k << endl;
  //pos->addX( (m_solver->getU(i,j,k) + m_solver->getU(i-1,j,k) + m_solver->getU(i+1,j,k) + m_solver->getU(i,j-1,k) + m_solver->getU(i,j+1,k) + m_solver->getU(i,j,k-1) + m_solver->getU(i,j,k+1))/7.0);
  //pos->addY( (m_solver->getV(i,j,k) + m_solver->getV(i-1,j,k) + m_solver->getV(i+1,j,k) + m_solver->getV(i,j-1,k) + m_solver->getV(i,j+1,k) + m_solver->getV(i,j,k-1) + m_solver->getV(i,j,k+1))/7.0);
  //pos->addZ( (m_solver->getW(i,j,k) + m_solver->getW(i-1,j,k) + m_solver->getW(i+1,j,k) + m_solver->getW(i,j-1,k) + m_solver->getW(i,j+1,k) + m_solver->getW(i,j,k-1) + m_solver->getW(i,j,k+1))/7.0);

  pos->x += m_solver->getU (i, j, k);
  pos->y += m_solver->getV (i, j, k);
  pos->z += m_solver->getW (i, j, k);

  if (pos->x < -m_solver->getDimX() / 2.0 || pos->x > m_solver->getDimX() / 2.0 || 
      pos->y < 0 || pos->y > m_solver->getDimY() ||
      pos->z < -m_solver->getDimZ() / 2.0 || pos->z > m_solver->getDimZ() / 2.0)
    return 0;
  
  return 1;
}

/* Gère la création et la destruction des particules */
void
PeriSkeleton::move ()
{
  Particle *tmp, *tmp2;
  double dist;
  int i;

  moveRoot ();

  if (getSize () < NB_PARTICULES - 1)
    {
      /* On détermine s'il faut lâcher une nouvelle particule */
      /* On calcule la distance entre la dernière particule lâchée et l'm_root */
      //tmp = getLastParticle ();
      //dist = m_root.squaredDistanceFrom (tmp);
      /* On laisse la distance au carré pour éviter un calcul de racine carré coûteux */
      //if (dist > .0025)
      addParticle (&m_root);
    }

  /* Affichage des particules */
  /* Boucle de parcours : du haut vers le bas */
  for (i = 0; i < getSize (); i++)
    {
      tmp = getParticle (i);

      if (moveParticle (tmp, i))
	{
	  updateParticle (i, tmp);

	  // /* Si on */
// 	  if (i < getSize () - 1)
// 	    {
// 	      tmp2 = getParticle (i + 1);

// 	      //      if(tmp->y < tmp2->y){
// 	      //        if(guide)
// 	      //          printf("guide alors : %f %f %d\n",tmp->y,tmp2->y,i);
// 	      //        else
// 	      //          printf("prout alors : %f %f %d\n",tmp->y,tmp2->y,i);

// 	      //        //swap(i,i+1);
// 	      //      }

// 	      /* Si deux particules sont trop proches on supprime la plus basse */
// 	      dist = tmp2->squaredDistanceFrom (tmp);

// 	      if (dist < .0005)
// 		{
// 		  puts ("particules trop proches : suppression...");
// 		  removeParticle (i);
// 		  i--;
// 		}
// 	    }
	}
      else
	{
	  removeParticle (i);
	  if (i < getSize () - 1)
	    i--;
	}
    }			/* for */

}

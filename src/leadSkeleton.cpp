#include "leadSkeleton.hpp"

#include <stdlib.h>
#include <math.h>

#include "graphicsFn.hpp"
#include "solver.hpp"


LeadSkeleton::LeadSkeleton (Solver * const s, const CPoint& position, const CPoint& pt, 
			    const CPoint& rootMoveFactor,  int pls) :
  Skeleton (s, position, pt, rootMoveFactor, pls)
{
}

LeadSkeleton::~LeadSkeleton ()
{
}

int
LeadSkeleton::moveRoot ()
{
  int i, j, k;
  CPoint tmp;
  float distx = 10 * m_solver->getDimX() / (float) m_solver->getX ();
  float distz = m_solver->getDimZ() / (float) m_solver->getZ ();
  
  /* Retrouver les quatres cellules adjacentes autour de la particule */
  i = (int) (m_root.getX () * m_solver->getDimX() * m_solver->getX ()) + 1 +
    m_solver->getX () / 2;
  j = (int) (m_root.getY () * m_solver->getDimY() * m_solver->getY ()) + 1;
  k = (int) (m_root.getZ () * m_solver->getDimZ() * m_solver->getZ ()) + 1 +
    m_solver->getZ () / 2;

  /* Calculer la nouvelle position */
  /* Int�gration d'Euler */

  // cout << pos->x << " " << pos->y << " " << pos->z << "     ";
  //   cout << i << " " << j << " " << k << endl;

  //  tmp.setX( m_rootSave.getX() + 4*(m_solver->getU(i,j,k) + m_solver->getU(i-1,j,k) + m_solver->getU(i+1,j,k) + m_solver->getU(i,j-1,k) + m_solver->getU(i,j+1,k)+ m_solver->getU(i,j,k-1) + m_solver->getU(i,j,k+1))/7.0 );
  //  tmp.setY( m_rootSave.getY() + 0.75*(m_solver->getV(i,j,k) + m_solver->getV(i-1,j,k) + m_solver->getV(i+1,j,k) + m_solver->getV(i,j-1,k) + m_solver->getV(i,j+1,k)+ m_solver->getV(i,j,k-1) + m_solver->getV(i,j,k+1))/7.0 );
  //  tmp.setZ( m_rootSave.getZ() + 4*(m_solver->getW(i,j,k) + m_solver->getW(i-1,j,k) + m_solver->getW(i+1,j,k) + m_solver->getW(i,j-1,k) + m_solver->getW(i,j+1,k)+ m_solver->getW(i,j,k-1) + m_solver->getW(i,j,k+1))/7.0 );

  tmp.setX (m_rootSave.getX () + m_rootMoveFactor.getX() * m_solver->getU (i, j, k));
  tmp.setY (m_rootSave.getY () + m_rootMoveFactor.getY() * m_solver->getV (i, j, k));
  tmp.setZ (m_rootSave.getZ () + m_rootMoveFactor.getZ() * m_solver->getW (i, j, k));

  if (tmp.getX () < (m_rootSave.getX () - distx) ||
      tmp.getX () > (m_rootSave.getX () + distx) ||
      tmp.getZ () < (m_rootSave.getZ () - distz) ||
      tmp.getZ () > (m_rootSave.getZ () + distz))
    return 0;
  
  m_root = tmp;
  
  return 1;
}

int
LeadSkeleton::moveParticle (Particle * const pos)
{
  int i, j, k;
  //  int light;

  if (pos->isDead ())
    return 0;

  /* Retrouver les quatres cellules adjacentes autour de la particule */
  i = (int) (pos->getX () * m_solver->getDimX() * m_solver->getX ()) + 1 + m_solver->getX () / 2;
  j = (int) (pos->getY () * m_solver->getDimY() * m_solver->getY ()) + 1;
  k = (int) (pos->getZ () * m_solver->getDimZ() * m_solver->getZ ()) + 1 + m_solver->getZ () / 2;

  /* Calculer la nouvelle position */
  /* Intégration d'Euler */

  // cout << pos->x << " " << pos->y << " " << pos->z << "     ";
  //   cout << i << " " << j << " " << k << endl;
  //   pos->addX( (m_solver->getU(i,j,k) + m_solver->getU(i-1,j,k) + m_solver->getU(i+1,j,k) + m_solver->getU(i,j-1,k) + m_solver->getU(i,j+1,k) + m_solver->getU(i,j,k-1) + m_solver->getU(i,j,k+1))/7.0);
  //   pos->addY( (m_solver->getV(i,j,k) + m_solver->getV(i-1,j,k) + m_solver->getV(i+1,j,k) + m_solver->getV(i,j-1,k) + m_solver->getV(i,j+1,k) + m_solver->getV(i,j,k-1) + m_solver->getV(i,j,k+1))/7.0);
  //   pos->addZ( (m_solver->getW(i,j,k) + m_solver->getW(i-1,j,k) + m_solver->getW(i+1,j,k) + m_solver->getW(i,j-1,k) + m_solver->getW(i,j+1,k) + m_solver->getW(i,j,k-1) + m_solver->getW(i,j,k+1))/7.0);

  pos->addX (m_solver->getU (i, j, k));
  pos->addY (m_solver->getV (i, j, k));
  pos->addZ (m_solver->getW (i, j, k));

  if (pos->getX () < -m_solver->getDimX() / 2.0 || pos->getX () > m_solver->getDimX() / 2.0
      || pos->getY () < 0 || pos->getY () > m_solver->getDimY()
      || pos->getZ () < -m_solver->getDimZ() / 2.0 || pos->getZ () > m_solver->getDimZ() / 2.0)
    return 0;
  
  return 1;
}

/* Gère la création et la destruction des particules */
void
LeadSkeleton::move ()
{
  Particle *tmp;/* *tmp2;*/
  double dist;
  int i;

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

      if (moveParticle
	  (tmp))
	{
	  updateParticle (i, tmp);

	  /* Si on */
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

void LeadSkeleton::drawParticle (Particle * const particle)
{
  CPoint position (m_flamePos + *particle);

  glColor4f (0.1, 0.1, 0.1, 0.8);
  glPushMatrix ();
  glTranslatef (position.getX (), position.getY (),
		position.getZ ());
  GraphicsFn::SolidSphere (0.01, 10, 10);
  glPopMatrix ();
}

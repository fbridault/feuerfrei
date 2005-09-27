#include "periSkeleton.hpp"

#include <stdlib.h>
#include <math.h>

#include "graphicsFn.hpp"
#include "solver.hpp"


PeriSkeleton::PeriSkeleton (Solver * const s, const CPoint position,
			    const CPoint pt, LeadSkeleton * guide):
  Skeleton (s, position, pt)
{
  this->guide = guide;
}

PeriSkeleton::PeriSkeleton (Solver * const s, const CPoint position,
			    const CPoint pt, LeadSkeleton * guide, int pls):
  Skeleton (s, position, pt, pls)
{
  this->guide = guide;
}

PeriSkeleton::~PeriSkeleton ()
{
}

int
PeriSkeleton::move_origine ()
{
  int i, j, k;
  CPoint tmp;
  float distx = 10 * dim_x / (float) solveur->getX ();
  float distz = dim_y / (float) solveur->getZ ();
  
  /* Retrouver les quatres cellules adjacentes autour de la particule */
  i = (int) (origine.getX () * dim_x * solveur->getX ()) + 1 +
    solveur->getX () / 2;
  j = (int) (origine.getY () * dim_y * solveur->getY ()) + 1;
  k = (int) (origine.getZ () * dim_z * solveur->getZ ()) + 1 +
    solveur->getZ () / 2;

  /* Calculer la nouvelle position */
  /* Intégration d'Euler */

  // cout << pos->x << " " << pos->y << " " << pos->z << "     ";
  //   cout << i << " " << j << " " << k << endl;

  //tmp.setX( origine_save.getX() + 4*(solveur->getU(i,j,k) + solveur->getU(i-1,j,k) + solveur->getU(i+1,j,k) + solveur->getU(i,j-1,k) + solveur->getU(i,j+1,k)+ solveur->getU(i,j,k-1) + solveur->getU(i,j,k+1))/7.0 );
  //tmp.setY( origine_save.getY() + 0.75*(solveur->getV(i,j,k) + solveur->getV(i-1,j,k) + solveur->getV(i+1,j,k) + solveur->getV(i,j-1,k) + solveur->getV(i,j+1,k)+ solveur->getV(i,j,k-1) + solveur->getV(i,j,k+1))/7.0 );
  //tmp.setZ( origine_save.getZ() + 4*(solveur->getW(i,j,k) + solveur->getW(i-1,j,k) + solveur->getW(i+1,j,k) + solveur->getW(i,j-1,k) + solveur->getW(i,j+1,k)+ solveur->getW(i,j,k-1) + solveur->getW(i,j,k+1))/7.0 );
	
#ifdef BOUGIE
  tmp.setX (origine_save.getX () + 4 * solveur->getU (i, j, k));
  tmp.setY (origine_save.getY () + 0.75 * solveur->getV (i, j, k));
  tmp.setZ (origine_save.getZ () + 4 * solveur->getW (i, j, k));
#else	
  tmp.setX (origine_save.getX () + 2 * solveur->getU (i, j, k));
  tmp.setY (origine_save.getY () + .1 * solveur->getV (i, j, k));
  tmp.setZ (origine_save.getZ () + .5*solveur->getW (i, j, k));
#endif
  if (tmp.getX () < (origine_save.getX () - distx)
      || tmp.getX () > (origine_save.getX () + distx)
      || tmp.getZ () < (origine_save.getZ () - distz)
      || tmp.getZ () > (origine_save.getZ () + distz))
    return 0;

  origine = tmp;

  return 1;
}

int
PeriSkeleton::move_particle (Particle * const pos, int n)
{
  int i, j, k;
  //  int light;

  if (pos->isDead ())
    return 0;

  /* Retrouver les quatres cellules adjacentes autour de la particule */
  i = (int) (pos->getX () * dim_x * solveur->getX ()) + 1 +
    solveur->getX () / 2;
  j = (int) (pos->getY () * dim_y * solveur->getY ()) + 1;
  k = (int) (pos->getZ () * dim_z * solveur->getZ ()) + 1 +
    solveur->getZ () / 2;

  /* Calculer la nouvelle position */
  /* Intégration d'Euler */

  // cout << pos->x << " " << pos->y << " " << pos->z << "     ";
  //   cout << i << " " << j << " " << k << endl;
  //pos->addX( (solveur->getU(i,j,k) + solveur->getU(i-1,j,k) + solveur->getU(i+1,j,k) + solveur->getU(i,j-1,k) + solveur->getU(i,j+1,k) + solveur->getU(i,j,k-1) + solveur->getU(i,j,k+1))/7.0);
  //pos->addY( (solveur->getV(i,j,k) + solveur->getV(i-1,j,k) + solveur->getV(i+1,j,k) + solveur->getV(i,j-1,k) + solveur->getV(i,j+1,k) + solveur->getV(i,j,k-1) + solveur->getV(i,j,k+1))/7.0);
  //pos->addZ( (solveur->getW(i,j,k) + solveur->getW(i-1,j,k) + solveur->getW(i+1,j,k) + solveur->getW(i,j-1,k) + solveur->getW(i,j+1,k) + solveur->getW(i,j,k-1) + solveur->getW(i,j,k+1))/7.0);

  pos->addX (solveur->getU (i, j, k));
  pos->addY (solveur->getV (i, j, k));
  pos->addZ (solveur->getW (i, j, k));

  if (pos->getX () < -dim_x / 2.0 || pos->getX () > dim_x / 2.0
      || pos->getY () < 0 || pos->getY () > dim_y
      || pos->getZ () < -dim_z / 2.0 || pos->getZ () > dim_z / 2.0)
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

  move_origine ();

  if (getSize () < NB_PARTICULES - 1)
    {
      /* On détermine s'il faut lâcher une nouvelle particule */
      /* On calcule la distance entre la dernière particule lâchée et l'origine */
      //tmp = getLastElt ();
      //dist = origine.squaredDistanceFrom (tmp);
      /* On laisse la distance au carré pour éviter un calcul de racine carré coûteux */
      //if (dist > .0025)
      entree (&origine);
    }

  /* Affichage des particules */
  /* Boucle de parcours : du haut vers le bas */
  for (i = 0; i < getSize (); i++)
    {
      tmp = getElt (i);

      if (move_particle (tmp, i))
	{
	  setEltFile (i, tmp);

	  // /* Si on */
// 	  if (i < getSize () - 1)
// 	    {
// 	      tmp2 = getElt (i + 1);

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
// 		  sortie (i);
// 		  i--;
// 		}
// 	    }
	}
      else
	{
	  sortie (i);
	  if (i < getSize () - 1)
	    i--;
	}
    }			/* for */

}

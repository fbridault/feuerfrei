#if !defined(HSOLVER_H)
#define HSOLVER_H

class Hsolver;

#include "GSsolver.hpp"

/** La classe Solver propose une implémentation 3D de la méthode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre modèle de flamme, le champ de densité n'est pas utilisé, ce qui,
 * physiquement semble assez curieux (d'après Alexei). Néanmoins Stam présente bien les choses ainsi dans
 * son code initial, la densité dépend de la vélocité, la vélocité évolue indépendament.
 * 
 * @author	Flavien Bridault
 */
class Hsolver : public GSsolver
{
public:
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilisé pour la simulation
   */
  Hsolver (Point& position, int n_x, int n_y, int n_z, double dim, double pas_de_temps);
  virtual ~Hsolver ();
  
protected:  
  /** Pas de diffusion avec hybridation avec relance de la méthode de Gauss-Seidel
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param x composante à traiter
   * @param x0 composante x au pas de temps précédent
   * @param residu residu de la composante à traiter
   * @param residu0 residu au pas de temps précédent
   * @param diff_visc paramètre correspondant soit à la diffusion si la fonction est utilisée pour
   * la résolution du pas de densité, soit à la viscosité si elle est employée pour la résolution
   * du pas de vélocité
   */
  void diffuse (int b, double *const x, const double *const x0,
			double *const residu, double *const residu0,
			double diff_visc);
    
  /** Pas de résolution de la vélocité, en utilisant l'hybridation */
  virtual void vel_step ();
  
  double *m_uResidu, *m_uPrevResidu, *m_vResidu, *m_vPrevResidu, *m_wResidu, *m_wPrevResidu;  
};

#endif

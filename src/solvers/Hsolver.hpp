#if !defined(HSOLVER_H)
#define HSOLVER_H

class Hsolver;

#include "GSsolver.hpp"

/** La classe Solver propose une impl�mentation 3D de la m�thode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre mod�le de flamme, le champ de densit� n'est pas utilis�, ce qui,
 * physiquement semble assez curieux (d'apr�s Alexei). N�anmoins Stam pr�sente bien les choses ainsi dans
 * son code initial, la densit� d�pend de la v�locit�, la v�locit� �volue ind�pendament.
 * 
 * @author	Flavien Bridault
 */
class Hsolver : public GSsolver
{
public:
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilis� pour la simulation
   */
  Hsolver (Point& position, int n_x, int n_y, int n_z, double dim, double pas_de_temps);
  virtual ~Hsolver ();
  
protected:  
  /** Pas de diffusion avec hybridation avec relance de la m�thode de Gauss-Seidel
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param x composante � traiter
   * @param x0 composante x au pas de temps pr�c�dent
   * @param residu residu de la composante � traiter
   * @param residu0 residu au pas de temps pr�c�dent
   * @param diff_visc param�tre correspondant soit � la diffusion si la fonction est utilis�e pour
   * la r�solution du pas de densit�, soit � la viscosit� si elle est employ�e pour la r�solution
   * du pas de v�locit�
   */
  void diffuse (int b, double *const x, const double *const x0,
			double *const residu, double *const residu0,
			double diff_visc);
    
  /** Pas de r�solution de la v�locit�, en utilisant l'hybridation */
  virtual void vel_step ();
  
  double *m_uResidu, *m_uPrevResidu, *m_vResidu, *m_vPrevResidu, *m_wResidu, *m_wPrevResidu;  
};

#endif

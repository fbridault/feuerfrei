#if !defined(HSOLVER_H)
#define HSOLVER_H

class Hsolver;

#include "GSsolver.hpp"

/** @deprecated La classe GCSSSolver propose d'utiliser la méthode d'hybridation comme
 * méthode de resolution des systèmes linéaires. La résolution ne marche pas correctement
 * mais de toute façon, cette méthode ne présente à priori plus d'intérêt.
 *
 * @author	Flavien Bridault
 */
class Hsolver : public GSsolver
{
public:
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x Résolution de la grille en x.
   * @param n_y Résolution de la grille en y.
   * @param n_z Résolution de la grille en z.
   * @param dim Dimension du solveur, utilisé pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilisé pour la simulation.
   */
  Hsolver (Point& position, uint n_x, uint n_y, uint n_z, double dim, double timeStep);
  /** Destructeur. */
  virtual ~Hsolver ();
  
protected:
  void diffuse (unsigned char b, double *const x, const double *const x0, double *const residu, 
		double *const residu0, double diff_visc);  
  virtual void vel_step ();
  
  double *m_uResidu, *m_uPrevResidu, *m_vResidu, *m_vPrevResidu, *m_wResidu, *m_wPrevResidu;  
};

#endif

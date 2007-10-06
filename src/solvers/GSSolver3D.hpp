#if !defined(GSSOLVER3D_H)
#define GSSOLVER3D_H

class GSSolver3D;

#include "solver3D.hpp"

/** La classe GCSSSolver propose d'utiliser la méthode de Gauss-Seidel comme
 * méthode de resolution des systèmes linéaires.
 * 
 * @author	Flavien Bridault et Michel Leblond
 */
class GSSolver3D : public virtual Solver3D
{
public:
  /** Constructeur par défaut nécessaire pour l'héritage multiple */
  GSSolver3D ();
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x Résolution de la grille en x.
   * @param n_y Résolution de la grille en y.
   * @param n_z Résolution de la grille en z.
   * @param dim Dimension du solveur, utilisé pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilisé pour la simulation.
   * @param buoyancy Intensité de la force de flottabilité dans le solveur.
   */
  GSSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, 
	      float timeStep, float buoyancy, float vorticityConfinement);
  /** Desctructeur. */
  virtual ~GSSolver3D ();
  
protected:
  /** Effectue une résolution des systèmes linéaires de la diffusion
   * et de la projection à l'aide de la méthode itérative de Gauss-Seidel.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param x Composante à traiter
   * @param x0 Composante x au pas de temps précédent
   * @param a Valeur des coefficients dans la matrice A
   * @param div Fraction des coefficients sur la diagonale de la matrice A ( 1/(1+6a) pour la diffusion
   * et 1/6 pour la projection
   * @param nb_steps Nombre d'itérations à effectuer
   */
  virtual void GS_solve(unsigned char b, float *const x, const float *const x0, float a, float div, uint nb_steps);
  
  virtual void diffuse (unsigned char b, float *const x, float *const x0, float a);
  virtual void project (float *const p, float *const div);
  
  float *m_res;
};

#endif

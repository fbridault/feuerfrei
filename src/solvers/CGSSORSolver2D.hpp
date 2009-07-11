#if !defined(CGSSORSOLVER2D_H)
#define CGSSORSOLVER2D_H

class CGSSORsolver2D;

#include "solver2D.hpp"

/** La classe CGSSORolver propose d'utiliser la méthode du CGSSOR comme
 * méthode de resolution des systèmes linéaires.
 *
 * @author	Flavien Bridault et Michel Leblond
 */
class CGSSORSolver2D : public virtual Solver2D
{
public:
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x Résolution de la grille en x.
   * @param n_y Résolution de la grille en y.
   * @param dim Dimension du solveur, utilisé pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilisé pour la simulation.
   * @param buoyancy Intensité de la force de flottabilité dans le solveur.
   */
  CGSSORSolver2D (CTransform& a_rTransform, uint n_x, uint n_y, float dim, float timeStep, float buoyancy,
		  float vorticityConfinement, float omegaDiff, float omegaProj, float epsilon);
  /** Desctructeur. */
  virtual ~CGSSORSolver2D ();

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
  void CGSSOR(float *x0, float *b, float a, float diagonal, float omega, int maxiter);

  virtual void diffuse (unsigned char b, float *const x, float *const x0, float a);
  virtual void project (float *const p, float *const div);

  /** Résidu, pour SSOR, direction de descente et ? */
  float *m_r, *m_z, *m_p, *m_q;

  /** Paramètre omega pour la diffusion */
  float m_omegaDiff;
  /** Paramètre omega pour la projection */
  float m_omegaProj;
  /** Tolérance d'erreur pour GCSSOR. */
  float m_epsilon;
};

#endif

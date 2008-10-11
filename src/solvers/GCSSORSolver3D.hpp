#if !defined(GCSSORSOLVER3D_H)
#define GCSSORSOLVER3D_H

class GCSSORSolver3D;

#include "solver3D.hpp"

/** La classe GCSSORSolver propose d'utiliser la méthode du gradient conjugué préconditionné comme
 * méthode de resolution des systèmes linéaires.
 *
 * @author	Flavien Bridault et Michel Leblond
 */
class GCSSORSolver3D : public virtual Solver3D
{
public:
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x Résolution de la grille en x.
   * @param n_y Résolution de la grille en y.
   * @param n_z Résolution de la grille en z.
   * @param dim Dimension du solveur, utilisé pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilisé pour la simulation.
   * @param buoyancy Intensité de la force de flottabilité dans le solveur.
   * @param omegaDiff Paramètre omega pour la diffusion.
   * @param omegaProj Paramètre omega pour la projection.
   * @param epsilon Tolérance d'erreur pour GCSSOR.
   */
  GCSSORSolver3D (const CPoint& position, uint n_x, uint n_y, uint n_z, float dim, const CPoint& scale, float timeStep,
		  float buoyancy, float vorticityConfinement, float omegaDiff, float omegaProj, float epsilon);

  /** Constructeur nécessaire pour l'héritage multiple.
   * @param omegaDiff Paramètre omega pour la diffusion.
   * @param omegaProj Paramètre omega pour la projection.
   * @param epsilon Tolérance d'erreur pour GCSSOR.
   */
  GCSSORSolver3D (float omegaDiff, float omegaProj, float epsilon);
  /** Destructeur. */
  virtual ~GCSSORSolver3D ();

protected:
  /** Effectue une résolution des systèmes linéaires de la diffusion
  * et de la projection à l'aide de la méthode du Gradient Conjugué
  * @param x0 Composante x au pas de temps précédent
  * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
  * @param a Valeur des coefficients dans la matrice A
  * @param diagonal Valeur du coefficient sur la diagonale de la matrice A : ( 1/(1+6a) pour la diffusion
  * et 1/6 pour la projection
  * @param omega Paramètre omega.
  * @param maxiter Nombre d'itérations maximal à effectuer
  */
  virtual void GCSSOR(float *const x0, const float *const b, float a, float diagonal, float omega, uint maxiter);

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

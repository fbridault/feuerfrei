#if !defined(CGSSORSOLVER2D_H)
#define CGSSORSOLVER2D_H

class CGSSORsolver2D;

#include "solver2D.hpp"

/** La classe CGSSORolver propose d'utiliser la m�thode du CGSSOR comme
 * m�thode de resolution des syst�mes lin�aires.
 *
 * @author	Flavien Bridault et Michel Leblond
 */
class CGSSORSolver2D : public virtual Solver2D
{
public:
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x R�solution de la grille en x.
   * @param n_y R�solution de la grille en y.
   * @param dim Dimension du solveur, utilis� pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilis� pour la simulation.
   * @param buoyancy Intensit� de la force de flottabilit� dans le solveur.
   */
  CGSSORSolver2D (CTransform& a_rTransform, uint n_x, uint n_y, float dim, float timeStep, float buoyancy,
		  float vorticityConfinement, float omegaDiff, float omegaProj, float epsilon);
  /** Desctructeur. */
  virtual ~CGSSORSolver2D ();

protected:
  /** Effectue une r�solution des syst�mes lin�aires de la diffusion
   * et de la projection � l'aide de la m�thode it�rative de Gauss-Seidel.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param x Composante � traiter
   * @param x0 Composante x au pas de temps pr�c�dent
   * @param a Valeur des coefficients dans la matrice A
   * @param div Fraction des coefficients sur la diagonale de la matrice A ( 1/(1+6a) pour la diffusion
   * et 1/6 pour la projection
   * @param nb_steps Nombre d'it�rations � effectuer
   */
  void CGSSOR(float *x0, float *b, float a, float diagonal, float omega, int maxiter);

  virtual void diffuse (unsigned char b, float *const x, float *const x0, float a);
  virtual void project (float *const p, float *const div);

  /** R�sidu, pour SSOR, direction de descente et ? */
  float *m_r, *m_z, *m_p, *m_q;

  /** Param�tre omega pour la diffusion */
  float m_omegaDiff;
  /** Param�tre omega pour la projection */
  float m_omegaProj;
  /** Tol�rance d'erreur pour GCSSOR. */
  float m_epsilon;
};

#endif

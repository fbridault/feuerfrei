#if !defined(GCSSORSOLVER3D_H)
#define GCSSORSOLVER3D_H

class GCSSORSolver3D;

#include "solver3D.hpp"

/** La classe GCSSORSolver propose d'utiliser la m�thode du gradient conjugu� pr�conditionn� comme
 * m�thode de resolution des syst�mes lin�aires.
 *
 * @author	Flavien Bridault et Michel Leblond
 */
class GCSSORSolver3D : public virtual Solver3D
{
public:
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x R�solution de la grille en x.
   * @param n_y R�solution de la grille en y.
   * @param n_z R�solution de la grille en z.
   * @param dim Dimension du solveur, utilis� pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilis� pour la simulation.
   * @param buoyancy Intensit� de la force de flottabilit� dans le solveur.
   * @param omegaDiff Param�tre omega pour la diffusion.
   * @param omegaProj Param�tre omega pour la projection.
   * @param epsilon Tol�rance d'erreur pour GCSSOR.
   */
  GCSSORSolver3D (const CPoint& position, uint n_x, uint n_y, uint n_z, float dim, const CPoint& scale, float timeStep,
		  float buoyancy, float vorticityConfinement, float omegaDiff, float omegaProj, float epsilon);

  /** Constructeur n�cessaire pour l'h�ritage multiple.
   * @param omegaDiff Param�tre omega pour la diffusion.
   * @param omegaProj Param�tre omega pour la projection.
   * @param epsilon Tol�rance d'erreur pour GCSSOR.
   */
  GCSSORSolver3D (float omegaDiff, float omegaProj, float epsilon);
  /** Destructeur. */
  virtual ~GCSSORSolver3D ();

protected:
  /** Effectue une r�solution des syst�mes lin�aires de la diffusion
  * et de la projection � l'aide de la m�thode du Gradient Conjugu�
  * @param x0 Composante x au pas de temps pr�c�dent
  * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
  * @param a Valeur des coefficients dans la matrice A
  * @param diagonal Valeur du coefficient sur la diagonale de la matrice A : ( 1/(1+6a) pour la diffusion
  * et 1/6 pour la projection
  * @param omega Param�tre omega.
  * @param maxiter Nombre d'it�rations maximal � effectuer
  */
  virtual void GCSSOR(float *const x0, const float *const b, float a, float diagonal, float omega, uint maxiter);

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

#if !defined(GSSOLVER2D_H)
#define GSSOLVER2D_H

class GSsolver2D;

#include "solver2D.hpp"

/** La classe GSolver propose d'utiliser la m�thode de Gauss-Seidel comme
 * m�thode de resolution des syst�mes lin�aires.
 *
 * @author	Flavien Bridault et Michel Leblond
 */
class GSSolver2D : public virtual Solver2D
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
  GSSolver2D (CTransform& a_rTransform, uint n_x, uint n_y, float dim, float timeStep, float buoyancy, float vorticityConfinement);
  /** Desctructeur. */
  virtual ~GSSolver2D ();

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
  virtual void GS_solve(unsigned char b, float *const x, const float *const x0, float a, float div, uint nb_steps);

  virtual void diffuse (unsigned char b, float *const x, float *const x0, float a);
  virtual void project (float *const p, float *const div);
};

#endif

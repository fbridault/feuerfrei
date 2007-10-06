#if !defined(GSSOLVER3D_H)
#define GSSOLVER3D_H

class GSSolver3D;

#include "solver3D.hpp"

/** La classe GCSSSolver propose d'utiliser la m�thode de Gauss-Seidel comme
 * m�thode de resolution des syst�mes lin�aires.
 * 
 * @author	Flavien Bridault et Michel Leblond
 */
class GSSolver3D : public virtual Solver3D
{
public:
  /** Constructeur par d�faut n�cessaire pour l'h�ritage multiple */
  GSSolver3D ();
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x R�solution de la grille en x.
   * @param n_y R�solution de la grille en y.
   * @param n_z R�solution de la grille en z.
   * @param dim Dimension du solveur, utilis� pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilis� pour la simulation.
   * @param buoyancy Intensit� de la force de flottabilit� dans le solveur.
   */
  GSSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, 
	      float timeStep, float buoyancy, float vorticityConfinement);
  /** Desctructeur. */
  virtual ~GSSolver3D ();
  
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
  
  float *m_res;
};

#endif

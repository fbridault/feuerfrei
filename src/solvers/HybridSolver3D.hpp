#if !defined(HYBRIDSOLVER3D_H)
#define HYBRIDSOLVER3D_H

class BenchSolver3D;

#include "GSSolver3D.hpp"
#include "GCSSORSolver3D.hpp"

#include <fstream>

class HybridSolver3D;

/** La classe HybridSolver sert d'interface pour les classes permettant de loguer les valeurs de r�sidus
 * des solveurs � base des m�thodes it�ratives de Gauss-Seidel et du gradient conjugu� pr�conditionn� avec SSOR.
 * Elle h�rite de GSSolver3D et GCSSORSolver3D qui tous deux h�ritent <b>virtuellement</b> de Solver
 * de mani�re � n'avoir qu'une seule instance de Solver.
 *
 * @author	Flavien Bridault
 */
class HybridSolver3D: public GCSSORSolver3D, public GSSolver3D
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
  HybridSolver3D (	CTransform& a_rTransform, uint n_x, uint n_y, uint n_z, float dim, float timeStep,
					float buoyancy, float vorticityConfinement, float omegaDiff, float omegaProj, float epsilon);

  /** Constructeur n�cessaire pour l'h�ritage multiple.
   * @param omegaDiff Param�tre omega pour la diffusion.
   * @param omegaProj Param�tre omega pour la projection.
   * @param epsilon Tol�rance d'erreur pour GCSSOR.
   */
  HybridSolver3D (float omegaDiff, float omegaProj, float epsilon);

  /** Desctructeur. */
  virtual ~ HybridSolver3D ();

protected:
  virtual void diffuse (unsigned char b, float *const x, float *const x0, float a);
  virtual void project (float *const p, float *const div);
};

#endif

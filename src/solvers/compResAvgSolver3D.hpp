#if !defined(COMPRESAVGSOLVER_H)
#define COMPRESAVGSOLVER_H

class CompResAvgSolver3D;

#include "logResAvgSolver3D.hpp"

#include <fstream>

class LogResAvgSolver3D;

/** La classe CompResAvgSolver3D permet de loguer les moyennes des valeurs de r�sidus des solveurs � base
 * des m�thodes it�ratives de Gauss-Seidel et du gradient conjugu� pr�conditionn� avec SSOR.
 *
 * @author	Flavien Bridault
 */
class CompResAvgSolver3D: public LogResAvgSolver3D
{
public:
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x R�solution de la grille en x.
   * @param n_y R�solution de la grille en y.
   * @param n_z R�solution de la grille en z.
   * @param dim Dimension du solveur, utilis� pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilis� pour la simulation.
   * @param nbTimeSteps Nombre de pas de temps de la simulation.
   * @param buoyancy Intensit� de la force de flottabilit� dans le solveur.
   * @param omegaDiff Param�tre omega pour la diffusion.
   * @param omegaProj Param�tre omega pour la projection.
   * @param epsilon Tol�rance d'erreur pour GCSSOR.
   */
  CompResAvgSolver3D (const CPoint& position, uint n_x, uint n_y, uint n_z, float dim, const CPoint& scale, float timeStep,
		     float buoyancy, float vorticityConfinement, float omegaDiff, float omegaProj,
		     float epsilon, uint nbTimeSteps);

  /** Constructeur n�cessaire pour l'h�ritage multiple.
   * @param nbTimeSteps Nombre de pas de temps de la simulation.
   * @param omegaDiff Param�tre omega pour la diffusion.
   * @param omegaProj Param�tre omega pour la projection.
   * @param epsilon Tol�rance d'erreur pour GCSSOR.
   */
  CompResAvgSolver3D (uint nbTimeSteps, float omegaDiff, float omegaProj, float epsilon);
  /** Destructeur. */
  virtual ~CompResAvgSolver3D ();

protected:
  virtual void vel_step ();

  virtual void diffuse (unsigned char b, float *const x, float *const x0, float a);
  virtual void project (float *const p, float *const div);
};

#endif

#if !defined(COMPRESAVGSOLVER_H)
#define COMPRESAVGSOLVER_H

class CompResAvgSolver3D;

#include "logResAvgSolver3D.hpp"

#include <fstream>

class LogResAvgSolver3D;

/** La classe CompResAvgSolver3D permet de loguer les moyennes des valeurs de résidus des solveurs à base
 * des méthodes itératives de Gauss-Seidel et du gradient conjugué préconditionné avec SSOR.
 *
 * @author	Flavien Bridault
 */
class CompResAvgSolver3D: public LogResAvgSolver3D
{
public:
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x Résolution de la grille en x.
   * @param n_y Résolution de la grille en y.
   * @param n_z Résolution de la grille en z.
   * @param dim Dimension du solveur, utilisé pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilisé pour la simulation.
   * @param nbTimeSteps Nombre de pas de temps de la simulation.
   * @param buoyancy Intensité de la force de flottabilité dans le solveur.
   * @param omegaDiff Paramètre omega pour la diffusion.
   * @param omegaProj Paramètre omega pour la projection.
   * @param epsilon Tolérance d'erreur pour GCSSOR.
   */
  CompResAvgSolver3D (const CPoint& position, uint n_x, uint n_y, uint n_z, float dim, const CPoint& scale, float timeStep,
		     float buoyancy, float vorticityConfinement, float omegaDiff, float omegaProj,
		     float epsilon, uint nbTimeSteps);

  /** Constructeur nécessaire pour l'héritage multiple.
   * @param nbTimeSteps Nombre de pas de temps de la simulation.
   * @param omegaDiff Paramètre omega pour la diffusion.
   * @param omegaProj Paramètre omega pour la projection.
   * @param epsilon Tolérance d'erreur pour GCSSOR.
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

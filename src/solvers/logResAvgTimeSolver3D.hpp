#if !defined(LOGRESAVGTIMESOLVER_H)
#define LOGRESAVGTIMESOLVER_H

class LogResAvgTimeSolver3D;

#include "logResAvgSolver3D.hpp"

#include <fstream>

class LogResAvgSolver3D;

/** La classe LogResAvgTimeSolver3D permet de loguer les moyennes des valeurs de résidus des solveurs à base
 * des méthodes itératives de Gauss-Seidel et du gradient conjugué préconditionné avec SSOR. A la différence
 * de LogResAvgSolver3D, elle stocke les moyennes des valeurs de résidus en fonction du temps
 *
 * @author	Flavien Bridault
 */
class LogResAvgTimeSolver3D: public LogResAvgSolver3D
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
  LogResAvgTimeSolver3D (const CPoint& position, uint n_x, uint n_y, uint n_z, float dim, const CPoint& scale,
			 float timeStep, float buoyancy, float vorticityConfinement,
			 float omegaDiff, float omegaProj, float epsilon, uint nbTimeSteps);
  /** Destructeur. */
  virtual ~LogResAvgTimeSolver3D ();

private:
  void vel_step ();

  void GS_solve(unsigned char b, float *const x, const float *const x0, float a, float div, uint nb_steps);
  void GCSSOR(float *const x0, const float *const b, float a, float diagonal, float omega, uint maxiter);

  void computeAverage ( uint iter, float value, float time );

  /** Tableau contenant les moyennes des temps. Il est organisé comme ceci :<br>
   *    - de O à m_nbSteps : diffusion en u avec GS
   *    - de m_nbSteps à 2*m_nbSteps : diffusion en v avec GS
   *    - de 2*m_nbSteps à 3*m_nbSteps : diffusion en w avec GS
   *    - de 3*m_nbSteps à 4*m_nbSteps : diffusion en u avec GCSSOR
   *    - de 4*m_nbSteps à 5*m_nbSteps : diffusion en v avec GCSSOR
   *    - de 5*m_nbSteps à 6*m_nbSteps : diffusion en w avec GCSSOR
   *    - de 6*m_nbSteps à 7*m_nbSteps :  projection 1 avec GS
   *    - de 7*m_nbSteps à 8*m_nbSteps :  projection 1 avec GS
   *    - de 8*m_nbSteps à 9*m_nbSteps :  projection 2 avec GCSSOR
   *    - de 9*m_nbSteps à 10*m_nbSteps : projection 2 avec GCSSOR
   */
  float *m_times;
};

#endif

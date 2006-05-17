#if !defined(LOGRESAVGTIMESOLVER_H)
#define LOGRESAVGTIMESOLVER_H

class LogResAvgTimeSolver;

#include "logResAvgSolver.hpp"

#include <fstream>

class LogResAvgSolver;

/** La classe LogResAvgTimeSolver permet de loguer les moyennes des valeurs de résidus des solveurs à base
 * des méthodes itératives de Gauss-Seidel et du gradient conjugué préconditionné avec SSOR. A la différence
 * de LogResAvgSolver, elle stocke les moyennes des valeurs de résidus en fonction du temps
 *
 * @author	Flavien Bridault
 */
class LogResAvgTimeSolver: public LogResAvgSolver
{
public:
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilisé pour la simulation
   */
  LogResAvgTimeSolver (Point& position, uint n_x, uint n_y, uint n_z, double dim, 
		       double timeStep, uint nbTimeSteps, double buoyancy, double omegaDiff, double omegaProj, double epsilon);
  virtual ~LogResAvgTimeSolver ();
  
private:
  void vel_step ();
  
  void GS_solve(unsigned char b, double *const x, double *const x0, double a, double div, uint nb_steps);
  void GCSSOR(double *const x0, const double *const b, double a, double diagonal, double omega, uint maxiter);
  
  void computeAverage ( uint iter, double value, double time );
  
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
  double *m_times;
};

#endif

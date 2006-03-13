#if !defined(LOGRESAVGSOLVER_H)
#define LOGRESAVGSOLVER_H

class LogResAvgSolver;

#include "benchsolver.hpp"

#include <fstream>

class BenchSolver;

/** La classe LogResAvgSolver permet de loguer les moyennes des valeurs de résidus des solveurs à base
 * des méthodes itératives de Gauss-Seidel et du gradient conjugué préconditionné avec SSOR.
 *
 * @author	Flavien Bridault
 */
class LogResAvgSolver: public BenchSolver
{
public:
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilisé pour la simulation
   */
  LogResAvgSolver (Point& position, int n_x, int n_y, int n_z, double dim, double timeStep, double buoyancy, double nbTimeSteps,
		   double omegaDiff, double omegaProj, double epsilon);
  LogResAvgSolver (double nbTimeSteps, double omegaDiff, double omegaProj, double epsilon);
  virtual ~LogResAvgSolver ();

protected:
  virtual void vel_step ();
  
  virtual void diffuse (int b, double *const x, double *const x0, double a, double diff_visc);
  virtual void project (double *const p, double *const div);
  
  virtual void GS_solve(int b, double *const x, double *const x0, double a, double div, double nb_steps);
  virtual void GCSSOR(double *const x0, const double *const b, double a, double diagonal, double omega, int maxiter);
  
  virtual void computeAverage ( int iter, double value );
  
  int m_nbAverages;
  /** Tableau contenant les moyennes. Il est organisé comme ceci :<br>
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
  double *m_averages;
  
  ofstream m_file;
  
  
};

#endif

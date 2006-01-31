#if !defined(LOGRESSOLVER_H)
#define LOGRESSOLVER_H

class LogResSolver;

#include "benchsolver.hpp"

#include <fstream>

class BenchSolver;

/** La classe BenchSolver permet de loguer les valeurs de résidus des solveurs à base
 * des méthodes itératives de Gauss-Seidel et du gradient conjugué préconditionné avec SSOR.
 *
 * @author	Flavien Bridault
 */
class LogResSolver: public BenchSolver
{
public:
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilisé pour la simulation
   */
  LogResSolver (CPoint& position, int n_x, int n_y, int n_z, double dim, double timeStep, double nbTimeSteps,
		double buoyancy, double omegaDiff, double omegaProj, double epsilon);
  virtual ~LogResSolver ();
  
private:
  void vel_step ();
  
  void diffuse (int b, double *const x, double *const x0, double a, double diff_visc);
  void project (double *const p, double *const div);
  
  void GS_solve(int b, double *const x, double *const x0, double a, double div, double nb_steps);
  void GCSSOR(double *const x0, const double *const b, double a, double diagonal, double omega, int maxiter);
  
  void logResidu (int iter, double value);
  
  ofstream m_fileDiff[NB_DIFF_LOGS];
  ofstream m_fileProj[NB_PROJ_LOGS];
  /* Pointeur sur le fichier courant */
  ofstream *m_file;
};

#endif

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
  LogResSolver (Point& position, uint n_x, uint n_y, uint n_z, double dim, double timeStep, 
		uint nbTimeSteps, double buoyancy, double omegaDiff, double omegaProj, double epsilon);
  virtual ~LogResSolver ();
  
private:
  void vel_step ();
  
  void diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc);
  void project (double *const p, double *const div);
  
  void GS_solve(unsigned char b, double *const x, double *const x0, double a, double div, uint nb_steps);
  void GCSSOR(double *const x0, const double *const b, double a, double diagonal, double omega, uint maxiter);
  
  void logResidu (uint iter, double value);
  
  ofstream m_fileDiff[NB_DIFF_LOGS];
  ofstream m_fileProj[NB_PROJ_LOGS];
  /* Pointeur sur le fichier courant */
  ofstream *m_file;
};

#endif

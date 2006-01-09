#if !defined(BENCHSOLVER_H)
#define BENCHSOLVER_H

class BenchSolver;

#include "GSsolver.hpp"
#include "GCSSORsolver.hpp"

#include <fstream>

class GSsolver;
class GCSSORsolver;

/** La classe BenchSolver permet de loguer les valeurs de r�sidus des solveurs � base
 * des m�thodes it�ratives de Gauss-Seidel et du gradient conjugu� pr�conditionn� avec SSOR.
 * Elle h�rite de GSsolver et GCSSORsolver qui tous deux h�ritent <b>virtuellement</b> de Solver
 * de mani�re � n'avoir qu'une seule instance de Solver.
 *
 * @author	Flavien Bridault
 */
class BenchSolver: public GSsolver, public GCSSORsolver
{
public:
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilis� pour la simulation
   */
  BenchSolver (CPoint& position, int n_x, int n_y, int n_z, double dim, double timeStep, double nbTimeSteps,
	       double omegaDiff, double omegaProj, double epsilon);
  virtual ~ BenchSolver ();
  
private:
  void vel_step ();
  
  void diffuse (int b, double *const x, double *const x0, double a, double diff_visc);
  void project (double *const p, double *const div);
  
  void GS_solve(int b, double *const x, double *const x0, double a, double div, double nb_steps);
  void GCSSOR(double *const x0, const double *const b, double a, double diagonal, double omega);
  
  void saveState (const double *const x, const double *const x2);
  /** Sauvegarde les diff�rentes composantes du solveur comme valeur de r�f�rence */
  //void save_ref (const double *const x, double *x_ref);
  void setPreviousState (double *const x, double *const x2);
  
  void logResidu (double value);
  
  ofstream m_fileDiff[6];
  ofstream m_fileProj[4];
  /* Pointeur sur le fichier courant */
  ofstream *m_file;

  double *m_save, *m_save2;

  short m_index;
  
  /* Nombre maximum de pas de temps � simuler */
  int m_nbMaxIter;
};

#endif

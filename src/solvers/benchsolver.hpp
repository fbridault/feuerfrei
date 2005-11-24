#if !defined(BENCHSOLVER_H)
#define BENCHSOLVER_H

class BenchSolver;

#include "solver.hpp"
#include <fstream>

class Solver;

/** La classe Solver propose une implémentation de la méthode stable implicite semi-lagrangienne de Stam.
 *
 * @author	Flavien Bridault
 */
class BenchSolver : public Solver
{
public:
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilisé pour la simulation
   */
  BenchSolver (CPoint& position, int n_x, int n_y, int n_z, double dim, double pas_de_temps);

    virtual ~ BenchSolver ();

  /** Lance plusieurs itérations du solveur avec différents pas de résolutions pour les méthodes
   * de Gauss-Seidel, en vue du profiling
   */
  void iterate (bool flickering);

private:
  int IX2 (int i, int j, int k)
  {
    return ((i) + (m_nbVoxelsX) * (j) + (m_nbVoxelsX) * (m_nbVoxelsY) * (k));
  };
  
  void diffuse_log (int b, double *const x, const double *const x0,
		    const double *const xref, double *const xlog,
		    double a, double diff_visc);
  void diffuse_hybride_log (int b, double *const x, const double *const x0,
			    const double *const xref, double *const residu,
			    double *const residu0, double *const xlog,
			    double a, double diff_visc);
  void project_log (double *const p, double *const div, double *const xref,
		    double *const xlog);
  void project_log_hybride (double *p, double *const div, double *const xref,
			    double *const xlog, double *const residu,
			    double *const residu0);
  void project_save (double *const p, double *const div, int num);

  void iterate (bool flickering, int nb_step_GS);
  void iterate_hybride (bool flickering, int nb_step_GS);

  /** Sauvegarde les différentes composantes du solveur comme valeur de référence */
//  void save_ref ();

  void save_state (const double *const x, double *x_save);
  void save_ref (const double *const x, double *x_ref);
  void set_previous_state (double *x, const double *const x_save);

  /** Sauvegarde les différentes composantes du solveur */
  void save_state ();
  /** Restaure le solveur dans l'état précédent sauvegardé par save_state() */
  void set_previous_state ();

  void vel_step_save ();
  void vel_step_compare_diffuse_normal ();
  void vel_step_compare_diffuse_hybride ();
  void vel_step_compare_project_normal ();
  void vel_step_compare_project_hybride ();
  /** Compare les valeurs de la vélocité à celles stockés dans save_ref().
   *  La méthode calcule une valeur d'erreur RMS pour chaque composante u,v,w
   *  et l'écrit ensuite dans le fichier "solver.log"
   */
  void compare ();
  void compare (int n, const double *const x, double *const xlog,
		const double *const xref);
  void writeLog ();

  const void save (const double *const x);
  void iterate_save (bool flickering);
  
  ofstream m_file;

  double *m_uSave, *m_vSave, *m_wSave, *m_uPrevSave, *m_vPrevSave, *m_wPrevSave;
  double *m_densSave, *m_densPrevSave, *m_densSrcSave;
  double *m_uSrcSave, *m_vSrcSave, *m_wSrcSave;
  double *m_pSave;
  double *m_rowSave;
  double *m_uRef, *m_vRef, *m_wRef, *m_pRef, *m_densRef;
  /** Tableaux temporaires pour loguer les valeurs calculées */
  double *m_uLog, *m_vLog, *m_wLog, *m_pLog;

  int m_rowSize;
  static const int m_refVal = 25;
};

#endif

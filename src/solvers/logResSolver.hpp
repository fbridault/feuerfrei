#if !defined(LOGRESSOLVER_H)
#define LOGRESSOLVER_H

class LogResSolver;

#include "benchsolver.hpp"

#include <fstream>

class BenchSolver;

/** La classe LogResSolver permet de loguer les valeurs de résidus des solveurs à base
 * des méthodes itératives de Gauss-Seidel et du gradient conjugué préconditionné avec SSOR.
 *
 * @author	Flavien Bridault
 */
class LogResSolver: public BenchSolver
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
  LogResSolver (Point& position, uint n_x, uint n_y, uint n_z, double dim, double timeStep, 
		uint nbTimeSteps, double buoyancy, double omegaDiff, double omegaProj, double epsilon);
  /** Destructeur. */
  virtual ~LogResSolver ();
  
private:
  void vel_step ();
  
  void diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc);
  void project (double *const p, double *const div);
  
  void GS_solve(unsigned char b, double *const x, double *const x0, double a, double div, uint nb_steps);
  void GCSSOR(double *const x0, const double *const b, double a, double diagonal, double omega, uint maxiter);
  
  /** Ecrit la valeur du résidu dans le fichier de log.
   * @param iter Numéro de l'itération de lé méthode de résolution.
   * @param value Valeur à ajouter à la moyenne.
   */
  void logResidu (uint iter, double value);
  
  /** Fichier de log pour la diffusion. */
  ofstream m_fileDiff[NB_DIFF_LOGS];
  /** Fichier de log pour la projection. */
  ofstream m_fileProj[NB_PROJ_LOGS];
  
  /** Pointeur sur le fichier courant. */
  ofstream *m_file;
};

#endif

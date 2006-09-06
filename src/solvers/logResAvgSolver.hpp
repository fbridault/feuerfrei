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
  LogResAvgSolver (Point& position, uint n_x, uint n_y, uint n_z, double dim, double timeStep, 
		   uint nbTimeSteps, double buoyancy, double omegaDiff, double omegaProj, double epsilon);
 
  /** Constructeur nécessaire pour l'héritage multiple.
   * @param nbTimeSteps Nombre de pas de temps de la simulation.
   * @param omegaDiff Paramètre omega pour la diffusion.
   * @param omegaProj Paramètre omega pour la projection.
   * @param epsilon Tolérance d'erreur pour GCSSOR.
   */
  LogResAvgSolver (uint nbTimeSteps, double omegaDiff, double omegaProj, double epsilon);
  /** Destructeur. */
  virtual ~LogResAvgSolver ();

protected:
  virtual void vel_step ();
  
  virtual void diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc);
  virtual void project (double *const p, double *const div);
  
  virtual void GS_solve(unsigned char b, double *const x, double *const x0, double a, double div, uint nb_steps);
  virtual void GCSSOR(double *const x0, const double *const b, double a, double diagonal, double omega, uint maxiter);
  
  /** Calcul des moyennes pour une itération de la méthode de résolution de système linéaire.
   * @param iter Numéro de l'itération de lé méthode de résolution.
   * @param value Valeur à ajouter à la moyenne.
   */
  virtual void computeAverage ( uint iter, double value );
  
  /** Nombre de valeurs incluses dans la moyenne. */
  uint m_nbAverages;
  
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
  
  /** Fichier de log. */
  ofstream m_file;  
};

#endif

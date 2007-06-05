#if !defined(LOGRESSOLVER_H)
#define LOGRESSOLVER_H

class LogResSolver3D;

#include "benchsolver3D.hpp"

#include <fstream>

class BenchSolver3D;

/** La classe LogResSolver3D permet de loguer les valeurs de r�sidus des solveurs � base
 * des m�thodes it�ratives de Gauss-Seidel et du gradient conjugu� pr�conditionn� avec SSOR.
 *
 * @author	Flavien Bridault
 */
class LogResSolver3D: public BenchSolver3D
{
public:
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x R�solution de la grille en x.
   * @param n_y R�solution de la grille en y.
   * @param n_z R�solution de la grille en z.
   * @param dim Dimension du solveur, utilis� pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilis� pour la simulation.
   * @param nbTimeSteps Nombre de pas de temps de la simulation.
   * @param buoyancy Intensit� de la force de flottabilit� dans le solveur.
   * @param omegaDiff Param�tre omega pour la diffusion.
   * @param omegaProj Param�tre omega pour la projection.
   * @param epsilon Tol�rance d'erreur pour GCSSOR.
   */
  LogResSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, float timeStep, 
		  uint nbTimeSteps, float buoyancy, float omegaDiff, float omegaProj, float epsilon);
  /** Destructeur. */
  virtual ~LogResSolver3D ();
  
private:
  void vel_step ();
  
  void diffuse (unsigned char b, float *const x, float *const x0, float a, float diff_visc);
  void project (float *const p, float *const div);
  
  void GS_solve(unsigned char b, float *const x, float *const x0, float a, float div, uint nb_steps);
  void GCSSOR(float *const x0, const float *const b, float a, float diagonal, float omega, uint maxiter);
  
  /** Ecrit la valeur du r�sidu dans le fichier de log.
   * @param iter Num�ro de l'it�ration de l� m�thode de r�solution.
   * @param value Valeur � ajouter � la moyenne.
   */
  void logResidu (uint iter, float value);
  
  /** Fichier de log pour la diffusion. */
  ofstream m_fileDiff[NB_DIFF_LOGS];
  /** Fichier de log pour la projection. */
  ofstream m_fileProj[NB_PROJ_LOGS];
  
  /** Pointeur sur le fichier courant. */
  ofstream *m_file;
};

#endif

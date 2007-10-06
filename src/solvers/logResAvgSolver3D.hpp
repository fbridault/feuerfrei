#if !defined(LOGRESAVGSOLVER_H)
#define LOGRESAVGSOLVER_H

class LogResAvgSolver3D;

#include "benchsolver3D.hpp"

#include <fstream>

class BenchSolver3D;

/** La classe LogResAvgSolver3D permet de loguer les moyennes des valeurs de r�sidus des solveurs � base
 * des m�thodes it�ratives de Gauss-Seidel et du gradient conjugu� pr�conditionn� avec SSOR.
 *
 * @author	Flavien Bridault
 */
class LogResAvgSolver3D: public BenchSolver3D
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
  LogResAvgSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, float timeStep,
		     float buoyancy, float vorticityConfinement, float omegaDiff, float omegaProj, 
		     float epsilon, uint nbTimeSteps);
  
  /** Constructeur n�cessaire pour l'h�ritage multiple.
   * @param nbTimeSteps Nombre de pas de temps de la simulation.
   * @param omegaDiff Param�tre omega pour la diffusion.
   * @param omegaProj Param�tre omega pour la projection.
   * @param epsilon Tol�rance d'erreur pour GCSSOR.
   */
  LogResAvgSolver3D (uint nbTimeSteps, float omegaDiff, float omegaProj, float epsilon);
  /** Destructeur. */
  virtual ~LogResAvgSolver3D ();

protected:
  virtual void vel_step ();
  
  virtual void diffuse (unsigned char b, float *const x, float *const x0, float a);
  virtual void project (float *const p, float *const div);
  
  virtual void GS_solve(unsigned char b, float *const x, const float *const x0, float a, float div, uint nb_steps);
  virtual void GCSSOR(float *const x0, const float *const b, float a, float diagonal, float omega, uint maxiter);
  
  /** Calcul des moyennes pour une it�ration de la m�thode de r�solution de syst�me lin�aire.
   * @param iter Num�ro de l'it�ration de l� m�thode de r�solution.
   * @param value Valeur � ajouter � la moyenne.
   */
  virtual void computeAverage ( uint iter, float value );
  
  /** Nombre de valeurs incluses dans la moyenne. */
  uint m_nbAverages;
  
  /** Tableau contenant les moyennes. Il est organis� comme ceci :<br>
   *    - de O � m_nbSteps : diffusion en u avec GS
   *    - de m_nbSteps � 2*m_nbSteps : diffusion en v avec GS
   *    - de 2*m_nbSteps � 3*m_nbSteps : diffusion en w avec GS
   *    - de 3*m_nbSteps � 4*m_nbSteps : diffusion en u avec GCSSOR
   *    - de 4*m_nbSteps � 5*m_nbSteps : diffusion en v avec GCSSOR
   *    - de 5*m_nbSteps � 6*m_nbSteps : diffusion en w avec GCSSOR
   *    - de 6*m_nbSteps � 7*m_nbSteps :  projection 1 avec GS
   *    - de 7*m_nbSteps � 8*m_nbSteps :  projection 1 avec GS
   *    - de 8*m_nbSteps � 9*m_nbSteps :  projection 2 avec GCSSOR
   *    - de 9*m_nbSteps � 10*m_nbSteps : projection 2 avec GCSSOR
   */
  float *m_averages;
  
  /** Fichier de log. */
  ofstream m_file;  
};

#endif

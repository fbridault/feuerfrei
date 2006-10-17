#if !defined(BENCHSOLVER_H)
#define BENCHSOLVER_H

class BenchSolver3D;

#include "GSSolver3D.hpp"
#include "GCSSORSolver3D.hpp"

#include <fstream>

#define NB_DIFF_LOGS 6
#define NB_PROJ_LOGS 4

class GSSolver3D;
class GCSSORSolver3D;

/** La classe BenchSolver3D sert d'interface pour les classes permettant de loguer les valeurs de r�sidus
 * des solveurs � base des m�thodes it�ratives de Gauss-Seidel et du gradient conjugu� pr�conditionn� avec SSOR.
 * Elle h�rite de GSsolver et GCSSORSolver3D qui tous deux h�ritent <b>virtuellement</b> de Solver3D
 * de mani�re � n'avoir qu'une seule instance de Solver3D.
 * La simulation est limit�e dans le temps par l'attribut m_nbMaxIter afin de faciliter la g�n�ration
 * des graphiques.
 *
 * @author	Flavien Bridault
 */
class BenchSolver3D: public GSSolver3D, public GCSSORSolver3D
{
public:
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x R�solution de la grille en x.
   * @param n_y R�solution de la grille en y.
   * @param n_z R�solution de la grille en z.
   * @param dim Dimension du solveur, utilis� pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilis� pour la simulation.
   * @param buoyancy Intensit� de la force de flottabilit� dans le solveur.
   * @param nbTimeSteps Nombre de pas de temps de la simulation.
   * @param omegaDiff Param�tre omega pour la diffusion.
   * @param omegaProj Param�tre omega pour la projection.
   * @param epsilon Tol�rance d'erreur pour GCSSOR.
   */
  BenchSolver3D (Point& position, uint n_x, uint n_y, uint n_z, double dim, double timeStep, double buoyancy, 
	       uint nbTimeSteps, double omegaDiff, double omegaProj, double epsilon);
  
  /** Constructeur n�cessaire pour l'h�ritage multiple.
   * @param nbTimeSteps Nombre de pas de temps de la simulation.
   * @param omegaDiff Param�tre omega pour la diffusion.
   * @param omegaProj Param�tre omega pour la projection.
   * @param epsilon Tol�rance d'erreur pour GCSSOR.
   */
  BenchSolver3D (uint nbTimeSteps, double omegaDiff, double omegaProj, double epsilon);
  /** Destructeur. */
  virtual ~ BenchSolver3D ();
  
protected:
  virtual void diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc) = 0;
  virtual void project (double *const p, double *const div) = 0;  

  /** Sauvegarde les diff�rentes composantes du solveur pour pouvoir effectuer plusieurs tests sur
   * un m�me �tat de la grille de r�solution.
   */
  void saveState (const double *const x, const double *const x2);
  /** Sauvegarde les diff�rentes composantes du solveur comme valeur de r�f�rence. */
  void setPreviousState (double *const x, double *const x2);
  
  /* Nombre maximum de pas de temps � simuler */
  uint m_nbMaxIter;
  
  /** Indice utilis� pour la g�n�ration du nom du fichier de log. */
  unsigned short m_index;
  
private:
  double *m_save, *m_save2; /** Permet de sauver une composante. */
};

#endif

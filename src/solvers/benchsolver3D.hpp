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

/** La classe BenchSolver3D sert d'interface pour les classes permettant de loguer les valeurs de résidus
 * des solveurs à base des méthodes itératives de Gauss-Seidel et du gradient conjugué préconditionné avec SSOR.
 * Elle hérite de GSsolver et GCSSORSolver3D qui tous deux héritent <b>virtuellement</b> de Solver3D
 * de manière à n'avoir qu'une seule instance de Solver3D.
 * La simulation est limitée dans le temps par l'attribut m_nbMaxIter afin de faciliter la génération
 * des graphiques.
 *
 * @author	Flavien Bridault
 */
class BenchSolver3D: public GSSolver3D, public GCSSORSolver3D
{
public:
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x Résolution de la grille en x.
   * @param n_y Résolution de la grille en y.
   * @param n_z Résolution de la grille en z.
   * @param dim Dimension du solveur, utilisé pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilisé pour la simulation.
   * @param buoyancy Intensité de la force de flottabilité dans le solveur.
   * @param nbTimeSteps Nombre de pas de temps de la simulation.
   * @param omegaDiff Paramètre omega pour la diffusion.
   * @param omegaProj Paramètre omega pour la projection.
   * @param epsilon Tolérance d'erreur pour GCSSOR.
   */
  BenchSolver3D (Point& position, uint n_x, uint n_y, uint n_z, double dim, double timeStep, double buoyancy, 
	       uint nbTimeSteps, double omegaDiff, double omegaProj, double epsilon);
  
  /** Constructeur nécessaire pour l'héritage multiple.
   * @param nbTimeSteps Nombre de pas de temps de la simulation.
   * @param omegaDiff Paramètre omega pour la diffusion.
   * @param omegaProj Paramètre omega pour la projection.
   * @param epsilon Tolérance d'erreur pour GCSSOR.
   */
  BenchSolver3D (uint nbTimeSteps, double omegaDiff, double omegaProj, double epsilon);
  /** Destructeur. */
  virtual ~ BenchSolver3D ();
  
protected:
  virtual void diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc) = 0;
  virtual void project (double *const p, double *const div) = 0;  

  /** Sauvegarde les différentes composantes du solveur pour pouvoir effectuer plusieurs tests sur
   * un même état de la grille de résolution.
   */
  void saveState (const double *const x, const double *const x2);
  /** Sauvegarde les différentes composantes du solveur comme valeur de référence. */
  void setPreviousState (double *const x, double *const x2);
  
  /* Nombre maximum de pas de temps à simuler */
  uint m_nbMaxIter;
  
  /** Indice utilisé pour la génération du nom du fichier de log. */
  unsigned short m_index;
  
private:
  double *m_save, *m_save2; /** Permet de sauver une composante. */
};

#endif

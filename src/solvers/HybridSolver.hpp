#if !defined(HYBRIDSOLVER_H)
#define HYBRIDSOLVER_H

class BenchSolver;

#include "GSsolver.hpp"
#include "GCSSORsolver.hpp"

#include <fstream>

#define NB_DIFF_LOGS 6
#define NB_PROJ_LOGS 4

class GSsolver;
class GCSSORsolver;
class HybridSolver;
class LODHybridSolver;

/** La classe HybridSolver sert d'interface pour les classes permettant de loguer les valeurs de résidus
 * des solveurs à base des méthodes itératives de Gauss-Seidel et du gradient conjugué préconditionné avec SSOR.
 * Elle hérite de GSsolver et GCSSORsolver qui tous deux héritent <b>virtuellement</b> de Solver
 * de manière à n'avoir qu'une seule instance de Solver.
 *
 * @author	Flavien Bridault
 */
class HybridSolver: public GSsolver, public GCSSORsolver
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
   * @param omegaDiff Paramètre omega pour la diffusion.
   * @param omegaProj Paramètre omega pour la projection.
   * @param epsilon Tolérance d'erreur pour GCSSOR.
   */
  HybridSolver (Point& position, uint n_x, uint n_y, uint n_z, double dim, double timeStep,
		double buoyancy, double omegaDiff, double omegaProj, double epsilon);
  
  /** Constructeur nécessaire pour l'héritage multiple.
   * @param omegaDiff Paramètre omega pour la diffusion.
   * @param omegaProj Paramètre omega pour la projection.
   * @param epsilon Tolérance d'erreur pour GCSSOR.
   */
  HybridSolver (double omegaDiff, double omegaProj, double epsilon);
  
  /** Desctructeur. */
  virtual ~ HybridSolver ();
  
protected:
  virtual void diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc);
  virtual void project (double *const p, double *const div);
  
  double m_time;
};

/** @test La classe LODHybridSolver implémente un solveur de classe HybridSolver permettant d'utiliser une grille
 * adaptative. La résolution de la grille ne pourra jamais dépasser la résolution initiale passée au constructeur.
 */
class LODHybridSolver: public HybridSolver
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
   * @param omegaDiff Paramètre omega pour la diffusion.
   * @param omegaProj Paramètre omega pour la projection.
   * @param epsilon Tolérance d'erreur pour GCSSOR.
   */
  LODHybridSolver (Point& position, uint n_x, uint n_y, uint n_z, double dim, double timeStep,
		   double buoyancy, double omegaDiff, double omegaProj, double epsilon);
  virtual ~LODHybridSolver ();
  
  virtual void divideRes ();  
  virtual void multiplyRes ();  
  virtual void decreaseRes ();  
  virtual void increaseRes ();
  
private:
  double *m_uTmp, *m_vTmp, *m_wTmp;

  /** Nombre de voxels initiaux en X. */
  uint initialNbVoxelsX;
  /** Nombre de voxels initiaux en Y. */
  uint initialNbVoxelsY;
  /** Nombre de voxels initiaux en Z. */
  uint initialNbVoxelsZ;
};

#endif

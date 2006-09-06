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

/** La classe HybridSolver sert d'interface pour les classes permettant de loguer les valeurs de r�sidus
 * des solveurs � base des m�thodes it�ratives de Gauss-Seidel et du gradient conjugu� pr�conditionn� avec SSOR.
 * Elle h�rite de GSsolver et GCSSORsolver qui tous deux h�ritent <b>virtuellement</b> de Solver
 * de mani�re � n'avoir qu'une seule instance de Solver.
 *
 * @author	Flavien Bridault
 */
class HybridSolver: public GSsolver, public GCSSORsolver
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
   * @param omegaDiff Param�tre omega pour la diffusion.
   * @param omegaProj Param�tre omega pour la projection.
   * @param epsilon Tol�rance d'erreur pour GCSSOR.
   */
  HybridSolver (Point& position, uint n_x, uint n_y, uint n_z, double dim, double timeStep,
		double buoyancy, double omegaDiff, double omegaProj, double epsilon);
  
  /** Constructeur n�cessaire pour l'h�ritage multiple.
   * @param omegaDiff Param�tre omega pour la diffusion.
   * @param omegaProj Param�tre omega pour la projection.
   * @param epsilon Tol�rance d'erreur pour GCSSOR.
   */
  HybridSolver (double omegaDiff, double omegaProj, double epsilon);
  
  /** Desctructeur. */
  virtual ~ HybridSolver ();
  
protected:
  virtual void diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc);
  virtual void project (double *const p, double *const div);
  
  double m_time;
};

/** @test La classe LODHybridSolver impl�mente un solveur de classe HybridSolver permettant d'utiliser une grille
 * adaptative. La r�solution de la grille ne pourra jamais d�passer la r�solution initiale pass�e au constructeur.
 */
class LODHybridSolver: public HybridSolver
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
   * @param omegaDiff Param�tre omega pour la diffusion.
   * @param omegaProj Param�tre omega pour la projection.
   * @param epsilon Tol�rance d'erreur pour GCSSOR.
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

#if !defined(HYBRIDSOLVER3D_H)
#define HYBRIDSOLVER3D_H

class BenchSolver3D;

#include "GSSolver3D.hpp"
#include "GCSSORSolver3D.hpp"

#include <fstream>

#define NB_DIFF_LOGS 6
#define NB_PROJ_LOGS 4
#define RESOLUTION_MIN 6

class GSSolver3D;
class GCSSORSolver3D;
class HybridSolver3D;
class LODHybridSolver3D;

/** La classe HybridSolver sert d'interface pour les classes permettant de loguer les valeurs de r�sidus
 * des solveurs � base des m�thodes it�ratives de Gauss-Seidel et du gradient conjugu� pr�conditionn� avec SSOR.
 * Elle h�rite de GSSolver3D et GCSSORSolver3D qui tous deux h�ritent <b>virtuellement</b> de Solver
 * de mani�re � n'avoir qu'une seule instance de Solver.
 *
 * @author	Flavien Bridault
 */
class HybridSolver3D: public GSSolver3D, public GCSSORSolver3D
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
  HybridSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale, double timeStep,
		  double buoyancy, double omegaDiff, double omegaProj, double epsilon);
  
  /** Constructeur n�cessaire pour l'h�ritage multiple.
   * @param omegaDiff Param�tre omega pour la diffusion.
   * @param omegaProj Param�tre omega pour la projection.
   * @param epsilon Tol�rance d'erreur pour GCSSOR.
   */
  HybridSolver3D (double omegaDiff, double omegaProj, double epsilon);
  
  /** Desctructeur. */
  virtual ~ HybridSolver3D ();
  
protected:
  virtual void diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc);
  virtual void project (double *const p, double *const div);
  
  double m_time;
};

/** @test La classe LODHybridSolver impl�mente un solveur de classe HybridSolver permettant d'utiliser une grille
 * adaptative. La r�solution de la grille ne pourra jamais d�passer la r�solution initiale pass�e au constructeur.
 */
class LODHybridSolver3D: public HybridSolver3D
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
  LODHybridSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale, double timeStep,
		   double buoyancy, double omegaDiff, double omegaProj, double epsilon);
  virtual ~LODHybridSolver3D ();
  
  virtual void divideRes ();  
  virtual void multiplyRes ();  
  virtual void decreaseRes ();  
  virtual void increaseRes ();
  virtual uint getNbMaxDiv () { return m_nbMaxDiv; };
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

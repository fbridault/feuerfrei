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

/** La classe HybridSolver sert d'interface pour les classes permettant de loguer les valeurs de résidus
 * des solveurs à base des méthodes itératives de Gauss-Seidel et du gradient conjugué préconditionné avec SSOR.
 * Elle hérite de GSSolver3D et GCSSORSolver3D qui tous deux héritent <b>virtuellement</b> de Solver
 * de manière à n'avoir qu'une seule instance de Solver.
 *
 * @author	Flavien Bridault
 */
class HybridSolver3D: public GSSolver3D, public GCSSORSolver3D
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
  HybridSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, float timeStep,
		  float buoyancy, float omegaDiff, float omegaProj, float epsilon);
  
  /** Constructeur nécessaire pour l'héritage multiple.
   * @param omegaDiff Paramètre omega pour la diffusion.
   * @param omegaProj Paramètre omega pour la projection.
   * @param epsilon Tolérance d'erreur pour GCSSOR.
   */
  HybridSolver3D (float omegaDiff, float omegaProj, float epsilon);
  
  /** Desctructeur. */
  virtual ~ HybridSolver3D ();
  
protected:
  virtual void diffuse (unsigned char b, float *const x, float *const x0, float a, float diff_visc);
  virtual void project (float *const p, float *const div);
};

/** @test La classe LODHybridSolver implémente un solveur de classe HybridSolver permettant d'utiliser une grille
 * adaptative. La résolution de la grille ne pourra jamais dépasser la résolution initiale passée au constructeur.
 */
class LODHybridSolver3D: public HybridSolver3D
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
  LODHybridSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, float timeStep,
		   float buoyancy, float omegaDiff, float omegaProj, float epsilon);
  virtual ~LODHybridSolver3D ();
  
  virtual void divideRes ();
  virtual void multiplyRes ();
  virtual void decreaseRes ();
  virtual void increaseRes ();
  virtual uint getNbMaxDiv () { return m_nbMaxDiv; };

  /** Fonction de dessin de la grille */
  virtual void displayGrid ();
  
  /** Fonction de dessin du repère de base */
  virtual void displayBase ();
  
private:
  float *m_uTmp, *m_vTmp, *m_wTmp;

  /** Nombre de voxels initiaux en X. */
  uint initialNbVoxelsX;
  /** Nombre de voxels initiaux en Y. */
  uint initialNbVoxelsY;
  /** Nombre de voxels initiaux en Z. */
  uint initialNbVoxelsZ;
};

#endif

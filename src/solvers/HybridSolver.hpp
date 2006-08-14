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
 * des solveurs à base
 * des méthodes itératives de Gauss-Seidel et du gradient conjugué préconditionné avec SSOR.
 * Elle hérite de GSsolver et GCSSORsolver qui tous deux héritent <b>virtuellement</b> de Solver
 * de manière à n'avoir qu'une seule instance de Solver.
 *
 * @author	Flavien Bridault
 */
class HybridSolver: public GSsolver, public GCSSORsolver
{
public:
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilisé pour la simulation
   */
  HybridSolver (Point& position, uint n_x, uint n_y, uint n_z, double dim, double timeStep,
		double buoyancy, double omegaDiff, double omegaProj, double epsilon);
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilisé pour la simulation
   */
  HybridSolver (double omegaDiff, double omegaProj, double epsilon);
  virtual ~ HybridSolver ();
  
protected:
  //virtual void iterate ();

  /** Pas de diffusion.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param x composante à traiter
   * @param x0 composante x au pas de temps précédent
   * @param diff_visc paramètre correspondant soit à la diffusion si la fonction est utilisée pour
   * la résolution du pas de densité, soit à la viscosité si elle est employée pour la résolution
   * du pas de vélocité
   */
  virtual void diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc);

  /** Pas de projection pour garantir la conservation de la masse.
   * Les tableaux passés en paramètre sont modifiés ici et ne doivent donc plus servir après l'appel de la projection
   */
  virtual void project (double *const p, double *const div);

  double m_time;
};

class LODHybridSolver: public HybridSolver
{
public:
    /** Constructeur du solveur.
   * @param n : taille de la grille la plus grande
   * @param pas_de_temps : pas de temps utilisé pour la simulation
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

  uint initialNbVoxelsX, initialNbVoxelsY, initialNbVoxelsZ;
};

#endif

#if !defined(BENCHSOLVER_H)
#define BENCHSOLVER_H

class BenchSolver;

#include "GSsolver.hpp"
#include "GCSSORsolver.hpp"

#include <fstream>

#define NB_DIFF_LOGS 6
#define NB_PROJ_LOGS 4

class GSsolver;
class GCSSORsolver;

/** La classe BenchSolver sert d'interface pour les classes permettant de loguer les valeurs de résidus
 * des solveurs à base
 * des méthodes itératives de Gauss-Seidel et du gradient conjugué préconditionné avec SSOR.
 * Elle hérite de GSsolver et GCSSORsolver qui tous deux héritent <b>virtuellement</b> de Solver
 * de manière à n'avoir qu'une seule instance de Solver.
 *
 * @author	Flavien Bridault
 */
class BenchSolver: public GSsolver, public GCSSORsolver
{
public:
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilisé pour la simulation
   */
  BenchSolver (CPoint& position, int n_x, int n_y, int n_z, double dim, double timeStep, double nbTimeSteps,
	       double omegaDiff, double omegaProj, double epsilon);
  virtual ~ BenchSolver ();
  
protected:
  /** Pas de diffusion.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param x composante à traiter
   * @param x0 composante x au pas de temps précédent
   * @param diff_visc paramètre correspondant soit à la diffusion si la fonction est utilisée pour
   * la résolution du pas de densité, soit à la viscosité si elle est employée pour la résolution
   * du pas de vélocité
   */
  virtual void diffuse (int b, double *const x, double *const x0, double a, double diff_visc) = 0;

  /** Pas de projection pour garantir la conservation de la masse.
   * Les tableaux passés en paramètre sont modifiés ici et ne doivent donc plus servir après l'appel de la projection
   */
  virtual void project (double *const p, double *const div) = 0;  

  void saveState (const double *const x, const double *const x2);
  /** Sauvegarde les différentes composantes du solveur comme valeur de référence */
  //void save_ref (const double *const x, double *x_ref);
  void setPreviousState (double *const x, double *const x2);
  
  /* Nombre maximum de pas de temps à simuler */
  int m_nbMaxIter;
  
  short m_index;

private:
  double *m_save, *m_save2;
  
};

#endif

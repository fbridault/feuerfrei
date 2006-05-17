#if !defined(GSSOLVER_H)
#define GSSOLVER_H

class GSsolver;

#include "solver.hpp"

/** La classe Solver propose une implémentation 3D de la méthode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre modèle de flamme, le champ de densité n'est pas utilisé, ce qui,
 * physiquement semble assez curieux (d'après Alexei). Néanmoins Stam présente bien les choses ainsi dans
 * son code initial, la densité dépend de la vélocité, la vélocité évolue indépendament.
 * 
 * @author	Flavien Bridault
 */
class GSsolver : public virtual Solver
{
public:
  /** Constructeur par défaut nécessaire pour l'héritage multiple */
  GSsolver ();
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilisé pour la simulation
   */
  GSsolver (Point& position, uint n_x, uint n_y, uint n_z, double dim, double pas_de_temps, double buoyancy);
  virtual ~GSsolver ();
  
protected:
  /** Effectue une résolution des systèmes linéaires de la diffusion
  * et de la projection à l'aide de la méthode itérative de Gauss-Seidel
  * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
  * @param x composante à traiter
  * @param x0 composante x au pas de temps précédent
  * @param a valeur des coefficients dans la matrice A
  * @param div fraction des coefficients sur la diagonale de la matrice A ( 1/(1+6a) pour la diffusion
  * et 1/6 pour la projection
  * @param nb_steps nombre d'itérations à effectuer
  */
  virtual void GS_solve(unsigned char b, double *const x, const double *const x0, double a, double div, uint nb_steps);
  
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
};

#endif

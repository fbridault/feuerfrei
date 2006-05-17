#if !defined(GSSOLVER_H)
#define GSSOLVER_H

class GSsolver;

#include "solver.hpp"

/** La classe Solver propose une impl�mentation 3D de la m�thode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre mod�le de flamme, le champ de densit� n'est pas utilis�, ce qui,
 * physiquement semble assez curieux (d'apr�s Alexei). N�anmoins Stam pr�sente bien les choses ainsi dans
 * son code initial, la densit� d�pend de la v�locit�, la v�locit� �volue ind�pendament.
 * 
 * @author	Flavien Bridault
 */
class GSsolver : public virtual Solver
{
public:
  /** Constructeur par d�faut n�cessaire pour l'h�ritage multiple */
  GSsolver ();
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilis� pour la simulation
   */
  GSsolver (Point& position, uint n_x, uint n_y, uint n_z, double dim, double pas_de_temps, double buoyancy);
  virtual ~GSsolver ();
  
protected:
  /** Effectue une r�solution des syst�mes lin�aires de la diffusion
  * et de la projection � l'aide de la m�thode it�rative de Gauss-Seidel
  * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
  * @param x composante � traiter
  * @param x0 composante x au pas de temps pr�c�dent
  * @param a valeur des coefficients dans la matrice A
  * @param div fraction des coefficients sur la diagonale de la matrice A ( 1/(1+6a) pour la diffusion
  * et 1/6 pour la projection
  * @param nb_steps nombre d'it�rations � effectuer
  */
  virtual void GS_solve(unsigned char b, double *const x, const double *const x0, double a, double div, uint nb_steps);
  
  /** Pas de diffusion.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param x composante � traiter
   * @param x0 composante x au pas de temps pr�c�dent
   * @param diff_visc param�tre correspondant soit � la diffusion si la fonction est utilis�e pour
   * la r�solution du pas de densit�, soit � la viscosit� si elle est employ�e pour la r�solution
   * du pas de v�locit�
   */
  virtual void diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc);

  /** Pas de projection pour garantir la conservation de la masse.
   * Les tableaux pass�s en param�tre sont modifi�s ici et ne doivent donc plus servir apr�s l'appel de la projection
   */
  virtual void project (double *const p, double *const div);  
};

#endif

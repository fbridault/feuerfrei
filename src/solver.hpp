#if !defined(SOLVER_H)
#define SOLVER_H

class Solver;

#include "header.h"
#include "flame.hpp"

class Flame;

/** La classe Solver propose une impl�mentation 3D de la m�thode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre mod�le de flamme, le champ de densit� n'est pas utilis�, ce qui,
 * physiquement semble assez curieux (d'apr�s Alexei). N�anmoins Stam pr�sente bien les choses ainsi dans
 * son code initial, la densit� d�pend de la v�locit�, la v�locit� �volue ind�pendament.
 * 
 * @author	Flavien Bridault
 */
class Solver
{
public:
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilis� pour la simulation
   */
  Solver (int n_x, int n_y, int n_z, double dim, double pas_de_temps);
  virtual ~Solver ();

  /** Lance une it�ration du solveur. */
  virtual void iterate (bool flickering);

  /** Informe au solveur la pr�sence de flammes, en vue de leur contribution à l'�l�vation thermique 
   * (m�me si celle-ci n'est que fictive, puisque "simul�e" par la v�locit�)
   * @param f tableau contenant les pointeurs vers les flammes
   * @param nb_f nombre de flammes (taille du tableau)
   */
  void setFlames (Flame ** const f, int nb_f)
  {
    flammes = f;
    nb_flammes = nb_f;
  };

  double getU (int i, int j, int k)
  {
    return u[IX (i, j, k)];
  };
  double getV (int i, int j, int k)
  {
    return v[IX (i, j, k)];
  };
  double getW (int i, int j, int k)
  {
    return w[IX (i, j, k)];
  };

  /** Ajout d'une force externe pour la composante U */
  void addUsrc (int i, int j, int k, double value)
  {
    u_src[IX (i, j, k)] += value;
  };
  /** Ajout d'une force externe pour la composante V */
  void addVsrc (int i, int j, int k, double value)
  {
    v_src[IX (i, j, k)] += value;
  };
  /** Ajout d'une force externe pour la composante W */
  void addWsrc (int i, int j, int k, double value)
  {
    w_src[IX (i, j, k)] += value;
  };

  /** Affectation d'une force externe pour la composante V */
  void setUsrc (int i, int j, int k, double value)
  {
    u_src[IX (i, j, k)] = value;
  };
  /** Affectation d'une force externe pour la composante V */
  void setVsrc (int i, int j, int k, double value)
  {
    v_src[IX (i, j, k)] = value;
  };
  /** Ajout des forces externes.
   * @param x composante � traiter
   * @param src composante contenant les forces engendr�es par les sources externes
   */
  void add_source (double *const x, double *const src);
  /** Remet � z�ro toutes les forces externes */
  void cleanSources ();

  /** Retourne le nombre de voxels en X */
  int getX ()
  {
    return N_x;
  };
  /** Retourne le nombre de voxels en Y */
  int getY ()
  {
    return N_y;
  };
  /** Retourne le nombre de voxels en Z */
  int getZ ()
  {
    return N_z;
  };

  /** Retourne la dimension en X */
  double getDimX ()
  {
    return dim_x;
  };
  /** Retourne la dimension en Y */
  double getDimY ()
  {
    return dim_y;
  };
  /** Retourne la dimension en Z */
  double getDimZ ()
  {
    return dim_z;
  };

  /** Retourne le pas de temps */
  double getTimeStep()
  {
    return dt;
  };

  /** NOTE : Les 6 m�thodes ci-dessus pourraient faire partie d'un objet h�rit� Grid3D par exemple */
  /** Fonction de construction de la display list de la grille du solveur */
  void buildDLGrid ();
  /** Fonction de construction de la display list du rep�re du solveur */
  void buildDLBase ();
  /** Fonction de dessin de la grille */
  void displayGrid (){
    glCallList (GRILLE);
  };
  /** Fonction de dessin du rep�re de base */
  void displayBase (){
    glCallList (REPERE);
  };
  /** Fonction de dessin du champ de v�locit� */
  void displayVelocityField (void);
  /** Fonction de dessin de la v�locit� d'une cellule */
  void displayArrow (CVector * const direction);

protected:
  int IX (int i, int j, int k)
  {
    return ((i) + (N_x + 2) * (j) + (N_x + 2) * (N_y + 2) * (k));
  };
  int IX2 (int i)
  {
    int x,y,z,tmp,tmp2;
    tmp2=N_x*N_y;
    z=i/tmp2;
    tmp=i-z*tmp2;
    y=tmp/N_x;
    x=tmp-y*N_x;
    
    return( IX( x+1, y+1, z+1 ) );
   //int n=i/N_x; /* Nombre de lignes dans la grille */
    
    //return ( (N_x+2)*(N_y+2) + N_x+3 + n/N_y * ((N_x+2)*(N_y+2)) + (n % N_x) *2 + (i % (N_x*N_y) ) );
  };

  /** Traitement de valeurs aux bords du solveur.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param x composante � traiter
   */
  void set_bnd (int b, double *const x);

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
  void GS_solve(int b, double *const x, const double *const x0,
				double a, double div, double nb_steps);
  
  /** Effectue une r�solution des syst�mes lin�aires de la diffusion
  * et de la projection � l'aide de la m�thode du Gradient Conjugu�
  * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
  * @param x composante � traiter
  * @param x0 composante x au pas de temps pr�c�dent
  * @param a valeur des coefficients dans la matrice A
  * @param div fraction des coefficients sur la diagonale de la matrice A ( 1/(1+6a) pour la diffusion
  * et 1/6 pour la projection
  * @param nb_steps nombre d'it�rations � effectuer
  */
  void GCSSOR(double *const x0, const double *const b, double a, double diagonal, 
		     double nb_steps, double omega );
  
  /** Pas de diffusion.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param x composante � traiter
   * @param x0 composante x au pas de temps pr�c�dent
   * @param diff_visc param�tre correspondant soit � la diffusion si la fonction est utilis�e pour
   * la r�solution du pas de densit�, soit � la viscosit� si elle est employ�e pour la r�solution
   * du pas de v�locit�
   */
  void diffuse (int b, double *const x, const double *const x0,
				double a, double diff_visc);

  /** Pas de diffusion avec hybridation avec relance de la m�thode de Gauss-Seidel
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param x composante � traiter
   * @param x0 composante x au pas de temps pr�c�dent
   * @param residu residu de la composante � traiter
   * @param residu0 residu au pas de temps pr�c�dent
   * @param diff_visc param�tre correspondant soit � la diffusion si la fonction est utilis�e pour
   * la r�solution du pas de densit�, soit � la viscosit� si elle est employ�e pour la r�solution
   * du pas de v�locit�
   */
  void diffuse_hybride (int b, double *const x, const double *const x0,
			double *const residu, double *const residu0,
			double diff_visc);

  /** Pas d'advection => d�placement du fluide sur lui-même.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param d composante � traiter
   * @param d0 composante d au pas de temps pr�c�dent
   * @param u vecteur de v�locit� en u
   * @param v vecteur de v�locit� en v
   * @param w vecteur de v�locit� en w
   */
  void advect (int b, double *const d, const double *const d0,
	       const double *const u, const double *const v,
	       const double *const w);

  /** Pas de projection pour garantir la conservation de la masse.
   * Les tableaux pass�s en param�tre sont modifi�s ici et ne doivent donc plus servir apr�s l'appel de la projection
   */
  void project (double *const p, double *const div);

  /** Pas de r�solution de la densit�. */
  void dens_step ();

  /** Pas de r�solution de la v�locit�. */
  virtual void vel_step ();

  /** Pas de r�solution de la v�locit�, en utilisant l'hybridation */
  void vel_step_hybride ();

  /** Nombre de voxels sur un c�t� du cube de r�solution. */
  int N_x, N_y, N_z;
  
  /** Dimensions du solveur */
  double dim_x, dim_y, dim_z;

  /** Taille totale du cube en nombre de voxels, �gal � (N+2)^3. */
  int size;
  double *u, *v, *w, *u_prev, *v_prev, *w_prev, *u_src, *v_src, *w_src;
  double *dens, *dens_prev, *dens_src;
  double *residu_u, *residu_u_prev, *residu_v, *residu_v_prev, *residu_w,
    *residu_w_prev;
  double *r, *z, *q, *p;
	
  /** Nombre de pas de r�solutions de Gauss-seidel dans les m�thodes de diffusion et de projection */
  int nb_step_gauss_seidel;

  /** Tableau de pointeurs vers les flammes.
   * A REVOIR : il existe de ce fait une relation circulaire entre la classe Solver et la classe Flame
   * qu'il serait bon se supprimer.
   */
  Flame **flammes;
  /** Nombre de flammes. */
  int nb_flammes;

  /** Nombre d'it�rations pour Gauss-Seidel */
  int nb_iter;
  const static int nb_iter_flickering = 50;
  /** Viscosit� cin�matique de l'air 15*10E-6. */
  double visc;
  /** Diffusion. */
  double diff;
  /** Pas de temps. */
  double dt;
  
  double a_visc, a_diff;
};

#endif

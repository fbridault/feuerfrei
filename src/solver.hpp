#if !defined(SOLVER_H)
#define SOLVER_H

class Solver;

#include "header.h"
#include "flame.hpp"

class Flame;

/** La classe Solver propose une implémentation 3D de la méthode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre modèle de flamme, le champ de densité n'est pas utilisé, ce qui,
 * physiquement semble assez curieux (d'après Alexei). Néanmoins Stam présente bien les choses ainsi dans
 * son code initial, la densité dépend de la vélocité, la vélocité évolue indépendament.
 * 
 * @author	Flavien Bridault
 */
class Solver
{
public:
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilisé pour la simulation
   */
  Solver (int n_x, int n_y, int n_z, double dim, double pas_de_temps);
  virtual ~Solver ();

  /** Lance une itération du solveur. */
  virtual void iterate (bool flickering);

  /** Informe au solveur la présence de flammes, en vue de leur contribution Ã  l'élévation thermique 
   * (même si celle-ci n'est que fictive, puisque "simulée" par la vélocité)
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
   * @param x composante à traiter
   * @param src composante contenant les forces engendrées par les sources externes
   */
  void add_source (double *const x, double *const src);
  /** Remet à zéro toutes les forces externes */
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

  /** NOTE : Les 6 méthodes ci-dessus pourraient faire partie d'un objet hérité Grid3D par exemple */
  /** Fonction de construction de la display list de la grille du solveur */
  void buildDLGrid ();
  /** Fonction de construction de la display list du repère du solveur */
  void buildDLBase ();
  /** Fonction de dessin de la grille */
  void displayGrid (){
    glCallList (GRILLE);
  };
  /** Fonction de dessin du repère de base */
  void displayBase (){
    glCallList (REPERE);
  };
  /** Fonction de dessin du champ de vélocité */
  void displayVelocityField (void);
  /** Fonction de dessin de la vélocité d'une cellule */
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
   * @param x composante à traiter
   */
  void set_bnd (int b, double *const x);

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
  void GS_solve(int b, double *const x, const double *const x0,
				double a, double div, double nb_steps);
  
  /** Effectue une résolution des systèmes linéaires de la diffusion
  * et de la projection à l'aide de la méthode du Gradient Conjugué
  * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
  * @param x composante à traiter
  * @param x0 composante x au pas de temps précédent
  * @param a valeur des coefficients dans la matrice A
  * @param div fraction des coefficients sur la diagonale de la matrice A ( 1/(1+6a) pour la diffusion
  * et 1/6 pour la projection
  * @param nb_steps nombre d'itérations à effectuer
  */
  void GCSSOR(double *const x0, const double *const b, double a, double diagonal, 
		     double nb_steps, double omega );
  
  /** Pas de diffusion.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param x composante à traiter
   * @param x0 composante x au pas de temps précédent
   * @param diff_visc paramètre correspondant soit à la diffusion si la fonction est utilisée pour
   * la résolution du pas de densité, soit à la viscosité si elle est employée pour la résolution
   * du pas de vélocité
   */
  void diffuse (int b, double *const x, const double *const x0,
				double a, double diff_visc);

  /** Pas de diffusion avec hybridation avec relance de la méthode de Gauss-Seidel
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param x composante à traiter
   * @param x0 composante x au pas de temps précédent
   * @param residu residu de la composante à traiter
   * @param residu0 residu au pas de temps précédent
   * @param diff_visc paramètre correspondant soit à la diffusion si la fonction est utilisée pour
   * la résolution du pas de densité, soit à la viscosité si elle est employée pour la résolution
   * du pas de vélocité
   */
  void diffuse_hybride (int b, double *const x, const double *const x0,
			double *const residu, double *const residu0,
			double diff_visc);

  /** Pas d'advection => déplacement du fluide sur lui-mÃªme.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param d composante à traiter
   * @param d0 composante d au pas de temps précédent
   * @param u vecteur de vélocité en u
   * @param v vecteur de vélocité en v
   * @param w vecteur de vélocité en w
   */
  void advect (int b, double *const d, const double *const d0,
	       const double *const u, const double *const v,
	       const double *const w);

  /** Pas de projection pour garantir la conservation de la masse.
   * Les tableaux passés en paramètre sont modifiés ici et ne doivent donc plus servir après l'appel de la projection
   */
  void project (double *const p, double *const div);

  /** Pas de résolution de la densité. */
  void dens_step ();

  /** Pas de résolution de la vélocité. */
  virtual void vel_step ();

  /** Pas de résolution de la vélocité, en utilisant l'hybridation */
  void vel_step_hybride ();

  /** Nombre de voxels sur un côté du cube de résolution. */
  int N_x, N_y, N_z;
  
  /** Dimensions du solveur */
  double dim_x, dim_y, dim_z;

  /** Taille totale du cube en nombre de voxels, égal à (N+2)^3. */
  int size;
  double *u, *v, *w, *u_prev, *v_prev, *w_prev, *u_src, *v_src, *w_src;
  double *dens, *dens_prev, *dens_src;
  double *residu_u, *residu_u_prev, *residu_v, *residu_v_prev, *residu_w,
    *residu_w_prev;
  double *r, *z, *q, *p;
	
  /** Nombre de pas de résolutions de Gauss-seidel dans les méthodes de diffusion et de projection */
  int nb_step_gauss_seidel;

  /** Tableau de pointeurs vers les flammes.
   * A REVOIR : il existe de ce fait une relation circulaire entre la classe Solver et la classe Flame
   * qu'il serait bon se supprimer.
   */
  Flame **flammes;
  /** Nombre de flammes. */
  int nb_flammes;

  /** Nombre d'itérations pour Gauss-Seidel */
  int nb_iter;
  const static int nb_iter_flickering = 50;
  /** Viscosité cinématique de l'air 15*10E-6. */
  double visc;
  /** Diffusion. */
  double diff;
  /** Pas de temps. */
  double dt;
  
  double a_visc, a_diff;
};

#endif

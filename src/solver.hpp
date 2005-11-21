#if !defined(SOLVER_H)
#define SOLVER_H

class Solver;

#include "header.h"

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

  double getU (int i, int j, int k)
  {
    return m_u[IX (i, j, k)];
  };
  double getV (int i, int j, int k)
  {
    return m_v[IX (i, j, k)];
  };
  double getW (int i, int j, int k)
  {
    return m_w[IX (i, j, k)];
  };

  /** Ajout d'une force externe pour la composante U */
  void addUsrc (int i, int j, int k, double value)
  {
    m_uSrc[IX (i, j, k)] += value;
  };
  /** Ajout d'une force externe pour la composante V */
  void addVsrc (int i, int j, int k, double value)
  {
    m_vSrc[IX (i, j, k)] += value;
  };
  /** Ajout d'une force externe pour la composante W */
  void addWsrc (int i, int j, int k, double value)
  {
    m_wSrc[IX (i, j, k)] += value;
  };

  /** Affectation d'une force externe pour la composante V */
  void setUsrc (int i, int j, int k, double value)
  {
    m_uSrc[IX (i, j, k)] = value;
  };
  /** Affectation d'une force externe pour la composante V */
  void setVsrc (int i, int j, int k, double value)
  {
    m_vSrc[IX (i, j, k)] = value;
  };
  /** Ajout des forces externes.
   * @param x composante à traiter
   * @param src composante contenant les forces engendrées par les sources externes
   */
  void add_source (double *const x, double *const src);
  /** Remet à zéro toutes les forces externes */
  void cleanSources ();

  /** Retourne le nombre de voxels en X */
  int getXRes()
  {
    return m_nbVoxelsX;
  };
  /** Retourne le nombre de voxels en Y */
  int getYRes()
  {
    return m_nbVoxelsY;
  };
  /** Retourne le nombre de voxels en Z */
  int getZRes()
  {
    return m_nbVoxelsZ;
  };

  /** Retourne la dimension en X */
  double getDimX ()
  {
    return m_dimX;
  };
  /** Retourne la dimension en Y */
  double getDimY ()
  {
    return m_dimY;
  };
  /** Retourne la dimension en Z */
  double getDimZ ()
  {
    return m_dimZ;
  };

  /** Retourne le pas de temps */
  double getTimeStep()
  {
    return m_dt;
  };

  /** NOTE : Les 6 méthodes ci-dessus pourraient faire partie d'un objet hérité Grid3D par exemple */
  /** Fonction de construction de la display list de la grille du solveur */
  void buildDLGrid ();
  /** Fonction de construction de la display list du repère du solveur */
  void buildDLBase ();
  /** Fonction de dessin de la grille */
  void displayGrid (){
    glCallList (m_gridDisplayList);
  };
  /** Fonction de dessin du repère de base */
  void displayBase (){
    glCallList (m_baseDisplayList);
  };
  /** Fonction de dessin du champ de vélocité */
  void displayVelocityField (void);
  /** Fonction de dessin de la vélocité d'une cellule */
  void displayArrow (CVector * const direction);

  /* Retrouver la cellule où est située la particule */
  void findPointPosition(CPoint& p, int& i, int& j, int& k)
  {
    i = (int) (p.x * m_dimX * m_nbVoxelsX) + 1 + m_nbVoxelsX / 2;
    j = (int) (p.y * m_dimY * m_nbVoxelsY) + 1;
    k = (int) (p.z * m_dimZ * m_nbVoxelsZ) + 1 + m_nbVoxelsZ / 2;
  };


protected:
  int IX (int i, int j, int k)
  {
    return ((i) + (m_nbVoxelsX + 2) * (j) + (m_nbVoxelsX + 2) * (m_nbVoxelsY + 2) * (k));
  };
  int IX2 (int i)
  {
    int x,y,z,tmp,tmp2;
    tmp2=m_nbVoxelsX*m_nbVoxelsY;
    z=i/tmp2;
    tmp=i-z*tmp2;
    y=tmp/m_nbVoxelsX;
    x=tmp-y*m_nbVoxelsX;
    
    return( IX( x+1, y+1, z+1 ) );
   //int n=i/m_nbVoxelsX; /* Nombre de lignes dans la grille */
    
    //return ( (m_nbVoxelsX+2)*(m_nbVoxelsY+2) + m_nbVoxelsX+3 + n/m_nbVoxelsY * ((m_nbVoxelsX+2)*(m_nbVoxelsY+2)) + (n % m_nbVoxelsX) *2 + (i % (m_nbVoxelsX*m_nbVoxelsY) ) );
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
  int m_nbVoxelsX, m_nbVoxelsY, m_nbVoxelsZ;
  
  /** Dimensions du solveur */
  double m_dimX, m_dimY, m_dimZ;

  /** Taille totale du cube en nombre de voxels, égal à (N+2)^3. */
  int m_nbVoxels;
  double *m_u, *m_v, *m_w, *m_uPrev, *m_vPrev, *m_wPrev, *m_uSrc, *m_vSrc, *m_wSrc;
  double *m_dens, *m_densPrev, *m_densSrc;
  double *m_uResidu, *m_uPrevResidu, *m_vResidu, *m_vPrevResidu, *m_wResidu, *m_wPrevResidu;
  double *m_r, *m_z, *m_q, *m_p;
	
  /** Nombre de pas de résolutions de Gauss-seidel dans les méthodes de diffusion et de projection */
  int m_nbStepsGS;
  
  /** Nombre d'itérations pour Gauss-Seidel */
  int m_nbIter;
  const static int m_nbIterFlickering = 50;
  /** Viscosité cinématique de l'air 15*10E-6. */
  double m_visc;
  /** Diffusion. */
  double m_diff;
  /** Pas de temps. */
  double m_dt;
  
  double m_aVisc, m_aDiff;

  GLuint m_gridDisplayList,  m_baseDisplayList;
};

#endif

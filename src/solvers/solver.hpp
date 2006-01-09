#if !defined(SOLVER_H)
#define SOLVER_H

#define SWAP(x0,x) {double *tmp=x0;x0=x;x=tmp;}
// Pour le calcul du d�grad� de couleur des pyramides
#define VELOCITE_MAX .2

#define RAD_TO_DEG 180/PI

#include <GL/gl.h>
#include <math.h>

#include "../common.hpp"
#include "../vector.hpp"

class Solver;

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
  /** Constructeur par d�faut n�cessaire pour l'h�ritage multiple */
  Solver ();
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilis� pour la simulation
   */
  Solver (CPoint& position, int n_x, int n_y, int n_z, double dim, double pas_de_temps);
  virtual ~Solver ();

  /** Lance une it�ration du solveur. */
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
   * @param x composante � traiter
   * @param src composante contenant les forces engendr�es par les sources externes
   */
  void add_source (double *const x, double *const src);
  /** Remet � z�ro toutes les forces externes */
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

  /** NOTE : Les 6 m�thodes ci-dessus pourraient faire partie d'un objet h�rit� Grid3D par exemple */
  /** Fonction de construction de la display list de la grille du solveur */
  void buildDLGrid ();
  /** Fonction de construction de la display list du rep�re du solveur */
  void buildDLBase ();
  /** Fonction de dessin de la grille */
  void displayGrid (){
    glCallList (m_gridDisplayList);
  };
  /** Fonction de dessin du rep�re de base */
  void displayBase (){
    glCallList (m_baseDisplayList);
  };
  /** Fonction de dessin du champ de v�locit� */
  void displayVelocityField (void);
  /** Fonction de dessin de la v�locit� d'une cellule */
  void displayArrow (CVector * const direction);

  /* Retrouver la cellule o� est situ�e la particule */
  void findPointPosition(CPoint& p, int& i, int& j, int& k)
  {
    i = (int) (p.x * m_dimX * m_nbVoxelsX) + 1 + m_nbVoxelsX / 2;
    j = (int) (p.y * m_dimY * m_nbVoxelsY) + 1;
    k = (int) (p.z * m_dimZ * m_nbVoxelsZ) + 1 + m_nbVoxelsZ / 2;
  };
  
  CPoint& getPosition ()
  {
    return (m_position);
  };
  
  void moveTo(CPoint& position);
  
protected:
  int IX (int i, int j, int k)
  {  
    return( (i) + (m_nbVoxelsX + 2) * (j) + (m_nbVoxelsX + 2) * (m_nbVoxelsY + 2) * (k) );
  };

  /** Traitement de valeurs aux bords du solveur.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param x composante � traiter
   */
  void set_bnd (int b, double *const x);
  
  /** Pas de diffusion.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param x composante � traiter
   * @param x0 composante x au pas de temps pr�c�dent
   * @param diff_visc param�tre correspondant soit � la diffusion si la fonction est utilis�e pour
   * la r�solution du pas de densit�, soit � la viscosit� si elle est employ�e pour la r�solution
   * du pas de v�locit�
   */
  virtual void diffuse (int b, double *const x, double *const x0, double a, double diff_visc) = 0;
  
  /** Pas d'advection => d�placement du fluide sur lui-même.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param d composante � traiter
   * @param d0 composante d au pas de temps pr�c�dent
   * @param u vecteur de v�locit� en u
   * @param v vecteur de v�locit� en v
   * @param w vecteur de v�locit� en w
   */
  virtual void advect (int b, double *const d, const double *const d0,
		       const double *const u, const double *const v,
		       const double *const w);

  /** Pas de projection pour garantir la conservation de la masse.
   * Les tableaux pass�s en param�tre sont modifi�s ici et ne doivent donc plus servir apr�s l'appel de la projection
   */
  virtual void project (double *const p, double *const div) = 0;

  /** Pas de r�solution de la densit�. */
  void dens_step ();

  /** Pas de r�solution de la v�locit�. */
  virtual void vel_step ();

  /** Nombre de voxels sur un c�t� du cube de r�solution. */
  int m_nbVoxelsX, m_nbVoxelsY, m_nbVoxelsZ;
  
  /** Dimensions du solveur */
  double m_dimX, m_dimY, m_dimZ;

  /** Position du solveur dans l'espace */
  CPoint m_position;

  /** Taille totale du cube en nombre de voxels, �gal � (N+2)^3. */
  int m_nbVoxels;
  double *m_u, *m_v, *m_w, *m_uPrev, *m_vPrev, *m_wPrev, *m_uSrc, *m_vSrc, *m_wSrc;
  double *m_dens, *m_densPrev, *m_densSrc;
  
  /** Nombre d'it�rations */
  int m_nbIter;
  const static int m_nbIterFlickering = 50;
  /** Viscosit� cin�matique de l'air 15*10E-6. */
  double m_visc;
  /** Diffusion. */
  double m_diff;
  /** Pas de temps. */
  double m_dt;
  
  double m_aVisc, m_aDiff;

  GLuint m_gridDisplayList,  m_baseDisplayList;

  /** Nombre de pas de r�solutions dans les m�thodes de diffusion et de projection */
  int m_nbSteps;
};

#endif

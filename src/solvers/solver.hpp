#if !defined(SOLVER_H)
#define SOLVER_H

#define SWAP(x0,x) {double *tmp=x0;x0=x;x=tmp;}
// Pour le calcul du d�grad� de couleur des pyramides
#define VELOCITE_MAX .2

#define RAD_TO_DEG 180/PI

#include <GL/gl.h>
#include <math.h>

#include "../common.hpp"

class Solver;

/** La classe Solver propose une impl�mentation 3D de la m�thode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre mod�le de flamme, le champ de densit� n'est pas utilis�, ce qui,
 * physiquement semble assez curieux (d'apr�s Alexei). N�anmoins Stam pr�sente bien les choses ainsi dans
 * son code initial, la densit� d�pend de la v�locit�, la v�locit� �volue ind�pendament.
 * 
 * @author	Flavien Bridault et Michel Lebond.
 */
class Solver
{
public:
  /** Constructeur par d�faut n�cessaire pour l'h�ritage multiple */
  Solver ();
  
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x R�solution de la grille en x.
   * @param n_y R�solution de la grille en y.
   * @param n_z R�solution de la grille en z.
   * @param dim Dimension du solveur, utilis� pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilis� pour la simulation.
   * @param buoyancy Intensit� de la force de flottabilit� dans le solveur.
   */
  Solver (Point& position, uint n_x, uint n_y, uint n_z, double dim, 
	  double timeStep, double buoyancy);
  /** Destructeur */
  virtual ~Solver ();
  
  /** Lance une it�ration du solveur. */
  virtual void iterate ();

  /** R�cup�ration d'une valeur de la composante horizontale de la v�locit� dans la grille du solveur.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   * @return Valeur de v�locit�.
   */   
  double getU (uint i, uint j, uint k)
  {
    return m_u[IX (i, j, k)];
  };
  
  /** R�cup�ration d'une valeur de la composante verticale de la v�locit� dans la grille du solveur.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   * @return Valeur de v�locit�.
   */   
  double getV (uint i, uint j, uint k)
  {
    return m_v[IX (i, j, k)];
  };
  
  /** R�cup�ration d'une valeur de la composante verticale de la v�locit� dans la grille du solveur.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   * @return Valeur de v�locit�.
   */   
  double getW (uint i, uint j, uint k)
  {
    return m_w[IX (i, j, k)];
  };
  
  /** Ajout d'une force externe pour la composante U.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   * @param value Valeur de v�locit� � ajouter.
   */
  void addUsrc (uint i, uint j, uint k, double value)
  {
    m_uSrc[IX (i, j, k)] += value*m_nbVoxelsX;
  };
  /** Ajout d'une force externe pour la composante V.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   * @param value Valeur de v�locit� � ajouter.
   */
  void addVsrc (uint i, uint j, uint k, double value)
  {
    m_vSrc[IX (i, j, k)] += value;
  };
  /** Ajout d'une force externe pour la composante W.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   * @param value Valeur de v�locit� � ajouter.
   */
  void addWsrc (uint i, uint j, uint k, double value)
  {
    m_wSrc[IX (i, j, k)] += value*m_nbVoxelsX;
  };

  /** Affectation d'une force externe pour la composante U.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   * @param value Valeur de v�locit� � ajouter.
   */
  void setUsrc (uint i, uint j, uint k, double value)
  {
    m_uSrc[IX (i, j, k)] = value*m_nbVoxelsX;
  };
  
  /** Affectation d'une force externe pour la composante V.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   * @param value Valeur de v�locit� � ajouter.
   */
  void setVsrc (uint i, uint j, uint k, double value)
  {
    m_vSrc[IX (i, j, k)] = value*m_nbVoxelsX;
  };
  
  /** Ajout des forces externes.
   * @param x Composante � traiter.
   * @param src Composante contenant les forces engendr�es par les sources externes.
   */
  void add_source (double *const x, double *const src);
  
  /** Remet � z�ro toutes les forces externes */
  void cleanSources ();

  /** Retourne le nombre de voxels en X.
   * @return Nombre de voxels.
   */
  uint getXRes()
  {
    return m_nbVoxelsX;
  };
  
  /** Retourne le nombre de voxels en Y.
   * @return Nombre de voxels.
   */
  uint getYRes()
  {
    return m_nbVoxelsY;
  };
  
  /** Retourne le nombre de voxels en Z.
   * @return Nombre de voxels.
   */
  uint getZRes()
  {
    return m_nbVoxelsZ;
  };

  /** Retourne la dimension en X.
   * @return Dimension.
   */
  double getDimX ()
  {
    return m_dimX;
  };
  /** Retourne la dimension en Y.
   * @return Dimension.
   */
  double getDimY ()
  {
    return m_dimY;
  };
  /** Retourne la dimension en Z.
   * @return Dimension.
   */
  double getDimZ ()
  {
    return m_dimZ;
  };

  /** Retourne le pas de temps.
   * @return Valeur du pas de temps en ms.
   */
  double getTimeStep()
  {
    return m_dt;
  };

  /** Retourne l'it�ration en cours.
   * @return Num�ro de l'it�ration.
   */
  double getNbIter()
  {
    return m_nbIter;
  };
  
  /** @todo : Les 6 m�thodes ci-apr�s pourraient faire partie d'un objet h�rit� Grid3D par exemple : 
   * buildDLGrid (), buildDLBase (), displayGrid (), displayBase (),  displayVelocityField, 
   * displayArrow (Vector * const direction).
   */
  
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
  void displayArrow (Vector * const direction);
  
  /** M�thode permettant de retrouver les indices (i,j,k) de la cellule o� est situ�e la particule.
   * @param p Point dans l'espace.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   */
  void findPointPosition(Point& p, uint& i, uint& j, uint& k)
  {
    i = (uint) (p.x * m_nbVoxelsX) + 1 + m_halfNbVoxelsX;
    j = (uint) (p.y * m_nbVoxelsY) + 1;
    k = (uint) (p.z * m_nbVoxelsZ) + 1 + m_halfNbVoxelsZ;
  };
  
  /** Retourne la position du solveur dans le rep�re du monde 
   * @return Position dans l'espace.
   */
  Point& getPosition ()
  {
    return (m_position);
  };
  
  /** Ajoute de fa�on ponctuelle des forces externes sur une des faces du solveur. Cette m�thode
   * est utilis�e principalement lorsque qu'une flamme est d�plac�e.
   * @param position Nouvelle position du solveur. D�termine l'intensit� de la force.
   * @param move Si true, alors le solveur est en plus d�plac� � la position pass�e en param�tre.
   */
  void addExternalForces(Point& position, bool move);
  
  
  /** Ajoute de fa�on permanente des forces externes sur une des faces du solveur. Cette m�thode
   * est utilis�e principalement lorsque que du vent est appliqu� sur une flamme.
   * @param forces Intensit� de la force en (x,y,z).
   */
  void addPermanentExternalForces(Point& forces)
  {
    permanentExternalForces = forces;
    if(!forces.x && !forces.y && !forces.z)
      arePermanentExternalForces = false;
    else
      arePermanentExternalForces = true;
  }
  
  /** Modifie la force de flottabilit� dans le solveur
   * @param value Nouvelle valeur.
   */
  virtual void setBuoyancy(double value){ m_buoyancy=value; };
  
  /** Divise la r�solution de la grille par 2 */
  virtual void divideRes () {} ;

  /** Multiplie la r�solution de la grille par 2 */
  virtual void multiplyRes () {} ;
  
  /** Diminue la r�solution de la grille de un voxel */
  virtual void decreaseRes () {} ;
  
  /** Augmente la r�solution de la grille de un voxel */
  virtual void increaseRes () {} ;
  
protected:
  /** M�thode permettant de simplifier l'indexage d'un voxel.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   * @return Indice � utiliser pour une des composantes de v�locit� {m_u,m_v,m_w}.
   */
  uint IX (uint i, uint j, uint k)
  {  
    return( (i) + (m_nbVoxelsX + 2) * (j) + (m_nbVoxelsX + 2) * (m_nbVoxelsY + 2) * (k) );
  };
  
  /** M�thode permettant de simplifier l'indexage d'un voxel dans une grille de taille
   * deux fois inf�rieure � la grille courante.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   * @return Indice � utiliser pour une des composantes de v�locit� {m_u,m_v,m_w}.
   */
  uint IX2h (int i, int j, int k)
  {  
    return( (i) + (m_nbVoxelsX/2 + 2) * (j) + (m_nbVoxelsX/2 + 2) * (m_nbVoxelsY/2 + 2) * (k) );
  }; 
  
  /** Traitement de valeurs aux bords du solveur.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w.
   * @param x Composante de la v�locit� {m_u,m_v,m_w} � traiter.
   */
  void set_bnd (unsigned char b, double *const x);
  
  /** Etape de diffusion.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w.
   * @param x Composante � traiter.
   * @param x0 Composante x au pas de temps pr�c�dent.
   * @param a Valeur des coefficients dans la matrice A
   * @param diff_visc Param�tre correspondant soit � la diffusion si la fonction est utilis�e pour
   * la r�solution de la densit�, soit � la viscosit� si elle est employ�e pour la r�solution
   * de la v�locit�
   */
  virtual void diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc) = 0;
  
  /** Pas d'advection => d�placement du fluide sur lui-m�me.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param d Xomposante � traiter.
   * @param d0 Composante d au pas de temps pr�c�dent
   * @param u Vecteur de v�locit� en u
   * @param v Vecteur de v�locit� en v
   * @param w Vecteur de v�locit� en w
   */
  virtual void advect (unsigned char b, double *const d, const double *const d0,
		       const double *const u, const double *const v, const double *const w);

  /** Pas de projection pour garantir la conservation de la masse.
   * Les tableaux pass�s en param�tre sont modifi�s ici et ne doivent donc plus servir apr�s l'appel de la projection
   * @param p Champ de v�locit�.
   * @param div Divergence.
   */
  virtual void project (double *const p, double *const div) = 0;
  
  /** Pas de r�solution de la densit�. */
  void dens_step ();
  
  /** Pas de r�solution de la v�locit�. */
  virtual void vel_step ();
  
  /** Prolonger sur une grille fine un vecteur d�fini sur une grille grossi�re.
   * On utilise une interpolation bilin�aire.
   * La grille fine est de taille nx*ny*nz
   * @param vgros vecteur connu de taille (nx/2)*(ny/2)*(nz/2).
   * @param vfin vecteur r�sultat de la prolongation de taille nx*ny*nz.
   * @author Michel Leblond.
   */
  void prolonger(double *const vgros, double *const vfin);

  /** Restreindre sur une grille grossi�re un vecteur d�fini sur une grille fine.
   * On utilise une moyenne pond�r�e : sch�ma en 27 points.
   * La grille fine est de taille nx*ny*nz.
   * @param vfin vecteur connu de taille nx*ny*nz
   * @param vgros vecteur r�sultat de la restriction de taille (nx/2)*(ny/2)*(nz/2)
   * @author Michel Leblond.
   */
  void restreindre(double *const vfin, double *const vgros);
  
  /** Nombre de voxels en X sur un c�t� du cube. */
  uint m_nbVoxelsX;
  /** Nombre de voxels en Y sur un c�t� du cube. */
  uint m_nbVoxelsY;
  /** Nombre de voxels en Z sur un c�t� du cube. */
  uint m_nbVoxelsZ;
  
  /** Dimension du solveur en X. */
  double m_dimX;
  /** Dimension du solveur en Y. */
  double m_dimY;
  /** Dimension du solveur en Z. */
  double m_dimZ;

  double m_dimXTimesNbVoxelsX,  m_dimXTimesNbVoxelsY,  m_dimXTimesNbVoxelsZ;
  uint m_halfNbVoxelsX,  m_halfNbVoxelsZ;

  /** Position du solveur dans l'espace */
  Point m_position;

  /** Taille totale du cube en nombre de voxels, �gal � (N+2)^3. */
  uint m_nbVoxels;
  double *m_u, *m_v, *m_w, *m_uPrev, *m_vPrev, *m_wPrev, *m_uSrc, *m_vSrc, *m_wSrc;
  double *m_dens, *m_densPrev, *m_densSrc;
  
  /** Intensit� de la force de flottabilit� */
  double m_buoyancy;

  /** Nombre d'it�rations */
  uint m_nbIter;
  /** Viscosit� cin�matique de l'air 15*10E-6. */
  double m_visc;
  /** Diffusion. */
  double m_diff;
  /** Pas de temps. */
  double m_dt;
  
  double m_aVisc, m_aDiff;

  /** Display list de la grille du solveur. */
  GLuint m_gridDisplayList;
  /** Display list de la base de la grille du solveur. */
  GLuint m_baseDisplayList;

  /** Nombre de pas de r�solutions dans les m�thodes de diffusion et de projection */
  uint m_nbSteps;
  
  uint t, n2, nx, t1, t2nx;
  
  Point permanentExternalForces;
  bool arePermanentExternalForces;
};

#endif

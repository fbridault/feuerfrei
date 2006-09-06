#if !defined(SOLVER_H)
#define SOLVER_H

#define SWAP(x0,x) {double *tmp=x0;x0=x;x=tmp;}
// Pour le calcul du dégradé de couleur des pyramides
#define VELOCITE_MAX .2

#define RAD_TO_DEG 180/PI

#include <GL/gl.h>
#include <math.h>

#include "../common.hpp"

class Solver;

/** La classe Solver propose une implémentation 3D de la méthode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre modèle de flamme, le champ de densité n'est pas utilisé, ce qui,
 * physiquement semble assez curieux (d'après Alexei). Néanmoins Stam présente bien les choses ainsi dans
 * son code initial, la densité dépend de la vélocité, la vélocité évolue indépendament.
 * 
 * @author	Flavien Bridault et Michel Lebond.
 */
class Solver
{
public:
  /** Constructeur par défaut nécessaire pour l'héritage multiple */
  Solver ();
  
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x Résolution de la grille en x.
   * @param n_y Résolution de la grille en y.
   * @param n_z Résolution de la grille en z.
   * @param dim Dimension du solveur, utilisé pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilisé pour la simulation.
   * @param buoyancy Intensité de la force de flottabilité dans le solveur.
   */
  Solver (Point& position, uint n_x, uint n_y, uint n_z, double dim, 
	  double timeStep, double buoyancy);
  /** Destructeur */
  virtual ~Solver ();
  
  /** Lance une itération du solveur. */
  virtual void iterate ();

  /** Récupération d'une valeur de la composante horizontale de la vélocité dans la grille du solveur.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param k Indice en profondeur (z).
   * @return Valeur de vélocité.
   */   
  double getU (uint i, uint j, uint k)
  {
    return m_u[IX (i, j, k)];
  };
  
  /** Récupération d'une valeur de la composante verticale de la vélocité dans la grille du solveur.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param k Indice en profondeur (z).
   * @return Valeur de vélocité.
   */   
  double getV (uint i, uint j, uint k)
  {
    return m_v[IX (i, j, k)];
  };
  
  /** Récupération d'une valeur de la composante verticale de la vélocité dans la grille du solveur.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param k Indice en profondeur (z).
   * @return Valeur de vélocité.
   */   
  double getW (uint i, uint j, uint k)
  {
    return m_w[IX (i, j, k)];
  };
  
  /** Ajout d'une force externe pour la composante U.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param k Indice en profondeur (z).
   * @param value Valeur de vélocité à ajouter.
   */
  void addUsrc (uint i, uint j, uint k, double value)
  {
    m_uSrc[IX (i, j, k)] += value*m_nbVoxelsX;
  };
  /** Ajout d'une force externe pour la composante V.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param k Indice en profondeur (z).
   * @param value Valeur de vélocité à ajouter.
   */
  void addVsrc (uint i, uint j, uint k, double value)
  {
    m_vSrc[IX (i, j, k)] += value;
  };
  /** Ajout d'une force externe pour la composante W.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param k Indice en profondeur (z).
   * @param value Valeur de vélocité à ajouter.
   */
  void addWsrc (uint i, uint j, uint k, double value)
  {
    m_wSrc[IX (i, j, k)] += value*m_nbVoxelsX;
  };

  /** Affectation d'une force externe pour la composante U.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param k Indice en profondeur (z).
   * @param value Valeur de vélocité à ajouter.
   */
  void setUsrc (uint i, uint j, uint k, double value)
  {
    m_uSrc[IX (i, j, k)] = value*m_nbVoxelsX;
  };
  
  /** Affectation d'une force externe pour la composante V.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param k Indice en profondeur (z).
   * @param value Valeur de vélocité à ajouter.
   */
  void setVsrc (uint i, uint j, uint k, double value)
  {
    m_vSrc[IX (i, j, k)] = value*m_nbVoxelsX;
  };
  
  /** Ajout des forces externes.
   * @param x Composante à traiter.
   * @param src Composante contenant les forces engendrées par les sources externes.
   */
  void add_source (double *const x, double *const src);
  
  /** Remet à zéro toutes les forces externes */
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

  /** Retourne l'itération en cours.
   * @return Numéro de l'itération.
   */
  double getNbIter()
  {
    return m_nbIter;
  };
  
  /** @todo : Les 6 méthodes ci-après pourraient faire partie d'un objet hérité Grid3D par exemple : 
   * buildDLGrid (), buildDLBase (), displayGrid (), displayBase (),  displayVelocityField, 
   * displayArrow (Vector * const direction).
   */
  
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
  void displayArrow (Vector * const direction);
  
  /** Méthode permettant de retrouver les indices (i,j,k) de la cellule où est située la particule.
   * @param p Point dans l'espace.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param k Indice en profondeur (z).
   */
  void findPointPosition(Point& p, uint& i, uint& j, uint& k)
  {
    i = (uint) (p.x * m_nbVoxelsX) + 1 + m_halfNbVoxelsX;
    j = (uint) (p.y * m_nbVoxelsY) + 1;
    k = (uint) (p.z * m_nbVoxelsZ) + 1 + m_halfNbVoxelsZ;
  };
  
  /** Retourne la position du solveur dans le repère du monde 
   * @return Position dans l'espace.
   */
  Point& getPosition ()
  {
    return (m_position);
  };
  
  /** Ajoute de façon ponctuelle des forces externes sur une des faces du solveur. Cette méthode
   * est utilisée principalement lorsque qu'une flamme est déplacée.
   * @param position Nouvelle position du solveur. Détermine l'intensité de la force.
   * @param move Si true, alors le solveur est en plus déplacé à la position passée en paramètre.
   */
  void addExternalForces(Point& position, bool move);
  
  
  /** Ajoute de façon permanente des forces externes sur une des faces du solveur. Cette méthode
   * est utilisée principalement lorsque que du vent est appliqué sur une flamme.
   * @param forces Intensité de la force en (x,y,z).
   */
  void addPermanentExternalForces(Point& forces)
  {
    permanentExternalForces = forces;
    if(!forces.x && !forces.y && !forces.z)
      arePermanentExternalForces = false;
    else
      arePermanentExternalForces = true;
  }
  
  /** Modifie la force de flottabilité dans le solveur
   * @param value Nouvelle valeur.
   */
  virtual void setBuoyancy(double value){ m_buoyancy=value; };
  
  /** Divise la résolution de la grille par 2 */
  virtual void divideRes () {} ;

  /** Multiplie la résolution de la grille par 2 */
  virtual void multiplyRes () {} ;
  
  /** Diminue la résolution de la grille de un voxel */
  virtual void decreaseRes () {} ;
  
  /** Augmente la résolution de la grille de un voxel */
  virtual void increaseRes () {} ;
  
protected:
  /** Méthode permettant de simplifier l'indexage d'un voxel.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param k Indice en profondeur (z).
   * @return Indice à utiliser pour une des composantes de vélocité {m_u,m_v,m_w}.
   */
  uint IX (uint i, uint j, uint k)
  {  
    return( (i) + (m_nbVoxelsX + 2) * (j) + (m_nbVoxelsX + 2) * (m_nbVoxelsY + 2) * (k) );
  };
  
  /** Méthode permettant de simplifier l'indexage d'un voxel dans une grille de taille
   * deux fois inférieure à la grille courante.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param k Indice en profondeur (z).
   * @return Indice à utiliser pour une des composantes de vélocité {m_u,m_v,m_w}.
   */
  uint IX2h (int i, int j, int k)
  {  
    return( (i) + (m_nbVoxelsX/2 + 2) * (j) + (m_nbVoxelsX/2 + 2) * (m_nbVoxelsY/2 + 2) * (k) );
  }; 
  
  /** Traitement de valeurs aux bords du solveur.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w.
   * @param x Composante de la vélocité {m_u,m_v,m_w} à traiter.
   */
  void set_bnd (unsigned char b, double *const x);
  
  /** Etape de diffusion.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w.
   * @param x Composante à traiter.
   * @param x0 Composante x au pas de temps précédent.
   * @param a Valeur des coefficients dans la matrice A
   * @param diff_visc Paramètre correspondant soit à la diffusion si la fonction est utilisée pour
   * la résolution de la densité, soit à la viscosité si elle est employée pour la résolution
   * de la vélocité
   */
  virtual void diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc) = 0;
  
  /** Pas d'advection => déplacement du fluide sur lui-même.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param d Xomposante à traiter.
   * @param d0 Composante d au pas de temps précédent
   * @param u Vecteur de vélocité en u
   * @param v Vecteur de vélocité en v
   * @param w Vecteur de vélocité en w
   */
  virtual void advect (unsigned char b, double *const d, const double *const d0,
		       const double *const u, const double *const v, const double *const w);

  /** Pas de projection pour garantir la conservation de la masse.
   * Les tableaux passés en paramètre sont modifiés ici et ne doivent donc plus servir après l'appel de la projection
   * @param p Champ de vélocité.
   * @param div Divergence.
   */
  virtual void project (double *const p, double *const div) = 0;
  
  /** Pas de résolution de la densité. */
  void dens_step ();
  
  /** Pas de résolution de la vélocité. */
  virtual void vel_step ();
  
  /** Prolonger sur une grille fine un vecteur défini sur une grille grossière.
   * On utilise une interpolation bilinéaire.
   * La grille fine est de taille nx*ny*nz
   * @param vgros vecteur connu de taille (nx/2)*(ny/2)*(nz/2).
   * @param vfin vecteur résultat de la prolongation de taille nx*ny*nz.
   * @author Michel Leblond.
   */
  void prolonger(double *const vgros, double *const vfin);

  /** Restreindre sur une grille grossière un vecteur défini sur une grille fine.
   * On utilise une moyenne pondérée : schèma en 27 points.
   * La grille fine est de taille nx*ny*nz.
   * @param vfin vecteur connu de taille nx*ny*nz
   * @param vgros vecteur résultat de la restriction de taille (nx/2)*(ny/2)*(nz/2)
   * @author Michel Leblond.
   */
  void restreindre(double *const vfin, double *const vgros);
  
  /** Nombre de voxels en X sur un côté du cube. */
  uint m_nbVoxelsX;
  /** Nombre de voxels en Y sur un côté du cube. */
  uint m_nbVoxelsY;
  /** Nombre de voxels en Z sur un côté du cube. */
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

  /** Taille totale du cube en nombre de voxels, égal à (N+2)^3. */
  uint m_nbVoxels;
  double *m_u, *m_v, *m_w, *m_uPrev, *m_vPrev, *m_wPrev, *m_uSrc, *m_vSrc, *m_wSrc;
  double *m_dens, *m_densPrev, *m_densSrc;
  
  /** Intensité de la force de flottabilité */
  double m_buoyancy;

  /** Nombre d'itérations */
  uint m_nbIter;
  /** Viscosité cinématique de l'air 15*10E-6. */
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

  /** Nombre de pas de résolutions dans les méthodes de diffusion et de projection */
  uint m_nbSteps;
  
  uint t, n2, nx, t1, t2nx;
  
  Point permanentExternalForces;
  bool arePermanentExternalForces;
};

#endif

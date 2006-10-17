#if !defined(SOLVER3D_H)
#define SOLVER3D_H

#include "solver.hpp"

class Solver;

/** La classe Solver3D propose une implémentation 3D de la méthode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre modèle de flamme, le champ de densité n'est pas utilisé, ce qui,
 * physiquement semble assez curieux (d'après Alexei). 
 * 
 * @author	Flavien Bridault et Michel Lebond.
 */
class Solver3D : public Solver
{
public:
  /** Constructeur par défaut nécessaire pour l'héritage multiple */
  Solver3D ();
  
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x Résolution de la grille en x.
   * @param n_y Résolution de la grille en y.
   * @param n_z Résolution de la grille en z.
   * @param dim Dimension du solveur, utilisé pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilisé pour la simulation.
   * @param buoyancy Intensité de la force de flottabilité dans le solveur.
   */
  Solver3D (Point& position, uint n_x, uint n_y, uint n_z, double dim, 
	    double timeStep, double buoyancy);
  /** Destructeur */
  virtual ~Solver3D ();
  
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
  
  /** Divise la résolution de la grille par 2 */
  virtual void divideRes () {} ;

  /** Multiplie la résolution de la grille par 2 */
  virtual void multiplyRes () {} ;
  
  /** Diminue la résolution de la grille de un voxel */
  virtual void decreaseRes () {} ;
  
  /** Augmente la résolution de la grille de un voxel */
  virtual void increaseRes () {} ;
  
  /** Fonction de dessin du champ de vélocité */
  void displayVelocityField (void);
  
  /** Fonction de dessin du champ de densité */
  void displayDensityField (void) {};
  
  /** Fonction de dessin de la vélocité d'une cellule */
  void displayArrow (Vector& direction);
  
protected:
  /** Fonction de construction de la display list de la grille du solveur */
  void buildDLGrid ();
  
  /** Fonction de construction de la display list du repère du solveur */
  void buildDLBase ();

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
  
  /** Pas de résolution de la densité. */
  //void dens_step ();
  
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

  double m_dimXTimesNbVoxelsX,  m_dimYTimesNbVoxelsY,  m_dimZTimesNbVoxelsZ;
  uint m_halfNbVoxelsX,  m_halfNbVoxelsZ;

  double *m_u, *m_v, *m_w, *m_uPrev, *m_vPrev, *m_wPrev, *m_uSrc, *m_vSrc, *m_wSrc;
  double *m_dens, *m_densPrev, *m_densSrc;
  
  uint n2, t2nx;
};

#endif

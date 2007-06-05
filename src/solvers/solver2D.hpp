#if !defined(SOLVER2D_H)
#define SOLVER2D_H

class Solver2D;

#include "field.hpp"
#include "solver.hpp"
#include <math.h>

class Solver;
/** La classe Solver2D propose une implémentation 3D de la méthode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre modèle de flamme, le champ de densité n'est pas utilisé, ce qui,
 * physiquement semble assez curieux (d'après Alexei). 
 * 
 * @author	Flavien Bridault et Michel Lebond.
 */
class Solver2D : public Field, public Solver
{
public:
  /** Constructeur par défaut nécessaire pour l'héritage multiple */
  Solver2D ();
  
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x Résolution de la grille en x.
   * @param n_y Résolution de la grille en y.
   * @param dim Dimension du solveur, utilisé pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilisé pour la simulation.
   * @param buoyancy Intensité de la force de flottabilité dans le solveur.
   */
  Solver2D (const Point& position, uint n_x, uint n_y, float dim, float timeStep, float buoyancy);
  /** Destructeur */
  virtual ~Solver2D ();
  
  /** Lance une itération du solveur. */
  virtual void iterate ();

  /** Récupération d'une valeur de la composante horizontale de la vélocité dans la grille du solveur.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @return Valeur de vélocité.
   */   
  float getU (uint i, uint j) const
  {
    return m_u[IX (i, j)];
  };
  
  /** Récupération d'une valeur de la composante verticale de la vélocité dans la grille du solveur.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @return Valeur de vélocité.
   */   
  float getV (uint i, uint j) const
  {
    return m_v[IX (i, j)];
  };
  
  /** Récupération d'une valeur de la composante verticale de la vélocité dans la grille du solveur.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @return Valeur de vélocité.
   */   
  float getDens (uint i, uint j) const
  {
    return m_dens[IX (i, j)];
  };
  
  /** Ajout d'une force externe pour la composante U.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param value Valeur de vélocité à ajouter.
   */
  void addUsrc (uint i, uint j, float value)
  {
    m_uSrc[IX (i, j)] += value*m_nbVoxelsX;
  };
  /** Ajout d'une force externe pour la composante V.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param value Valeur de vélocité à ajouter.
   */
  void addVsrc (uint i, uint j, float value)
  {
    m_vSrc[IX (i, j)] += value*m_nbVoxelsY;
  };
  /** Ajout d'une force externe pour la composante W.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param k Indice en profondeur (z).
   * @param value Valeur de vélocité à ajouter.
   */
  void addDensSrc (uint i, uint j, float value)
  {
    m_densSrc[IX (i, j)] += value*m_nbVoxels;
  };

  /** Affectation d'une force externe pour la composante U.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param value Valeur de vélocité à ajouter.
   */
  void setUsrc (uint i, uint j, float value)
  {
    m_uSrc[IX (i, j)] = value;
  };
  
  /** Affectation d'une force externe pour la composante V.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param value Valeur de vélocité à ajouter.
   */
  void setVsrc (uint i, uint j, float value)
  {
    m_vSrc[IX (i, j)] = value;
  };
    
  /** Remet à zéro toutes les forces externes */
  void cleanSources ();
  
  /** Méthode permettant de retrouver les indices (i,j,k) de la cellule où est située la particule.
   * @param p Point dans l'espace.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param k Indice en profondeur (z).
   */
  void findPointPosition(const Point& p, uint& i, uint& j)
  {
    i = (uint) (p.x * m_nbVoxelsX) + 1 + m_halfNbVoxelsX;
    j = (uint) (p.y * m_nbVoxelsY) + 1;
  };
  
  /** Ajoute de façon ponctuelle des forces externes sur une des faces du solveur. Cette méthode
   * est utilisée principalement lorsque qu'une flamme est déplacée.
   * @param position Nouvelle position du solveur. Détermine l'intensité de la force.
   * @param move Si true, alors le solveur est en plus déplacé à la position passée en paramètre.
   */
  void addExternalForces(const Point& position, bool move);
  
  void addDensity(int id);
  
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
  void displayDensityField (void);
  
  /** Fonction de dessin de la vélocité d'une cellule */
  void displayArrow (const Vector& direction);
  
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
  uint IX (uint i, uint j) const
  {  
    return( (i) + (m_nbVoxelsX + 2) * (j) );
  };
  
  /** Méthode permettant de simplifier l'indexage d'un voxel dans une grille de taille
   * deux fois inférieure à la grille courante.
   * @param i Indice à l'horizontale (x).
   * @param j Indice à la verticale (y).
   * @param k Indice en profondeur (z).
   * @return Indice à utiliser pour une des composantes de vélocité {m_u,m_v,m_w}.
   */
  uint IX2h (int i, int j, int k) const
  {  
    return( (i) + (m_nbVoxelsX/2 + 2) * (j) );
  }; 
  
  /** Traitement de valeurs aux bords du solveur.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w.
   * @param x Composante de la vélocité {m_u,m_v,m_w} à traiter.
   */
  void set_bnd (unsigned char b, float *const x);
  
  /** Pas d'advection => déplacement du fluide sur lui-même.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param d Xomposante à traiter.
   * @param d0 Composante d au pas de temps précédent
   * @param u Vecteur de vélocité en u
   * @param v Vecteur de vélocité en v
   * @param w Vecteur de vélocité en w
   */
  virtual void advect (unsigned char b, float *const d, const float *const d0,
		       const float *const u, const float *const v);
  
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
//   void prolonger(float *const vgros, float *const vfin);

  /** Restreindre sur une grille grossière un vecteur défini sur une grille fine.
   * On utilise une moyenne pondérée : schèma en 27 points.
   * La grille fine est de taille nx*ny*nz.
   * @param vfin vecteur connu de taille nx*ny*nz
   * @param vgros vecteur résultat de la restriction de taille (nx/2)*(ny/2)*(nz/2)
   * @author Michel Leblond.
   */
//   void restreindre(float *const vfin, float *const vgros);
  
  /** Nombre de voxels en X sur un côté du cube. */
  uint m_nbVoxelsX;
  /** Nombre de voxels en Y sur un côté du cube. */
  uint m_nbVoxelsY;
  
  /** Dimension du solveur en X. */
  float m_dimX;
  /** Dimension du solveur en Y. */
  float m_dimY;

  float m_nbVoxelsXDivDimX,  m_nbVoxelsYDivDimY;
  uint m_halfNbVoxelsX;
  
  float *m_u, *m_v, *m_uPrev, *m_vPrev, *m_uSrc, *m_vSrc;
  float *m_dens, *m_densPrev, *m_densSrc;
};

#endif

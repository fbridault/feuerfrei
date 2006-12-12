#if !defined(SOLVER2D_H)
#define SOLVER2D_H

class Solver2D;

#include "field.hpp"
#include "solver.hpp"
#include <math.h>

class Solver;
/** La classe Solver2D propose une impl�mentation 3D de la m�thode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre mod�le de flamme, le champ de densit� n'est pas utilis�, ce qui,
 * physiquement semble assez curieux (d'apr�s Alexei). 
 * 
 * @author	Flavien Bridault et Michel Lebond.
 */
class Solver2D : public Field, public Solver
{
public:
  /** Constructeur par d�faut n�cessaire pour l'h�ritage multiple */
  Solver2D ();
  
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x R�solution de la grille en x.
   * @param n_y R�solution de la grille en y.
   * @param dim Dimension du solveur, utilis� pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilis� pour la simulation.
   * @param buoyancy Intensit� de la force de flottabilit� dans le solveur.
   */
  Solver2D (Point& position, uint n_x, uint n_y, double dim, double timeStep, double buoyancy);
  /** Destructeur */
  virtual ~Solver2D ();
  
  /** Lance une it�ration du solveur. */
  virtual void iterate ();

  /** R�cup�ration d'une valeur de la composante horizontale de la v�locit� dans la grille du solveur.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @return Valeur de v�locit�.
   */   
  double getU (uint i, uint j) const
  {
    return m_u[IX (i, j)];
  };
  
  /** R�cup�ration d'une valeur de la composante verticale de la v�locit� dans la grille du solveur.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @return Valeur de v�locit�.
   */   
  double getV (uint i, uint j) const
  {
    return m_v[IX (i, j)];
  };
  
  /** R�cup�ration d'une valeur de la composante verticale de la v�locit� dans la grille du solveur.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @return Valeur de v�locit�.
   */   
  double getDens (uint i, uint j) const
  {
    return m_dens[IX (i, j)];
  };
  
  /** Ajout d'une force externe pour la composante U.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param value Valeur de v�locit� � ajouter.
   */
  void addUsrc (uint i, uint j, double value)
  {
    m_uSrc[IX (i, j)] += value*m_nbVoxelsX;
  };
  /** Ajout d'une force externe pour la composante V.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param value Valeur de v�locit� � ajouter.
   */
  void addVsrc (uint i, uint j, double value)
  {
    m_vSrc[IX (i, j)] += value*m_nbVoxelsY;
  };
  /** Ajout d'une force externe pour la composante W.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   * @param value Valeur de v�locit� � ajouter.
   */
  void addDensSrc (uint i, uint j, double value)
  {
    m_densSrc[IX (i, j)] += value*m_nbVoxels;
  };

  /** Affectation d'une force externe pour la composante U.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param value Valeur de v�locit� � ajouter.
   */
  void setUsrc (uint i, uint j, double value)
  {
    m_uSrc[IX (i, j)] = value;
  };
  
  /** Affectation d'une force externe pour la composante V.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param value Valeur de v�locit� � ajouter.
   */
  void setVsrc (uint i, uint j, double value)
  {
    m_vSrc[IX (i, j)] = value;
  };
    
  /** Remet � z�ro toutes les forces externes */
  void cleanSources ();
  
  /** M�thode permettant de retrouver les indices (i,j,k) de la cellule o� est situ�e la particule.
   * @param p Point dans l'espace.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   */
  void findPointPosition(Point& p, uint& i, uint& j)
  {
    i = (uint) (p.x * m_nbVoxelsX) + 1 + m_halfNbVoxelsX;
    j = (uint) (p.y * m_nbVoxelsY) + 1;
  };
  
  /** Ajoute de fa�on ponctuelle des forces externes sur une des faces du solveur. Cette m�thode
   * est utilis�e principalement lorsque qu'une flamme est d�plac�e.
   * @param position Nouvelle position du solveur. D�termine l'intensit� de la force.
   * @param move Si true, alors le solveur est en plus d�plac� � la position pass�e en param�tre.
   */
  void addExternalForces(Point& position, bool move);
  
  void addDensity(int id);
  
  /** Divise la r�solution de la grille par 2 */
  virtual void divideRes () {} ;

  /** Multiplie la r�solution de la grille par 2 */
  virtual void multiplyRes () {} ;
  
  /** Diminue la r�solution de la grille de un voxel */
  virtual void decreaseRes () {} ;
  
  /** Augmente la r�solution de la grille de un voxel */
  virtual void increaseRes () {} ;
  
  /** Fonction de dessin du champ de v�locit� */
  void displayVelocityField (void);
  
  /** Fonction de dessin du champ de densit� */
  void displayDensityField (void);
  
  /** Fonction de dessin de la v�locit� d'une cellule */
  void displayArrow (Vector& direction);
  
protected:
  /** Fonction de construction de la display list de la grille du solveur */
  void buildDLGrid ();
  
  /** Fonction de construction de la display list du rep�re du solveur */
  void buildDLBase ();
  
  /** M�thode permettant de simplifier l'indexage d'un voxel.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   * @return Indice � utiliser pour une des composantes de v�locit� {m_u,m_v,m_w}.
   */
  uint IX (uint i, uint j) const
  {  
    return( (i) + (m_nbVoxelsX + 2) * (j) );
  };
  
  /** M�thode permettant de simplifier l'indexage d'un voxel dans une grille de taille
   * deux fois inf�rieure � la grille courante.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   * @return Indice � utiliser pour une des composantes de v�locit� {m_u,m_v,m_w}.
   */
  uint IX2h (int i, int j, int k) const
  {  
    return( (i) + (m_nbVoxelsX/2 + 2) * (j) );
  }; 
  
  /** Traitement de valeurs aux bords du solveur.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w.
   * @param x Composante de la v�locit� {m_u,m_v,m_w} � traiter.
   */
  void set_bnd (unsigned char b, double *const x);
  
  /** Pas d'advection => d�placement du fluide sur lui-m�me.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param d Xomposante � traiter.
   * @param d0 Composante d au pas de temps pr�c�dent
   * @param u Vecteur de v�locit� en u
   * @param v Vecteur de v�locit� en v
   * @param w Vecteur de v�locit� en w
   */
  virtual void advect (unsigned char b, double *const d, const double *const d0,
		       const double *const u, const double *const v);
  
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
//   void prolonger(double *const vgros, double *const vfin);

  /** Restreindre sur une grille grossi�re un vecteur d�fini sur une grille fine.
   * On utilise une moyenne pond�r�e : sch�ma en 27 points.
   * La grille fine est de taille nx*ny*nz.
   * @param vfin vecteur connu de taille nx*ny*nz
   * @param vgros vecteur r�sultat de la restriction de taille (nx/2)*(ny/2)*(nz/2)
   * @author Michel Leblond.
   */
//   void restreindre(double *const vfin, double *const vgros);
  
  /** Nombre de voxels en X sur un c�t� du cube. */
  uint m_nbVoxelsX;
  /** Nombre de voxels en Y sur un c�t� du cube. */
  uint m_nbVoxelsY;
  
  /** Dimension du solveur en X. */
  double m_dimX;
  /** Dimension du solveur en Y. */
  double m_dimY;

  double m_dimXTimesNbVoxelsX,  m_dimYTimesNbVoxelsY;
  uint m_halfNbVoxelsX;
  
  double *m_u, *m_v, *m_uPrev, *m_vPrev, *m_uSrc, *m_vSrc;
  double *m_dens, *m_densPrev, *m_densSrc;
};

#endif

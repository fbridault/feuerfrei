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

/** La classe Solver propose une impl�mentation de la m�thode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre mod�le de flamme, le champ de densit� n'est pas utilis�, ce qui,
 * physiquement semble assez curieux (d'apr�s Alexei). 
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
   * @param timeStep Pas de temps utilis� pour la simulation.
   * @param buoyancy Intensit� de la force de flottabilit� dans le solveur.
   */
  Solver (Point& position, double timeStep, double buoyancy);
  /** Destructeur */
  virtual ~Solver ();
  
  /** Lance une it�ration du solveur. */
  virtual void iterate () = 0;
  
  /** Ajout des forces externes.
   * @param x Composante � traiter.
   * @param src Composante contenant les forces engendr�es par les sources externes.
   */
  virtual void add_source (double *const x, double *const src);
  
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
  virtual void addExternalForces(Point& position, bool move) = 0;
  
  /** Ajoute de fa�on permanente des forces externes sur une des faces du solveur. Cette m�thode
   * est utilis�e principalement lorsque que du vent est appliqu� sur une flamme.
   * @param forces Intensit� de la force en (x,y,z).
   */
  void addPermanentExternalForces(Point& forces)
  {
    permanentExternalForces = forces;
    if(!forces.x && !forces.y)
      arePermanentExternalForces = false;
    else
      arePermanentExternalForces = true;
  }

  virtual void addDensity(int id) {} ;
  
  /** Modifie la force de flottabilit� dans le solveur
   * @param value Nouvelle valeur.
   */
  virtual void setBuoyancy(double value){ m_buoyancy=value; };
  
  /** Divise la r�solution de la grille par 2 */
  virtual void divideRes () = 0;

  /** Multiplie la r�solution de la grille par 2 */
  virtual void multiplyRes () = 0;
  
  /** Diminue la r�solution de la grille de un voxel */
  virtual void decreaseRes () = 0;
  
  /** Augmente la r�solution de la grille de un voxel */
  virtual void increaseRes () = 0;
  
  /** Fonction de dessin de la grille */
  void displayGrid (){
    glCallList (m_gridDisplayList);
  };
  
  /** Fonction de dessin du rep�re de base */
  void displayBase (){
    glCallList (m_baseDisplayList);
  };
  
  /** Fonction de dessin du champ de v�locit� */
  virtual void displayVelocityField (void) = 0;
  
  /** Fonction de dessin du champ de densit� */
  virtual void displayDensityField (void) = 0;
  
  /** Fonction de dessin de la v�locit� d'une cellule */
  virtual void displayArrow (Vector& direction) = 0;
  
protected:
  /** Fonction de construction de la display list de la grille du solveur */
  virtual void buildDLGrid () = 0;
  
  /** Fonction de construction de la display list du rep�re du solveur */
  virtual void buildDLBase () = 0;
  
  /** Traitement de valeurs aux bords du solveur.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w.
   * @param x Composante de la v�locit� {m_u,m_v,m_w} � traiter.
   */
  virtual void set_bnd (unsigned char b, double *const x) = 0;
  
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
  
  /** Pas de projection pour garantir la conservation de la masse.
   * Les tableaux pass�s en param�tre sont modifi�s ici et ne doivent donc plus servir apr�s l'appel de la projection
   * @param p Champ de v�locit�.
   * @param div Divergence.
   */
  virtual void project (double *const p, double *const div) = 0;
  
  /** Pas de r�solution de la v�locit�. */
  virtual void vel_step () = 0;
  
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
  
  /** Position du solveur dans l'espace */
  Point m_position;

  /** Taille totale en nombre de voxels */
  uint m_nbVoxels;
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
  
  uint t, nx, t1;
  
  Point permanentExternalForces;
  bool arePermanentExternalForces;
};

#endif

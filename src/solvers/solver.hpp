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

/** La classe Solver propose une implémentation de la méthode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre modèle de flamme, le champ de densité n'est pas utilisé, ce qui,
 * physiquement semble assez curieux (d'après Alexei). 
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
   * @param timeStep Pas de temps utilisé pour la simulation.
   * @param buoyancy Intensité de la force de flottabilité dans le solveur.
   */
  Solver (Point& position, double timeStep, double buoyancy);
  /** Destructeur */
  virtual ~Solver ();
  
  /** Lance une itération du solveur. */
  virtual void iterate () = 0;
  
  /** Ajout des forces externes.
   * @param x Composante à traiter.
   * @param src Composante contenant les forces engendrées par les sources externes.
   */
  virtual void add_source (double *const x, double *const src);
  
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
  virtual void addExternalForces(Point& position, bool move) = 0;
  
  /** Ajoute de façon permanente des forces externes sur une des faces du solveur. Cette méthode
   * est utilisée principalement lorsque que du vent est appliqué sur une flamme.
   * @param forces Intensité de la force en (x,y,z).
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
  
  /** Modifie la force de flottabilité dans le solveur
   * @param value Nouvelle valeur.
   */
  virtual void setBuoyancy(double value){ m_buoyancy=value; };
  
  /** Divise la résolution de la grille par 2 */
  virtual void divideRes () = 0;

  /** Multiplie la résolution de la grille par 2 */
  virtual void multiplyRes () = 0;
  
  /** Diminue la résolution de la grille de un voxel */
  virtual void decreaseRes () = 0;
  
  /** Augmente la résolution de la grille de un voxel */
  virtual void increaseRes () = 0;
  
  /** Fonction de dessin de la grille */
  void displayGrid (){
    glCallList (m_gridDisplayList);
  };
  
  /** Fonction de dessin du repère de base */
  void displayBase (){
    glCallList (m_baseDisplayList);
  };
  
  /** Fonction de dessin du champ de vélocité */
  virtual void displayVelocityField (void) = 0;
  
  /** Fonction de dessin du champ de densité */
  virtual void displayDensityField (void) = 0;
  
  /** Fonction de dessin de la vélocité d'une cellule */
  virtual void displayArrow (Vector& direction) = 0;
  
protected:
  /** Fonction de construction de la display list de la grille du solveur */
  virtual void buildDLGrid () = 0;
  
  /** Fonction de construction de la display list du repère du solveur */
  virtual void buildDLBase () = 0;
  
  /** Traitement de valeurs aux bords du solveur.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w.
   * @param x Composante de la vélocité {m_u,m_v,m_w} à traiter.
   */
  virtual void set_bnd (unsigned char b, double *const x) = 0;
  
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
  
  /** Pas de projection pour garantir la conservation de la masse.
   * Les tableaux passés en paramètre sont modifiés ici et ne doivent donc plus servir après l'appel de la projection
   * @param p Champ de vélocité.
   * @param div Divergence.
   */
  virtual void project (double *const p, double *const div) = 0;
  
  /** Pas de résolution de la vélocité. */
  virtual void vel_step () = 0;
  
  /** Prolonger sur une grille fine un vecteur défini sur une grille grossière.
   * On utilise une interpolation bilinéaire.
   * La grille fine est de taille nx*ny*nz
   * @param vgros vecteur connu de taille (nx/2)*(ny/2)*(nz/2).
   * @param vfin vecteur résultat de la prolongation de taille nx*ny*nz.
   * @author Michel Leblond.
   */
//   void prolonger(double *const vgros, double *const vfin);

  /** Restreindre sur une grille grossière un vecteur défini sur une grille fine.
   * On utilise une moyenne pondérée : schèma en 27 points.
   * La grille fine est de taille nx*ny*nz.
   * @param vfin vecteur connu de taille nx*ny*nz
   * @param vgros vecteur résultat de la restriction de taille (nx/2)*(ny/2)*(nz/2)
   * @author Michel Leblond.
   */
//   void restreindre(double *const vfin, double *const vgros);
  
  /** Position du solveur dans l'espace */
  Point m_position;

  /** Taille totale en nombre de voxels */
  uint m_nbVoxels;
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
  
  uint t, nx, t1;
  
  Point permanentExternalForces;
  bool arePermanentExternalForces;
};

#endif

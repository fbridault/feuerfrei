#if !defined(SOLVER3D_H)
#define SOLVER3D_H

#include "realField3D.hpp"
#include "solver.hpp"
#include <math.h>

class Solver;

/** La classe Solver3D propose une implémentation 3D de la méthode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre modèle de flamme, le champ de densité n'est pas utilisé, ce qui,
 * physiquement semble assez curieux (d'après Alexei). 
 * 
 * @author	Flavien Bridault et Michel Lebond.
 */
class Solver3D : public RealField3D, public Solver
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
  Solver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale,
	    float timeStep, float buoyancy, float vorticityConfinement);
  /** Destructeur */
  virtual ~Solver3D ();
  
  /** Lance une itération du solveur. */
  virtual void iterate ();
  /** Ajoute de façon ponctuelle des forces externes sur une des faces du solveur.
   * @param position Nouvelle position du solveur. Détermine l'intensité de la force.
   * @param move Si true, alors le solveur est en plus déplacé à la position passée en paramètre.
   */
  void addExternalForces(const Point& position, bool move);
  
  void addForcesOnFace(unsigned char face, const Point& BLStrength, const Point& TLStrength, 
		       const Point& TRStrength, const Point& BRStrength);
protected:
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
		       const float *const u, const float *const v, const float *const w);
  
  /** Pas de résolution de la densité. */
  //void dens_step ();
  
  /** Pas de résolution de la vélocité. */
  virtual void vel_step ();

  /** Ajouter le vorticity confinement */
  void addVorticityConfinement( float * const u,  float * const v, float * const w);

  /** Prolonger sur une grille fine un vecteur défini sur une grille grossière.
   * On utilise une interpolation bilinéaire.
   * La grille fine est de taille nx*ny*nz
   * @param vgros vecteur connu de taille (nx/2)*(ny/2)*(nz/2).
   * @param vfin vecteur résultat de la prolongation de taille nx*ny*nz.
   * @author Michel Leblond.
   */
  void prolonger(float *const vgros, float *const vfin);

  /** Restreindre sur une grille grossière un vecteur défini sur une grille fine.
   * On utilise une moyenne pondérée : schèma en 27 points.
   * La grille fine est de taille nx*ny*nz.
   * @param vfin vecteur connu de taille nx*ny*nz
   * @param vgros vecteur résultat de la restriction de taille (nx/2)*(ny/2)*(nz/2)
   * @author Michel Leblond.
   */
  void restreindre(float *const vfin, float *const vgros);

#ifdef RTFLUIDS_BUILD
  /** Permet d'ajouter une force périodique venant de la droite. Pour les tests uniquement. */
  void addRightForce();
#endif
  
  float *m_uPrev, *m_vPrev, *m_wPrev;
  float *m_dens, *m_densPrev, *m_densSrc;
  float *m_rotx, *m_roty, *m_rotz, *m_rot;
  
  uint m_n2;
  
  /** Demi-Pas du solveur en Z. */
  float m_hz;
  /** son inverse */
  float m_invhz;

  uint m_perturbateCount;
};

#endif

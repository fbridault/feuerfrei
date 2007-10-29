#if !defined(SOLVER3D_H)
#define SOLVER3D_H

#include "realField3D.hpp"
#include "solver.hpp"
#include <math.h>

class Solver;

/** La classe Solver3D propose une impl�mentation 3D de la m�thode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre mod�le de flamme, le champ de densit� n'est pas utilis�, ce qui,
 * physiquement semble assez curieux (d'apr�s Alexei). 
 * 
 * @author	Flavien Bridault et Michel Lebond.
 */
class Solver3D : public RealField3D, public Solver
{
public:
  /** Constructeur par d�faut n�cessaire pour l'h�ritage multiple */
  Solver3D ();
  
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x R�solution de la grille en x.
   * @param n_y R�solution de la grille en y.
   * @param n_z R�solution de la grille en z.
   * @param dim Dimension du solveur, utilis� pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilis� pour la simulation.
   * @param buoyancy Intensit� de la force de flottabilit� dans le solveur.
   */
  Solver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale,
	    float timeStep, float buoyancy, float vorticityConfinement);
  /** Destructeur */
  virtual ~Solver3D ();
  
  /** Lance une it�ration du solveur. */
  virtual void iterate ();
  /** Ajoute de fa�on ponctuelle des forces externes sur une des faces du solveur.
   * @param position Nouvelle position du solveur. D�termine l'intensit� de la force.
   * @param move Si true, alors le solveur est en plus d�plac� � la position pass�e en param�tre.
   */
  void addExternalForces(const Point& position, bool move);
  
  void addForcesOnFace(unsigned char face, const Point& BLStrength, const Point& TLStrength, 
		       const Point& TRStrength, const Point& BRStrength);
protected:
  /** Traitement de valeurs aux bords du solveur.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w.
   * @param x Composante de la v�locit� {m_u,m_v,m_w} � traiter.
   */
  void set_bnd (unsigned char b, float *const x);
  
  /** Pas d'advection => d�placement du fluide sur lui-m�me.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param d Xomposante � traiter.
   * @param d0 Composante d au pas de temps pr�c�dent
   * @param u Vecteur de v�locit� en u
   * @param v Vecteur de v�locit� en v
   * @param w Vecteur de v�locit� en w
   */
  virtual void advect (unsigned char b, float *const d, const float *const d0,
		       const float *const u, const float *const v, const float *const w);
  
  /** Pas de r�solution de la densit�. */
  //void dens_step ();
  
  /** Pas de r�solution de la v�locit�. */
  virtual void vel_step ();

  /** Ajouter le vorticity confinement */
  void addVorticityConfinement( float * const u,  float * const v, float * const w);

  /** Prolonger sur une grille fine un vecteur d�fini sur une grille grossi�re.
   * On utilise une interpolation bilin�aire.
   * La grille fine est de taille nx*ny*nz
   * @param vgros vecteur connu de taille (nx/2)*(ny/2)*(nz/2).
   * @param vfin vecteur r�sultat de la prolongation de taille nx*ny*nz.
   * @author Michel Leblond.
   */
  void prolonger(float *const vgros, float *const vfin);

  /** Restreindre sur une grille grossi�re un vecteur d�fini sur une grille fine.
   * On utilise une moyenne pond�r�e : sch�ma en 27 points.
   * La grille fine est de taille nx*ny*nz.
   * @param vfin vecteur connu de taille nx*ny*nz
   * @param vgros vecteur r�sultat de la restriction de taille (nx/2)*(ny/2)*(nz/2)
   * @author Michel Leblond.
   */
  void restreindre(float *const vfin, float *const vgros);

#ifdef RTFLUIDS_BUILD
  /** Permet d'ajouter une force p�riodique venant de la droite. Pour les tests uniquement. */
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

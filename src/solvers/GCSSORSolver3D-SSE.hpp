#if !defined(GCSSORSOLVER3D_SSE_H)
#define GCSSORSOLVER3D_SSE_H
#include "SSE4.hpp"
class GCSSORSolver3D_SSE;

#include "solver3D.hpp"

/** La classe GCSSSolver propose d'utiliser la méthode du gradient conjugué préconditionné comme
 * méthode de resolution des systèmes linéaires.
 * 
 * @author	Flavien Bridault et Michel Leblond
 */
class GCSSORSolver3D_SSE : public virtual Solver3D
{
public:
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x Résolution de la grille en x.
   * @param n_y Résolution de la grille en y.
   * @param n_z Résolution de la grille en z.
   * @param dim Dimension du solveur, utilisé pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilisé pour la simulation.
   * @param buoyancy Intensité de la force de flottabilité dans le solveur.
   * @param omegaDiff Paramètre omega pour la diffusion.
   * @param omegaProj Paramètre omega pour la projection.
   * @param epsilon Tolérance d'erreur pour GCSSOR.
   */
  GCSSORSolver3D_SSE (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, float timeStep, 
		  float buoyancy, float omegaDiff, float omegaProj, float epsilon);
  
  /** Constructeur nécessaire pour l'héritage multiple.
   * @param omegaDiff Paramètre omega pour la diffusion.
   * @param omegaProj Paramètre omega pour la projection.
   * @param epsilon Tolérance d'erreur pour GCSSOR.
   */
  GCSSORSolver3D_SSE (float omegaDiff, float omegaProj, float epsilon);
  /** Destructeur. */
  virtual ~GCSSORSolver3D_SSE ();
  
protected:
  /** Effectue une résolution des systèmes linéaires de la diffusion
  * et de la projection à l'aide de la méthode du Gradient Conjugué
  * @param x0 Composante x au pas de temps précédent
  * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
  * @param a Valeur des coefficients dans la matrice A
  * @param diagonal Valeur du coefficient sur la diagonale de la matrice A : ( 1/(1+6a) pour la diffusion
  * et 1/6 pour la projection
  * @param omega Paramètre omega.
  * @param maxiter Nombre d'itérations maximal à effectuer
  */
  virtual void GCSSOR(float *const x0, const float *const b, float a, float diagonal, float omega, uint maxiter);
  
  virtual void diffuse (unsigned char b, float *const x, float *const x0, float a, float diff_visc);  
  virtual void project (float *const p, float *const div);
  
  /** Résidu, pour SSOR, direction de descente et ? */
  float *m_r, *m_z, *m_p, *m_q;

	/** Pointeurs SSE vers m_r, m_z, m_p, m_q */
  __m128 *m_r_sse, *m_z_sse,*m_p_sse,*m_q_sse;

  /** Paramètre omega pour la diffusion */
  float m_omegaDiff;
  /** Paramètre omega pour la projection */
  float m_omegaProj;
  /** Tolérance d'erreur pour GCSSOR. */
  float m_epsilon;
	
};

#endif

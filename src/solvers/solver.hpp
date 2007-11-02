#if !defined(SOLVER_H)
#define SOLVER_H

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
  
  /** Destructeur */
  virtual ~Solver ();
  
protected:
  /** Traitement de valeurs aux bords du solveur.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w.
   * @param x Composante de la v�locit� {m_u,m_v,m_w} � traiter.
   */
  virtual void set_bnd (unsigned char b, float *const x) = 0;
  
  /** Etape de diffusion.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w.
   * @param x Composante � traiter.
   * @param x0 Composante x au pas de temps pr�c�dent.
   * @param a Valeur des coefficients dans la matrice A
   * la r�solution de la densit�, soit � la viscosit� si elle est employ�e pour la r�solution
   * de la v�locit�
   */
  virtual void diffuse (unsigned char b, float *const x, float *const x0, float a) = 0;
  
  /** Pas de projection pour garantir la conservation de la masse.
   * Les tableaux pass�s en param�tre sont modifi�s ici et ne doivent donc plus servir apr�s l'appel de la projection
   * @param p Champ de v�locit�.
   * @param div Divergence.
   */
  virtual void project (float *const p, float *const div) = 0;
  
  /** Viscosit� cin�matique de l'air 15*10E-6. */
  float m_visc;
  /** Diffusion. */
  float m_diff;
  
  float m_aVisc, m_aDiff;
  
  /** Nombre de pas de r�solutions dans les m�thodes de diffusion et de projection */
  uint m_nbSteps;
  
  uint m_t, m_nx, m_t2nx, m_t1,m_debut,m_nbgrps,m_dernier;
  
  /** Demi-Pas du solveur en X. */
  float m_hx;
  /** son inverse */
  float m_invhx;
  /** Demi-Pas du solveur en Y. */
  float m_hy;
  /** son inverse */
  float m_invhy;
};

#endif

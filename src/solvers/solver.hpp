#if !defined(SOLVER_H)
#define SOLVER_H

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
  
  /** Destructeur */
  virtual ~Solver ();
  
protected:
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
  
  /** Viscosité cinématique de l'air 15*10E-6. */
  double m_visc;
  /** Diffusion. */
  double m_diff;
  
  double m_aVisc, m_aDiff;
  
  /** Nombre de pas de résolutions dans les méthodes de diffusion et de projection */
  uint m_nbSteps;
  
  uint m_t, m_nx, m_t1;
};

#endif

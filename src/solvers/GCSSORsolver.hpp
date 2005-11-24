#if !defined(GCSSORSOLVER_H)
#define GCSSORSOLVER_H

class GCSSORsolver;

#include "solver.hpp"

/** La classe Solver propose une impl�mentation 3D de la m�thode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre mod�le de flamme, le champ de densit� n'est pas utilis�, ce qui,
 * physiquement semble assez curieux (d'apr�s Alexei). N�anmoins Stam pr�sente bien les choses ainsi dans
 * son code initial, la densit� d�pend de la v�locit�, la v�locit� �volue ind�pendament.
 * 
 * @author	Flavien Bridault
 */
class GCSSORsolver : public Solver
{
public:
  /** Constructeur du solveur.
   * @param n : taille de la grille
   * @param pas_de_temps : pas de temps utilis� pour la simulation
   */
  GCSSORsolver (CPoint& position, int n_x, int n_y, int n_z, double dim, double pas_de_temps);
  virtual ~GCSSORsolver ();
  
protected:
  int IX2 (int i)
  {
    int x,y,z,tmp,tmp2;
    tmp2=m_nbVoxelsX*m_nbVoxelsY;
    z=i/tmp2;
    tmp=i-z*tmp2;
    y=tmp/m_nbVoxelsX;
    x=tmp-y*m_nbVoxelsX;
    
    //     return( IX( x+1, y+1, z+1 ) );
    int a = IX( x+1, y+1, z+1 );
    if(a < 0 || a >= m_nbVoxels)
      cerr << "ALERTE" << endl;
    return( a );
   //int n=i/m_nbVoxelsX; /* Nombre de lignes dans la grille */
    
    //return ( (m_nbVoxelsX+2)*(m_nbVoxelsY+2) + m_nbVoxelsX+3 + n/m_nbVoxelsY * ((m_nbVoxelsX+2)*(m_nbVoxelsY+2)) + (n % m_nbVoxelsX) *2 + (i % (m_nbVoxelsX*m_nbVoxelsY) ) );
  };
  /** Effectue une r�solution des syst�mes lin�aires de la diffusion
  * et de la projection � l'aide de la m�thode du Gradient Conjugu�
  * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
  * @param x composante � traiter
  * @param x0 composante x au pas de temps pr�c�dent
  * @param a valeur des coefficients dans la matrice A
  * @param div fraction des coefficients sur la diagonale de la matrice A ( 1/(1+6a) pour la diffusion
  * et 1/6 pour la projection
  * @param nb_steps nombre d'it�rations � effectuer
  */
  void GCSSOR(double *const x0, const double *const b, double a, double diagonal,
		      double nb_steps, double omega );
  
  /** Pas de diffusion.
   * @param b 1 pour composante u, 2 pour composante v, 3 pour composante w
   * @param x composante � traiter
   * @param x0 composante x au pas de temps pr�c�dent
   * @param diff_visc param�tre correspondant soit � la diffusion si la fonction est utilis�e pour
   * la r�solution du pas de densit�, soit � la viscosit� si elle est employ�e pour la r�solution
   * du pas de v�locit�
   */
  virtual void diffuse (int b, double *const x, const double *const x0,
			double a, double diff_visc);

  /** Pas de projection pour garantir la conservation de la masse.
   * Les tableaux pass�s en param�tre sont modifi�s ici et ne doivent donc plus servir apr�s l'appel de la projection
   */
  virtual void project (double *const p, double *const div);
  
  /** R�sidu, pour SSOR, direction de descente et ? */
  double *m_r, *m_z, *m_p, *m_q;
  
};

#endif

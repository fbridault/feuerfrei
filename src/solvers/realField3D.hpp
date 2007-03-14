#ifndef REALFIELD3D_HPP
#define REALFIELD3D_HPP

#include "field3D.hpp"

class Field3D;

/** La classe RealField3D est une impl�mentation d'un champ de vecteurs 3D. Aucune �quation diff�rentielle n'est
 * utilis�e ici, tout au plus il s'agira d'une �quation lin�aire. Cette classe est � r�server pour des tests, car
 * dans la pratique, il est inutile de calculer pour les valeurs de v�locit� sur toute une grille pour une �quation
 * lin�aire. Il est en effet pr�f�rable de n'effectuer le calcul que pour les particules, ce que propose de faire
 * la classe FakeField3D.
 * 
 * @author	Flavien Bridault.
 */
class RealField3D : public Field3D
{
public:
  /** Constructeur par d�faut n�cessaire pour l'h�ritage multiple */
  RealField3D ();
  
  /** Constructeur du champ.
   * @param position Position du solveur de la flamme.
   * @param n_x R�solution de la grille en x.
   * @param n_y R�solution de la grille en y.
   * @param n_z R�solution de la grille en z.
   * @param dim Dimension du solveur, utilis� pour l'affichage de la flamme.
   * @param scale Echelle utilis�e pour la taille du solveur.
   * @param timeStep Pas de temps utilis� pour la simulation.
   * @param buoyancy Intensit� de la force de flottabilit� dans le solveur.
   */
  RealField3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale, double timeStep, double buoyancy);
  /** Destructeur */
  virtual ~RealField3D ();
  
  /********************* Red�finition des m�thodes h�rit�es *********************/
  /** Lance une it�ration du solveur. */
  virtual void iterate ();
  
  Point getUVW (const Point& pos, double selfVelocity) const
  {
    uint i,j,k,n;
    
    findPointPosition(pos,i,j,k); n=IX (i, j, k);
    
    return Point(m_u[n], m_v[n], m_w[n]);
  };
  
  void addUsrc (const Point& pos, double value)
  {
    uint i,j,k;
    
    findPointPosition(pos,i,j,k);
    
    m_uSrc[IX (i, j, k)] += value;
  };
  
  void addVsrc (const Point& pos, double value, double& selfVelocity)
  {
    uint i,j,k;
    
    findPointPosition(pos,i,j,k);
    
    m_vSrc[IX (i, j, k)] += value;
  };
  
  void addWsrc (const Point& pos, double value)
  {
    uint i,j,k;
    
    findPointPosition(pos,i,j,k);
    
    m_wSrc[IX (i, j, k)] += value;
  };
  
  void setVsrc (const Point& pos, double value)
  {
    uint i,j,k;
    
    findPointPosition(pos,i,j,k);
    
    m_vSrc[IX (i, j, k)] = value;
  };
  
  
  /** M�thode permettant de retrouver les indices (i,j,k) de la cellule o� est situ�e la particule.
   * @param p Point dans l'espace.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   */
  void findPointPosition(const Point& p, uint& i, uint& j, uint& k) const
  {
    i = (uint) (p.x * m_nbVoxelsXDivDimX) + 1;
    j = (uint) (p.y * m_nbVoxelsYDivDimY) + 1;
    k = (uint) (p.z * m_nbVoxelsZDivDimZ) + 1;
  };
  
  virtual void addExternalForces(const Point& position, bool move);
  
  void cleanSources ();
  
  virtual void displayVelocityField (void);

protected:
  /***************************** Nouvelles m�thodes *****************************/
  virtual void vel_step ();
  
  /** M�thode permettant de simplifier l'indexage d'un voxel.
   * @param i Indice � l'horizontale (x).
   * @param j Indice � la verticale (y).
   * @param k Indice en profondeur (z).
   * @return Indice � utiliser pour une des composantes de v�locit� {m_u,m_v,m_w}.
   */
  uint IX (uint i, uint j, uint k) const
  {  
    return( (i) + (m_nbVoxelsX + 2) * (j) + (m_nbVoxelsX + 2) * (m_nbVoxelsY + 2) * (k) );
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
    return( (i) + (m_nbVoxelsX/2 + 2) * (j) + (m_nbVoxelsX/2 + 2) * (m_nbVoxelsY/2 + 2) * (k) );
  };
  
  double *m_u, *m_v, *m_w, *m_uSrc, *m_vSrc, *m_wSrc;
};

#endif

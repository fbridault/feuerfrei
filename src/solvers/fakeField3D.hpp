#ifndef FAKEFIELD3D_HPP
#define FAKEFIELD3D_HPP

#include "field3D.hpp"

class Field3D;

/** La classe FakeField3D simule un champ de vecteurs 3D. Aucune �quation diff�rentielle n'est utilis�e ici, tout au plus 
 * il s'agira d'une �quation lin�aire. Cette classe, � la diff�rence de RealField3D, permet de n'effectuer le calcul des 
 * v�locit�s que pour des points particuliers de la grille autrement dit les particules. Le constructeur demande tout de m�me
 * une r�solution en x,y,z, mais elle n'est utilis�e que si l'affichage du champ ou de la grille est demand�e. Elle n'intervient
 * en aucun cas dans le calcul des v�locit�s des particules.
 * 
 * @author	Flavien Bridault.
 */
class FakeField3D : public Field3D
{
public:  
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
  FakeField3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale, double timeStep, double buoyancy);
  /** Destructeur */
  virtual ~FakeField3D ();
  
  /********************* Red�finition des m�thodes h�rit�es *********************/
  virtual void iterate ();
    
  Point getUVW (const Point& pos, double selfVelocity) const
  {
    Point value;
    
    value.x += m_dt * m_forceCoef * m_uSrc * (pos.y+.2)/m_dim.y;
    value.z += m_dt * m_forceCoef * m_wSrc * (pos.y+.2)/m_dim.y;
    value.y += (m_dt * m_forceCoef) * (m_buoyancy * (pos.y+.2)/m_dim.y + selfVelocity) - fabs(value.x) - fabs(value.z);
    
    return value;
  };
  
  void addUsrc (const Point& pos, double value)
  {
    m_uSrc += value;
  };
  
  void addVsrc (const Point& pos, double value, double& selfVelocity)
  {
    selfVelocity += value;
  };
  
  void addWsrc (const Point& pos, double value)
  {
    m_wSrc += value;
  };
  
  void setVsrc (const Point& pos, double value)
  {
    m_vSrc = value;
  };
  
  virtual void addExternalForces(const Point& position, bool move);
    
  void cleanSources ();
  
  void displayVelocityField (void);
  
protected:
  
  /** Coefficients pour les forces externes. */
  double m_uSrc, m_vSrc, m_wSrc;
};

#endif

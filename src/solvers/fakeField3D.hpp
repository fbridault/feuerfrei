#ifndef FAKEFIELD3D_HPP
#define FAKEFIELD3D_HPP

#include "field3D.hpp"

class Field3D;

/** La classe FakeField3D simule un champ de vecteurs 3D. Aucune équation différentielle n'est utilisée ici, tout au plus 
 * il s'agira d'une équation linéaire. Cette classe, à la différence de RealField3D, permet de n'effectuer le calcul des 
 * vélocités que pour des points particuliers de la grille autrement dit les particules. Le constructeur demande tout de même
 * une résolution en x,y,z, mais elle n'est utilisée que si l'affichage du champ ou de la grille est demandée. Elle n'intervient
 * en aucun cas dans le calcul des vélocités des particules.
 * 
 * @author	Flavien Bridault.
 */
class FakeField3D : public Field3D
{
public:  
  /** Constructeur du champ.
   * @param position Position du solveur de la flamme.
   * @param n_x Résolution de la grille en x.
   * @param n_y Résolution de la grille en y.
   * @param n_z Résolution de la grille en z.
   * @param dim Dimension du solveur, utilisé pour l'affichage de la flamme.
   * @param scale Echelle utilisée pour la taille du solveur.
   * @param timeStep Pas de temps utilisé pour la simulation.
   * @param buoyancy Intensité de la force de flottabilité dans le solveur.
   */
  FakeField3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale, double timeStep, double buoyancy);
  /** Destructeur */
  virtual ~FakeField3D ();
  
  /********************* Redéfinition des méthodes héritées *********************/
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

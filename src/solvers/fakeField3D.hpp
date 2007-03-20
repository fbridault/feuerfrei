#ifndef FAKEFIELD3D_HPP
#define FAKEFIELD3D_HPP

class FakeField3D;

#include "field3D.hpp"

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
    
//     value.x += m_dt * m_forceCoef * m_src.x * (m_src.x < 0 ? pos.x/m_dim.x : (m_dim.x-pos.x/m_dim.x)) * (pos.y+.1)/m_dim.y;
//     value.z += m_dt * m_forceCoef * m_src.z * (m_src.z < 0 ? pos.z/m_dim.z : (m_dim.z-pos.z/m_dim.z)) * (pos.y+.1)/m_dim.y;
    value.x += m_dt * m_forceCoef * m_src.x * (pos.y+.1)/m_dim.y;
    value.z += m_dt * m_forceCoef * m_src.z * (pos.y+.1)/m_dim.y;
    value.y += (m_dt * m_forceCoef) * (m_buoyancy * (pos.y+.1)/m_dim.y + selfVelocity) - 2*(fabs(value.x) + fabs(value.z));
    
    return value;
  };
  
  void addUsrc (const Point& pos, double value)
  {
    m_src.x += value;
  };
  
  void addVsrc (const Point& pos, double value, double& selfVelocity)
  {
    selfVelocity += value;
  };
  
  void addWsrc (const Point& pos, double value)
  {
    m_src.z += value;
  };
  
  void setVsrc (const Point& pos, double value)
  {
    m_src.y = value;
  };
  
  virtual void addExternalForces(const Point& position, bool move);
    
  virtual void addPermanentExternalForces(Point& forces)
  {
    m_permanentExternalForces = forces;
    //    m_latentForces.resetToNull();
    if(!forces.x && !forces.y && !forces.z)
      m_arePermanentExternalForces = false;
    else
      m_arePermanentExternalForces = true;
  }
  void cleanSources ();
  
  void displayVelocityField (void);
  
  virtual bool isRealSolver () const { return false; };
protected:
  
  /** Coefficients pour les forces externes. */
  Point m_src;
  Point m_latentForces;
};

#endif

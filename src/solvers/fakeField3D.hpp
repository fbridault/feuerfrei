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
  FakeField3D (CTransform& a_rTransform, float dim, float timeStep, float buoyancy);
  /** Destructeur */
  virtual ~FakeField3D ();

  /********************* Redéfinition des méthodes héritées *********************/
  virtual void iterate ();

  CPoint getUVW (const CPoint& pos, float selfVelocity) const
  {
    CPoint value;

    value.x = m_dt * m_forceCoef * m_src.x * ((pos.y/m_dim.y)+.1);
    value.z = m_dt * m_forceCoef * m_src.z * ((pos.y/m_dim.y)+.1);
    value.y = m_dt * m_forceCoef * (m_buoyancy * ((pos.y/m_dim.y)+.1) + selfVelocity) - 2*(fabs(value.x) + fabs(value.z));
    return value;
  };

  void moveParticle (CParticle& particle, float selfVelocity) const
  {
    /** Sauvegarde de la position à t-1 */
    particle.xprev = particle.x; particle.yprev = particle.y; particle.zprev = particle.z;

    /* Pour indication, m_coef = m_dt * m_dt * m_forceCoef (calculé dans le constructeur) */
    particle.x = 2*particle.x - particle.xprev + m_coef * m_src.x;
    particle.y = 2*particle.y - particle.yprev + m_coef * (m_buoyancy * (particle.y/m_dim.y) + selfVelocity - 2*(fabs(m_src.x) + fabs(m_src.z)));
    particle.z = 2*particle.z - particle.zprev + m_coef * m_src.z;
  };

  void addUsrc (const CPoint& pos, float value)
  {
    m_src.x += value;
  };

  /** Ajoute une force verticale dans le champ. La différence ici pour les
   * FakeFields, puisque qu'il n'existe pas de grille à proprement parler, est
   * que la force est en réalité transmise via la vélocité propre du
   * squelette. L'appel de fonction peut donc paraître superflu, mais il permet
   * ainsi de garder la même interface quelque soit le type de champ.
   */
  void addVsrc (const CPoint& pos, float value, float& selfVelocity)
  {
    selfVelocity += value*4.0;
  };

  void addWsrc (const CPoint& pos, float value)
  {
    m_src.z += value;
  };

  virtual void setBuoyancy(float value){ m_buoyancy=30*value; };

  void cleanSources ();
  void displayVelocityField (void);

  virtual void addExternalForces(const CPoint& position, bool move);

  virtual bool isRealSolver () const { return false; };

  void addForcesOnFace(unsigned char face, const CPoint& BLStrength, const CPoint& TLStrength,
		       const CPoint& TRStrength, const CPoint& BRStrength);

protected:
  void buildDLGrid ();
  /** Coefficients pour les forces externes. */
  CPoint m_src;
  CPoint m_latentForces;
  float m_coef;
};

#endif

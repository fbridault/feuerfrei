#ifndef PARTICLE_HPP
#define PARTICLE_HPP

/** Particule dans l'espace, utilisée pour construire les squelettes
 * et la surface NURBS. Chaque particule est simplement un point, et
 * possède une durée de vie.
 *
 * @author	Flavien Bridault
 */
class CParticle : public CPoint
{
public:
  /** Durée de vie de la particule. */
  uint m_lifespan;
  /** Coordonnées à t-1 */
  float xprev, yprev, zprev;
  /**
   * Constructeur par d&eacute;faut. Ce constructeur donne à la particule des coordonn&eacute;es nulles par d&eacute;faut.
   */
  CParticle():CPoint(){m_lifespan=0; xprev=yprev=zprev=0.0;};

  /** Ce constructeur prend un point de l'espace comme base.
   * @param P point dans l'espace
   * @param lifespan durée de vie initiale
   */
  CParticle(const CPoint& P, uint lifespan):CPoint(P)
  {
    xprev=yprev=zprev=0.0;
    m_lifespan=lifespan;
  };

  /** Constructeur par recopie
   * @param P particule source
   */
  CParticle(const CParticle& P):CPoint(P)
  {
    m_lifespan=P.m_lifespan;
    xprev = P.xprev;
    yprev = P.yprev;
    zprev = P.zprev;
  };

  /** Décrémente la durée de vie de la particule. */
  void decreaseLife(){m_lifespan--;};

  /** Test pour déterminer si la particule est encore vivante.
   * @return vraie si la particule est morte
   */
  bool isDead(){return (m_lifespan < 1);};

  /** Naissance d'une particule, on affecte sa durée de vie à la valeur passée en paramètre.
   * @param l durée de vie en terme d'itérations
   */
  void birth(uint l){m_lifespan=l;};

  /** Surchage de l'opérateur =
   * @param P particule source
   */
  virtual CParticle& operator= (const CParticle& P){
    x=P.x; y=P.y; z=P.z;
    xprev = P.xprev; yprev = P.yprev; zprev = P.zprev;
    m_lifespan=P.m_lifespan;
    return *this;
  };

  /** Surchage de l'opérateur =
   * @param P point dans l'espace
   */
  virtual CParticle& operator= (const CPoint& P){
    x=P.x; y=P.y; z=P.z;
    xprev = 0.0; yprev = 0.0; zprev = 0.0;
    return *this;
  };
};


#endif

#ifndef PARTICLE_HPP
#define PARTICLE_HPP

/** Particule dans l'espace, utilisée pour construire les squelettes
 * et la surface NURBS. Chaque particule est simplement un point, et
 * possède une durée de vie.
 *
 * @author	Flavien Bridault
 */
class Particle : public Point
{
public:
  /** Durée de vie de la particule. */
  uint m_lifespan;
  /**
   * Constructeur par d&eacute;faut. Ce constructeur donne à la particule des coordonn&eacute;es nulles par d&eacute;faut.
   */
  Particle():Point(){m_lifespan=0;};
  
  /** Ce constructeur prend un point de l'espace comme base.
   * @param P point dans l'espace
   * @param lifespan durée de vie initiale
   */
  Particle(const Point& P, uint lifespan):Point(P)
  {
  };
  
  /** Constructeur par recopie 
   * @param P particule source
   */
  Particle(const Particle& P):Point(P)
  {
    m_lifespan=P.m_lifespan;
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
  virtual Particle& operator= (const Particle& P){x=P.x; y=P.y; z=P.z; m_lifespan=P.m_lifespan; return *this;};
  
  /** Surchage de l'opérateur =
   * @param P point dans l'espace
   */
  virtual Particle& operator= (const Point& P){x=P.x; y=P.y; z=P.z; return *this;};
};


#endif

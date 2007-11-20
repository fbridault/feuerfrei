#ifndef CLONEFLAME_H
#define CLONEFLAME_H

class CloneFlame;
class CloneLineFlame;
class ClonePointFlame;

#include "realFlames.hpp"

/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE CLONEFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe CloneFlame permet de répliquer une flamme de type RealFlame à un endroit quelconque de la scène.
 * Ce clone n'est en effet pas conçu d'après l'état d'un solveur de fluides, mais uniquement à partir des
 * points de contrôle d'une RealFlame.
 * @todo Il faut trouver un moyen simple et convaincant de perturber les points de contrôle pour que le clone
 * ressemble un peu moins à sa source.
 *
 * @author	Flavien Bridault
 */
class CloneFlame : public FixedFlame
{
public:
  /** Constructeur.
   * @param flameConfig Pointeur sur le configuration de la flamme.
   * @param source Pointeur sur la flamme qui sert de source au clone.
   * @param offset Décalage de la flamme par rapport à la flamme source.
   */
  CloneFlame(const FlameConfig& flameConfig, const RealFlame* const source, const Point& offset);
  virtual ~CloneFlame();
  
  /** Dessine la mèche de la flamme.
   * @param displayBoxes Affiche ou non le partitionnement de la mèche. Ce paramètre
   * est simplement gardé pour des raisons de compatibilité, il est en effet ignoré
   * puisque la mèche n'est pas découpée.
   */
  virtual void drawWick(bool displayBoxes) const
  {
    Point diffPos = m_position;// - m_source->getPosition();
    glPushMatrix();
    glTranslatef(diffPos.x, diffPos.y, diffPos.z);
    m_source->drawWick(displayBoxes);
    glPopMatrix();
  };
  
  virtual Vector getMainDirection() const { return(m_source->getMainDirection()); };  
  virtual Point getCenter() const { return (m_source->getCenter()); };
  
  virtual bool build();
  
  Point getTop() const { return m_source->getTop(); };
  Point getBottom() const { return m_source->getBottom(); };
  
protected:
  const RealFlame *m_source;
  Point m_position;
};

/** La classe CloneLineFlame implémente une flamme clone dont la source est une LineFlame.
 *
 * @author	Flavien Bridault
 * @see         CloneFlame
 */
class CloneLineFlame : public CloneFlame
{
public:
  /** Constructeur.
   * @param flameConfig Pointeur sur le configuration de la flamme.
   * @param source Pointeur sur la flamme qui sert de source au clone.
   * @param offset Décalage de la flamme par rapport à la flamme source.
   */
  CloneLineFlame(const FlameConfig& flameConfig, const LineFlame* const source, const Point& offset);
  virtual ~CloneLineFlame();
  
  virtual void drawFlame(bool display, bool displayParticle) const{ 
    glPushMatrix();
    glTranslatef (m_position.x, m_position.y, m_position.z);
    if( display) drawLineFlame();
    glPopMatrix();
  };
};

/** La classe ClonePointFlame implémente une flamme clone dont la source est une PointFlame.
 *
 * @author	Flavien Bridault
 * @see         CloneFlame
 */
class ClonePointFlame : public CloneFlame
{
public:
  /** Constructeur.
   * @param flameConfig Pointeur sur le configuration de la flamme.
   * @param source Pointeur sur la flamme qui sert de source au clone.
   * @param offset Décalage de la flamme par rapport à la flamme source.
   */
  ClonePointFlame(const FlameConfig& flameConfig, const PointFlame* const source, const Point& offset);
  virtual ~ClonePointFlame();
  
  virtual void drawFlame(bool display, bool displayParticle) const{ 
    glPushMatrix();
    glTranslatef (m_position.x, m_position.y, m_position.z);
    if( display) drawPointFlame(); 
    glPopMatrix();
  };
};

#endif

#ifndef CLONEFLAME_H
#define CLONEFLAME_H

class CloneFlame;
class CloneLineFlame;
class CloneCPointFlame;

#include "realFlames.hpp"

/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE CLONEFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe CloneFlame permet de répliquer une flamme de type IRealFlame à un endroit quelconque de la scène.
 * Ce clone n'est en effet pas conçu d'après l'état d'un solveur de fluides, mais uniquement à partir des
 * points de contrôle d'une IRealFlame.
 * @todo Il faut trouver un moyen simple et convaincant de perturber les points de contrôle pour que le clone
 * ressemble un peu moins à sa source.
 *
 * @author	Flavien Bridault
 */
class CloneFlame : public IFixedFlame
{
public:
  /** Constructeur.
   * @param flameConfig CPointeur sur le configuration de la flamme.
   * @param source CPointeur sur la flamme qui sert de source au clone.
   * @param offset Décalage de la flamme par rapport à la flamme source.
   */
  CloneFlame(const FlameConfig& flameConfig, const IRealFlame* const source, const CPoint& offset);
  virtual ~CloneFlame();

  /** Dessine la mèche de la flamme.
   * @param displayBoxes Affiche ou non le partitionnement de la mèche. Ce paramètre
   * est simplement gardé pour des raisons de compatibilité, il est en effet ignoré
   * puisque la mèche n'est pas découpée.
   */
  virtual void drawWick(bool displayBoxes) const
  {
    CPoint diffPos = m_position;// - m_source->getPosition();
    glPushMatrix();
    glTranslatef(diffPos.x, diffPos.y, diffPos.z);
    m_source->drawWick(displayBoxes);
    glPopMatrix();
  };

  virtual CVector getMainDirection() const { return(m_source->getMainDirection()); };
  virtual CPoint const& getCenter() const { return (m_source->getCenter()); };

  virtual bool build();

  CPoint getTop() const { return m_source->getTop(); };
  CPoint getBottom() const { return m_source->getBottom(); };

protected:
  const IRealFlame *m_source;
  CPoint m_position;
};

/** La classe CloneLineFlame implémente une flamme clone dont la source est une CLineFlame.
 *
 * @author	Flavien Bridault
 * @see         CloneFlame
 */
class CloneLineFlame : public CloneFlame
{
public:
  /** Constructeur.
   * @param flameConfig CPointeur sur le configuration de la flamme.
   * @param source CPointeur sur la flamme qui sert de source au clone.
   * @param offset Décalage de la flamme par rapport à la flamme source.
   */
  CloneLineFlame(const FlameConfig& flameConfig, const CLineFlame* const source, const CPoint& offset);
  virtual ~CloneLineFlame();

  virtual void drawFlame(bool display, bool displayParticle) const{
    glPushMatrix();
    glTranslatef (m_position.x, m_position.y, m_position.z);
    if( display) drawLineFlame();
    glPopMatrix();
  };
};

/** La classe CloneCPointFlame implémente une flamme clone dont la source est une CPointFlame.
 *
 * @author	Flavien Bridault
 * @see         CloneFlame
 */
class CloneCPointFlame : public CloneFlame
{
public:
  /** Constructeur.
   * @param flameConfig CPointeur sur le configuration de la flamme.
   * @param source CPointeur sur la flamme qui sert de source au clone.
   * @param offset Décalage de la flamme par rapport à la flamme source.
   */
  CloneCPointFlame(const FlameConfig& flameConfig, const CPointFlame* const source, const CPoint& offset);
  virtual ~CloneCPointFlame();

  virtual void drawFlame(bool display, bool displayParticle) const{
    glPushMatrix();
    glTranslatef (m_position.x, m_position.y, m_position.z);
    if( display) drawCPointFlame();
    glPopMatrix();
  };
};

#endif

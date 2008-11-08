#ifndef CLONEFLAME_H
#define CLONEFLAME_H

class CloneFlame;
class CloneLineFlame;
class CloneCPointFlame;

#include "realFlames.hpp"

/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE CLONEFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe CloneFlame permet de r�pliquer une flamme de type IRealFlame � un endroit quelconque de la sc�ne.
 * Ce clone n'est en effet pas con�u d'apr�s l'�tat d'un solveur de fluides, mais uniquement � partir des
 * points de contr�le d'une IRealFlame.
 * @todo Il faut trouver un moyen simple et convaincant de perturber les points de contr�le pour que le clone
 * ressemble un peu moins � sa source.
 *
 * @author	Flavien Bridault
 */
class CloneFlame : public IFixedFlame
{
public:
  /** Constructeur.
   * @param flameConfig CPointeur sur le configuration de la flamme.
   * @param source CPointeur sur la flamme qui sert de source au clone.
   * @param offset D�calage de la flamme par rapport � la flamme source.
   */
  CloneFlame(const FlameConfig& flameConfig, const IRealFlame* const source, const CPoint& offset);
  virtual ~CloneFlame();

  /** Dessine la m�che de la flamme.
   * @param displayBoxes Affiche ou non le partitionnement de la m�che. Ce param�tre
   * est simplement gard� pour des raisons de compatibilit�, il est en effet ignor�
   * puisque la m�che n'est pas d�coup�e.
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

/** La classe CloneLineFlame impl�mente une flamme clone dont la source est une CLineFlame.
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
   * @param offset D�calage de la flamme par rapport � la flamme source.
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

/** La classe CloneCPointFlame impl�mente une flamme clone dont la source est une CPointFlame.
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
   * @param offset D�calage de la flamme par rapport � la flamme source.
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

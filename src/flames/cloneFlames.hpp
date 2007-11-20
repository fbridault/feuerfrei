#ifndef CLONEFLAME_H
#define CLONEFLAME_H

class CloneFlame;
class CloneLineFlame;
class ClonePointFlame;

#include "realFlames.hpp"

/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE CLONEFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe CloneFlame permet de r�pliquer une flamme de type RealFlame � un endroit quelconque de la sc�ne.
 * Ce clone n'est en effet pas con�u d'apr�s l'�tat d'un solveur de fluides, mais uniquement � partir des
 * points de contr�le d'une RealFlame.
 * @todo Il faut trouver un moyen simple et convaincant de perturber les points de contr�le pour que le clone
 * ressemble un peu moins � sa source.
 *
 * @author	Flavien Bridault
 */
class CloneFlame : public FixedFlame
{
public:
  /** Constructeur.
   * @param flameConfig Pointeur sur le configuration de la flamme.
   * @param source Pointeur sur la flamme qui sert de source au clone.
   * @param offset D�calage de la flamme par rapport � la flamme source.
   */
  CloneFlame(const FlameConfig& flameConfig, const RealFlame* const source, const Point& offset);
  virtual ~CloneFlame();
  
  /** Dessine la m�che de la flamme.
   * @param displayBoxes Affiche ou non le partitionnement de la m�che. Ce param�tre
   * est simplement gard� pour des raisons de compatibilit�, il est en effet ignor�
   * puisque la m�che n'est pas d�coup�e.
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

/** La classe CloneLineFlame impl�mente une flamme clone dont la source est une LineFlame.
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
   * @param offset D�calage de la flamme par rapport � la flamme source.
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

/** La classe ClonePointFlame impl�mente une flamme clone dont la source est une PointFlame.
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
   * @param offset D�calage de la flamme par rapport � la flamme source.
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

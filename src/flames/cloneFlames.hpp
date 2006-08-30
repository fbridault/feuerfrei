#ifndef CLONEFLAME_H
#define CLONEFLAME_H

class CloneFlame;
class CloneLineFlame;
class ClonePointFlame;

#include "realFlames.hpp"

/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE CLONEFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe PointFlame implémente une flamme qui provient d'une mèche verticale droite.<br>
 * Elle génère ses squelettes à partir du maillage de la mèche dont le nom est fourni au
 * constructeur.
 *
 * @author	Flavien Bridault
 */
class CloneFlame : public FixedFlame
{
public:
  /** Constructeur.
   * @param s pointeur sur le solveur de fluides
   * @param nbSkeletons nombre de squelettes.
   * @param posRel Position relative du centre de la flamme
   * @param innerForce Force intérieure de la flamme
   * @param scene Pointeur sur la scène
   * @param rayon rayon de la flamme
   */
  CloneFlame(FlameConfig* flameConfig, RealFlame *source, Point offset);
  virtual ~CloneFlame();
  
  /** Dessine la mèche de la flamme 
   * @param displayBoxes affiche ou non le partitionnement de la mèche. Ce paramètre
   * est ignoré pour ce type de flamme puisque la mèche n'est pas découpé. 
   */
  virtual void drawWick(bool displayBoxes)
  {
    Point diffPos = m_position - m_source->getPosition();
    glPushMatrix();
    glTranslatef(diffPos.x, diffPos.y, diffPos.z);
    m_source->drawWick(displayBoxes);
    glPopMatrix();
  };
  
  /** Retourne la direction de la base de la flamme vers la derniere particule
   * pour orienter le solide photométrique.
   */
  Vector getMainDirection(){
    return(m_source->getMainDirection());
  };
  
  virtual Point getCenter (){    
    return (m_source->getCenter() + m_position);
  };

  /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
   * des vecteurs de noeuds.
   * @return false si un problème dans la contruction est survenu (pas assez de particules par exemple)
   */
  virtual bool build();
  
  Point* getTop(){ return m_source->getTop(); };
  Point* getBottom() { return m_source->getBottom(); };
protected:
  RealFlame *m_source;
};

/** La classe PointFlame implémente une flamme qui provient d'une mèche verticale droite.<br>
 * Elle génère ses squelettes à partir du maillage de la mèche dont le nom est fourni au
 * constructeur.
 *
 * @author	Flavien Bridault
 */
class CloneLineFlame : public CloneFlame
{
public:
  /** Constructeur.
   * @param s pointeur sur le solveur de fluides
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit Ãªtre pair en raison de l'affichage. A VERIFIER
   * @param posRel Position relative du centre de la flamme
   * @param innerForce Force intérieure de la flamme
   * @param scene Pointeur sur la scène
   * @param rayon rayon de la flamme
   */
  CloneLineFlame(FlameConfig* flameConfig, LineFlame *source, Point offset);
  virtual ~CloneLineFlame();
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
   * build() avec le placage de texture
   */
  virtual void drawFlame(bool displayParticle){ 
    glPushMatrix();
    glTranslatef (m_position.x, m_position.y, m_position.z);
    drawLineFlame();
    glPopMatrix();
  };
  /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
   * des vecteurs de noeuds.
   */
//   virtual void build() = 0;
};

/** La classe PointFlame implémente une flamme qui provient d'une mèche verticale droite.<br>
 * Elle génère ses squelettes à partir du maillage de la mèche dont le nom est fourni au
 * constructeur.
 *
 * @author	Flavien Bridault
 */
class ClonePointFlame : public CloneFlame
{
public:
  /** Constructeur.
   * @param s pointeur sur le solveur de fluides
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit Ãªtre pair en raison de l'affichage. A VERIFIER
   * @param posRel Position relative du centre de la flamme
   * @param innerForce Force intérieure de la flamme
   * @param scene Pointeur sur la scène
   * @param rayon rayon de la flamme
   */
  ClonePointFlame(FlameConfig* flameConfig, PointFlame *source, Point offset);
  virtual ~ClonePointFlame();
    
  /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
   * build() avec le placage de texture
   */
  virtual void drawFlame(bool displayParticle){ 
    glPushMatrix();
    glTranslatef (m_position.x, m_position.y, m_position.z);
    drawPointFlame(); 
    glPopMatrix();
  };
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
   * des vecteurs de noeuds.
   */
//   virtual void build() = 0;
};

#endif

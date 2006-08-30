#ifndef CLONEFLAME_H
#define CLONEFLAME_H

class CloneFlame;
class CloneLineFlame;
class ClonePointFlame;

#include "realFlames.hpp"

/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE CLONEFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe PointFlame impl�mente une flamme qui provient d'une m�che verticale droite.<br>
 * Elle g�n�re ses squelettes � partir du maillage de la m�che dont le nom est fourni au
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
   * @param innerForce Force int�rieure de la flamme
   * @param scene Pointeur sur la sc�ne
   * @param rayon rayon de la flamme
   */
  CloneFlame(FlameConfig* flameConfig, RealFlame *source, Point offset);
  virtual ~CloneFlame();
  
  /** Dessine la m�che de la flamme 
   * @param displayBoxes affiche ou non le partitionnement de la m�che. Ce param�tre
   * est ignor� pour ce type de flamme puisque la m�che n'est pas d�coup�. 
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
   * pour orienter le solide photom�trique.
   */
  Vector getMainDirection(){
    return(m_source->getMainDirection());
  };
  
  virtual Point getCenter (){    
    return (m_source->getCenter() + m_position);
  };

  /** Fonction appel�e par la fonction de dessin OpenGL. Elle commence par d�placer les particules 
   * des squelettes p�riph�riques. Ensuite, elle d�finit la matrice de points de contr�le de la NURBS,
   * des vecteurs de noeuds.
   * @return false si un probl�me dans la contruction est survenu (pas assez de particules par exemple)
   */
  virtual bool build();
  
  Point* getTop(){ return m_source->getTop(); };
  Point* getBottom() { return m_source->getBottom(); };
protected:
  RealFlame *m_source;
};

/** La classe PointFlame impl�mente une flamme qui provient d'une m�che verticale droite.<br>
 * Elle g�n�re ses squelettes � partir du maillage de la m�che dont le nom est fourni au
 * constructeur.
 *
 * @author	Flavien Bridault
 */
class CloneLineFlame : public CloneFlame
{
public:
  /** Constructeur.
   * @param s pointeur sur le solveur de fluides
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage. A VERIFIER
   * @param posRel Position relative du centre de la flamme
   * @param innerForce Force int�rieure de la flamme
   * @param scene Pointeur sur la sc�ne
   * @param rayon rayon de la flamme
   */
  CloneLineFlame(FlameConfig* flameConfig, LineFlame *source, Point offset);
  virtual ~CloneLineFlame();
  
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture
   */
  virtual void drawFlame(bool displayParticle){ 
    glPushMatrix();
    glTranslatef (m_position.x, m_position.y, m_position.z);
    drawLineFlame();
    glPopMatrix();
  };
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle commence par d�placer les particules 
   * des squelettes p�riph�riques. Ensuite, elle d�finit la matrice de points de contr�le de la NURBS,
   * des vecteurs de noeuds.
   */
//   virtual void build() = 0;
};

/** La classe PointFlame impl�mente une flamme qui provient d'une m�che verticale droite.<br>
 * Elle g�n�re ses squelettes � partir du maillage de la m�che dont le nom est fourni au
 * constructeur.
 *
 * @author	Flavien Bridault
 */
class ClonePointFlame : public CloneFlame
{
public:
  /** Constructeur.
   * @param s pointeur sur le solveur de fluides
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage. A VERIFIER
   * @param posRel Position relative du centre de la flamme
   * @param innerForce Force int�rieure de la flamme
   * @param scene Pointeur sur la sc�ne
   * @param rayon rayon de la flamme
   */
  ClonePointFlame(FlameConfig* flameConfig, PointFlame *source, Point offset);
  virtual ~ClonePointFlame();
    
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture
   */
  virtual void drawFlame(bool displayParticle){ 
    glPushMatrix();
    glTranslatef (m_position.x, m_position.y, m_position.z);
    drawPointFlame(); 
    glPopMatrix();
  };
  
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle commence par d�placer les particules 
   * des squelettes p�riph�riques. Ensuite, elle d�finit la matrice de points de contr�le de la NURBS,
   * des vecteurs de noeuds.
   */
//   virtual void build() = 0;
};

#endif

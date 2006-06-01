#ifndef CLONEFLAME_H
#define CLONEFLAME_H

class CloneFlame;
class CloneLineFlame;
class ClonePointFlame;

#include "basicFlames.hpp"

/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE CLONEFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe PointFlame implémente une flamme qui provient d'une mèche verticale droite.<br>
 * Elle génère ses squelettes à partir du maillage de la mèche dont le nom est fourni au
 * constructeur.
 *
 * @author	Flavien Bridault
 */
class CloneFlame : public MetaFlame
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
  CloneFlame(FlameConfig* flameConfig, BasicFlame *source, const wxString& texname, GLint wrap_s, GLint wrap_t);
  virtual ~CloneFlame();
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
   * des vecteurs de noeuds.
   */
  virtual void build() = 0;

  Point* getTop(){ return m_source->getTop(); };
  Point* getBottom() { return m_source->getBottom(); };
protected:
  BasicFlame *m_source;
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
  CloneLineFlame(FlameConfig* flameConfig, LineFlame *source);
  virtual ~CloneLineFlame();
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
   * des vecteurs de noeuds.
   */
  virtual void build() = 0;
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
  ClonePointFlame(FlameConfig* flameConfig, PointFlame *source);
  virtual ~ClonePointFlame();
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
   * des vecteurs de noeuds.
   */
  virtual void build() = 0;
};

#endif

#if !defined(FIRESOURCES_H)
#define FIRESOURCES_H

class Candle;
class Firmalampe;

#include "fire.hpp"
#include "cloneFlames.hpp"

class PointFlame;
class LineFlame;
class FireSource;
class CloneFlame;

/** La classe Candle permet la définition d'une bougie.
 *
 * @author	Flavien Bridault
 */
class Candle : public FireSource
{
public:
  /** Constructeur d'une bougie.
   * @param s pointeur sur le solveur de fluides
   * @param posRel position du centre de la flamme dans le solveur.
   * @param scene pointeur sur la scène
   * @param innerForce force intérieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL)
   * @param shader pointeur sur le shader chargé de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes périphériques
   */
  Candle(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *filename, uint index, 
	 CgSVShader * shader, double rayon );
  virtual ~Candle(){};
};


/** La classe Firmalampe permet la définition d'une firmalampe.
 *
 * @author	Flavien Bridault
 */
class Firmalampe : public FireSource
{
public:
  /** Constructeur d'une bougie.
   * @param s pointeur sur le solveur de fluides
   * @param posRel position du centre de la flamme dans le solveur.
   * @param scene pointeur sur la scène
   * @param innerForce force intérieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL)
   * @param shader pointeur sur le shader chargé de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes périphériques
   * @param wickFileName nom du fichier contenant la mèche
   */
  Firmalampe(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *filename, uint index, 
	     CgSVShader * shader, const char *wickFileName);
  virtual ~Firmalampe(){};
};

/** La classe Torche permet la définition d'une flamme de type torche.
 * Le fichier OBJ représentant le luminaire contient des mèches qui doivent avoir un nom
 * en Wick*. Le luminaire en tant que tel doit s'appeler Torch.
 *
 * @author	Flavien Bridault
 */
class Torch : public FireSource
{
public:
  /** Constructeur d'une bougie.
   * @param s pointeur sur le solveur de fluides
   * @param posRel position du centre de la flamme dans le solveur.
   * @param scene pointeur sur la scène
   * @param innerForce force intérieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL)
   * @param shader pointeur sur le shader chargé de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes périphériques
   * @param wickFileName nom du fichier contenant la torche
   */
  Torch(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *torchName, uint index, CgSVShader * shader);
  virtual ~Torch(){}; 

  /** Dessine la mèche de la flamme. Les mèches des BasicFlame sont définies en (0,0,0), une translation
   * est donc effectuée pour tenir compte du placement du feu dans le monde.
   */
  virtual void drawWick(bool displayBoxes) {};
};

/** La classe CampFire permet la définition d'un feu de camp.
 * Le fichier OBJ représentant le luminaire contient des mèches qui doivent avoir un nom
 * en Wick*. Le luminaire en tant que tel doit s'appeler Torch.
 *
 * @author	Flavien Bridault
 */
class CampFire : public FireSource
{
public:
  /** Constructeur d'une bougie.
   * @param s pointeur sur le solveur de fluides
   * @param posRel position du centre de la flamme dans le solveur.
   * @param scene pointeur sur la scène
   * @param innerForce force intérieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL)
   * @param shader pointeur sur le shader chargé de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes périphériques
   * @param wickFileName nom du fichier contenant la torche
   */
  CampFire(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *fireName, uint index, CgSVShader * shader);
  virtual ~CampFire(){}; 
};

/** La classe CandleStick permet la définition d'un chandelier.
 *
 * @author	Flavien Bridault
 */
class CandleStick : public FireSource
{
public:
  /** Constructeur d'une bougie.
   * @param s pointeur sur le solveur de fluides
   * @param posRel position du centre de la flamme dans le solveur.
   * @param scene pointeur sur la scène
   * @param innerForce force intérieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL)
   * @param shader pointeur sur le shader chargé de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes périphériques
   */
  CandleStick(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *filename, uint index, 
	      CgSVShader * shader, double rayon );
  virtual ~CandleStick();
  
  /** Fonction chargée de construire les flammes composant la source de feu. Elle se charge également 
   * de déterminer la position de la source de lumière.
   */
  virtual void build();
  
    /** Dessine la mèche de la flamme. Les mèches des BasicFlame sont définies en (0,0,0), une translation
   * est donc effectuée pour tenir compte du placement du feu dans le monde.
   */
  virtual void drawWick(bool displayBoxes)
  {      
    Point pt(getPosition());
    glPushMatrix();
    glTranslatef (pt.x, pt.y, pt.z);
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->drawWick(displayBoxes);
    for (uint i = 0; i < m_nbCloneFlames; i++)
      m_cloneFlames[i]->drawWick(displayBoxes);
    glPopMatrix();
  }
  
    /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
   * build() avec le placage de texture. La flamme d'une BasicFlame est définie dans le repère du solveur,
   * donc seule une translation correspondant à la position du solveur est effectuée.
   *
   * @param displayParticle affiche ou non les particules des squelettes
   */
  virtual void drawFlame(bool displayParticle)
  {
    Point pt(m_solver->getPosition());
    glPushMatrix();
    glTranslatef (pt.x, pt.y, pt.z);
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->drawFlame(displayParticle);
    for (uint i = 0; i < m_nbCloneFlames; i++)
      m_cloneFlames[i]->drawFlame(displayParticle); 
    glPopMatrix();
  }
  
  virtual void toggleSmoothShading ();

  virtual void setSamplingTolerance(double value){
    FireSource::setSamplingTolerance(value);
    for (uint i = 0; i < m_nbCloneFlames; i++)
      m_cloneFlames[i]->setSamplingTolerance(value);
  };
  void setRenderMode() {  
    FireSource::setRenderMode();
      for (uint i = 0; i < m_nbCloneFlames; i++)
      m_cloneFlames[i]->setRenderMode();
  };

  void setTesselateMode() { 
    FireSource::setTesselateMode();
    for (uint i = 0; i < m_nbCloneFlames; i++)
      m_cloneFlames[i]->setTesselateMode();
  };
private:  
  /** Nombre de flammes */
  uint m_nbCloneFlames;
  /** Tableau contenant les flammes */
  ClonePointFlame **m_cloneFlames;
};


#endif

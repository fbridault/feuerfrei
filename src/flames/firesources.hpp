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

/** La classe Candle permet la d�finition d'une bougie.
 *
 * @author	Flavien Bridault
 */
class Candle : public FireSource
{
public:
  /** Constructeur d'une bougie.
   * @param s pointeur sur le solveur de fluides
   * @param posRel position du centre de la flamme dans le solveur.
   * @param scene pointeur sur la sc�ne
   * @param innerForce force int�rieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL)
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes p�riph�riques
   */
  Candle(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *filename, uint index, 
	 CgSVShader * shader, double rayon );
  virtual ~Candle(){};
};


/** La classe Firmalampe permet la d�finition d'une firmalampe.
 *
 * @author	Flavien Bridault
 */
class Firmalampe : public FireSource
{
public:
  /** Constructeur d'une bougie.
   * @param s pointeur sur le solveur de fluides
   * @param posRel position du centre de la flamme dans le solveur.
   * @param scene pointeur sur la sc�ne
   * @param innerForce force int�rieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL)
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes p�riph�riques
   * @param wickFileName nom du fichier contenant la m�che
   */
  Firmalampe(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *filename, uint index, 
	     CgSVShader * shader, const char *wickFileName);
  virtual ~Firmalampe(){};
};

/** La classe Torche permet la d�finition d'une flamme de type torche.
 * Le fichier OBJ repr�sentant le luminaire contient des m�ches qui doivent avoir un nom
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
   * @param scene pointeur sur la sc�ne
   * @param innerForce force int�rieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL)
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes p�riph�riques
   * @param wickFileName nom du fichier contenant la torche
   */
  Torch(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *torchName, uint index, CgSVShader * shader);
  virtual ~Torch(){}; 

  /** Dessine la m�che de la flamme. Les m�ches des BasicFlame sont d�finies en (0,0,0), une translation
   * est donc effectu�e pour tenir compte du placement du feu dans le monde.
   */
  virtual void drawWick(bool displayBoxes) {};
};

/** La classe CampFire permet la d�finition d'un feu de camp.
 * Le fichier OBJ repr�sentant le luminaire contient des m�ches qui doivent avoir un nom
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
   * @param scene pointeur sur la sc�ne
   * @param innerForce force int�rieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL)
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes p�riph�riques
   * @param wickFileName nom du fichier contenant la torche
   */
  CampFire(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *fireName, uint index, CgSVShader * shader);
  virtual ~CampFire(){}; 
};

/** La classe CandleStick permet la d�finition d'un chandelier.
 *
 * @author	Flavien Bridault
 */
class CandleStick : public FireSource
{
public:
  /** Constructeur d'une bougie.
   * @param s pointeur sur le solveur de fluides
   * @param posRel position du centre de la flamme dans le solveur.
   * @param scene pointeur sur la sc�ne
   * @param innerForce force int�rieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL)
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes
   * @param rayon rayon de la flamme
   * @param nbSkeletons nombre de squelettes p�riph�riques
   */
  CandleStick(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *filename, uint index, 
	      CgSVShader * shader, double rayon );
  virtual ~CandleStick();
  
  /** Fonction charg�e de construire les flammes composant la source de feu. Elle se charge �galement 
   * de d�terminer la position de la source de lumi�re.
   */
  virtual void build();
  
    /** Dessine la m�che de la flamme. Les m�ches des BasicFlame sont d�finies en (0,0,0), une translation
   * est donc effectu�e pour tenir compte du placement du feu dans le monde.
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
  
    /** Fonction appel�e par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture. La flamme d'une BasicFlame est d�finie dans le rep�re du solveur,
   * donc seule une translation correspondant � la position du solveur est effectu�e.
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

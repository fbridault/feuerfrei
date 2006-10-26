#if !defined(FIRESOURCES_H)
#define FIRESOURCES_H

class Candle;
class Firmalampe;

#include "abstractFires.hpp"
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
   * @param flameConfig Configuration de la flamme.
   * @param s Pointeur sur le solveur de fluides.
   * @param scene Pointeur sur la sc�ne.
   * @param filename Nom du fichier contenant le luminaire.
   * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL).
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes.
   * @param rayon Rayon de la flamme.
   */
  Candle(FlameConfig *flameConfig, Solver3D * s, Scene *scene, const char *filename, uint index, 
	 CgSVShader * shader, double rayon);
  /** Destructeur */
  virtual ~Candle(){};
};


/** La classe Firmalampe permet la d�finition d'une firmalampe.
 *
 * @author	Flavien Bridault
 */
class Firmalampe : public FireSource
{
public:
  /** Constructeur d'une torche.
   * @param flameConfig Configuration de la flamme.
   * @param s Pointeur sur le solveur de fluides.
   * @param scene Pointeur sur la sc�ne.
   * @param filename nom du fichier contenant le luminaire.
   * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL).
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes.
   * @param wickFileName nom du fichier contenant la m�che
   */
  Firmalampe(FlameConfig *flameConfig, Solver3D * s, Scene *scene, const char *filename, uint index, 
	     CgSVShader * shader, const char *wickFileName);
  /** Destructeur */
  virtual ~Firmalampe(){};
};

/** La classe Torche permet la d�finition d'une flamme de type torche.
 * Le fichier OBJ repr�sentant le luminaire contient des m�ches qui doivent avoir un nom
 * en Wick*. Celle-ci ne sont pas affich�es � l'�cran. Les objets composant le luminaire 
 * doivent s'appeler Torch.*
 *
 * @author	Flavien Bridault
 */
class Torch : public DetachableFireSource
{
public:
  /** Constructeur d'une torche.
   * @param flameConfig Configuration de la flamme.
   * @param s Pointeur sur le solveur de fluides.
   * @param scene Pointeur sur la sc�ne.
   * @param torchName nom du fichier contenant le luminaire.
   * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL).
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes.
   */
  Torch(FlameConfig *flameConfig, Solver3D * s, Scene *scene, const char *torchName, uint index, 
	CgSVShader * shader);
  /** Destructeur */
  virtual ~Torch(){}; 

  /** Dessine la m�che de la flamme. Les m�ches des RealFlame sont d�finies en (0,0,0), une translation
   * est donc effectu�e pour tenir compte du placement du feu dans le monde.
   */
  virtual void drawWick(bool displayBoxes) {};

};

/** La classe CampFire permet la d�finition d'un feu de camp.
 * Le fichier OBJ repr�sentant le luminaire contient des m�ches qui doivent avoir un nom
 * en Wick*. A la diff�rence de la classe Torch, les m�ches sont affich�es. En revanche,
 * il n'existe pas de luminaire.
 *
 * @author	Flavien Bridault
 */
class CampFire : public DetachableFireSource
{
public:
  /** Constructeur d'une torche.
   * @param flameConfig Configuration de la flamme.
   * @param s Pointeur sur le solveur de fluides.
   * @param scene Pointeur sur la sc�ne.
   * @param fireName nom du fichier contenant le luminaire.
   * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL).
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes.
   */
  CampFire(FlameConfig *flameConfig, Solver3D * s, Scene *scene, const char *fireName, uint index, 
	   CgSVShader * shader);
  /** Destructeur */
  virtual ~CampFire(){};
};

/** La classe CandleStick permet la d�finition d'un chandelier. Elle est compos�e de flammes
 * clones.
 *
 * @author	Flavien Bridault
 */
class CandleStick : public FireSource
{
public:
  /** Constructeur d'un chandelier.
   * @param flameConfig Configuration de la flamme.
   * @param s Pointeur sur le solveur de fluides.
   * @param scene Pointeur sur la sc�ne.
   * @param filename Nom du fichier contenant le luminaire.
   * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL).
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes.
   * @param rayon Rayon de la flamme.
   */
  CandleStick(FlameConfig *flameConfig, Solver3D * s, Scene *scene, const char *filename, uint index, 
	      CgSVShader * shader, double rayon);
  /** Destructeur */
  virtual ~CandleStick();
  
  virtual void build();
  
  virtual void drawWick(bool displayBoxes)
  {      
    Point pt(getPosition());
    glPushMatrix();
    glTranslatef (pt.x, pt.y, pt.z);
    glScalef (m_solver->getDimX(), m_solver->getDimY(), m_solver->getDimZ());
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->drawWick(displayBoxes);
    for (uint i = 0; i < m_nbCloneFlames; i++)
      m_cloneFlames[i]->drawWick(displayBoxes);
    glPopMatrix();
  }
  
  virtual void drawFlame(bool display, bool displayParticle)
  {
    Point pt(m_solver->getPosition());
    glPushMatrix();
    glTranslatef (pt.x, pt.y, pt.z);
    glScalef (m_solver->getDimX(), m_solver->getDimY(), m_solver->getDimZ());
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->drawFlame(display, displayParticle);
    for (uint i = 0; i < m_nbCloneFlames; i++)
      m_cloneFlames[i]->drawFlame(display, displayParticle); 
    glPopMatrix();
  }
  
  virtual void toggleSmoothShading ();  
  virtual void setSamplingTolerance(double value){
    FireSource::setSamplingTolerance(value);
    for (uint i = 0; i < m_nbCloneFlames; i++)
      m_cloneFlames[i]->setSamplingTolerance(value);
  };
  
private:  
  /** Nombre de flammes */
  uint m_nbCloneFlames;
  /** Tableau contenant les flammes */
  ClonePointFlame **m_cloneFlames;
};


#endif

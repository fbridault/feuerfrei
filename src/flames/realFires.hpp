#if !defined(FIRESOURCES_H)
#define FIRESOURCES_H

class Candle;
class Firmalampe;

#include "abstractFires.hpp"
#include "cloneFlames.hpp"

class PointFlame;
class LineFlame;
class FireSource;

/** La classe Candle permet la définition d'une bougie.
 *
 * @author	Flavien Bridault
 */
class Candle : public FireSource
{
public:
  /** Constructeur d'une bougie.
   * @param flameConfig Configuration de la flamme.
   * @param s Pointeur sur le solveur de fluides.
   * @param scene Pointeur sur la scène.
   * @param filename Nom du fichier contenant le luminaire.
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL).
   * @param program pointeur sur le program chargé de la construction des shadow volumes.
   * @param rayon Rayon de la flamme.
   */
  Candle(FlameConfig* const flameConfig, Field3D * s, Scene *scene, const char *filename, uint index, 
	 const GLSLProgram * const program, double rayon);
  /** Destructeur */
  virtual ~Candle(){};
};


/** La classe Firmalampe permet la définition d'une firmalampe.
 *
 * @author	Flavien Bridault
 */
class Firmalampe : public FireSource
{
public:
  /** Constructeur d'une torche.
   * @param flameConfig Configuration de la flamme.
   * @param s Pointeur sur le solveur de fluides.
   * @param scene Pointeur sur la scène.
   * @param filename nom du fichier contenant le luminaire.
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL).
   * @param program pointeur sur le program chargé de la construction des shadow volumes.
   * @param wickFileName nom du fichier contenant la mèche
   */
  Firmalampe(FlameConfig* const flameConfig, Field3D * s, Scene *scene, const char *filename, uint index, 
	     const GLSLProgram * const program, const char *wickFileName);
  /** Destructeur */
  virtual ~Firmalampe(){};
};

/** La classe Torche permet la définition d'une flamme de type torche.
 * Le fichier OBJ représentant le luminaire contient des mèches qui doivent avoir un nom
 * en Wick*. Celle-ci ne sont pas affichées à l'écran. Les objets composant le luminaire 
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
   * @param scene Pointeur sur la scène.
   * @param torchName nom du fichier contenant le luminaire.
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL).
   * @param program pointeur sur le program chargé de la construction des shadow volumes.
   */
  Torch(FlameConfig* const flameConfig, Field3D * s, Scene *scene, const char *torchName, uint index, 
	const GLSLProgram * const program);
  /** Destructeur */
  virtual ~Torch(){}; 

  /** Dessine la mèche de la flamme. Les mèches des RealFlame sont définies en (0,0,0), une translation
   * est donc effectuée pour tenir compte du placement du feu dans le monde.
   */
  virtual void drawWick(bool displayBoxes) const {};

};

/** La classe CampFire permet la définition d'un feu de camp.
 * Le fichier OBJ représentant le luminaire contient des mèches qui doivent avoir un nom
 * en Wick*. A la différence de la classe Torch, les mèches sont affichées. En revanche,
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
   * @param scene Pointeur sur la scène.
   * @param fireName nom du fichier contenant le luminaire.
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL).
   * @param program pointeur sur le program chargé de la construction des shadow volumes.
   */
  CampFire(FlameConfig* const flameConfig, Field3D * s, Scene *scene, const char *fireName, uint index, 
	   const GLSLProgram * const program);
  /** Destructeur */
  virtual ~CampFire(){};
};

#include "../solvers/fieldThread.hpp"

/** La classe Torche permet la définition d'une flamme de type torche.
 * Le fichier OBJ représentant le luminaire contient des mèches qui doivent avoir un nom
 * en Wick*. Celle-ci ne sont pas affichées à l'écran. Les objets composant le luminaire 
 * doivent s'appeler Torch.*
 *
 * @author	Flavien Bridault
 */
class CandlesSet : public FireSource
{
public:
  /** Constructeur d'un ensemble de bougies. A la base, cette classe a été créée pour permettre
   * de modéliser la lampe "tour" vue au Cyprus Museum de Nicosie. La particularité de cette source de feu
   * qu'elle crée elle même une liste de solveurs. En effet, à la lecture du fichier OBJ contenant le luminaire,
   * chaque objet nommé "Wick*" donne naissance à une bougie, qui chacune a besoin d'un solveur.
   * @param flameConfig Configuration de la flamme.
   * @param s Pointeur sur le solveur de fluides.
   * @param flameSolvers Liste des solveurs créés.
   * @param scene Pointeur sur la scène.
   * @param lampName nom du fichier contenant le luminaire.
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL).
   * @param program pointeur sur le program chargé de la construction des shadow volumes.
   */
  CandlesSet(FlameConfig* const flameConfig, Field3D *s, list <FieldFlamesThread *>& fieldThreads, 
	     FieldThreadsScheduler* const scheduler, Scene *scene,
	     const char *lampName, uint index, const GLSLProgram * const program, Point scale);
  /** Destructeur */
  virtual ~CandlesSet();
  
  virtual void build();
  
  virtual void drawFlame(bool display, bool displayParticle, u_char boundingVolume=0) const
  {
    switch(boundingVolume){
    case BOUNDING_SPHERE : drawBoundingSphere(); break;
    case BOUNDING_BOX : drawBoundingBox(); break;
    default : 
      if(m_visibility)
	{
	  Point pt(m_solver->getPosition());
	  Point scale(m_solver->getScale());	
	  pt = getPosition();
	  scale = m_solver->getScale();
	  glPushMatrix();
	  glTranslatef (pt.x, pt.y, pt.z);
	  glScalef (scale.x, scale.y, scale.z);
	  
	  for (uint i = 0; i < m_nbFlames; i++){
	    pt = m_flames[i]->getSolver()->getPosition();
	    scale =  m_flames[i]->getSolver()->getScale();
	    glPushMatrix();
	    glTranslatef (pt.x, pt.y, pt.z);
	    glScalef (scale.x, scale.y, scale.z);
 	    m_flames[i]->drawFlame(display, displayParticle);
//  	    m_flames[i]->getSolver()->displayGrid();
	    glPopMatrix();
	  }
	  glPopMatrix();
	}
    }
  };
  
  virtual void drawBoundingSphere() const
  {
    Point pt;
    if(m_visibility)
      for (uint i = 0; i < m_nbFlames; i++){
	pt = m_flames[i]->getSolver()->getPosition();
	m_flames[i]->drawBoundingSphere();
      }
  }

  virtual void drawBoundingBox() const
  {
    Point pt;
    if(m_visibility)
      for (uint i = 0; i < m_nbFlames; i++){
	pt = m_flames[i]->getSolver()->getPosition();
	m_flames[i]->drawBoundingSphere();
      }
  }

  virtual void computeVisibility(const Camera &view, bool forceSpheresBuild=false);
private:
  list < FieldFlamesAssociation* > m_fieldFlamesAssociations;
};

/** La classe CandleStick permet la définition d'un chandelier. Elle est composée de flammes
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
   * @param scene Pointeur sur la scène.
   * @param filename Nom du fichier contenant le luminaire.
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL).
   * @param program pointeur sur le program chargé de la construction des shadow volumes.
   * @param rayon Rayon de la flamme.
   */
  CandleStick(FlameConfig* const flameConfig, Field3D * s, Scene *scene, const char *filename, uint index, 
	      const GLSLProgram * const program, double rayon);
  /** Destructeur */
  virtual ~CandleStick();
  
  virtual void build();
  
  virtual void drawWick(bool displayBoxes) const
  {
    Point pt(getPosition());
    Point scale(m_solver->getScale());
    glPushMatrix();
    glTranslatef (pt.x, pt.y, pt.z);
    glScalef (scale.x, scale.y, scale.z);
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->drawWick(displayBoxes);
    for (uint i = 0; i < m_nbCloneFlames; i++)
      m_cloneFlames[i]->drawWick(displayBoxes);
    glPopMatrix();
  }
  
  virtual void drawFlame(bool display, bool displayParticle, u_char boundingVolume=0) const
  {
    switch(boundingVolume){
    case BOUNDING_SPHERE : drawBoundingSphere(); break;
    case BOUNDING_BOX : drawBoundingBox(); break;
    default : 
      if(m_visibility)
	{
	  Point pt(m_solver->getPosition());
	  Point scale(m_solver->getScale());
	  glPushMatrix();
	  glTranslatef (pt.x, pt.y, pt.z);
	  glScalef (scale.x, scale.y, scale.z);
	  for (uint i = 0; i < m_nbFlames; i++)
	    m_flames[i]->drawFlame(display, displayParticle);
	  for (uint i = 0; i < m_nbCloneFlames; i++)
	    m_cloneFlames[i]->drawFlame(display, displayParticle); 
	  glPopMatrix();
	}
    }
  }
  
  virtual void toggleSmoothShading (bool state);  
  virtual void setSamplingTolerance(u_char value){
    FireSource::setSamplingTolerance(value);
    for (uint i = 0; i < m_nbCloneFlames; i++)
      m_cloneFlames[i]->setSamplingTolerance(value);
  };
  
private:  
  /** Nombre de flammes clones */
  uint m_nbCloneFlames;
  /** Tableau contenant les flammes clones */
  ClonePointFlame **m_cloneFlames;
};


#endif

#ifndef OBJECT_H
#define OBJECT_H

class Object;
class Mesh;

#include "material.hpp"
#include "source.hpp"

#define ALL      0
#define TEXTURED 1
#define FLAT     2
#define AMBIENT  3

#include <list>

class Scene;

/** Classe représentant un groupe d'objets. Elle stocke les points, les normales et les coordonnées de ces points. 
 * Ceci permet d'éviter des changements trop fréquents de VBO.
 */
class Object
{ 
public:
  /**
   * Constructeur par d&eacute;faut.
   * @param scene Pointeur vers la scene.
   */
  Object(Scene *scene);
  
  /** Destructeur par défaut. */
  virtual ~Object ();
  
  void addMesh(Mesh* const mesh)
  {
    m_meshesList.push_back(mesh);
  }
  
  /** Affecte les points, les normales et les coordonnées de texture.
   * @param newVertex point à ajouter 
   */
  void setGeometryCount (uint nbVertex, uint nbNormals, uint nbTexCoords)
  {
    m_nbVertex = nbVertex;
    m_nbNormals = nbNormals;
    m_nbTexCoords = nbTexCoords;
  };
  
  /** Affecte les points, les normales et les coordonnées de texture.
   * @param newVertex point à ajouter 
   */
  void setGeometry (GLfloat *vertex, GLfloat *normals, GLfloat *texCoords)
  {
    m_vertexArray = vertex;
    m_normalsArray = normals;
    m_texCoordsArray = texCoords;
  };
  
  /** Lecture du nombre de points contenus dans l'objet.
   * @return Nombre de points.
   */
  int getVertexArraySize () const { return m_nbVertex; };
  
  /** Lecture du nombre de normales contenus dans l'objet.
   * @return Nombre de normales.
   */
  int getNormalsArraySize () const { return m_nbNormals; };
  
  /** Lecture du nombre de normales contenus dans l'objet.
   * @return Nombre de normales.
   */
  int getTexCoordsArraySize () const { return m_nbTexCoords; };
  
  /** Lecture des points contenus dans l'objet.
   * @return Tableau de points.
   */
  GLfloat getVertex (GLuint i) const { return m_vertexArray[i]; };
  
  /** Lecture des normales contenus dans l'objet.
   * @return Tableau des normales.
   */
  GLfloat getNormal (GLuint i) const { return m_normalsArray[i]; };
  
  /** Lecture des coordonnées de texture contenus dans l'objet.
   * @return Tableau des coordonnées de texture.
   */
  GLfloat getTexCoord (GLuint i) const { return m_texCoordsArray[i]; };
  
  /** Donne l'englobant de l'objet.
   * @param max Retourne le coin supérieur de l'englobant.
   * @param min Retourne le coin inférieur de l'englobant.
   */
  void getBoundingBox (Point& max, Point& min);
  
  /** Fonction de dessin du groupe d'objets.
   * @param drawCode 
   * si TEXTURED, alors l'objet n'est dessiné que s'il possède une texture
   * si FLAT alors l'objet n'est dessiné que s'il ne possède pas une texture
   * si ALL alors l'objet est dessiné inconditionnellement
   * si AMBIENT alors l'objet est dessiné avec un matériau blanc en composante ambiante (pour les ombres)
   * @param tex false si l'objet texturé doit être affiché sans sa texture
   */
  void draw(char drawCode=ALL, bool tex=true);
  
  /** Lecture du nombre de polygones contenus dans l'objet.
   * @return Nombre de polygones.
   */
  uint getPolygonsCount ();
  
protected:
  /**<Liste des points de l'objet */
  GLfloat *m_vertexArray;
  /**<Liste des coordonnées de textures de l'objet */
  GLfloat *m_texCoordsArray;
  /**<Liste des normales de l'objet */
  GLfloat *m_normalsArray;
  GLuint m_nbVertex, m_nbTexCoords, m_nbNormals;

private:
  /** Liste des objets */
  list < Mesh* > m_meshesList;  
  
  /** Indice du matériau utilisé par le point précédent. Ceci permet de savoir lors de la phase de dessin
   * si le point courant utilise un autre matériau qui nécessite un appel à glMaterial().
   */
  int m_lastMaterialIndex;
  /** Permet de vérifier si le mesh précédent était texturé, permet de limiter les activations et 
   * désactivations des unités de texture. 
   */
  bool m_previousMeshWasTextured;
  
  /** Pointeur vers la scène. */
  Scene *m_scene;
};

/** 
 * Classe repr&eacute;sentant un maillage.
 * Un maillage sc&egrave;ne comporte une liste index&eacute;e des polygones, des normales et des coordonnées de textures. 
 * Un seul matériau est appliqué à un maillage.
 *
 * @author	Flavien Bridault
 */
class Mesh
{
public:
  /**
   * Constructeur par d&eacute;faut.
   * @param scene Pointeur vers la scene.
   */
  Mesh (Scene * const scene, uint materialIndex, Object *parent);
  
  /** Destructeur par défaut. */
  virtual ~Mesh ();
  
  /** Affecte les points, les normales et les coordonnées de texture.
   * @param newVertex point à ajouter 
   */
  void setGeometryIndexCount (GLuint nbVertexIndex, GLuint nbNormalsIndex, GLuint nbTexCoordsIndex)
  {
    m_nbVertexIndex = nbVertexIndex;
    m_nbNormalsIndex = nbNormalsIndex;
    m_nbTexCoordsIndex = nbTexCoordsIndex;
  };
  
  /** Affecte les points, les normales et les coordonnées de texture.
   * @param newVertex point à ajouter 
   */
  void setGeometryIndex (GLuint *vertexIndex, GLuint *normalsIndex, GLuint *texCoordsIndex)
  {
    m_vertexIndexArray = vertexIndex;
    m_normalsIndexArray = normalsIndex;
    m_texCoordsIndexArray = texCoordsIndex;
  };
  
  /** Lecture du nombre de polygones contenus dans le maillage.
   * @return Nombre de polygones.
   */
  uint getPolygonsCount () const { return (m_nbVertexIndex / 3); };
  uint getMaterialIndex () const { return (m_materialIndex); };
  
  /** Met à jour les attributs de l'objet. */
  void setAttributes (int attr)
  {
    m_attributes = attr;
  };
  
  /** Fonction de dessin de l'objet.
   * @param drawCode 
   * si TEXTURED, alors l'objet n'est dessiné que s'il possède une texture
   * si FLAT alors l'objet n'est dessiné que s'il ne possède pas une texture
   * si ALL alors l'objet est dessiné inconditionnellement
   * si AMBIENT alors l'objet est dessiné avec un matériau blanc en composante ambiante (pour les ombres)
   * @param tex false si l'objet texturé doit être affiché sans sa texture
   */
  void draw(char drawCode, bool tex, uint& lastMaterialIndex);

  const bool isTransparent ();

private:
  /**<Liste des indices des points des facettes */
  GLuint *m_vertexIndexArray;
  /**<Liste des indices des normales des facettes */
  GLuint *m_normalsIndexArray;
  /**<Liste des indices des coordonnées de texture des facettes */
  GLuint *m_texCoordsIndexArray;
  
  GLuint m_nbVertexIndex, m_nbTexCoordsIndex, m_nbNormalsIndex;
  
  /** Pointeur vers la scène. */
  Scene *m_scene;
  /** Pointeur vers l'objet parent */
  Object *m_parent;
  
  /** Pointeur vers le matériau utilisé. */
  uint m_materialIndex;
  int m_attributes;
};

#endif

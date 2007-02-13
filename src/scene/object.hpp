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

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

#include <list>

class Vertex
{
public:
  GLfloat u, v, nx, ny, nz, x, y, z;
  /* On ne peut pas d�finir d'op�rateur sans condamner l'utilisation du memcpy */
  static void copy(const Vertex& src, Vertex& dest){ 
    dest.u  = src.u ; dest.v  = src.v;
    dest.nx = src.nx; dest.ny = src.ny; dest.nz = src.nz;
    dest.x  = src.x ; dest.y  = src.y;  dest.z  = src.z;
  };
};

class Scene;

/** Classe repr�sentant un groupe d'objets. Elle stocke les points, les normales et les coordonn�es de ces points. 
 * Ceci permet d'�viter des changements trop fr�quents de VBO.
 */
class Object
{ 
public:
  /**
   * Constructeur par d&eacute;faut.
   * @param scene Pointeur vers la scene.
   */
  Object(Scene *scene);
  
  /** Destructeur par d�faut. */
  virtual ~Object ();
  
  void addMesh(Mesh* const mesh)
  {
    m_meshesList.push_back(mesh);
  }
  
  /** Lecture du nombre de points contenus dans l'objet.
   * @return Nombre de points.
   */
  uint getVertexArraySize () const { return m_vertexArray.size(); };
  
  Vertex getVertex (GLuint i) const { return m_vertexArray[i]; };
  
  /** Donne l'englobant de l'objet.
   * @param max Retourne le coin sup�rieur de l'englobant.
   * @param min Retourne le coin inf�rieur de l'englobant.
   */
  void getBoundingBox (Point& max, Point& min);
  
  void bindVBO() { 
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
    glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(2*sizeof(float)));
    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(5*sizeof(float)));
   };
  
  void buildVBOs();
  /** Fonction de dessin du groupe d'objets.
   * @param drawCode 
   * si TEXTURED, alors l'objet n'est dessin� que s'il poss�de une texture
   * si FLAT alors l'objet n'est dessin� que s'il ne poss�de pas une texture
   * si ALL alors l'objet est dessin� inconditionnellement
   * si AMBIENT alors l'objet est dessin� avec un mat�riau blanc en composante ambiante (pour les ombres)
   * @param tex false si l'objet textur� doit �tre affich� sans sa texture
   */
  void draw(char drawCode=ALL, bool tex=true);
  
  /** Lecture du nombre de polygones contenus dans l'objet.
   * @return Nombre de polygones.
   */
  uint getPolygonsCount ();
  
  /** Ajout d'un point dans le tableau de points.*/
  void addVertex( const Vertex& v) { m_vertexArray.push_back(v); };
  
  /** Affectation des coordonn�es de texture et de la normale d'un point donn�.*/
  void setVertex( uint i, float u, float v, float nx, float ny, float nz) { 
    m_vertexArray[i].u  =  u;
    m_vertexArray[i].v  =  v;
    m_vertexArray[i].nx = nx;
    m_vertexArray[i].ny = ny;
    m_vertexArray[i].nz = nz;
  };
  
  void allocHashTable(){ m_hashTable = new int[m_vertexArray.size()]; };
  
  void initHashTable(){ for(uint i=0; i<m_vertexArray.size(); i++) m_hashTable[i] = -1; };
  
  /** Ajout d'un indice dans la table de hachage.
   * @param i indice du point dans le tableau de points.
   * @param ref indice de la r�f�rence du ponit dans le tableau d'indice du mesh courant.
   */
  void addRefInHashTable(uint i, uint ref){ m_hashTable[i] = ref; };
  
  /** Recherche d'un indice dans la table de hachage. *
   * @param i indice recherch�
   */
  bool findRefInHashTable(uint i, Vertex& v)
  { 
    if( m_hashTable[i] >= 0){
      v = m_vertexArray[m_hashTable[i]];
      return true;
    }
    return false;
  }

protected:
  /**<Liste des points de l'objet */
  vector <Vertex> m_vertexArray;
  
private:
  /** Liste des objets */
  list < Mesh* > m_meshesList;
  /** Table de hachage permettant, lors de la reconstruction des index des normales et des coordonn�es de texture
   * d'un mesh, de stocker les r�f�rences � un indice d'un point. Seule une r�f�rence est stock�e pour chaque point 
   * identique. Ce point est ensuite r�cup�r� pour comparaison.
   */
  int *m_hashTable;
  
  /** Indice du mat�riau utilis� par le point pr�c�dent. Ceci permet de savoir lors de la phase de dessin
   * si le point courant utilise un autre mat�riau qui n�cessite un appel � glMaterial().
   */
  int m_lastMaterialIndex;
  /** Permet de v�rifier si le mesh pr�c�dent �tait textur�, permet de limiter les activations et 
   * d�sactivations des unit�s de texture. 
   */
  bool m_previousMeshWasTextured;
  
  /** Pointeur vers la sc�ne. */
  Scene *m_scene;

  GLuint m_bufferID;
  
  uint m_attributes;
};

/** 
 * Classe repr&eacute;sentant un maillage.
 * Un maillage sc&egrave;ne comporte une liste index&eacute;e des polygones, des normales et des coordonn�es de textures. 
 * Un seul mat�riau est appliqu� � un maillage.
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
  
  /** Destructeur par d�faut. */
  virtual ~Mesh ();
  
  /** Lecture du nombre de polygones contenus dans le maillage.
   * @return Nombre de polygones.
   */
  uint getPolygonsCount () const { return (m_indexArray.size() / 3); };
  uint getMaterialIndex () const { return (m_materialIndex); };
  
  /** Met � jour les attributs de l'objet. */
  void setAttributes (uint attr) { m_attributes = attr; };
  /** R�cup�rer les attributs de l'objet. */
  uint getAttributes () { return m_attributes; };
  
  void buildVBOs();
  /** Fonction de dessin de l'objet avec utilisation des VBOs.
   * @param drawCode 
   * si TEXTURED, alors l'objet n'est dessin� que s'il poss�de une texture
   * si FLAT alors l'objet n'est dessin� que s'il ne poss�de pas une texture
   * si ALL alors l'objet est dessin� inconditionnellement
   * si AMBIENT alors l'objet est dessin� avec un mat�riau blanc en composante ambiante (pour les ombres)
   * @param tex false si l'objet textur� doit �tre affich� sans sa texture
   * @param lastMaterialIndex indice du dernier mat�riau appliqu�, utilis� en entr�e et en sortie.
   */
  void draw(char drawCode, bool tex, uint& lastMaterialIndex);
  
  void setUVsAndNormals(vector < Vector > &normalsVector,   vector < GLuint > &normalsIndexVector, 
			vector < Point >  &texCoordsVector, vector < GLuint > &texCoordsIndexVector);

  const bool isTransparent ();
  
  void addIndex( GLuint i ) { m_indexArray.push_back(i); };
  
private:
  /**<Liste des indices des points des facettes */
  vector <GLuint> m_indexArray;
  
  /** Pointeur vers la sc�ne. */
  Scene *m_scene;
  /** Pointeur vers l'objet parent */
  Object *m_parent;
  
  /** Pointeur vers le mat�riau utilis�. */
  uint m_materialIndex;
  uint m_attributes;
  GLuint m_bufferID;
};

#endif

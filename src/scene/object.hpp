#ifndef OBJECT_H
#define OBJECT_H

class Object;
class Mesh;

#include "source.hpp"
#include "camera.hpp"
#include "graphicsFn.hpp"

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
};

class BoundingSphere
{
public:
  BoundingSphere() : radius(0.0) {};
  /** Centre de la sph�re englobante. */
  Point centre;
  
  /** Rayon de la sph�re englobante. */
  double radius;
  
  /** Calcule la visibilit� de la sph�re par rapport au point de vue courant.
   * @param view R�f�rence sur la cam�ra
   */
  bool isVisible(const Camera &view) const{
    uint i;
    const double *plan;
    
    // Centre dans le frustrum ?
    //   for( p = 0; p < 6; p++ )
    //     if( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= 0 ){
    //       m_visibility = false;
    //       return;
    //     }
    
    // Sph�re dans le frustrum ?
    for( i = 0; i < 6; i++ ){
      plan=view.getFrustum(i);
      if( plan[0] * centre.x + plan[1] * centre.y + plan[2] * centre.z + plan[3] <= -radius )
	return false;
    }
    return true;
  }  
  
  /** Calcule la visibilit� de la sph�re par rapport au point de vue courant.
   * Variation permettant de r�cup�rer en m�me temps la distance par rapport � la cam�ra.
   * @param view R�f�rence sur la cam�ra.
   * @return 0 si non visible, distance � la cam�ra sinon.
   */
  double visibleDistance(const Camera &view) const{
    uint i;
    const double *plan;
    double d;
    
    // Sph�re dans le frustrum ?
    for( i = 0; i < 6; i++ ){
      plan=view.getFrustum(i);
      d = plan[0] * centre.x + plan[1] * centre.y + plan[2] * centre.z + plan[3];
      if( d <= -radius )
	return 0;
    }
   return d + radius;
  }
  
  void draw(void) const{
    glPushMatrix();
    glTranslatef(centre.x, centre.y, centre.z);
    glColor3d(1.0,0.0,0.0);
    GraphicsFn::SolidSphere(radius, 20, 20);
    glPopMatrix();
  }
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
  Object(const Scene* const scene);
  
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
  
  void bindVBO() const 
  { 
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
    glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(2*sizeof(float)));
    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(5*sizeof(float)));
   };
  
  /** Construction du Vertex Buffer Object de l'objet, ici le tableau de points, normales et coordonn�es de texture. */
  void buildVBO();
  
  /** Fonction de dessin du groupe d'objets.
   * @param drawCode 
   * si TEXTURED, alors l'objet n'est dessin� que s'il poss�de une texture
   * si FLAT alors l'objet n'est dessin� que s'il ne poss�de pas une texture
   * si ALL alors l'objet est dessin� inconditionnellement
   * si AMBIENT alors l'objet est dessin� avec un mat�riau blanc en composante ambiante (pour les ombres)
   * @param tex false si l'objet textur� doit �tre affich� sans sa texture
   * @param boundingSpheres true si l'objet doit afficher les sph�res englobantes
   */
  void draw(char drawCode=ALL, bool tex=true, bool boundingSpheres=false) const;
  
  /** Lecture du nombre de polygones contenus dans l'objet.
   * @return Nombre de polygones.
   */
  uint getPolygonsCount () const;
  
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
  
  /** Allocation de la table de hachage, �ventuellement d�truite si elle a �t� allou�e pr�cedemment. */
  void allocHashTable(){ if(m_hashTable) delete [] m_hashTable; m_hashTable = new int[m_vertexArray.size()]; };
  
  /** Initialisation de tous les �l�ments de la table de hachage � -1. */
  void initHashTable(){ for(uint i=0; i<m_vertexArray.size(); i++) m_hashTable[i] = -1; };
  
  /** Ajout d'un indice dans la table de hachage.
   * @param i indice du point dans le tableau de points.
   * @param ref indice de la r�f�rence du point dans le tableau d'indice du mesh courant.
   */
  void addRefInHashTable(uint i, uint ref){ m_hashTable[i] = ref; };
  
  /** Recherche d'un indice dans la table de hachage. *
   * @param i indice recherch�
   */
  bool findRefInHashTable(uint i)
  { 
    if( m_hashTable[i] >= 0)
      return true;
    return false;
  }

  /** Calcule la visibilit� de l'objet
   * @param view R�f�rence sur la cam�ra
   */
  void computeVisibility(const Camera &view);

  /** Construction des sph�res englobantes de l'objet. A appeler apr�s l'import de la sc�ne. */
  void buildBoundingSpheres ();

  /** Dessin des sph�res englobantes. */
  void drawBoundingSpheres ();
  
protected:
  /**<Liste des points de l'objet */
  vector <Vertex> m_vertexArray;
  
private:
  /** Liste des objets */
  list < Mesh* > m_meshesList;
  /** Table de hachage permettant, lors de la reconstruction des index des normales et des coordonn�es de texture
   * d'un mesh, de stocker les r�f�rences � un indice d'un point. Seule une r�f�rence est stock�e pour chaque point 
   * identique. Ce point est ensuite utilis� pour comparaison.
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
  const Scene *m_scene;

  /* Identifiant du Vertex Buffer Object. */
  GLuint m_bufferID;
  
  /** Type d'attributs pr�sents dans le maillage soit :<li>
   * <ol>0 pour points,</ol>
   * <ol>1 pour points et normales,</ol>
   * <ol>2 pour points et normales,</ol>
   * <ol>3 pour points, normales et coordonn�es de texture.</ol>
   * </li> */
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
  Mesh (const Scene* const scene, uint materialIndex, Object *parent);
  
  /** Destructeur par d�faut. */
  virtual ~Mesh ();
  
  /** Lecture du nombre de polygones contenus dans le maillage.
   * @return Nombre de polygones. */
  uint getPolygonsCount () const { return (m_indexArray.size() / 3); };
  
  /** Lecture de l'index du mat�riau utilis� par le maillage. 
   * @return Index du mat�riau dans la liste de mat�riau contenu dans la sc�ne. */
  uint getMaterialIndex () const { return (m_materialIndex); };
  
  /** Met � jour les attributs de l'objet. */
  void setAttributes (uint attr) { m_attributes = attr; };
  /** R�cup�rer les attributs de l'objet. */
  uint getAttributes () const { return m_attributes; };
  
  /** Construction du Vertex Buffer Object du maillage, ici le tableau d'indice. */
  void buildVBO() const;
  
  /** Fonction de dessin de l'objet avec utilisation des VBOs.
   * @param drawCode 
   * si TEXTURED, alors l'objet n'est dessin� que s'il poss�de une texture
   * si FLAT alors l'objet n'est dessin� que s'il ne poss�de pas une texture
   * si ALL alors l'objet est dessin� inconditionnellement
   * si AMBIENT alors l'objet est dessin� avec un mat�riau blanc en composante ambiante (pour les ombres)
   * @param tex false si l'objet textur� doit �tre affich� sans sa texture
   * @param lastMaterialIndex indice du dernier mat�riau appliqu�, utilis� en entr�e et en sortie.
   */
  void draw(char drawCode, bool tex, uint& lastMaterialIndex) const;
  
  /** Fusion des trois tableaux en un seul tableau.
   * Le format OBJ g�re trois tableaux d'indices, alors qu'en OpenGL il n'y a qu'un seul tableau d'indice.
   * L'algorithme pr�sent dans cette fonction r�alise la fusion des ces trois tableaux en un seul.<br>
   * Le principe est le suivant : pour chaque indice du tableau de points, on regarde si il a d�j� �t� r�f�renc�
   * auparavant dans le tableau. Si c'est le cas, alors on regarde si ses normales et ses coordonn�es de texture 
   * (pas les indices mais les coordonn�es r�elles, car certains exportateurs en OBJ ont tendance � dupliquer les
   * normales et les coordonn�es de textures) sont �gales � celle de la premi�re r�f�rence. Dans l'affirmative, il n'y
   * a rien � faire, l'indice du point est correct. Dans le cas contraire, il est n�cessaire de dupliquer le point dans
   * le tableau de point de l'objet, d'affecter les coordonn�es de texture et la normale, et enfin de modifier l'indice
   * du point courant en cons�quence. Enfin dans le cas o� le point n'a pas encore �t� r�f�renc� dans le tableau, on
   * m�morise cette r�f�rence dans une table de hachage utilis� pour le premier cas, et ensuite on affecte les coordonn�es
   * de texture et la normale.
   *
   * @param normalsVector Vecteur des normales.
   * @param normalsIndexVector Vecteur des indices des normales.
   * @param texCoordsVector Vecteur des coordonn�es de texture.
   * @param texCoordsIndexVector Vecteur des indices des coordonn�es de texture. 
   */
  void setUVsAndNormals(const vector < Vector > &normalsVector,   const vector < GLuint > &normalsIndexVector, 
			const vector < Point >  &texCoordsVector, const vector < GLuint > &texCoordsIndexVector);

  const bool isTransparent () const;
  
  /** Ajout d'un index de point dans le tableau d'indices.
   * @param i indice � ajouter.
   */
  void addIndex( GLuint i ) { m_indexArray.push_back(i); };
  
  /** Calcule la visibilit� de l'objet
   * @param view R�f�rence sur la cam�ra
   */
  void computeVisibility(const Camera &view);
  
  /** Construction des sph�res englobantes de l'objet. A appeler apr�s l'import de la sc�ne. */
  void buildBoundingSphere ();
  
  /** Dessin des sph�res englobantes. */
  void drawBoundingSphere ();

private:
  /**<Liste des indices des points des facettes */
  vector <GLuint> m_indexArray;
  
  /** Pointeur vers la sc�ne. */
  const Scene *m_scene;
  /** Pointeur vers l'objet parent */
  Object *m_parent;
  
  /** Pointeur vers le mat�riau utilis�. */
  uint m_materialIndex;

  /** Type d'attributs pr�sents dans le maillage soit :
   * <li>
   * <ol>0 pour points,</ol>
   * <ol>1 pour points et normales,</ol>
   * <ol>2 pour points et normales,</ol>
   * <ol>3 pour points, normales et coordonn�es de texture.</ol>
   * </li> */
  uint m_attributes;
  
  /* Identifiant du Vertex Buffer Object. */
  GLuint m_bufferID;
  
  /** Visibilit� de l'objet par rapport au frustum. */
  bool m_visibility;

  BoundingSphere m_boundingSphere;
};

#endif

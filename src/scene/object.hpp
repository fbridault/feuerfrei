#ifndef OBJECT_H
#define OBJECT_H

class Object;

#include "material.hpp"
#include "source.hpp"

#define ALL      0
#define TEXTURED 1
#define FLAT     2
#define AMBIENT  3

#include <vector>

class Scene;

/** 
 * Classe englobant, pour un point donn�, les indices du point, de la normale,
 * de la coordonn�e de texture, et du mat�riau
*/
class PointIndices
{
public:
  /** Constructeur.
   * @param nv Indice de la position du point.
   * @param nvn Indice de la normale du point.
   * @param nvt Indice de la coordonn�e de texture du point.
   * @param nvm Indice du mat�riau pour ce point.
   */
  PointIndices(uint nv, uint nvn, uint nvt, uint nvm){ v=nv; vn=nvn; vt=nvt; vm=nvm;};
  
  uint v; /** Indice de la position du point. */
  uint vn;  /** Indice de la normale du point. */
  uint vt; /** Indice de la coordonn�e de texture du point. */
  uint vm; /** Indice du mat�riau pour ce point. */
};

/** 
 * Classe repr&eacute;sentant un objet g&eacute;om&eacute;trique.
 * Une sc&egrave;ne comporte une liste index&eacute;e des polygones, une liste des mat&eacute;riaux 
 * associ&eacute;s &agrave; ces polygones et ainsi qu'une liste des sources lumineuses.
 *
 * @author	Flavien Bridault
 */
class Object
{
protected:
  /**<Liste des points de l'objet */
  vector < Point *>m_vertexArray;
  /**<Liste des coordonn�es de textures de l'objet */
  vector < Point *>m_texCoordsArray;
  /**<Liste des normales de l'objet */
  vector < Vector *>m_normalsArray;
  /**<Liste des indices des points des facettes */
  vector < PointIndices *>m_vertexIndexArray;
  
  /** Indique le type de donn�es disponibles pour l'objet. Ceci est fortement li� � l'impl�mentation
   * de l'importateur de fichier OBJ.<br>
   * 0 : coordonn�es<br>
   * 1 : coordonn�es + normales<br>
   * 2 : coordonn�es + normales<br>
   * 3 : coordonn�es + normales + coordonn�es de texture
   */
  int m_attributes;
  
  /** Indice du mat�riau utilis� par le point pr�c�dent. Ceci permet de savoir lors de la phase de dessin
   * si le point courant utilise un autre mat�riau qui n�cessite un appel � glMaterial().
   */
  int m_lastMaterialIndex;
  bool m_previousTriangleWasTextured;
  
  /** D�calage de l'objet dans la sc�ne. */
  Point m_offset;
  
  /** Pointeur vers la sc�ne. */
  Scene *m_scene;
  
public:
  /**
   * Constructeur par d&eacute;faut.
   * @param scene Pointeur vers la scene.
   */
  Object (Scene * const scene)
  {
    m_scene = scene;
    m_attributes = 0;
  };
  
  /** Constructeur permettant de donner une position absolue � l'objet.
   * @param scene Pointeur vers la scene.
   * @param offset D�calage de l'objet dans la sc�ne.
   */
  Object (Scene * const scene, Point& offset)
  {
    m_scene = scene;
    m_attributes = 0;
    m_offset = offset;
  };
  
  /** Destructeur par d�faut. */
  virtual ~Object ();
  
  /** Ajoute un point dans l'objet.
   * @param newVertex point � ajouter 
   */
  virtual void addVertex ( Point* const newVertex)
  {
    newVertex->x = newVertex->x + m_offset.x ;
    newVertex->y = newVertex->y + m_offset.y ;
    newVertex->z = newVertex->z + m_offset.z ;
    m_vertexArray.push_back(newVertex);
  };
  
  /** Ajoute une normale dans l'objet.
   * @param newNormal normale � ajouter 
   */
  virtual void addNormal (Vector* const newNormal)
  {
    m_normalsArray.push_back(newNormal);
  };
  
  /** Ajoute une coordonn�e de texture dans l'objet.
   * @param newTexCoord coordonn�e de texture � ajouter 
   */
  virtual void addTexCoord (Point* const newTexCoord)
  {
    m_texCoordsArray.push_back(newTexCoord);
  };
  
  /** Ajoute une facette dans l'objet.
   * @param vertexIndex1 Indices du premier point de la facette.
   * @param vertexIndex2 Indices du deuxi�me point de la facette.
   * @param vertexIndex3 Indices du troisi�me point de la facette.
   */
  virtual void addFacet (PointIndices* const vertexIndex1, PointIndices* const vertexIndex2, PointIndices* const vertexIndex3)
  {
    m_vertexIndexArray.push_back(vertexIndex1);
    m_vertexIndexArray.push_back(vertexIndex2);
    m_vertexIndexArray.push_back(vertexIndex3);
  };
  
  /** V�rifie si le mat�riau de la facette courante est diff�rent de la pr�c�dente et applique
   * le nouveau mat�riau le cas �ch�ant.
   * @param currentMaterialIndex Indice du mat�riau courant.
   * @param tex Bool�en indiquant si la facette est textur�e ou non.
   */
  void checkAndApplyMaterial(int currentMaterialIndex, bool tex);
  
  /** Lecture du nombre de points contenus dans l'objet.
   * @return Nombre de points.
   */
  virtual int getVertexArraySize () const
  {
    return m_vertexArray.size ();
  };
  
  /** Lecture du nombre de normales contenus dans l'objet.
   * @return Nombre de normales.
   */
  virtual int getNormalsArraySize () const
  {
    return m_normalsArray.size ();
  };
  
  /** Lecture du nombre de normales contenus dans l'objet.
   * @return Nombre de normales.
   */
  virtual int getTexCoordsArraySize () const
  {
    return m_texCoordsArray.size ();
  };
  
  /** Lecture du nombre de polygones contenus dans l'objet.
   * @return Nombre de polygones.
   */
  virtual int getPolygonsCount () const
  {
    return (m_vertexIndexArray.size () / 3);
  };
  
  /** Lecture d'un point sp�cifique de l'objet.
   * @param index indice du point � obtenir.
   * @return Un pointeur vers le point recherch�.
   */
  virtual Point *getPoint(int index) const
  {
    return (m_vertexArray[index]);
  };
  
  /** Donne l'englobant de l'objet.
   * @param max Retourne le coin sup�rieur de l'englobant.
   * @param min Retourne le coin inf�rieur de l'englobant.
   */
  void getBoundingBox (Point& max, Point& min);
  
  /** Met � jour les attributs de l'objet. */
  void setAttributes (int attr)
  {
    m_attributes = attr;
  };
  
  /** Fonction de dessin de l'objet.
   * @param drawCode 
   * si TEXTURED, alors l'objet n'est dessin� que s'il poss�de une texture
   * si FLAT alors l'objet n'est dessin� que s'il ne poss�de pas une texture
   * si ALL alors l'objet est dessin� inconditionnellement
   * si AMBIENT alors l'objet est dessin� avec un mat�riau blanc en composante ambiante (pour les ombres)
   * @param tex false si l'objet textur� doit �tre affich� sans sa texture
   */
  virtual void draw(char drawCode=ALL, bool tex=true);

  virtual const bool isTransparent ();
};

#endif

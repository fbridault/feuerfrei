#ifndef OBJECT_H
#define OBJECT_H

class CObject;

#include "material.hpp"
#include "source.hpp"
#include "OBJReader.hpp"

#define ALL      0
#define TEXTURED 1
#define FLAT     2
#define AMBIENT  3

#include <vector>

class CScene;

/** 
 * Classe englobant, pour un point donné, les indices du point, de la normale,
 * de la coordonnée de texture, et du matériau
*/
class CIndex
{
public:
  int v,vn,vt,vm;
  
  CIndex(int nv, int nvn, int nvt, int nvm){ v=nv; vn=nvn; vt=nvt; vm=nvm;};
};

/** 
 * Classe repr&eacute;sentant un objet g&eacute;om&eacute;trique.
 * Une sc&egrave;ne comporte une liste index&eacute;e des polygones, une liste des mat&eacute;riaux 
 * associ&eacute;s &agrave; ces polygones et ainsi qu'une liste des sources lumineuses.
 *
 * @author	Flavien Bridault
 * @version	%I%, %G%
 * @since	1.0
 */
class CObject
{
protected:
  /**<Liste des points de l'objet */
  vector < CPoint *>m_vertexArray;
  /**<Liste des coordonnées de textures de l'objet */
  vector < CPoint *>m_texCoordsArray;
  /**<Liste des normales de l'objet */
  vector < CVector *>m_normalsArray;
  /**<Liste des indices des points des facettes */
  vector < CIndex *>m_vertexIndexArray;
  
  int m_attributes;
  int m_lastMaterialIndex;
  CPoint m_offset;
  CScene *m_scene;
  
public:
  /**
   * Constructeur par d&eacute;faut.
   */
  CObject (CScene *scene)
  {
    m_scene = scene;
    m_attributes = 0;
  };
  /**
   * Constructeur permettant de donner une position absolue à l'objet
   * @param pos Position à donner à l'objet
   */
  CObject (CScene *scene, CPoint& offset)
  {
    m_scene = scene;
    m_attributes = 0;
    m_offset = offset;
  };
  /**
   * Destructeur par d&eacute;faut.
   */
  virtual ~CObject ();
  
  /** Ajoute un point dans l'objet
   * @param newVertex point à ajouter 
   */
  virtual void addVertex ( CPoint* const newVertex)
  {
    newVertex->x = newVertex->x + m_offset.x ;
    newVertex->y = newVertex->y + m_offset.y ;
    newVertex->z = newVertex->z + m_offset.z ;
    m_vertexArray.push_back(newVertex);
  };
  
  /** Ajoute une normale dans l'objet
   * @param newNormal normale à ajouter 
   */
  virtual void addNormal (CVector* const newNormal)
  {
    m_normalsArray.push_back(newNormal);
  };
  
  /** Ajoute une coordonnée de texture dans l'objet
   * @param newTexCoord coordonnée de texture à ajouter 
   */
  virtual void addTexCoord (CPoint* const newTexCoord)
  {
    m_texCoordsArray.push_back(newTexCoord);
  };
  
  /** Ajoute une facette dans l'objet
   * @param  à ajouter 
   */
  virtual void addFacet (CIndex* const vertexIndex1, CIndex* const vertexIndex2, CIndex* const vertexIndex3)
  {
    m_vertexIndexArray.push_back(vertexIndex1);
    m_vertexIndexArray.push_back(vertexIndex2);
    m_vertexIndexArray.push_back(vertexIndex3);
  };

  void checkAndApplyMaterial(int currentMaterialIndex, bool tex);
  
  /**
   * Lecture du nombre de points contenus dans la sc&egrave;ne.
   */
  virtual int getVertexArraySize () const
  {
    return m_vertexArray.size ();
  };
  
  virtual int getNormalsArraySize () const
  {
    return m_normalsArray.size ();
  };

  virtual int getPolygonsCount () const
  {
    return (m_vertexIndexArray.size () / 3);
  };
  /**
   * Lecture d'un point sp&eacute;cifique contenu dans la sc&egrave;ne.
   * @param index indice du point &agrave; obtenir.
   * @return Un pointeur vers le point recherch&eacute;.
   */
  virtual CPoint *getPoint(int index) const
  {
    return (m_vertexArray[index]);
  };
  
  void getBoundingBox (CPoint& max, CPoint& min);

  void setAttributes (int attr)
  {
    m_attributes = attr;
  };
  
  /** Fonction de dessin de l'objet
   * @param drawCode 
   * si TEXTURED, alors l'objet n'est dessiné que s'il possède une texture
   * si FLAT alors l'objet n'est dessiné que s'il ne possède pas une texture
   * si ALL alors l'objet est dessiné inconditionnellement
   * si AMBIENT alors l'objet est dessiné avec un matériau blanc en composante ambiante (pour les ombres)
   * @param tex false si l'objet texturé doit être affiché sans sa texture
   */
  virtual void draw(char drawCode=ALL, bool tex=true);
};

#endif

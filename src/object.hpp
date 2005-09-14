/* Scene.h: interface for the CScene class. */

#ifndef OBJECT_H
#define OBJECT_H

#define ALL      0
#define TEXTURED 1
#define FLAT    -1

class CObject;

#include "vector.hpp"
#include "material.hpp"
#include "source.hpp"
#include "OBJReader.hpp"

#include <vector>

using namespace std;

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
 * Classe repr&eacute;sentant une sc&egrave;ne g&eacute;om&eacute;trique.
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
  vector < CPoint *>vertexArray;
  /**<Liste des coordonnées de textures de l'objet */
  vector < CPoint *>texCoordsArray;
  /**<Liste des normales de l'objet */
  vector < CVector *>normalsArray;
  /**<Liste des indices des points des facettes */
  vector < CIndex *>vertexIndexArray;
  /**<Liste des materiaux*/
  vector < CMaterial *>materialArray;
  
  int attributes;
  int lastMaterialIndex;
  CPoint offset;
  
public:
  /**
   * Constructeur par d&eacute;faut.
   */
  CObject ()
  {
    attributes = 0;
  };
  /**
   * Constructeur permettant de donner une position absolue à l'objet
   * @param pos Position à donner à l'objet
   */
  CObject (CPoint* pos)
  {
    attributes = 0;
    offset = *pos;
  };
  /**
   * Destructeur par d&eacute;faut.
   */
  virtual ~ CObject ();
  
  /** Ajoute un point dans l'objet
   * @param newVertex point à ajouter 
   */
  virtual void addVertex ( CPoint* const newVertex)
  {
    newVertex->setX(newVertex->getX() + offset.getX() );
    newVertex->setY(newVertex->getY() + offset.getY() );
    newVertex->setZ(newVertex->getZ() + offset.getZ() );
    vertexArray.push_back(newVertex);
  };
  
  /** Ajoute une normale dans l'objet
   * @param newNormal normale à ajouter 
   */
  virtual void addNormal (CVector* const newNormal)
  {
    normalsArray.push_back(newNormal);
  };
  
  /** Ajoute une coordonnée de texture dans l'objet
   * @param newTexCoord coordonnée de texture à ajouter 
   */
  virtual void addTexCoord (CPoint* const newTexCoord)
  {
    texCoordsArray.push_back(newTexCoord);
  };
  
  /** Ajoute une facette dans l'objet
   * @param  à ajouter 
   */
  virtual void addFacet (CIndex* const vertexIndex1, CIndex* const vertexIndex2, CIndex* const vertexIndex3)
  {
    vertexIndexArray.push_back(vertexIndex1);
    vertexIndexArray.push_back(vertexIndex2);
    vertexIndexArray.push_back(vertexIndex3);
  };

  void checkAndApplyMaterial(int currentMaterialIndex, bool tex);

  virtual int addMaterial(CMaterial *newMaterial)
  {
    materialArray.push_back(newMaterial);
    return (materialArray.size() - 1);
  };
  
  /**
   * Lecture du nombre de points contenus dans la sc&egrave;ne.
   */
  virtual int getVertexArraySize () const
  {
    return vertexArray.size ();
  };
  
  virtual int getNormalsArraySize () const
  {
    return normalsArray.size ();
  };

  virtual int getPolygonsCount () const
  {
    return (vertexIndexArray.size () / 3);
  };
  /**
   * Lecture d'un point sp&eacute;cifique contenu dans la sc&egrave;ne.
   * @param index indice du point &agrave; obtenir.
   * @return Un pointeur vers le point recherch&eacute;.
   */
  virtual CPoint *getPoint(int index) const
  {
    return (vertexArray[index]);
  };
  
  void getBoundingBox (CPoint & max, CPoint & min);

  void setAttributes (int attr)
  {
    attributes = attr;
  };

  virtual CMaterial* getMaterial() const
  { return materialArray[0]; };
  
  /** Fonction de dessin de l'objet */
  virtual void draw(char drawCode, bool tex);
};

#endif

/* Scene.h: interface for the CScene class. */

#ifndef SCENE_H
#define SCENE_H

class CScene;

#include "vector.hpp"
#include "material.hpp"
#include "source.hpp"
#include "OBJReader.hpp"
#include "object.hpp"
#include "flame.hpp"
#include <vector>

class CObject;

using namespace std;

/** 
 * Classe repr&eacute;sentant une sc&egrave;ne g&eacute;om&eacute;trique.
 * Une sc&egrave;ne comporte une liste index&eacute;e des polygones, une liste des mat&eacute;riaux 
 * associ&eacute;s &agrave; ces polygones et ainsi qu'une liste des sources lumineuses.
 *
 * @author	Flavien Bridault
 * @version	%I%, %G%
 * @since	1.0
 */
class CScene
{
private:
  vector<CObject*> objectsArray;/**<Liste des objets de la scene ne projetant pas d'ombres*/
  vector<CObject*> objectsArrayWSV;/**<Liste des objets de la scene projetant des ombres*/
  vector<CSource*> lightSourcesArray;/**<Liste des sources de lumiere*/

public:
  /**
   * Constructeur par d&eacute;faut.
   */
  CScene(const char* const filename, Flame **flammes, int nb_flammes);
  /**
   * Destructeur par d&eacute;faut.
   */
  ~CScene();

  /**
   * Ajoute une surface g&eacute;om&eacute;trique &agrave; la sc&egrave;ne.
   * @param newFacet pointeur vers le polygone &agrave; ajouter.
   */
  void addObject(CObject* const newObj, bool objectWSV = false)
  { 
    if(objectWSV)
      objectsArrayWSV.push_back(newObj);
    else
      objectsArray.push_back(newObj);
  };
  /**
   * Ajoute une source lumineuse &agrave; la sc&egrave;ne.
   * @param newSource pointeur vers la source lumineuse &agrave; ajouter.
   */
  void addSource(CSource* const newSource)
  { lightSourcesArray.push_back(newSource); };
 
  /**
   * Lecture du nombre de points contenus dans la sc&egrave;ne.
   */
  int getVertexCount();
  
  /**
   * Lecture du nombre de polygones contenus dans la sc&egrave;ne.
   */
  int getPolygonsCount();
  
  /**
   * Lecture du nombre d'objets contenus dans la sc&egrave;ne.
   */
  int getObjectsCount() const
  { return objectsArray.size() + objectsArrayWSV.size(); };
  /**
   * Lecture du nombre de sources lumineuses contenus dans la sc&egrave;ne.
   */	
  int getSourcesCount() const
  { return lightSourcesArray.size(); };
  /**
   * Lecture d'un polygone sp&eacute;cifique contenu dans la sc&egrave;ne.
   * @param index indice du polygone &agrave; obtenir.
   * @return Un pointeur vers le polygone recherch&eacute;.
   */
  CObject* getObject(const int& index) const
  { return (objectsArray[index]); };
  /**
   * Lecture d'une sources lumineuse sp&eacute;cifique contenue dans la sc&egrave;ne.
   * @param index indice de la source &agrave; obtenir.
   * @return Un pointeur vers la source recherch&eacute;.
   */
  CSource* getSource(const int& index) const
  { return (lightSourcesArray[index]); };

  /** Dessin de la scène pour les objets texturés */
  void draw_sceneTEX(void) const
  {
    glCallList(SCENE_OBJECTS_TEX);
    glCallList(SCENE_OBJECTS_WSV_TEX);
  };
  
  /** Dessin de la scène pour les objets non texturés */
  void draw_sceneWTEX(void) const
  {
    glCallList(SCENE_OBJECTS_WTEX);
    glCallList(SCENE_OBJECTS_WSV_WTEX);
  };
  
  /** Dessin de la scène */
  void draw_scene (void) const
  {
    glCallList (SCENE_OBJECTS);
    glCallList (SCENE_OBJECTS_WSV);
  };
  
};//Scene

#endif

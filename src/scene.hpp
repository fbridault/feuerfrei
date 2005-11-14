#ifndef SCENE_H
#define SCENE_H

class CScene;

#include "material.hpp"
#include "source.hpp"
#include "object.hpp"
#include "OBJReader.hpp"
#include <vector>
#include <list>

class COBJReader;
class CObject;

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
  vector<CObject*> m_objectsArray;/**<Liste des objets de la scene ne projetant pas d'ombres*/
  vector<CObject*> m_objectsArrayWSV;/**<Liste des objets de la scene projetant des ombres*/
  vector<CSource*> m_lightSourcesArray;/**<Liste des sources de lumiere*/
  vector<CMaterial*> m_materialArray;/**< Liste des mat&eacute;riaux.*/
public:
  /**
   * Constructeur par d&eacute;faut.
   */
  CScene(const char* const filename);
  /**
   * Destructeur par d&eacute;faut.
   */
  ~CScene();

  /** Crée les display lists - A n'appeler qu'une fois que tous les objets
   * ont été ajouté à la scène, soit via le constructeur, soit via la méthode
   * loadObject() qui permet d'ajouter des objets individuels
   */
  void createDisplayLists(void);

  /**
   * Ajoute un objet géométrique &agrave; la sc&egrave;ne.
   * @param newObj pointeur vers l'objet &agrave; ajouter.
   * @param object WSV true si l'objet projette des ombres, false sinon
   */
  void addObject(CObject* const newObj, bool objectWSV = false)
  { 
    if(objectWSV)
      m_objectsArrayWSV.push_back(newObj);
    else
      m_objectsArray.push_back(newObj);
  };
  
  /**
   * Charge un objet contenu dans un fichier OBJ et l'ajoute &agrave; la sc&egrave;ne.
   * @param newObj pointeur vers le polygone &agrave; ajouter.
   * @param detached si true, alors l'objet ne sera pas contenu dans la liste des objets de la scène
   * (pour le moment, cela n'est utile que pour dessiner la mèche d'une flamme) mais son matériau
   * le sera
   */
  void loadObject(const char *filename, CObject* const newObj, bool detached = false);

  /** Ajoute un matériau */
  void addMaterial(CMaterial *material)
  { m_materialArray.push_back(material); };

  /**
   * Affecte un matériau à un objet
   */
  int getMaterialIndexByName(const char *name);
  
  /**
   * Ajoute une source lumineuse &agrave; la sc&egrave;ne.
   * @param newSource pointeur vers la source lumineuse &agrave; ajouter.
   */
  void addSource(CSource* const newSource)
  { m_lightSourcesArray.push_back(newSource); };
 
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
  { return m_objectsArray.size() + m_objectsArrayWSV.size(); };
  /**
   * Lecture du nombre de sources lumineuses contenus dans la sc&egrave;ne.
   */	
  int getSourcesCount() const
  { return m_lightSourcesArray.size(); };
  
  /**
   * Lecture d'un polygone sp&eacute;cifique contenu dans la sc&egrave;ne.
   * @param index indice du polygone &agrave; obtenir.
   * @return Un pointeur vers le polygone recherch&eacute;.
   */
  CObject* getObject(const int& index) const
  { return (m_objectsArray[index]); };

  /**
   * Lecture d'une sources lumineuse sp&eacute;cifique contenue dans la sc&egrave;ne.
   * @param index indice de la source &agrave; obtenir.
   * @return Un pointeur vers la source recherch&eacute;e.
   */
  CSource* getSource(const int& index) const
  { return (m_lightSourcesArray[index]); };

  /**
   * Lecture d'un matériau sp&eacute;cifique contenue dans la sc&egrave;ne.
   * @param index indice du matériau &agrave; obtenir.
   * @return Un pointeur vers le matériau recherch&eacute;.
   */
  CMaterial* getMaterial(const int& index) const
  { return (m_materialArray[index]); };

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

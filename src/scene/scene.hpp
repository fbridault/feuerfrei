#ifndef SCENE_H
#define SCENE_H

class Scene;

class FireSource;

#include "../flames/fire.hpp"
#include "material.hpp"
#include "source.hpp"
#include "object.hpp"
#include <vector>
#include <list>

class COBJReader;
class Object;

#define NB_DISPLAY_LISTS 8

/** 
 * Classe repr&eacute;sentant une sc&egrave;ne g&eacute;om&eacute;trique.
 * Une sc&egrave;ne comporte une liste index&eacute;e des polygones, une liste des mat&eacute;riaux 
 * associ&eacute;s &agrave; ces polygones et ainsi qu'une liste des sources lumineuses.
 *
 * @author	Flavien Bridault
 * @version	%I%, %G%
 * @since	1.0
 */
class Scene
{
private:
  vector<Object*> m_objectsArray;/**<Liste des objets de la scene ne projetant pas d'ombres*/
  vector<Object*> m_objectsArrayWSV;/**<Liste des objets de la scene projetant des ombres*/
  vector<Source*> m_lightSourcesArray;/**<Liste des sources de lumiere*/
  vector<Material*> m_materialArray;/**< Liste des mat&eacute;riaux.*/

  /** Display lists de la scène
   * [0] Tous les objets de la scène ne projetant pas d'ombres
   * [1] Objets texturés ne projetant pas d'ombres
   * [2] Objets sans textures ne projetant pas d'ombres
   * [3] Objets texturés qui projettent des ombres
   * [4] Objets sans textures qui projettent des ombres
   * [5] Tous les objets qui projettent des ombres
   * [6] Tous les objets qui projettent des ombres, sans les textures éventuelles
   * [7] Tous les objets sans les textures éventuelles 
   */
  GLuint m_displayLists[8];
  int m_nbFlames;
  FireSource **m_flames;
  
public:
  /**
   * Constructeur par d&eacute;faut.
   */
  Scene(const char* const fileName, FireSource **flames, int nbFlames);
  /**
   * Destructeur par d&eacute;faut.
   */
  ~Scene();
  
  /** Lit un fichier OBJ pass&eacute; en param&egrave;tres et l'importe dans la sc&egrave;ne.
   * Si object est non nul, on ne charge qu'un seul objet sinon on charge tous les objets.
   * Si false est à nul, l'objet ne sera pas compris dans la scène.
   *
   * @param fileName nom du fichier OBJ &agrave; importer
   * @param object optionnel, objet dans lequel importer le fichier
   * @param detached optionnel, permet de spécifier si l'objet object doit appartenir à la scène ou non
   */
  void importOBJ(const char* fileName, Object* object=NULL, bool detached=false, const char* objName=NULL);
  
  /** Lit un fichier OBJ pass&eacute; en param&egrave;tres et importe les objets correspondant
   * au préfixe prefix. La liste des objets est retournée dans objectsList, ceux-ci ne sont en effet
   * pas stockés dans la liste des objets de la scène.
   *
   * @param fileName nom du fichier OBJ
   * @param objectsList liste des noms des objets retournés
   * @param prefix préfixe servant de filtre pour les noms des objets
   */  
  void getObjectsNameFromOBJ(const char* fileName, vector<string> &objectsList, const char* prefix);
  
  /** Lit un fichier OBJ pass&eacute; en param&egrave;tre et cherche le nom du fichier MTL.
   *
   * @param fileName nom du fichier OBJ à lire
   * @param mtlName nom du fichier récupéré, la chaîne doit être allouée au préalable
   *
   * @return true si trouve un fichier MTL
   */
  bool getMTLFileNameFromOBJ(const char* fileName, char* mtlName);
  
   /** Lit un fichier MTL pass&eacute; en param&egrave;tres et importe les matériaux qu'il contient 
    * dans la sc&egrave;ne.
   *
   * @param fileName nom du fichier OBJ &agrave; importer
   */
  void importMTL(const char* fileName);

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
  void addObject(Object* const newObj, bool objectWSV = false)
  { 
    if(objectWSV)
      m_objectsArrayWSV.push_back(newObj);
    else
      m_objectsArray.push_back(newObj);
  };
  
  /** Ajoute un matériau */
  void addMaterial(Material *material)
  { m_materialArray.push_back(material); };

  /**
   * Affecte un matériau à un objet
   */
  int getMaterialIndexByName(const char *name);
  
  /**
   * Ajoute une source lumineuse &agrave; la sc&egrave;ne.
   * @param newSource pointeur vers la source lumineuse &agrave; ajouter.
   */
  void addSource(Source* const newSource)
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
  Object* getObject(const int& index) const
  { return (m_objectsArray[index]); };

  /**
   * Lecture d'une sources lumineuse sp&eacute;cifique contenue dans la sc&egrave;ne.
   * @param index indice de la source &agrave; obtenir.
   * @return Un pointeur vers la source recherch&eacute;e.
   */
  Source* getSource(const int& index) const
  { return (m_lightSourcesArray[index]); };

  /**
   * Lecture d'un matériau sp&eacute;cifique contenue dans la sc&egrave;ne.
   * @param index indice du matériau &agrave; obtenir.
   * @return Un pointeur vers le matériau recherch&eacute;.
   */
  Material* getMaterial(const int& index) const
  { return (m_materialArray[index]); };

  /** Dessin de la scène pour les objets texturés */
  void draw_sceneTEX(void) const
  {
    glCallList(m_displayLists[1]);
    glCallList(m_displayLists[3]);
  };
  
  /** Dessin de la scène pour les objets non texturés */
  void draw_sceneWTEX() const
  {
    glCallList(m_displayLists[2]);
    glCallList(m_displayLists[4]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary();
  };
    /** Dessin de la scène pour les objets non texturés */
  void draw_sceneWTEX(CgBasicVertexShader& shader) const
  {
    glCallList(m_displayLists[2]);
    glCallList(m_displayLists[4]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary(shader);
    
  };

  /** Dessin de tous les objets de la scène sans les textures */
  void draw_sceneWT(void) const
  {
    glCallList(m_displayLists[6]);
    glCallList(m_displayLists[7]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary();
  };
  
  /** Dessin de tous les objets de la scène */
  void draw_scene (void) const
  {
    glCallList (m_displayLists[0]);
    glCallList (m_displayLists[5]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary();
  };
  /** Dessin de tous les objets de la scène */
  void draw_scene (CgBasicVertexShader& shader) const
  {
    glCallList (m_displayLists[0]);
    glCallList (m_displayLists[5]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary(shader);
  };

    /** Dessin de tous les objets qui projettent des ombres */
  void draw_sceneWSV (void) const
  {
    glCallList (m_displayLists[6]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary();
  };
};//Scene

#endif

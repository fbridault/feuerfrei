#ifndef SCENE_H
#define SCENE_H

class Scene;

class FireSource;

#include "../flames/abstractFires.hpp"
#include "material.hpp"
#include "source.hpp"
#include "object.hpp"
#include <vector>
#include <list>

class Material;
class Object;

#define NB_DISPLAY_LISTS 8

/** 
 * Classe repr&eacute;sentant une sc&egrave;ne g&eacute;om&eacute;trique.
 * Une sc&egrave;ne comporte deux listes indexées des objets, une pour les objets projetant
 * des ombres et une pour les autres, une liste des matériaux associés à ces polygones et
 * une liste des sources lumineuses (inutilisée pour l'instant).<br>
 * Cette classe encapsule également les fonctions d'import de fichiers OBJ.
 * @todo Certaines fonctions d'import semble être un peu décoréllées de la classe.
 * Je demande à voir si il ne faudrait pas les ressortir.
 *
 * @author	Flavien Bridault
 */
class Scene
{
public:
  /**
   * Constructeur par défaut.
   * @param fileName Nom du fichier OBJ.
   * @param flames Tableau contenant les flammes.
   * @param nbFlames Nombre de flammes dans le tableau.
   */
  Scene(const char* const fileName, FireSource **flames, int nbFlames);

  /** Destructeur par d&eacute;faut. */
  ~Scene();
  
  /** Lit un fichier OBJ pass&eacute; en param&egrave;tres et l'importe dans la sc&egrave;ne.
   * Si object est non nul, un seul objet est chargé, le premier figurant dans le fichier. Si objName est
   * non nul, on cherchera à importer l'objet portant ce nom. Sinon si object est null tous les objets 
   * contenu dans le fichier sont importés.
   * Si detached est true, l'objet ne sera pas ajouté dans la liste des objets de la scène. Ceci est par
   * exemple utilisé pour les luminaires des flammes qui sont stockés dans FireSource et non dans Scene.
   *
   * @param fileName nom du fichier OBJ &agrave; importer.
   * @param object Optionnel, objet dans lequel importer le fichier.
   * @param detached Optionnel, permet de spécifier si l'objet object doit appartenir à la scène ou non.
   * @param objName Optionel, permet de spécifier le nom de l'objet à charger.
   *
   * @return false si l'import a échoué.
   */
  bool importOBJ(const char* fileName, Object* object=NULL, bool detached=false, const char* objName=NULL);
  
  /** Lit un fichier OBJ pass&eacute; en param&egrave;tres et importe les objets correspondant
   * au préfixe prefix. La liste des objets est retournée dans objectsList, ceux-ci ne sont en effet
   * pas stockés dans la liste des objets de la scène.
   *
   * @param fileName Nom du fichier OBJ.
   * @param objectsList Liste des noms des objets retournés.
   * @param prefix Préfixe servant de filtre pour les noms des objets.
   */  
  void getObjectsNameFromOBJ(const char* fileName, list<string> &objectsList, const char* prefix);
  
  /** Lit un fichier OBJ pass&eacute; en param&egrave;tre et cherche le nom du fichier MTL.
   *
   * @param fileName Nom du fichier OBJ à lire.
   * @param mtlName Nom du fichier récupéré, la chaîne doit être allouée au préalable.
   *
   * @return true si trouve un fichier MTL
   */
  bool getMTLFileNameFromOBJ(const char* fileName, char* mtlName);
  
   /** Lit un fichier MTL pass&eacute; en param&egrave;tres et importe les matériaux qu'il contient 
    * dans la scène.
   *
   * @param fileName nom du fichier OBJ &agrave; importer.
   */
  void importMTL(const char* fileName);

  /** Crée les display lists - A n'appeler qu'une fois que tous les objets
   * ont été ajouté à la scène, soit via le constructeur, soit via la méthode
   * loadObject() qui permet d'ajouter des objets individuels.
   */
  void createDisplayLists(void);

  void sortTransparentObjects();
  
  /** Ajoute un objet dans la scène.
   * @param newObj Pointeur vers l'objet &agrave; ajouter.
   * @param objectWSV True si l'objet projette des ombres, false sinon.
   */
  void addObject(Object* const newObj, bool objectWSV = false)
  { 
    if(objectWSV)
      m_objectsArrayWSV.push_back(newObj);
    else
      m_objectsArray.push_back(newObj);
  };
  
  /** Ajoute un matériau dans la scène.
   * @param material Pointeur sur le nouveau matériau à référencer.
   */
  void addMaterial(Material *material)
  { m_materialArray.push_back(material); };
  
  /** Ajoute une texture dans la scène.
   * @param texture Pointeur sur la nouvelle texture à référencer.
   * @return Indice OpenGL de la texture.
   */
  GLuint addTexture(Texture *texture)
  { m_texturesArray.push_back(texture); return m_texturesArray.size()-1; };

  /** Donne l'indice d'un matériau dans la liste des matériaux de la scène.
   * @param name Nom du matériau.
   * @return Index du matériau.
   */
  int getMaterialIndexByName(const char *name);
  
  /** Donne l'indice d'une texture dans la liste des textures de la scène.
   * @param name Nom du fichier de la texture.
   * @return Index de la texture, -1 si elle n'existe pas
   */
  int searchTextureIndexByName(const char *name);
  
  /** Ajoute une source lumineuse à la scène.
   * @param newSource pointeur vers la source lumineuse à ajouter.
   */
  void addSource(Source* const newSource)
  { m_lightSourcesArray.push_back(newSource); };
 
  /** Lecture du nombre de points contenus dans la scène.
   * @return Nombre de points.
   */
  int getVertexCount();
  
  /** Lecture du nombre de polygones contenus dans la scène.
   * @return Nombre de polygones.
   */
  int getPolygonsCount();
  
  /** Lecture du nombre d'objets contenus dans la scène.
   * @return Nombre d'objets.
   */
  int getObjectsCount() const
  { return m_objectsArray.size() + m_objectsArrayWSV.size(); };
  
  /** Lecture du nombre de sources lumineuses contenus dans la scène.
   * @return Nombre de sources lumineuses.
   */	
  int getSourcesCount() const
  { return m_lightSourcesArray.size(); };
  
  /** Lecture d'un polygone spécifique contenu dans la scène.
   * @param index Indice du polygone à obtenir.
   * @return Un pointeur vers le polygone recherché.
   */
  Object* getObject(const int index) const
  { return (m_objectsArray[index]); };
  
  /** Lecture d'une source lumineuse spécifique contenue dans la scène.
   * @param index Indice de la source à obtenir.
   * @return Un pointeur vers la source recherchée.
   */
  Source* getSource(const int index) const
  { return (m_lightSourcesArray[index]); };
  
  /** Lecture d'un matériau spécifique contenue dans la scène.
   * @param index Indice du matériau à obtenir.
   * @return Un pointeur vers le matériau recherché.
   */
  Material* getMaterial(const int index) const
  { return (m_materialArray[index]); };
  
  /** Lecture d'une texture spécifique contenue dans la scène.
   * @param index Indice de la texture à obtenir.
   * @return Un pointeur vers la texture recherchée.
   */
  Texture* getTexture(const int index) const
  { return (m_texturesArray[index]); };
  
  /** Dessin de la scène pour les objets texturés */
  void drawSceneTEX(void) const
  {
    glCallList(m_displayLists[1]);
    glCallList(m_displayLists[3]);
  };
  
  /** Dessin de la scène pour les objets non texturés */
  void drawSceneWTEX() const
  {
    glCallList(m_displayLists[2]);
    glCallList(m_displayLists[4]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary();
  };
  
  /** Dessin de la scène pour les objets non texturés.
   * @param shader Référence vers un vertex shader. Utilisé par exemple pour le dessin de la
   * scène éclairée avec des solides photométriques.
   */
  void drawSceneWTEX(CgBasicVertexShader& shader) const
  {
    glCallList(m_displayLists[2]);
    glCallList(m_displayLists[4]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary(shader);
  };
  
  /** Dessin de tous les objets de la scène sans les textures */
  void drawSceneWT(void) const
  {
    glCallList(m_displayLists[6]);
    glCallList(m_displayLists[7]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary();
  };
  
  /** Dessin de tous les objets de la scène */
  void drawScene (void) const
  {
    glCallList (m_displayLists[0]);
    glCallList (m_displayLists[5]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary();
  };
  /** Dessin de tous les objets de la scène.
   * @param shader Référence vers un vertex shader. Utilisé par exemple pour le dessin de la
   * scène éclairée avec des solides photométriques.
   */
  void drawScene (CgBasicVertexShader& shader) const
  {
    glCallList (m_displayLists[0]);
    glCallList (m_displayLists[5]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary(shader);
  };

    /** Dessin de tous les objets qui projettent des ombres */
  void drawSceneWSV (void) const
  {
    glCallList (m_displayLists[6]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary();
  };
  
  /** Dessin de tous les objets qui projettent des ombres */
  void drawSceneWSV (CgBasicVertexShader& shader) const
  {
    glCallList (m_displayLists[6]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary(shader);
  };
  
private:
  vector<Object*> m_objectsArray; /** Liste des objets de la scene ne projetant pas d'ombres. */
  vector<Object*> m_objectsArrayWSV; /** Liste des objets de la scene projetant des ombres. */
  vector<Source*> m_lightSourcesArray; /** Liste des sources de lumière. */
  vector<Material*> m_materialArray; /** Liste des matériaux.*/
  vector<Texture*> m_texturesArray; /** Liste des textures.*/
  
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
  
  /** Tableaux de flammes. Il s'agit d'un pointeur sur le tableau de flamme créé dans la classe GlFlameCanvas. 
   * Celles-ci sont nécessaires pour dessiner les luminaires lors du dessin de la scène.
   */
  FireSource **m_flames;
  /** Nombre de flammes. */
  int m_nbFlames;
  
  /** Chaîne de caractère contenant le chemin courant. Elle est utilisée dans les fonctions d'import pour
   * parcourir les différents répertoires (scenes, textures, ...).
   */
  char m_currentDir[255];
  
  /** Fonctions retournant le chemin absolu d'un fichier par rapport au chemin courant.
   * @param fileName Nom du fichier à traiter.
   */
  void getSceneAbsolutePath(const char* const fileName);
  
};//Scene

#endif

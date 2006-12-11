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
 * Une sc&egrave;ne comporte deux listes index�es des objets, une pour les objets projetant
 * des ombres et une pour les autres, une liste des mat�riaux associ�s � ces polygones et
 * une liste des sources lumineuses (inutilis�e pour l'instant).<br>
 * Cette classe encapsule �galement les fonctions d'import de fichiers OBJ.
 * @todo Certaines fonctions d'import semble �tre un peu d�cor�ll�es de la classe.
 * Je demande � voir si il ne faudrait pas les ressortir.
 *
 * @author	Flavien Bridault
 */
class Scene
{
public:
  /**
   * Constructeur par d�faut.
   * @param fileName Nom du fichier OBJ.
   * @param flames Tableau contenant les flammes.
   * @param nbFlames Nombre de flammes dans le tableau.
   */
  Scene(const char* const fileName, FireSource **flames, int nbFlames);

  /** Destructeur par d&eacute;faut. */
  ~Scene();
  
  /** Lit un fichier OBJ pass&eacute; en param&egrave;tres et l'importe dans la sc&egrave;ne.
   * Si object est non nul, un seul objet est charg�, le premier figurant dans le fichier. Si objName est
   * non nul, on cherchera � importer l'objet portant ce nom. Sinon si object est null tous les objets 
   * contenu dans le fichier sont import�s.
   * Si detached est true, l'objet ne sera pas ajout� dans la liste des objets de la sc�ne. Ceci est par
   * exemple utilis� pour les luminaires des flammes qui sont stock�s dans FireSource et non dans Scene.
   *
   * @param fileName nom du fichier OBJ &agrave; importer.
   * @param object Optionnel, objet dans lequel importer le fichier.
   * @param detached Optionnel, permet de sp�cifier si l'objet object doit appartenir � la sc�ne ou non.
   * @param objName Optionel, permet de sp�cifier le nom de l'objet � charger.
   *
   * @return false si l'import a �chou�.
   */
  bool importOBJ(const char* fileName, Object* object=NULL, bool detached=false, const char* objName=NULL);
  
  /** Lit un fichier OBJ pass&eacute; en param&egrave;tres et importe les objets correspondant
   * au pr�fixe prefix. La liste des objets est retourn�e dans objectsList, ceux-ci ne sont en effet
   * pas stock�s dans la liste des objets de la sc�ne.
   *
   * @param fileName Nom du fichier OBJ.
   * @param objectsList Liste des noms des objets retourn�s.
   * @param prefix Pr�fixe servant de filtre pour les noms des objets.
   */  
  void getObjectsNameFromOBJ(const char* fileName, list<string> &objectsList, const char* prefix);
  
  /** Lit un fichier OBJ pass&eacute; en param&egrave;tre et cherche le nom du fichier MTL.
   *
   * @param fileName Nom du fichier OBJ � lire.
   * @param mtlName Nom du fichier r�cup�r�, la cha�ne doit �tre allou�e au pr�alable.
   *
   * @return true si trouve un fichier MTL
   */
  bool getMTLFileNameFromOBJ(const char* fileName, char* mtlName);
  
   /** Lit un fichier MTL pass&eacute; en param&egrave;tres et importe les mat�riaux qu'il contient 
    * dans la sc�ne.
   *
   * @param fileName nom du fichier OBJ &agrave; importer.
   */
  void importMTL(const char* fileName);

  /** Cr�e les display lists - A n'appeler qu'une fois que tous les objets
   * ont �t� ajout� � la sc�ne, soit via le constructeur, soit via la m�thode
   * loadObject() qui permet d'ajouter des objets individuels.
   */
  void createDisplayLists(void);

  void sortTransparentObjects();
  
  /** Ajoute un objet dans la sc�ne.
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
  
  /** Ajoute un mat�riau dans la sc�ne.
   * @param material Pointeur sur le nouveau mat�riau � r�f�rencer.
   */
  void addMaterial(Material *material)
  { m_materialArray.push_back(material); };
  
  /** Ajoute une texture dans la sc�ne.
   * @param texture Pointeur sur la nouvelle texture � r�f�rencer.
   * @return Indice OpenGL de la texture.
   */
  GLuint addTexture(Texture *texture)
  { m_texturesArray.push_back(texture); return m_texturesArray.size()-1; };

  /** Donne l'indice d'un mat�riau dans la liste des mat�riaux de la sc�ne.
   * @param name Nom du mat�riau.
   * @return Index du mat�riau.
   */
  int getMaterialIndexByName(const char *name);
  
  /** Donne l'indice d'une texture dans la liste des textures de la sc�ne.
   * @param name Nom du fichier de la texture.
   * @return Index de la texture, -1 si elle n'existe pas
   */
  int searchTextureIndexByName(const char *name);
  
  /** Ajoute une source lumineuse � la sc�ne.
   * @param newSource pointeur vers la source lumineuse � ajouter.
   */
  void addSource(Source* const newSource)
  { m_lightSourcesArray.push_back(newSource); };
 
  /** Lecture du nombre de points contenus dans la sc�ne.
   * @return Nombre de points.
   */
  int getVertexCount();
  
  /** Lecture du nombre de polygones contenus dans la sc�ne.
   * @return Nombre de polygones.
   */
  int getPolygonsCount();
  
  /** Lecture du nombre d'objets contenus dans la sc�ne.
   * @return Nombre d'objets.
   */
  int getObjectsCount() const
  { return m_objectsArray.size() + m_objectsArrayWSV.size(); };
  
  /** Lecture du nombre de sources lumineuses contenus dans la sc�ne.
   * @return Nombre de sources lumineuses.
   */	
  int getSourcesCount() const
  { return m_lightSourcesArray.size(); };
  
  /** Lecture d'un polygone sp�cifique contenu dans la sc�ne.
   * @param index Indice du polygone � obtenir.
   * @return Un pointeur vers le polygone recherch�.
   */
  Object* getObject(const int index) const
  { return (m_objectsArray[index]); };
  
  /** Lecture d'une source lumineuse sp�cifique contenue dans la sc�ne.
   * @param index Indice de la source � obtenir.
   * @return Un pointeur vers la source recherch�e.
   */
  Source* getSource(const int index) const
  { return (m_lightSourcesArray[index]); };
  
  /** Lecture d'un mat�riau sp�cifique contenue dans la sc�ne.
   * @param index Indice du mat�riau � obtenir.
   * @return Un pointeur vers le mat�riau recherch�.
   */
  Material* getMaterial(const int index) const
  { return (m_materialArray[index]); };
  
  /** Lecture d'une texture sp�cifique contenue dans la sc�ne.
   * @param index Indice de la texture � obtenir.
   * @return Un pointeur vers la texture recherch�e.
   */
  Texture* getTexture(const int index) const
  { return (m_texturesArray[index]); };
  
  /** Dessin de la sc�ne pour les objets textur�s */
  void drawSceneTEX(void) const
  {
    glCallList(m_displayLists[1]);
    glCallList(m_displayLists[3]);
  };
  
  /** Dessin de la sc�ne pour les objets non textur�s */
  void drawSceneWTEX() const
  {
    glCallList(m_displayLists[2]);
    glCallList(m_displayLists[4]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary();
  };
  
  /** Dessin de la sc�ne pour les objets non textur�s.
   * @param shader R�f�rence vers un vertex shader. Utilis� par exemple pour le dessin de la
   * sc�ne �clair�e avec des solides photom�triques.
   */
  void drawSceneWTEX(CgBasicVertexShader& shader) const
  {
    glCallList(m_displayLists[2]);
    glCallList(m_displayLists[4]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary(shader);
  };
  
  /** Dessin de tous les objets de la sc�ne sans les textures */
  void drawSceneWT(void) const
  {
    glCallList(m_displayLists[6]);
    glCallList(m_displayLists[7]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary();
  };
  
  /** Dessin de tous les objets de la sc�ne */
  void drawScene (void) const
  {
    glCallList (m_displayLists[0]);
    glCallList (m_displayLists[5]);
    
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->drawLuminary();
  };
  /** Dessin de tous les objets de la sc�ne.
   * @param shader R�f�rence vers un vertex shader. Utilis� par exemple pour le dessin de la
   * sc�ne �clair�e avec des solides photom�triques.
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
  vector<Source*> m_lightSourcesArray; /** Liste des sources de lumi�re. */
  vector<Material*> m_materialArray; /** Liste des mat�riaux.*/
  vector<Texture*> m_texturesArray; /** Liste des textures.*/
  
  /** Display lists de la sc�ne
   * [0] Tous les objets de la sc�ne ne projetant pas d'ombres
   * [1] Objets textur�s ne projetant pas d'ombres
   * [2] Objets sans textures ne projetant pas d'ombres
   * [3] Objets textur�s qui projettent des ombres
   * [4] Objets sans textures qui projettent des ombres
   * [5] Tous les objets qui projettent des ombres
   * [6] Tous les objets qui projettent des ombres, sans les textures �ventuelles
   * [7] Tous les objets sans les textures �ventuelles 
   */
  GLuint m_displayLists[8];
  
  /** Tableaux de flammes. Il s'agit d'un pointeur sur le tableau de flamme cr�� dans la classe GlFlameCanvas. 
   * Celles-ci sont n�cessaires pour dessiner les luminaires lors du dessin de la sc�ne.
   */
  FireSource **m_flames;
  /** Nombre de flammes. */
  int m_nbFlames;
  
  /** Cha�ne de caract�re contenant le chemin courant. Elle est utilis�e dans les fonctions d'import pour
   * parcourir les diff�rents r�pertoires (scenes, textures, ...).
   */
  char m_currentDir[255];
  
  /** Fonctions retournant le chemin absolu d'un fichier par rapport au chemin courant.
   * @param fileName Nom du fichier � traiter.
   */
  void getSceneAbsolutePath(const char* const fileName);
  
};//Scene

#endif

#ifndef SCENE_H
#define SCENE_H

class CScene;

class CMaterial;

#include "CObject.hpp"
#include "Light.hpp"
#include "CCamera.hpp"
#include "Texture.hpp"

#include <vector>
#include <list>

#include "../Utility/ISingleton.hpp"

/**
 * Classe repr&eacute;sentant une sc&egrave;ne g&eacute;om&eacute;trique.
 * Une sc&egrave;ne comporte deux listes indexées des objets, une pour les objets projetant
 * des ombres et une pour les autres, une liste des matériaux associés à ces polygones et
 * une liste des sources lumineuses (inutilisée pour l'instant).<br>
 * Cette classe encapsule également les fonctions d'import de fichiers OBJ.
 *
 * @author	Flavien Bridault
 */
class CScene : public ISingleton<CScene>
{
  friend class ISingleton<CScene>;
private:
  /**
   * Constructeur par défaut.
   */
  CScene() {};
  /** Destructeur par d&eacute;faut. */
  ~CScene();

public:
  /** Initialisation de la scène
   * @param fileName Nom du fichier OBJ.
   */
  void init(const string& fileName);

  /** Crée les VBOs - A n'appeler qu'une fois que tous les objets
   * ont été ajouté à la scène, soit via le constructeur, soit via la méthode
   * loadCObject() qui permet d'ajouter des objets individuels.
   */
  void postInit(uint vplsDensity, uint nbPhotons);

  /** Crée les display lists - A n'appeler qu'une fois que tous les objets
   * ont été ajouté à la scène, soit via le constructeur, soit via la méthode
   * loadCObject() qui permet d'ajouter des objets individuels.
   */
  void createDisplayLists(void);

  void sortTransparentObjects();

  /** Ajoute un objet dans la scène.
   * @param newObj Pointeur vers l'objet &agrave; ajouter.
   * @param objectWSV True si l'objet projette des ombres, false sinon.
   */
//  void addCObject(CObject* const newObj)
//  {
//    m_objectsArray.push_back(newObj);
//  };

  /** Ajoute un matériau dans la scène.
   * @param material Pointeur sur le nouveau matériau à référencer.
   */
  void addMaterial(CMaterial *material)
  {
    m_materialArray.push_back(material);
  };

  /** Ajoute une texture dans la scène.
   * @param texture Pointeur sur la nouvelle texture à référencer.
   * @return Indice de la texture.
   */
  GLuint addTexture(CBitmapTexture * const texture)
  {
    m_texturesArray.push_back(texture);
    return m_texturesArray.size()-1;
  };

  /** Ajoute une source lumineuse à la scène.
   * @param newSource pointeur vers la source lumineuse à ajouter.
   */
  void addSource(ILight* const newSource)
  {
    m_lightSourcesArray.push_back(newSource);
  };

  /** Lecture du nombre de points contenus dans la scène.
   * @return Nombre de points.
   */
  uint getVertexCount();

  /** Lecture du nombre de polygones contenus dans la scène.
   * @return Nombre de polygones.
   */
  uint getPolygonsCount();

  /** Lecture du nombre d'objets contenus dans la scène.
   * @return Nombre d'objets.
   */
  uint getCObjectsCount() const
  {
    return m_objectsArray.size();
  };

  /** Lecture du nombre de sources lumineuses contenus dans la scène.
   * @return Nombre de sources lumineuses.
   */
  uint getSourcesCount() const
  {
    return m_lightSourcesArray.size();
  };

  float getArea()
  {
    float area=0.0f;
    for (vector < CObject * >::iterator objectsArrayIterator = m_objectsArray.begin ();
         objectsArrayIterator != m_objectsArray.end ();
         objectsArrayIterator++)
      area += (*objectsArrayIterator)->getArea();
    return area;
  }

  /** Lecture d'un polygone spécifique contenu dans la scène.
   * @param index Indice du polygone à obtenir.
   * @return Un pointeur vers le polygone recherché.
   */
  CObject* getCObject(const int index) const
  {
    return (m_objectsArray[index]);
  };

  /** Lecture d'une source lumineuse spécifique contenue dans la scène.
   * @param index Indice de la source à obtenir.
   * @return Un pointeur vers la source recherchée.
   */
  ILight* getSource(const int index) const
  {
    return (m_lightSourcesArray[index]);
  };

  /** Lecture d'un matériau spécifique contenue dans la scène.
   * @param index Indice du matériau à obtenir.
   * @return Un pointeur vers le matériau recherché.
   */
  CMaterial* getMaterial(const int index) const
  {
    return (m_materialArray[index]);
  };

  /** Lecture d'une texture spécifique contenue dans la scène.
   * @param index Indice de la texture à obtenir.
   * @return Un pointeur vers la texture recherchée.
   */
  CBitmapTexture* getTexture(const int index) const
  {
    return (m_texturesArray[index]);
  };

  /** Donne l'indice d'un matériau dans la liste des matériaux de la scène.
   * @param name Nom du matériau.
   * @return Index du matériau.
   */
  int getMaterialIndexByName(const string& name);

  /** Donne l'indice d'une texture dans la liste des textures de la scène.
   * @param name Nom du fichier de la texture.
   * @return Index de la texture, -1 si elle n'existe pas
   */
  int searchTextureIndexByName(const string& name);

  /** Change l'affichage des sphères englobantes. */
  void setBoundingSphereMode(bool mode) {
    m_boundingSpheresMode = mode;
  };

  void computeVisibility(const CCamera &view);

  /** Dessin des objets texturés */
  void drawSceneTEX(void) const
  {
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    for (vector<CObject*>::const_iterator objectsArrayIterator = m_objectsArray.begin();
         objectsArrayIterator != m_objectsArray.end();
         objectsArrayIterator++)
      (*objectsArrayIterator)->draw(TEXTURED,true, m_boundingSpheresMode);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer(GL_ARRAY_BUFFER, 0 );
  };

  /** Dessin des objets non texturés */
  void drawSceneWTEX() const
  {
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    for (vector<CObject*>::const_iterator objectsArrayIterator = m_objectsArray.begin();
         objectsArrayIterator != m_objectsArray.end();
         objectsArrayIterator++)
      (*objectsArrayIterator)->draw(FLAT,false, m_boundingSpheresMode);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer(GL_ARRAY_BUFFER, 0 );
  };

  /** Dessin de tous les objets de la scène en enlevant les textures si nécessaire */
  void drawSceneWT() const
  {
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    for (vector < CObject * >::const_iterator objectsArrayIterator = m_objectsArray.begin ();
         objectsArrayIterator != m_objectsArray.end ();
         objectsArrayIterator++)
      (*objectsArrayIterator)->draw (AMBIENT,false, m_boundingSpheresMode);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer(GL_ARRAY_BUFFER, 0 );
  };

  /** Dessin de tous les objets de la scène */
  void drawScene () const
  {
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    for (vector<CObject*>::const_iterator objectsArrayIterator = m_objectsArray.begin();
         objectsArrayIterator != m_objectsArray.end();
         objectsArrayIterator++)
      (*objectsArrayIterator)->draw(ALL,true, m_boundingSpheresMode);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer(GL_ARRAY_BUFFER, 0 );
  };

  /** Dessin de tous les objets de la scène en enlevant les textures si nécessaire */
  void drawSceneForSelection() const
  {
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    for (vector < CObject * >::const_iterator objectsArrayIterator = m_objectsArray.begin ();
         objectsArrayIterator != m_objectsArray.end ();
         objectsArrayIterator++)
      (*objectsArrayIterator)->drawForSelection ();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer(GL_ARRAY_BUFFER, 0 );
  };

  void computeBoundingBox();

  void selectItem(uint selectedItem)
  {
    assert ( selectedItem < m_objectsArray.size()+m_lightSourcesArray.size() );
    if(selectedItem < m_objectsArray.size())
      m_selectedItem = m_objectsArray[selectedItem];
    else
      m_selectedItem = m_lightSourcesArray[selectedItem-m_objectsArray.size()];
    m_selectedItem->select();
  }

  void deselectItem()
  {
    m_selectedItem->deselect();
    m_selectedItem = NULL;
  }

  void toggleItem(uint selectedItem)
  {
    assert ( selectedItem < m_objectsArray.size()+m_lightSourcesArray.size() );
    if(selectedItem >= m_objectsArray.size())
      m_lightSourcesArray[selectedItem-m_objectsArray.size()]->toggle();
  }

  void moveSelectedItem(float x, float y, float z, float oldX, float oldY);

private:
  vector<CObject*>        m_objectsArray; /** Liste des objets de la scene ne projetant pas d'ombres. */
  vector<CMaterial*>      m_materialArray; /** Liste des matériaux.*/
  vector<CBitmapTexture*> m_texturesArray; /** Liste des textures.*/
  vector<ILight*>        m_lightSourcesArray; /** Liste des sources de lumière. */

  /** Mode d'affichage */
  uint m_boundingSpheresMode;

  /** Englobant */
  CPoint m_min, m_max;

  float m_initialNbPhotons;

  CSceneItem *m_selectedItem;

public:
  /** Compteur permettant d'affecter un nom unique */
  static GLuint glNameCounter;
};//CScene

#endif

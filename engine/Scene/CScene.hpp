#ifndef SCENE_H
#define SCENE_H

class CScene;

class CMaterial;

#include "CObject.hpp"
#include "Light.hpp"
#include "CCamera.hpp"
#include "Texture.hpp"
#include "CDrawState.hpp"

#include <vector>
#include <list>




/**
 * Classe repr&eacute;sentant une sc&egrave;ne g&eacute;om&eacute;trique.
 * Une sc&egrave;ne comporte deux listes indexées des objets, une pour les objets projetant
 * des ombres et une pour les autres, une liste des matériaux associés à ces polygones et
 * une liste des sources lumineuses (inutilisée pour l'instant).<br>
 * Cette classe encapsule également les fonctions d'import de fichiers OBJ.
 *
 * @author	Flavien Bridault
 */
class CScene : public ITSingleton<CScene>
{
	friend class ITSingleton<CScene>;
private:
	/**
	 * Constructeur par défaut.
	 */
	CScene();

	/** Destructeur par d&eacute;faut. */
	~CScene();

public:

	/** Crée les VBOs - A n'appeler qu'une fois que tous les objets
	 * ont été ajouté à la scène, soit via le constructeur, soit via la méthode
	 * loadCObject() qui permet d'ajouter des objets individuels.
	 */
	void postInit(bool a_bNormalize);

	void computeBoundingBox(bool a_bNormalize);

	void sortTransparentObjects();

	/** Ajoute un objet dans la scène.
	 * @param newObj Pointeur vers l'objet &agrave; ajouter.
	 * @param objectWSV True si l'objet projette des ombres, false sinon.
	 */
	void addObject(CObject* const a_pObj)
	{
		assert(a_pObj != NULL);
		m_vpObjects.push_back(a_pObj);
	};

	/** Remove an object from the scene.
	 * @param newObj Pointeur vers l'objet &agrave; ajouter.
	 */
	void removeObject(CObject* const a_pObj)
	{
		assert(a_pObj != NULL);
		ForEachIter(itObject, CObjectsVector, m_vpObjects)
		{
			if(a_pObj == *itObject)
			{
				m_vpObjects.erase(itObject);
				return;
			}
		}
		// Object not found
		assert(false);
	};

	/** Ajoute un matériau dans la scène.
	 * @param material Pointeur sur le nouveau matériau à référencer.
	 */
	void addMaterial(CMaterial *a_pMaterial)
	{
		assert(a_pMaterial != NULL);
		m_vpMaterials.push_back(a_pMaterial);
	};

	/** Ajoute une texture dans la scène.
	 * @param texture Pointeur sur la nouvelle texture à référencer.
	 * @return Indice de la texture.
	 */
	GLuint addTexture(CBitmapTexture * const a_pTexture)
	{
		assert(a_pTexture != NULL);
		m_vpTextures.push_back(a_pTexture);
		return m_vpTextures.size()-1;
	};

	/** Ajoute une source lumineuse à la scène.
	 * @param newSource pointeur vers la source lumineuse à ajouter.
	 */
	void addSource(ILight* const a_rSource)
	{
		assert(a_rSource != NULL);
		m_vpLights.push_back(a_rSource);
	};

	/** Lecture du nombre de points contenus dans la scène.
	 * @return Nombre de points.
	 */
	uint GetVertexCount();

	/** Lecture du nombre de polygones contenus dans la scène.
	 * @return Nombre de polygones.
	 */
	uint GetPolygonsCount();

	/** Lecture du nombre d'objets contenus dans la scène.
	 * @return Nombre d'objets.
	 */
	uint GetObjectsCount() const
	{
		return m_vpObjects.size();
	};

	/** Lecture du nombre de sources lumineuses contenus dans la scène.
	 * @return Nombre de sources lumineuses.
	 */
	uint GetSourcesCount() const
	{
		return m_vpLights.size();
	};

	/*float GetArea()
	{
		float area=0.0f;
		ForEachIterC(it, CObjectsVector, m_vpObjects)
		{
			area += (*it)->GetArea();
		}
		return area;
	}*/

	/** Lecture d'un polygone spécifique contenu dans la scène.
	 * @param index Indice du polygone à obtenir.
	 * @return Un pointeur vers le polygone recherché.
	 */
	ISceneItem const& GetObject(const int index) const
	{
		assert( m_vpObjects[index] != NULL );
		return (*m_vpObjects[index]);
	};

	/** Lecture d'une source lumineuse spécifique contenue dans la scène.
	 * @param index Indice de la source à obtenir.
	 * @return Un pointeur vers la source recherchée.
	 */
	ILight const& GetSource(const int index) const
	{
		assert( m_vpLights[index] != NULL );
		return (*m_vpLights[index]);
	};
	/** Lecture d'une source lumineuse spécifique contenue dans la scène.
	 * @param index Indice de la source à obtenir.
	 * @return Un pointeur vers la source recherchée.
	 */
	ILight& GetSource(const int index)
	{
		assert( m_vpLights[index] != NULL );
		return (*m_vpLights[index]);
	};

	/** Lecture d'un matériau spécifique contenue dans la scène.
	 * @param index Indice du matériau à obtenir.
	 * @return Un pointeur vers le matériau recherché.
	 */
	CMaterial const& GetMaterial(const int index) const
	{
		assert( m_vpMaterials[index] != NULL );
		return (*m_vpMaterials[index]);
	};

	/** Lecture d'une texture spécifique contenue dans la scène.
	 * @param index Indice de la texture à obtenir.
	 * @return Un pointeur vers la texture recherchée.
	 */
	CBitmapTexture const& GetTexture(const int index) const
	{
		assert( m_vpTextures[index] != NULL );
		return (*m_vpTextures[index]);
	};

	/** Donne l'indice d'un matériau dans la liste des matériaux de la scène.
	 * @param name Nom du matériau.
	 * @return Index du matériau.
	 */
	int GetMaterialIndexByName(const string& name);

	/** Donne l'indice d'une texture dans la liste des textures de la scène.
	 * @param name Nom du fichier de la texture.
	 * @return Index de la texture, -1 si elle n'existe pas
	 */
	int searchTextureIndexByName(CharCPtrC name);

	void selectItem(uint a_uiSelectedItem)
	{
		assert ( a_uiSelectedItem < m_vpObjects.size()+m_vpLights.size() );

		if (a_uiSelectedItem < m_vpObjects.size())
			m_pSelectedItem = m_vpObjects[a_uiSelectedItem];
		else
			m_pSelectedItem = m_vpLights[a_uiSelectedItem-m_vpObjects.size()];
		m_pSelectedItem->Select();
	}

	void deselectItem()
	{
		m_pSelectedItem->Deselect();
		m_pSelectedItem = NULL;
	}

	void toggleItem(uint a_uiSelectedItem)
	{
		assert ( a_uiSelectedItem < m_vpObjects.size()+m_vpLights.size() );

		if (a_uiSelectedItem >= m_vpObjects.size())
			m_vpLights[a_uiSelectedItem-m_vpObjects.size()]->toggle();
	}

	void moveSelectedItem(float x, float y, float z, float oldX, float oldY);

private:

	typedef vector<CObject*> CObjectsVector;
	typedef vector<CMaterial*> CMaterialsVector;
	typedef vector<CBitmapTexture*> CTexturesVector;
	typedef vector<ILight*> CLightsVector;

	CObjectsVector		m_vpObjects; 		/** Liste des objets de la scène. */
	CMaterialsVector 	m_vpMaterials; 		/** Liste des matériaux.*/
	CTexturesVector		m_vpTextures; 		/** Liste des textures.*/
	CLightsVector    	m_vpLights; 		/** Liste des sources de lumière. */

	ISceneItem *m_pSelectedItem;

public:
	/** Compteur permettant d'affecter un nom unique */
	static GLuint glNameCounter;
};//CScene

#endif

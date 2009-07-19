#ifndef OBJECT_H
#define OBJECT_H

class CTransform;
class CObject;
class CMesh;

#include "../Common.hpp"
#include "CMesh.hpp"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

#include <list>
#include <vector>


//*********************************************************************************************************************
// TEMP: Introduce flags to combine possibilities
//*********************************************************************************************************************
struct _NRenderType
{
	enum EValue
	{
		eNormal,
		eTransparent,
		eFx,
		eImmediate,

		_NbValues
	};
};
DeclareNumerable(_NRenderType, NRenderType);

/** Class representing a single element of a scene. It abstracts simple selection process.
 */
class ISceneItem
{
protected:

	ISceneItem(NRenderType const& a_nRenderType);

public:

	virtual ~ISceneItem() {};

	/** Select/Unselect */
	void Select()
	{
		m_bSelected = true;
	}
	void Deselect()
	{
		m_bSelected = false;
	}
	bool IsSelected() const
	{
		return m_bSelected;
	}
	bool IsVisible() const
	{
		return m_bVisible;
	}

	/** Retrieve OpenGL item name for selection */
	GLuint GetItemName() const
	{
		return m_uiGlName;
	}
	NRenderType const& GetType() const
	{
		return m_nRenderType;
	}
//---------------------------------------------------------------------------------------------------------------------
//  Virtuals
//---------------------------------------------------------------------------------------------------------------------

	/** Donne l'englobant de l'objet.
	 * @param max Retourne le coin supérieur de l'englobant.
	 * @param min Retourne le coin inférieur de l'englobant.
	 */
	virtual void GetBoundingBox(CPoint& a_rMax, CPoint& a_rMin) const = 0;

	virtual void Render() const = 0;

	virtual void Move(CVector const& a_rDir) = 0;

	/** Calcule la visibilité de l'objet
	 * @param view Référence sur la caméra
	 */
	virtual void ComputeVisibility(const CCamera &a_rView) = 0;

protected:
	void SetVisibility(bool a_bVisible) { m_bVisible = a_bVisible; }

private:

//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

	bool m_bSelected;
	bool m_bVisible;
	GLuint m_uiGlName;
	NRenderType m_nRenderType;
};


/**********************************************************************************************************************/
/********************************************* DECLARATION DE LA CLASSE OBJECT ****************************************/
/**********************************************************************************************************************/

/** Classe représentant un groupe d'objets. Elle stocke les points, les normales et les coordonnées de ces points.
 * Ceci permet d'éviter des changements trop fréquents de VBO.
 */
class CObject : public ISceneItem
{
public:
	/**
	 * Constructeur par d&eacute;faut.
	 * @param scene Pointeur vers la scene.
	 */
	CObject(CScene& a_rScene);

	/**
	 * Constructeur par d&eacute;faut.
	 * @param scene Pointeur vers la scene.
	 */
	CObject(CScene& a_rScene, CTransform& rTransform);

	/** Destructeur par défaut. */
	virtual ~CObject ();

//---------------------------------------------------------------------------------------------------------------------
//  Inherited methods
//---------------------------------------------------------------------------------------------------------------------

	/** Donne l'englobant de l'objet.
	 * @param max Retourne le coin supérieur de l'englobant.
	 * @param min Retourne le coin inférieur de l'englobant.
	 */
	virtual void GetBoundingBox(CPoint& a_rMax, CPoint& a_rMin) const
	{
		a_rMax = m_oBBMax;
		a_rMin = m_oBBMin;
	}

	/** Move item at given position */
	virtual void Move(CVector const& a_rDir)
	{
		for (vector <CMesh* >::const_iterator meshesListIterator = m_meshesList.begin ();
			meshesListIterator != m_meshesList.end ();  meshesListIterator++)
			(*meshesListIterator)->Move(a_rDir);
	}

	/** Fonction de dessin du groupe d'objets.	 */
	virtual void Render() const;

	/** Calcule la visibilité de l'objet
	 * @param view Référence sur la caméra
	 */
	virtual void ComputeVisibility(const CCamera &a_rView);

	/** Redimensionne l'objet, utilisé lors de la normalisation de la scène.
	 * @param scaleFactor Facteur d'échelle.
	 */
	virtual void HardScale(float a_fScaleFactor);

	/** Translate l'objet, utilisé lors de la normalisation de la scène.
	 * @param offset Déplacement.
	 */
	virtual void HardTranslate(const CPoint& offset);

//---------------------------------------------------------------------------------------------------------------------
//  Specific methods
//---------------------------------------------------------------------------------------------------------------------

	void AddMesh(CMesh* const a_pMesh)
	{
		m_meshesList.push_back(a_pMesh);
	}

	/** Lecture du nombre de meshes contenus dans l'objet.
	 * @return Nombre de points.
	 */
	uint GetNbMeshes() const
	{
		return m_meshesList.size();
	}

	Vertex GetVertex(GLuint a_uiIndex) const
	{
		return m_vertexArray[a_uiIndex];
	}

	/** Lecture du nombre de points contenus dans l'objet.
	 * @return Nombre de points.
	 */
	uint GetVertexArraySize() const
	{
		return m_vertexArray.size();
	}

	/** Lecture du nombre de polygones contenus dans l'objet.
	 * @return Nombre de polygones.
	 */
	uint GetPolygonsCount() const;

	/** Retourne la position de l'objet, calculé en prenant le centre de la boîte englobante. */
	CPoint GetCenter() const
	{
		return (m_oBBMax+m_oBBMin)/2.0f;
	}

	/** Construction des sphères englobantes de l'objet. A appeler après l'import de la scène. */
	void BuildBoundingSpheres();

	/** Construction du Vertex Buffer CObject de l'objet, ici le tableau de points, normales et coordonnées de texture. */
	void BuildVBO();

	/** Bind du VBO */
	void bindVBO() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
		glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(2*sizeof(float)));
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(5*sizeof(float)));
	}

	/** Ajout d'un point dans le tableau de points.*/
	void addVertex( const Vertex& a_rVertex )
	{
		m_vertexArray.push_back(a_rVertex);

		// Adjust bounding box
		if ( a_rVertex.x > m_oBBMax.x)
			m_oBBMax.x = a_rVertex.x;
		if ( a_rVertex.y > m_oBBMax.y)
			m_oBBMax.y = a_rVertex.y;
		if ( a_rVertex.z > m_oBBMax.z)
			m_oBBMax.z = a_rVertex.z;

		if ( a_rVertex.x < m_oBBMin.x)
			m_oBBMin.x = a_rVertex.x;
		if ( a_rVertex.y < m_oBBMin.y)
			m_oBBMin.y = a_rVertex.y;
		if ( a_rVertex.z < m_oBBMin.z)
			m_oBBMin.z = a_rVertex.z;
	}

	/** Affectation des coordonnées de texture et de la normale d'un point donné.*/
	void SetVertexNormalAndTexcoord( uint i, float u, float v, float nx, float ny, float nz )
	{
		assert ( i < m_vertexArray.size() );
		m_vertexArray[i].u  =  u;
		m_vertexArray[i].v  =  v;
		m_vertexArray[i].nx = nx;
		m_vertexArray[i].ny = ny;
		m_vertexArray[i].nz = nz;
	}

	/** Dessin des sphères englobantes. */
	void drawBoundingSpheres();

	/** Return total area of the object */
	float GetArea() const
	{
		float area=0.0f;
		for (vector <CMesh* >::const_iterator meshesListIterator = m_meshesList.begin ();
		        meshesListIterator != m_meshesList.end ();  meshesListIterator++)
			area += (*meshesListIterator)->GetArea();
		return area;
	}

	/** Return a given triangle index in the mesh */
	void GetTriangle(uint a_uiMesh, uint iTriangle, CPoint &P1, CPoint &P2, CPoint &P3) const
	{
		assert( a_uiMesh < m_meshesList.size() );
		m_meshesList[a_uiMesh]->GetTriangle(iTriangle,P1,P2,P3);
	}

	/** Return triangle meshes count */
	uint GetNumTrianglesMeshes(uint a_uiMesh) const
	{
		assert( a_uiMesh < m_meshesList.size() );
		return m_meshesList[a_uiMesh]->GetPolygonsCount();
	}

	/** Lecture de l'index du matériau utilisé par un maillage de l'objet.
	 *  @return Index du matériau dans la liste de matériau contenu dans la scène.
	 */
	uint GetMaterialIndex(uint a_uiMesh) const
	{
		assert( a_uiMesh < m_meshesList.size() );
		return m_meshesList[a_uiMesh]->GetMaterialIndex();
	}

// CLEANUP
	CTransform const& GetTransform() const
	{
		return m_rTransform;
	}
	CTransform& GrabTransform()
	{
		return m_rTransform;
	}

//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

protected:
	/**<Liste des points de l'objet */
	vector <Vertex> m_vertexArray;

private:
	/** Liste des objets */
	typedef vector < CMesh* > CMeshesVector;
	CMeshesVector m_meshesList;

	bool m_bBuilt;

	/** Référence vers la scène. */
	CScene& m_rScene;

// CLEANUP
	/** Référence vers le noeud de transformation */
	CTransform& m_rTransform;

	/** Identifiant du Vertex Buffer CObject. */
	GLuint m_bufferID;

	/** Type d'attributs présents dans le maillage soit :<li>
	 * <ol>0 pour points,</ol>
	 * <ol>1 pour points et normales,</ol>
	 * <ol>2 pour points et normales,</ol>
	 * <ol>3 pour points, normales et coordonnées de texture.</ol>
	 * </li> */
	uint m_attributes;

	CPoint m_oBBMin, m_oBBMax;
};


#endif

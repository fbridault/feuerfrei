#ifndef CMESH_H
#define CMESH_H

#include "../Common.hpp"
#include "../Maths/CVector.hpp"
#include "CBoundingSphere.hpp"

#include <vector>

class CScene;
class CObject;
class CRefTable;
class Camera;

class Vertex
{
public:
	GLfloat u, v, nx, ny, nz, x, y, z;
};

/**********************************************************************************************************************/
/********************************************* DECLARATION DE LA CLASSE MESH ******************************************/
/**********************************************************************************************************************/

/**
 * Classe repr&eacute;sentant un maillage.
 * Un maillage sc&egrave;ne comporte une liste index&eacute;e des polygones, des normales et des coordonnées de textures.
 * Un seul matériau est appliqué à un maillage.
 *
 * @author	Flavien Bridault
 */
class CMesh
{
public:
	/**
	 * Constructeur par d&eacute;faut.
	 * @param scene Pointeur vers la scene.
	 */
	CMesh (CScene& a_rScene, uint a_uiMaterial, CObject& a_rParent);

	/** Destructeur par défaut. */
	virtual ~CMesh ();

	/** Lecture d'un indice dans le maillage.
	 * @return Indice. */
	uint getIndex(uint i) const
	{
		assert( i < m_indexArray.size()) ;
		return m_indexArray[i];
	}

	/** Affectation d'un indice dans le maillage.
	 * @return Indice. */
	void setIndex(uint i, GLuint value)
	{
		assert( i < m_indexArray.size()) ;
		m_indexArray[i] = value;
	}

	/** Lecture du nombre de polygones contenus dans le maillage.
	 * @return Nombre de polygones. */
	uint getIndexesCount () const
	{
		return (m_indexArray.size());
	};

	/** Lecture du nombre de polygones contenus dans le maillage.
	 * @return Nombre de polygones. */
	uint getPolygonsCount () const
	{
		return (m_indexArray.size() / 3);
	};

	/** Lecture de l'index du matériau utilisé par le maillage.
	 * @return Index du matériau dans la liste de matériau contenu dans la scène. */
	uint getMaterialIndex () const
	{
		return (m_uiMaterial);
	};

	/** Met à jour les attributs de l'objet. */
	void setAttributes (uint attr)
	{
		m_attributes = attr;
	};
	/** Récupérer les attributs de l'objet. */
	uint getAttributes () const
	{
		return m_attributes;
	};

	/** Construction du Vertex Buffer CObject du maillage, ici le tableau d'indice. */
	void buildVBO() const;

	/** Fonction de dessin de l'objet avec utilisation des VBOs.
	 * @param drawCode
	 * si TEXTURED, alors l'objet n'est dessiné que s'il possède une texture
	 * si FLAT alors l'objet n'est dessiné que s'il ne possède pas une texture
	 * si ALL alors l'objet est dessiné inconditionnellement
	 * si AMBIENT alors l'objet est dessiné avec un matériau blanc en composante ambiante (pour les ombres)
	 * @param tex false si l'objet texturé doit être affiché sans sa texture
	 * @param lastMaterialIndex indice du dernier matériau appliqué, utilisé en entrée et en sortie.
	 */
	void draw(char drawCode, bool tex, uint& lastMaterialIndex) const;

	void drawForSelection () const;

	const bool isTransparent () const;

	/** Ajout d'un index de point dans le tableau d'indices.
	 * @param i indice à ajouter.
	 */
	void addIndex( GLuint i )
	{
		m_indexArray.push_back(i);
	};

	/** Calcule la visibilité de l'objet
	 * @param view Référence sur la caméra
	 */
	void computeVisibility(const CCamera &view);

	/** Construction des sphères englobantes de l'objet. A appeler après l'import de la scène. */
	void buildBoundingSphere (CRefTable& refTable);

	/** Dessin des sphères englobantes. */
	void drawBoundingSphere ();

	void Move(const CVector& dir)
	{
		CPoint& rCenter = m_boundingSphere.GrabCentre();
		rCenter += dir;
	}

	CPoint const& GetPosition() const
	{
		return m_boundingSphere.GetCentre();
	}

	float getArea() const;

	void getTriangle(uint iTriangle, CPoint &P1, CPoint &P2, CPoint &P3) const;
	void getTriangle(uint iTriangle, CPoint &P1, CPoint &P2, CPoint &P3, CVector& normal) const;

	CVector generateRandomRayHemisphere(uint iTriangle) const;

private:
	/**<Liste des indices des points des facettes */
	vector <GLuint> m_indexArray;

	/** Référence vers la scène. */
	CScene& m_rScene;

	/** Référence vers l'objet parent */
	CObject& m_rParent;

	/** Pointeur vers le matériau utilisé. */
	uint m_uiMaterial;

	/** Type d'attributs présents dans le maillage soit :
	 * <li>
	 * <ol>0 pour points,</ol>
	 * <ol>1 pour points et normales,</ol>
	 * <ol>2 pour points et normales,</ol>
	 * <ol>3 pour points, normales et coordonnées de texture.</ol>
	 * </li> */
	uint m_attributes;

	/* Identifiant du Vertex Buffer CObject. */
	GLuint m_bufferID;

	/** Visibilité de l'objet par rapport au frustum. */
	bool m_visibility;

	CBoundingSphere m_boundingSphere;
};

#endif // CMESH_H

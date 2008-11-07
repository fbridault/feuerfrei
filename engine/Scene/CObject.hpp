#ifndef OBJECT_H
#define OBJECT_H

class CObject;
class CMesh;

#include "../Common.hpp"
#include "CMesh.hpp"

#define ALL      0
#define TEXTURED 1
#define FLAT     2
#define AMBIENT  3

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

#include <list>
#include <vector>


class CSceneItem
{
protected:
	CSceneItem(CPoint const& position) : m_position(position), m_selected(false) {};

public:
	virtual ~CSceneItem() {};

	virtual void drawForSelection() const = 0;

	CPoint const& getPosition () const
	{
		return m_position;
	}

	void GetPosition (float &x, float &y, float &z) const
	{
		x = m_position.x;
		y = m_position.y;
		z = m_position.z;
	}

	virtual void move (float x, float y, float z)
	{
		m_position.x += x;
		m_position.y += y;
		m_position.z += z;
	}

	void SetPosition (CPoint const& a_rPosition)
	{
		m_position = a_rPosition;
	}

	void select()
	{
		m_selected = true;
	};
	void deselect()
	{
		m_selected = false;
	};

protected:
	CPoint m_position;
	bool m_selected;
	/** Identifiant OpenGL pour la sélection */
	GLuint m_glName;
};

/**********************************************************************************************************************/
/********************************************* DECLARATION DE LA CLASSE OBJECT ****************************************/
/**********************************************************************************************************************/

/** Classe représentant un groupe d'objets. Elle stocke les points, les normales et les coordonnées de ces points.
 * Ceci permet d'éviter des changements trop fréquents de VBO.
 */
class CObject : public CSceneItem
{
public:
	/**
	 * Constructeur par d&eacute;faut.
	 * @param scene Pointeur vers la scene.
	 */
	CObject(CScene& a_rScene);

	/** Destructeur par défaut. */
	virtual ~CObject ();

	void addMesh(CMesh* const mesh)
	{
		m_meshesList.push_back(mesh);
	}

	/** Lecture du nombre de meshes contenus dans l'objet.
	 * @return Nombre de points.
	 */
	uint getNbCMeshes () const
	{
		return m_meshesList.size();
	};

	/** Lecture du nombre de points contenus dans l'objet.
	 * @return Nombre de points.
	 */
	uint getVertexArraySize () const
	{
		return m_vertexArray.size();
	};

	Vertex getVertex (GLuint i) const
	{
		return m_vertexArray[i];
	};

	/** Construit l'englobant de l'objet.
	 */
	void buildBoundingBox ();

	/** Donne l'englobant de l'objet.
	 * @param max Retourne le coin supérieur de l'englobant.
	 * @param min Retourne le coin inférieur de l'englobant.
	 */
	void getBoundingBox (CPoint& max, CPoint& min) const
	{
		max=m_max;
		min=m_min;
	};

	/** Retourne la position de l'objet, calculé en prenant le centre de la boîte englobante. */
	CPoint getCenter () const
	{
		return (m_max+m_min)/2.0f;
	};

	/** Redimensionne l'objet, utilisé lors de la normalisation de la scène.
	 * @param scaleFactor Facteur d'échelle.
	 */
	void scale (float scaleFactor, const CPoint& offset);

	void bindVBO() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
		glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(2*sizeof(float)));
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(5*sizeof(float)));
	};

	/** Construction du Vertex Buffer CObject de l'objet, ici le tableau de points, normales et coordonnées de texture. */
	void buildVBO();

	/** Fonction de dessin du groupe d'objets.
	 * @param drawCode
	 * si TEXTURED, alors l'objet n'est dessiné que s'il possède une texture
	 * si FLAT alors l'objet n'est dessiné que s'il ne possède pas une texture
	 * si ALL alors l'objet est dessiné inconditionnellement
	 * si AMBIENT alors l'objet est dessiné avec un matériau blanc en composante ambiante (pour les ombres)
	 * @param tex false si l'objet texturé doit être affiché sans sa texture
	 * @param boundingSpheres true si l'objet doit afficher les sphères englobantes
	 */
	void draw(char drawCode=ALL, bool tex=true, bool boundingSpheres=false) const;

	void drawForSelection () const;

	/** Lecture du nombre de polygones contenus dans l'objet.
	 * @return Nombre de polygones.
	 */
	uint getPolygonsCount () const;

	/** Ajout d'un point dans le tableau de points.*/
	void addVertex( const Vertex& v )
	{
		m_vertexArray.push_back(v);
	};

	/** Affectation des coordonnées de texture et de la normale d'un point donné.*/
	void setVertex( uint i, float u, float v, float nx, float ny, float nz )
	{
		assert ( i < m_vertexArray.size() );
		m_vertexArray[i].u  =  u;
		m_vertexArray[i].v  =  v;
		m_vertexArray[i].nx = nx;
		m_vertexArray[i].ny = ny;
		m_vertexArray[i].nz = nz;
	};

	/** Calcule la visibilité de l'objet
	 * @param view Référence sur la caméra
	 */
	void computeVisibility(const CCamera &view);

	/** Construction des sphères englobantes de l'objet. A appeler après l'import de la scène. */
	void buildBoundingSpheres ();

	/** Dessin des sphères englobantes. */
	void drawBoundingSpheres ();

	/** Translation en "dur" de l'objet. Toutes les coordonnées de ses points sont modifiées.
	 * @param direction Vecteur de translation.
	 */
	void translate(const CVector& direction);

	float getArea() const
	{
		float area=0.0f;
		for (vector <CMesh* >::const_iterator meshesListIterator = m_meshesList.begin ();
		     meshesListIterator != m_meshesList.end ();  meshesListIterator++)
			area += (*meshesListIterator)->getArea();
		return area;
	};

	void getTriangle(uint iCMesh, uint iTriangle, CPoint &P1, CPoint &P2, CPoint &P3) const
	{
		assert( iCMesh < m_meshesList.size() );
		m_meshesList[iCMesh]->getTriangle(iTriangle,P1,P2,P3);
	}

	uint getNbTrianglesMeshes(uint iCMesh) const
	{
		assert( iCMesh < m_meshesList.size() );
		return m_meshesList[iCMesh]->getPolygonsCount();
	};

	/** Lecture de l'index du matériau utilisé par un maillage de l'objet.
	 *  @return Index du matériau dans la liste de matériau contenu dans la scène.
	 */
	uint getMaterialIndex (uint iCMesh) const
	{
		assert( iCMesh < m_meshesList.size() );
		return m_meshesList[iCMesh]->getMaterialIndex();
	};

protected:
	/**<Liste des points de l'objet */
	vector <Vertex> m_vertexArray;

private:
	/** Liste des objets */
	vector < CMesh* > m_meshesList;

	/** Indice du matériau utilisé par le point précédent. Ceci permet de savoir lors de la phase de dessin
	 * si le point courant utilise un autre matériau qui nécessite un appel à glMaterial().
	 */
	int m_lastMaterialIndex;
	/** Permet de vérifier si le mesh précédent était texturé, permet de limiter les activations et
	 * désactivations des unités de texture.
	 */
	bool m_previousCMeshWasTextured;

	/** Pointeur vers la scène. */
	CScene& m_rScene;

	/* Identifiant du Vertex Buffer CObject. */
	GLuint m_bufferID;

	/** Type d'attributs présents dans le maillage soit :<li>
	 * <ol>0 pour points,</ol>
	 * <ol>1 pour points et normales,</ol>
	 * <ol>2 pour points et normales,</ol>
	 * <ol>3 pour points, normales et coordonnées de texture.</ol>
	 * </li> */
	uint m_attributes;

	CPoint m_min, m_max;
};

#endif

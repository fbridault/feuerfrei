#ifndef OBJECT_H
#define OBJECT_H

class Object;
class Mesh;

#include <engine/graphicsFn.hpp>
#include "camera.hpp"

#include <list>
#include <vector>

#define ALL      0
#define TEXTURED 1
#define FLAT     2
#define AMBIENT  3

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

/**********************************************************************************************************************/
/********************************************* DECLARATION DE LA CLASSE VERTEX ****************************************/
/**********************************************************************************************************************/

/** Classe utilisée pour manipuler les points avec les VBOs. Il s'agit
 * uniquement d'une structure contenant dans l'ordre les coordonnées de texture,
 * la normale et les coordonnées dans le repère global
 */
class Vertex
{
public:
	GLfloat u, v, nx, ny, nz, x, y, z;
};


/**********************************************************************************************************************/
/****************************************** DECLARATION DE LA CLASSE BOUNDINGSPHERE ***********************************/
/**********************************************************************************************************************/

/** Classe représentant les sphères englobantes utilisées pour le frustum
 * culling. */
class BoundingSphere
{
public:
	/** Constructeur par défaut. */

	BoundingSphere() : radius(0.0f) {};

	/** Calcule la visibilité de la sphère par rapport au point de vue courant.
	 *
	 * @param view Référence sur la caméra
	 */
	bool isVisible(const Camera &view) const{
		uint i;
		const float *plan;

		// Centre dans le frustum ?
		//   for( p = 0; p < 6; p++ )
		//     if( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= 0 ){
		//       m_visibility = false;
		//       return;
		//     }

		// Sphère dans le frustum ?
		for ( i = 0; i < 6; i++ ){
			plan=view.getFrustum(i);
			if ( plan[0] * centre.x + plan[1] * centre.y + plan[2] * centre.z + plan[3] <= -radius )
				return false;
		}
		return true;
	}

	/** Calcule la visibilité de la sphère par rapport au point de vue courant.
	 * Variation permettant de récupérer en même temps la distance par rapport à la caméra.
	 *
	 * @param view Référence sur la caméra.  @return 0 si non visible, distance à
	 * la caméra sinon.
	 */
	float visibleDistance(const Camera &view) const{
		uint i;
		const float *plan;
		float d;

		// Sphère dans le frustum ?
		for ( i = 0; i < 6; i++ ){
			plan=view.getFrustum(i);
			d = plan[0] * centre.x + plan[1] * centre.y + plan[2] * centre.z + plan[3];
			if ( d <= -radius )
				return 0.0f;
		}
		return d + radius;
	}

	/** Calcule la couverture en pixel de la sphère. On projette le centre et un
	 * point à la périphérie sur l'écran, puis on calcule l'aire du disque obtenu.
	 */
	float getPixelCoverage(const Camera &view) const{
		CPoint centerSC, periSC;

		view.getSphereCoordinates(centre, radius, centerSC, periSC);
		/* PI.R² */
		return ( M_PI* centerSC.squaredDistanceFrom(periSC));
	}

	/** Affichage de la sphère (en transparence). */
	void draw(void) const{
		glEnable(GL_BLEND);
		glPushMatrix();
		glTranslatef(centre.x, centre.y, centre.z);
		glColor4f(1.0f,0.0f,0.0f,0.3f);
		CUGraphicsFn::SolidSphere(radius, 30, 30);
		glPopMatrix();
		glDisable(GL_BLEND);
	}

	/** Centre de la sphère englobante. */
	CPoint centre;

	/** Rayon de la sphère englobante. */
	float radius;
};

class Scene;

/**********************************************************************************************************************/
/********************************************* DECLARATION DE LA CLASSE OBJECT ****************************************/
/**********************************************************************************************************************/

/** Classe représentant un groupe d'objets. Elle stocke les points, les normales
 * et les coordonnées de ces points.  Ceci permet d'éviter des changements trop
 * fréquents de VBO.
 */
class Object
{
public:
	/**
	 * Constructeur par d&eacute;faut.
	 * @param scene CPointeur vers la scene.
	 */
	Object(const Scene* const scene);

	/** Destructeur par défaut. */
	virtual ~Object ();

	void addMesh(Mesh* const mesh)
	{
		m_meshesList.push_back(mesh);
	}

	/** Lecture du nombre de points contenus dans l'objet.
	 * @return Nombre de points.
	 */
	uint getVertexArraySize () const {
		return m_vertexArray.size();
	};

	Vertex getVertex (GLuint i) const {
		return m_vertexArray[i];
	};

	/** Construit l'englobant de l'objet.
	 */
	void buildBoundingBox ();

	/** Donne l'englobant de l'objet.
	 * @param max Retourne le coin supérieur de l'englobant.
	 * @param min Retourne le coin inférieur de l'englobant.
	 */
	void getBoundingBox (CPoint& max, CPoint& min) {
		max=m_max;
		min=m_min;
	};

	void bindVBO() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
		glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(2*sizeof(float)));
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(5*sizeof(float)));
	};

	/** Construction du Vertex Buffer Object de l'objet, ici le tableau de points,
	 *  normales et coordonnées de texture. */
	void buildVBO();

	/** Fonction de dessin du groupe d'objets.
	 *
	 * @param drawCode si TEXTURED, alors l'objet n'est dessiné que s'il possède
	 * une texture si FLAT alors l'objet n'est dessiné que s'il ne possède pas une
	 * texture si ALL alors l'objet est dessiné inconditionnellement si AMBIENT
	 * alors l'objet est dessiné avec un matériau blanc en composante ambiante
	 * (pour les ombres) @param tex false si l'objet texturé doit être affiché
	 * sans sa texture @param boundingSpheres true si l'objet doit afficher les
	 * sphères englobantes
	 */
	void draw(char drawCode=ALL, bool tex=true, bool boundingSpheres=false) const;

	/** Lecture du nombre de polygones contenus dans l'objet.
	 * @return Nombre de polygones.
	 */
	uint getPolygonsCount () const;

	/** Ajout d'un point dans le tableau de points.*/
	void addVertex( const Vertex& v) {
		m_vertexArray.push_back(v);
	};

	/** Affectation des coordonnées de texture et de la normale d'un point donné.*/
	void setVertex( uint i, float u, float v, float nx, float ny, float nz) {
		m_vertexArray[i].u  =  u;
		m_vertexArray[i].v  =  v;
		m_vertexArray[i].nx = nx;
		m_vertexArray[i].ny = ny;
		m_vertexArray[i].nz = nz;
	};

	/** Calcule la visibilité de l'objet
	 * @param view Référence sur la caméra
	 */
	void computeVisibility(const Camera &view);

	/** Construction des sphères englobantes de l'objet. A appeler après l'import de la scène. */
	void buildBoundingSpheres ();

	/** Dessin des sphères englobantes. */
	void drawBoundingSpheres () const;

	/** Retourne la position de l'objet, calculé en prenant le centre de la boîte englobante. */
	CPoint getPosition () const {
		return (m_max+m_min)/2.0f;
	};

	/** Translation en "dur" de l'objet. Toutes les coordonnées de ses points sont modifiées.
	 * @param direction Vecteur de translation.
	 */
	void translate(const CVector& direction);

	/** Allocation de la table, éventuellement détruite si elle a été allouée précedemment. */
	void allocLookupTable(){
		if (m_lookupTable) delete [] m_lookupTable;
		m_lookupTable = new int[m_vertexArray.size()];
	};

	/** Initialisation de tous les éléments de la table à -1. */
	void initLookupTable(){
		for (uint i=0; i<m_vertexArray.size(); i++) m_lookupTable[i] = -1;
	};

	/** Ajout d'un indice dans la table.
	 *
	 * @param i indice du point dans le tableau de points.
	 * @param ref indice de * la référence du point dans le tableau d'indice du
	 * mesh courant.
	 */
	void addRefInLookupTable(uint i, uint ref){
		m_lookupTable[i] = ref;
	};

	/** Recherche d'un indice dans la table.
	 *
	 * @param i indice recherché
	 */
	bool findRefInLookupTable(uint i) const
	{
		if ( m_lookupTable[i] >= 0)
			return true;
		return false;
	}

protected:
	/**<Liste des points de l'objet */
	vector <Vertex> m_vertexArray;

private:
	/** Liste des objets */
	list < Mesh* > m_meshesList;

	/** Table permettant, lors de la reconstruction des index des normales et des
	 * coordonnées de texture d'un mesh en provenance d'un fichier OBJ, de stocker
	 * les références à un indice d'un point. Seule une référence est stockée pour
	 * chaque point identique. Ce point est ensuite utilisé pour comparaison.
	 */
	int *m_lookupTable;

	/** Indice du matériau utilisé par le point précédent. Ceci permet de savoir
	 * lors de la phase de dessin si le point courant utilise un autre matériau
	 * qui nécessite un appel à glMaterial().
	 */
	int m_lastMaterialIndex;
	/** Permet de vérifier si le mesh précédent était texturé, permet de limiter
	 * les activations et désactivations des unités de texture.
	 */
	bool m_previousMeshWasTextured;

	/** CPointeur vers la scène. */
	const Scene *m_scene;

	/* Identifiant du Vertex Buffer Object. */
	GLuint m_bufferID;

	/** Type d'attributs présents dans le maillage soit :<li>
	 * <ol>0 pour points,</ol>
	 * <ol>1 pour points et normales,</ol>
	 * <ol>2 pour points et normales,</ol>
	 * <ol>3 pour points, normales et coordonnées de texture.</ol>
	 * </li> */
	uint m_attributes;

	/** Min et max pour la boîte englobante. */
	CPoint m_min, m_max;
};

/**********************************************************************************************************************/
/********************************************* DECLARATION DE LA CLASSE MESH ******************************************/
/**********************************************************************************************************************/

/**
 * Classe repr&eacute;sentant un maillage.  Un maillage sc&egrave;ne comporte
 * une liste index&eacute;e des polygones, des normales et des coordonnées de
 * textures.  Un seul matériau est appliqué à un maillage.
 *
 * @author	Flavien Bridault
 */
class Mesh
{
public:
	/**
	 * Constructeur par d&eacute;faut.
	 * @param scene CPointeur vers la scene.
	 */
	Mesh (const Scene* const scene, uint materialIndex, Object *parent);

	/** Destructeur par défaut. */
	virtual ~Mesh ();

	/** Lecture du nombre de polygones contenus dans le maillage.
	 * @return Nombre de polygones. */
	uint getPolygonsCount () const {
		return (m_indexArray.size() / 3);
	};

	/** Lecture de l'index du matériau utilisé par le maillage.
	 * @return Index du matériau dans la liste de matériau contenu dans la scène. */
	uint getMaterialIndex () const {
		return (m_materialIndex);
	};

	/** Met à jour les attributs de l'objet. */
	void setAttributes (uint attr) {
		m_attributes = attr;
	};
	/** Récupérer les attributs de l'objet. */
	uint getAttributes () const {
		return m_attributes;
	};

	/** Construction du Vertex Buffer Object du maillage, ici le tableau d'indice. */
	void buildVBO() const;

	/** Fonction de dessin de l'objet avec utilisation des VBOs.
	 *
	 * @param drawCode si TEXTURED, alors l'objet n'est dessiné que s'il possède
	 * une texture si FLAT alors l'objet n'est dessiné que s'il ne possède pas une
	 * texture si ALL alors l'objet est dessiné inconditionnellement si AMBIENT
	 * alors l'objet est dessiné avec un matériau blanc en composante ambiante
	 * (pour les ombres) @param tex false si l'objet texturé doit être affiché
	 * sans sa texture @param lastMaterialIndex indice du dernier matériau
	 * appliqué, utilisé en entrée et en sortie.
	 */
	void draw(char drawCode, bool tex, uint& lastMaterialIndex) const;

	/** Fusion des trois tableaux en un seul tableau.  Le format OBJ gère trois
	 * tableaux d'indices, alors qu'en OpenGL il n'y a qu'un seul tableau
	 * d'indice.  L'algorithme présent dans cette fonction réalise la fusion des
	 * ces trois tableaux en un seul.<br> Le principe est le suivant : pour chaque
	 * indice du tableau de points, on regarde si il a déjà été référencé
	 * auparavant dans le tableau. Si c'est le cas, alors on regarde si ses
	 * normales et ses coordonnées de texture (pas les indices mais les
	 * coordonnées réelles, car certains exportateurs en OBJ ont tendance à
	 * dupliquer les normales et les coordonnées de textures) sont égales à celle
	 * de la première référence. Dans l'affirmative, il n'y a rien à faire,
	 * l'indice du point est correct. Dans le cas contraire, il est nécessaire de
	 * dupliquer le point dans le tableau de point de l'objet, d'affecter les
	 * coordonnées de texture et la normale, et enfin de modifier l'indice du
	 * point courant en conséquence. Enfin dans le cas où le point n'a pas encore
	 * été référencé dans le tableau, on mémorise cette référence dans une table
	 * utilisée pour le premier cas, et ensuite on affecte les coordonnées de
	 * texture et la normale.
	 *
	 * @param normalsVector Vecteur des normales.
	 * @param normalsIndexVector Vecteur des indices des normales.
	 * @param texCoordsVector Vecteur des coordonnées de texture.
	 * @param texCoordsIndexVector Vecteur des indices des coordonnées de texture.
	 */
	void setUVsAndNormals(const vector < CVector > &normalsVector,   const vector < GLuint > &normalsIndexVector,
	                      const vector < CPoint >  &texCoordsVector, const vector < GLuint > &texCoordsIndexVector);

	/** Indique si l'objet est transparent ou non (en vue d'un éventuel tri). */
	const bool isTransparent () const;

	/** Ajout d'un index de point dans le tableau d'indices.
	 *
	 * @param i indice à ajouter.
	 */
	void addIndex( GLuint i ) {
		m_indexArray.push_back(i);
	};

	/** Calcule la visibilité de l'objet
	 *
	 * @param view Référence sur la caméra
	 */
	void computeVisibility(const CCamera &view);

	/** Construction des sphères englobantes de l'objet. A appeler après l'import
	 * de la scène. */
	void buildBoundingSphere ();

	/** Dessin des sphères englobantes. */
	void drawBoundingSphere () const;

	/** Retourne la position du centre de l'objet. */
	CPoint getPosition () const {
		return m_boundingSphere.centre;
	};

private:
	/**<Liste des indices des points des facettes */
	vector <GLuint> m_indexArray;

	/** CPointeur vers la scène. */
	const Scene *m_scene;
	/** CPointeur vers l'objet parent */
	Object *m_parent;

	/** CPointeur vers le matériau utilisé. */
	uint m_materialIndex;

	/** Type d'attributs présents dans le maillage soit :
	 * <li>
	 * <ol>0 pour points,</ol>
	 * <ol>1 pour points et normales,</ol>
	 * <ol>2 pour points et normales,</ol>
	 * <ol>3 pour points, normales et coordonnées de texture.</ol>
	 * </li> */
	uint m_attributes;

	/* Identifiant du Vertex Buffer Object. */
	GLuint m_bufferID;

	/** Visibilité de l'objet par rapport au frustum. */
	bool m_visibility;

	/** Sphère englobante utilisée pour le frustum culling. */
	BoundingSphere m_boundingSphere;
};



#endif

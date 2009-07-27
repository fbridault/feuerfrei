#if !defined(FIRE_H)
#define FIRE_H

#define NO_BOUNDING_VOLUME 0
#define BOUNDING_SPHERE 1
#define IMPOSTOR 2

class FlameLight;
class IFireSource;

#include "flames.hpp"

#include <engine/Utility/GraphicsFn.hpp>
#include <engine/Shading/Glsl.hpp>
#include <engine/Scene/Texture.hpp>
#include <engine/Scene/CObject.hpp>
#include <engine/Scene/Light.hpp>

class CScene;

#include "../solvers/solver3D.hpp"
#include "realFlames.hpp"
#include "ies.hpp"

class IRealFlame;
class Field3D;
class IES;

#ifdef COUNT_NURBS_POLYGONS
extern uint g_count;
#endif

//*********************************************************************************************************************
// Bounding volume type used for display
//*********************************************************************************************************************
struct _NDisplayBoundingVolume
{
	enum EValue
	{
		eNone,
		eSphere,
		eImpostor,

		_NbValues
	};
};
DeclareNumerable(_NDisplayBoundingVolume, NDisplayBoundingVolume);


/*********************************************************************************************************************/
/**		Class CDrawState	          			  	 																 */
/*********************************************************************************************************************/
class CRenderFlameState : public ITSingleton<CRenderFlameState>
{
	friend class ITSingleton<CRenderFlameState>;

private:
	/**
	 * Constructeur par défaut.
	 */
	CRenderFlameState() :
		m_bDisplay(true),
		m_bDisplayParticle(false),
		m_nBoundingVolume(NDisplayBoundingVolume::eNone)
	{}

public:

	bool GetDisplay() const { return m_bDisplay; };
	void SetDisplay(bool a_bDisplay) { m_bDisplay = a_bDisplay; }

	bool GetDisplayParticle() const { return m_bDisplayParticle; };
	void SetDisplayParticle(bool a_bDisplayParticle) { m_bDisplayParticle = a_bDisplayParticle; }

	NDisplayBoundingVolume const& GetDisplayBoundingVolume() const { return m_nBoundingVolume; };
	void SetDisplayBoundingVolume(NDisplayBoundingVolume const& a_nBoundingVolume) { m_nBoundingVolume = a_nBoundingVolume; }

private:

	/** Affiche ou non la flamme */
	bool m_bDisplay;
	/** Affiche ou non les particules des squelettes */
	bool m_bDisplayParticle;
	/** Volume englobant à afficher: aucun, sphère, ou boîte */
	NDisplayBoundingVolume m_nBoundingVolume;
};

/**********************************************************************************************************************/
/************************************* DEFINITION DE L'INTERFACE CFLAMELIGHT ******************************************/
/**********************************************************************************************************************/
class CFlameLight : public COmniLight
{
public:
	CFlameLight(CTransform& a_rTransform,
				const CEnergy &a_rEnergy,
				GLuint a_uiDepthMapSize,
				const CShader& a_rGenShadowCubeMapShader,
				const CRenderTarget& a_rShadowRenderTarget,
				const string& a_rIESFileName) :
		COmniLight(a_rTransform, a_rEnergy, a_uiDepthMapSize, a_rGenShadowCubeMapShader, a_rShadowRenderTarget)
	{
		m_pIesFile = new IES(a_rIESFileName.c_str());
		setEnergy(CEnergy(1.0,0.5,0.0));
	}

	virtual ~CFlameLight()
	{
		delete m_pIesFile;
	}

//---------------------------------------------------------------------------------------------------------------------
//  Specific methods
//---------------------------------------------------------------------------------------------------------------------

	/** Modifie le coefficient pondérateur de l'intensité. */
	void SetIntensity(float coef)
	{
		CEnergy& rEnergy = GrabEnergy();
		rEnergy *= coef;
	}

	float getLazimut() const { return m_pIesFile->getLazimut(); }
	float getLzenith() const { return m_pIesFile->getLzenith(); }
	float getLazimutTEX() const { return m_pIesFile->getLazimutTEX(); }
	float getLzenithTEX() const { return m_pIesFile->getLzenithTEX();	}

	/** Retourne le nombre de valeurs en zénithal/azimuthal. */
	uint getIESZenithSize() const	{ return m_pIesFile->getNbzenith();	}
	uint getIESAzimuthSize() const { return m_pIesFile->getNbazimut();}

	/** Supprime le fichier IES courant et en utilise un autre. */
	void useNewIESFile(string const& a_rIESFileName)
	{
		delete m_pIesFile;
		m_pIesFile = new IES(a_rIESFileName.c_str());
	}


private:
	/** Fichier IES utilisé pour le solide photométrique de la source. */
	IES* m_pIesFile;
};


/**********************************************************************************************************************/
/************************************* DEFINITION DE L'INTERFACE IFIRESOURCE ******************************************/
/**********************************************************************************************************************/

/** La classe IFireSource désigne un objet qui produit du feu. Il se compose d'un luminaire (classe Object), de
 * flammes (classe IRealFlame) et d'une source de lumière (classe FlameLight). Cette classe est abstraite.<br>
 * Pour obtenir la position absolue dans le repère du monde, il faut passer par la méthode
 * getPosition() qui additionne la position relative de la flamme à la position absolue du solveur dans le monde.
 * Toute la construction de la flamme est faite dans le repère (0,0,0). Lors du dessin de la flamme dans la méthode
 * drawFlame(), une translation est effectuée en récupérant la position avec getPosition() pour placer la flamme
 * dans le repère du monde.
 *
 * @author	Flavien Bridault
 */
class IFireSource : public ISceneItem
{
public:
	/** Constructeur d'une source de flammes. La position de la source est donnée dans le repère du solveur.
	 * @param flameConfig Configuration de la flamme.
	 * @param s CPointeur sur le solveur de fluides.
	 * @param nbFlames Nombre de flammes, si = 0 alors le tableau contenant les flammes n'est pas alloué.
	 * et ceci doit alors être réalisé par la classe fille.
	 * @param scene CPointeur sur la scène.
	 * @param texname Nom du fichier contenant le luminaire.
	 */
	IFireSource(CTransform& a_rTransform,
				const FlameConfig& a_rFlameConfig,
				Field3D& a_rField,
				uint a_uiNbFlames,
				CharCPtrC a_szTexname,
				const CShader& a_rGenShadowCubeMapShader,
				const CRenderTarget& a_rShadowRenderTarget);

	/** Destructeur */
	virtual ~IFireSource ();

//---------------------------------------------------------------------------------------------------------------------
//  Inherited methods
//---------------------------------------------------------------------------------------------------------------------

	/** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
	 * build() avec le placage de texture. La flamme d'une IRealFlame est définie dans le repère du solveur,
	 * donc seule une translation correspondant à la position du solveur est effectuée.
	 *
	 */
	virtual void Render() const;

	/** Calcul de la visibilité de la source. La méthode crée d'abord une sphère englobante
	 * et teste ensuite la visibilité de celle-ci. */
	virtual void ComputeVisibility(const CCamera &view);

	/** Donne l'englobant de l'objet.
	 * @param max Retourne le coin supérieur de l'englobant.
	 * @param min Retourne le coin inférieur de l'englobant.
	 */
	virtual void GetBoundingBox(CPoint& a_rMax, CPoint& a_rMin) const {}

	virtual void Move(CVector const& a_rDir) {}

//---------------------------------------------------------------------------------------------------------------------
//  Virtual methods
//---------------------------------------------------------------------------------------------------------------------

	/** Ajuste le niveau de détail de la NURBS.
	 * @param value Valeur comprise entre 0 et LOD_VALUES.
	 */
	virtual void setLOD(u_char value);

	/** Fonction chargée de construire les flammes composant la source de feu. Elle se charge également
	 * de déterminer la position de la source de lumière.
	 */
	virtual void build();

	/** Dessine la mèche de la flamme. Les mèches des IRealFlame sont définies en (0,0,0), une translation
	 * est donc effectuée pour tenir compte du placement du feu dans le monde.
	 */
	virtual void drawWickBoxes() const
	{
		CTransform const& rTransform = m_rField.GetTransform();
		rTransform.Push();
		for (uint i = 0; i < m_nbFlames; i++)
			m_flames[i]->drawWickBoxes();
		rTransform.Pop();
	}


	/** Dessin de la sphère englobante. */
	virtual void drawBoundingSphere() const
	{
		if (m_visibility) m_boundingSphere.draw();
	}

	/** Dessin d'un imposteur pour le rendu. */
	virtual void drawImpostor() const;

	/** Fonction appelée par le solveur de fluides pour ajouter l'élévation
	 * thermique de la flamme.
	 */
	virtual void addForces ()
	{
		for (uint i = 0; i < m_nbFlames; i++)
			m_flames[i]->addForces(m_fdf, m_innerForce, m_perturbate);
	}

	/** Affectation du coefficient multiplicateur de la FDF.
	 * @param value Coefficient.
	 */
	virtual void setInnerForce(float value)
	{
		m_innerForce=value;
	}

	/** Affectation de la FDF.
	 * @param value FDF.
	 */
	virtual void setFDF(int value)
	{
		m_fdf=value;
	}

	/** Affectation de la méthode de perturbation.
	 * @param value Perturbation.
	 */
	virtual void setPerturbateMode(char value)
	{
		m_perturbate=value;
	}

	/** Affectation de la durée de vie des squelettes guides.
	 * @param value Durée de vie en itérations.
	 */
	virtual void setLeadLifeSpan(uint value)
	{
		for (uint i = 0; i < m_nbFlames; i++)
			m_flames[i]->setLeadLifeSpan(value);
	}

	/** Affectation de la durée de vie des squelettes périphériques.
	 * @param value Durée de vie en itérations.
	 */
	virtual void setPeriLifeSpan(uint value)
	{
		for (uint i = 0; i < m_nbFlames; i++)
			m_flames[i]->setPeriLifeSpan(value);
	}

	/** Active ou désactive l'affichage texturé sur la flamme. */
	virtual void setSmoothShading (bool state)
	{
		for (uint i = 0; i < m_nbFlames; i++)
			m_flames[i]->setSmoothShading(state);
	}

	/** Fonction permettant de récupérer le centre de la flamme dans le repère local. */
	virtual CPoint getCenter() const
	{
		return m_oCenter;
	};

	/** Fonction permettant de récupérer l'orientation principale de la flamme. */
	virtual CVector getMainDirection() const
	{
		return m_oDirection;
	}

	virtual bool operator<(const IFireSource& other) const;

//---------------------------------------------------------------------------------------------------------------------
//  Specific methods
//---------------------------------------------------------------------------------------------------------------------

	/** Modifie le type de flickering sur les flammes. */
	char getPerturbateMode() const
	{
		return m_perturbate;
	}

	/** Retourne la position absolue dans le repère du monde.
	 * @return Position absolue dans le repère du monde.
	 */
	CPoint getFieldPosition () const
	{
		CTransform const& rTransform = m_rField.GetTransform();
		return rTransform.GetLocalPosition();
	}

	/** Fonction recalculant le centre et la direction de la flamme, en vue de
	 * l'éclairage. Appelée à chaque pas de simulation. */
	void computeIntensityPositionAndDirection(void);

	/** Construction de la sphère englobante de l'objet. */
	void buildBoundingSphere ();

	/** Test si la flamme est visible ou non. Le calcul de visibilité proprement
	 * dit se fait à l'aide de la fonction computeVisibility. */
	bool isVisible()
	{
		return m_visibility;
	}

	/** Récupération de la distance par rapport à la caméra. */
	float getDistanceFromCamera()
	{
		return m_dist;
	};

	const GLfloat* getGlowWeights(uint index) const { return m_glowWeights[index]; }
	GLfloat getGlowDivide(uint index) const { return m_glowDivide[index]; }

	CFlameLight const& GetLight() const { assert(m_pLight != NULL); return *m_pLight; }
	CFlameLight& GrabLight() { assert(m_pLight != NULL); return *m_pLight; }

	void computeGlowWeights(uint index, float sigma);

	/** Opérateur de comparaison des flammes basées sur leur distance par rapport
	 * à la caméra. On est obligé de définir cet opérateur car on utilise des
	 * vecteurs de pointeurs pour stocker les flammes. */
	static bool cmp( const IFireSource* a, const IFireSource* b )
	{
		return a->m_dist < b->m_dist;
	}

protected:

	/** Pointeur vers la lumière */
	CFlameLight* m_pLight;

	/** Nombre de flammes */
	uint m_nbFlames;
	/** Tableau contenant les flammes */
	IRealFlame **m_flames;

	/** Pointeur sur le solveur de fluides */
	Field3D& m_rField;

	/** ITexture utilisée pour les flammes */
	CBitmapTexture m_oTexture;

	/** Sphère englobante utilisée pour vérifier la visibilité de la source. */
	CBoundingSphere m_boundingSphere;

	/** Visibilité de la flamme. */
	bool m_visibility;

	/** Distance par rapport à la caméra. */
	float m_dist;

	/** Dernière valeur du flickering connue avant que la gestion du LOD ne la modifie */
	char m_flickSave;

	/** Coefficient de force appliqué sur la FDF. */
	float m_innerForce;

	/** Méthode de perturbation appliquée sur la FDF. */
	char m_perturbate;

	/** Fonction de distribution de carburant. */
	int m_fdf;

	/** Centre de la flamme, recalculé à chaque itération dans build() */
	CPoint m_oCenter;

	/** Direction principale de la flamme, recalculée à chaque itération dans build() */
	CVector m_oDirection;

	/** Sauvegardes du niveau de détail précédent, permet de déterminer s'il y a un changement. */
	int m_fluidsLODSave, m_nurbsLODSave;

	/** Tableau contenant les poids des pixels du filtre du glow */
	GLfloat m_glowWeights[2][FILTER_SIZE];
	/** Diviseur correspondant à la somme des poids du glow */
	GLfloat m_glowDivide[2];

	float m_intensityCoef;
};

/** La classe Firesource ajoute la notion de flammes détachées.
 *
 * @author	Flavien Bridault
 */
class IDetachableFireSource : public IFireSource
{
public:
	/** Constructeur.
	 * @param flameConfig Configuration de la flamme.
	 * @param s CPointeur sur le solveur de fluides.
	 * @param filename Nom du fichier contenant le luminaire.
	 * @param nbFlames Nombre de flammes.
	 * @param scene CPointeur sur la scène.
	 * @param texname Nom du fichier image de la texture.
	 */
	IDetachableFireSource (	CTransform& a_rTransform,
							const FlameConfig& a_rFlameConfig,
							Field3D& a_rField,
							uint a_uiNbFlames,
							CharCPtrC a_szTexname,
							const CShader& a_rGenShadowCubeMapShader,
							const CRenderTarget& a_rShadowRenderTarget);
	virtual ~IDetachableFireSource();

	virtual void build();
	virtual void drawFlame(bool display, bool displayParticle, u_char boundingVolume=0) const;

	virtual void setLOD(u_char value);
//   virtual void drawImpostor () const;

	/** Ajoute une flamme détachée à la source.
	 * @param detachedFlame CPointeur sur la nouvelle flamme détachée à ajouter.
	 */
	virtual void addDetachedFlame(CDetachedFlame* detachedFlame)
	{
		m_detachedFlamesList.push_back(detachedFlame);
	}

	/** Supprime une flamme détachée à la source.
	 * @param detachedFlame CPointeur sur la flamme détachée à enlever.
	 */
	virtual void removeDetachedFlame(CDetachedFlame* detachedFlame)
	{
		m_detachedFlamesList.remove(detachedFlame);
	}

	/** Active ou désactive l'affichage texturé sur la flamme. */
	virtual void setSmoothShading (bool state);

	virtual void ComputeVisibility(const CCamera &view);

	void computeIntensityPositionAndDirection(void);
private:
	/** Construit la bounding box utilisée pour l'affichage */
	virtual void buildBoundingBox ();

	/** Liste des flammes détachées */
	list<CDetachedFlame *> m_detachedFlamesList;
	CPoint m_BBmin, m_BBmax;
};

#endif

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


/**********************************************************************************************************************/
/************************************ DEFINITION DE L'INTERFACE CFLAMELIGHT ************************************/
/**********************************************************************************************************************/
class CFlameLight : public COmniLight
{
public:
	CFlameLight(const CPoint &a_rPosition,
							const CEnergy &a_rEnergy,
							GLuint a_uiDepthMapSize,
							const CShader& a_rGenShadowCubeMapShader,
							const CRenderTarget& a_rShadowRenderTarget,
							const string& a_rIESFileName) :
		COmniLight(a_rPosition, a_rEnergy, a_uiDepthMapSize, a_rGenShadowCubeMapShader, a_rShadowRenderTarget)
	{
		m_pIesFile = new IES(a_rIESFileName.c_str());
		setEnergy(CEnergy(1.0,0.5,0.0));
	}

	virtual ~CFlameLight()
	{
		delete m_pIesFile;
	};

	/** Modifie le coefficient pondérateur de l'intensité. */
	virtual void SetIntensity(float coef)
	{
		m_lightEnergy *= coef;
	};

	const float getLazimut() const
	{
		return m_pIesFile->getLazimut();
	};

	const float getLzenith() const
	{
		return m_pIesFile->getLzenith();
	};

	const float getLazimutTEX() const
	{
		return m_pIesFile->getLazimutTEX();
	};

	const float getLzenithTEX() const
	{
		return m_pIesFile->getLzenithTEX();
	};

	/** Retourne le nombre de valeurs en zénithal. */
	const uint getIESZenithSize() const
	{
		return m_pIesFile->getNbzenith();
	};

	/** Retourne le nombre de valeurs en azimuthal. */
	const uint getIESAzimuthSize() const
	{
		return m_pIesFile->getNbazimut();
	};

	/** Supprime le fichier IES courant et en utilise un autre. */
	void useNewIESFile(const string& a_rIESFileName)
	{
		delete m_pIesFile;
		m_pIesFile = new IES(a_rIESFileName.c_str());
	};

private:
	/** Fichier IES utilisé pour le solide photométrique de la source. */
	IES *m_pIesFile;
};


/**********************************************************************************************************************/
/************************************* DEFINITION DE L'INTERFACE IFIRESOURCE ************************************/
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
class IFireSource : public CFlameLight
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
	IFireSource(	const FlameConfig& a_rFlameConfig,
							Field3D& a_rField,
							uint a_uiNbFlames,
							CharCPtrC a_szTexname,
							const CShader& a_rGenShadowCubeMapShader,
							const CRenderTarget& a_rShadowRenderTarget);

	/** Destructeur */
	virtual ~IFireSource ();

	/** Ajuste le niveau de détail de la NURBS.
	 * @param value Valeur comprise entre 0 et LOD_VALUES.
	 */
	virtual void setLOD(u_char value)
	{
		switch (value)
		{
			case 5:
				for (uint i = 0; i < m_nbFlames; i++)
				{
					m_flames[i]->setSamplingTolerance(4);
					m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
					m_flames[i]->setFlatFlame(false);
				}
				break;
			case 4:
				for (uint i = 0; i < m_nbFlames; i++)
				{
					m_flames[i]->setSamplingTolerance(3);
					m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
					m_flames[i]->setFlatFlame(false);
				}
				break;
			case 3:
				for (uint i = 0; i < m_nbFlames; i++)
				{
					m_flames[i]->setSamplingTolerance(2);
					m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
					m_flames[i]->setFlatFlame(false);
				}
				break;
			case 2:
				for (uint i = 0; i < m_nbFlames; i++)
				{
					m_flames[i]->setSamplingTolerance(2);
					m_flames[i]->setSkeletonsLOD(HALF_SKELETON);
					m_flames[i]->setFlatFlame(false);
				}
				break;
			case 1:
				for (uint i = 0; i < m_nbFlames; i++)
				{
					m_flames[i]->setSamplingTolerance(1);
					m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
					m_flames[i]->setFlatFlame(true);
				}
				break;
			case 0:
				for (uint i = 0; i < m_nbFlames; i++)
				{
					m_flames[i]->setSamplingTolerance(1);
					m_flames[i]->setSkeletonsLOD(HALF_SKELETON);
					m_flames[i]->setFlatFlame(true);
				}
				break;
			default:
				cerr << "Bad NURBS LOD parameter" << endl;
		}
	};

	/** Retourne la position absolue dans le repère du monde.
	 * @return Position absolue dans le repère du monde.
	 */
	CPoint const& getPosition () const
	{
		return m_rField.getPosition();
	}

	/** Fonction chargée de construire les flammes composant la source de feu. Elle se charge également
	 * de déterminer la position de la source de lumière.
	 */
	virtual void build();

	/** Dessine la mèche de la flamme. Les mèches des IRealFlame sont définies en (0,0,0), une translation
	 * est donc effectuée pour tenir compte du placement du feu dans le monde.
	 */
	virtual void drawWick(bool displayBoxes) const
	{
		CPoint const& rPt = getPosition();
		CPoint const& rScale = m_rField.getScale();
		glPushMatrix();
		glTranslatef (rPt.x,rPt.y, rPt.z);
		glScalef (rScale.x, rScale.y, rScale.z);
		for (uint i = 0; i < m_nbFlames; i++)
			m_flames[i]->drawWick(displayBoxes);
		glPopMatrix();
	}

	/** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
	 * build() avec le placage de texture. La flamme d'une IRealFlame est définie dans le repère du solveur,
	 * donc seule une translation correspondant à la position du solveur est effectuée.
	 *
	 * @param display affiche ou non la flamme
	 * @param displayParticle affiche ou non les particules des squelettes
	 * @param boundingVolume Optionel, volume englobant à afficher: 0 aucun, 1 sphère, 2 boîte
	 */
	virtual void drawFlame(bool display, bool displayParticle, u_char boundingVolume=0) const
	{
#ifdef COUNT_NURBS_POLYGONS
		g_count=0;
#endif
		switch (boundingVolume)
		{
			case BOUNDING_SPHERE :
				drawBoundingSphere();
				break;
			case IMPOSTOR :
				drawImpostor();
				break;
			default :
				if (m_visibility)
				{
					CPoint const& rPt = getPosition();
					CPoint const& rScale = m_rField.getScale();
					glPushMatrix();
					glTranslatef (rPt.x,rPt.y, rPt.z);
					glScalef (rScale.x, rScale.y, rScale.z);
					for (uint i = 0; i < m_nbFlames; i++)
						m_flames[i]->drawFlame(display, displayParticle);
					glPopMatrix();
					// Trace une sphère indiquant le centre
// 	  	    glPushMatrix();
// 	  	    glTranslatef (m_centreSP.x, m_centreSP.y, m_centreSP.z);
// 	  	    CUGraphicsFn::SolidSphere (.05, 10, 10);
// 	  	    glPopMatrix();
				}
				break;
		}
#ifdef COUNT_NURBS_POLYGONS
		cerr << g_count << endl;
#endif
	}

	/** Dessin de la sphère englobante. */
	virtual void drawBoundingSphere() const
	{
		if (m_visibility) m_boundingSphere.draw();
	};

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
	};

	/** Affectation de la FDF.
	 * @param value FDF.
	 */
	virtual void setFDF(int value)
	{
		m_fdf=value;
	};

	/** Affectation de la méthode de perturbation.
	 * @param value Perturbation.
	 */
	virtual void setPerturbateMode(char value)
	{
		m_perturbate=value;
	};

	/** Modifie le type de flickering sur les flammes. */
	char getPerturbateMode() const
	{
		return m_perturbate;
	};

	/** Affectation de la durée de vie des squelettes guides.
	 * @param value Durée de vie en itérations.
	 */
	virtual void setLeadLifeSpan(uint value)
	{
		for (uint i = 0; i < m_nbFlames; i++)
			m_flames[i]->setLeadLifeSpan(value);
	};

	/** Affectation de la durée de vie des squelettes périphériques.
	 * @param value Durée de vie en itérations.
	 */
	virtual void setPeriLifeSpan(uint value)
	{
		for (uint i = 0; i < m_nbFlames; i++)
			m_flames[i]->setPeriLifeSpan(value);
	};

	/** Active ou désactive l'affichage texturé sur la flamme. */
	virtual void setSmoothShading (bool state)
	{
		for (uint i = 0; i < m_nbFlames; i++)
			m_flames[i]->setSmoothShading(state);
	}

	/** Fonction permettant de récupérer le centre de la flamme dans le repère local. */
	virtual CPoint getCenter() const
	{
		return m_center;
	};

	/** Fonction permettant de récupérer l'orientation principale de la flamme. */
	virtual CVector getMainDirection() const
	{
		return m_direction;
	};

	/** Fonction recalculant le centre et la direction de la flamme, en vue de
	 * l'éclairage. Appelée à chaque pas de simulation. */
	void computeIntensityPositionAndDirection(void);

	/** Construction de la sphère englobante de l'objet. */
	void buildBoundingSphere ();

	/** Calcul de la visibilité de la source. La méthode crée d'abord une sphère englobante
	 * et teste ensuite la visibilité de celle-ci. */
	virtual void computeVisibility(const CCamera &view, bool forceSpheresBuild=false);

	/** Test si la flamme est visible ou non. Le calcul de visibilité proprement
	 * dit se fait à l'aide de la fonction computeVisibility. */
	bool isVisible()
	{
		return m_visibility;
	};

	/** Récupération de la distance par rapport à la caméra. */
	float getDistanceFromCamera()
	{
		return m_dist;
	};

	const GLfloat* getGlowWeights(uint index) const
	{
		return m_glowWeights[index];
	};
	GLfloat getGlowDivide(uint index) const
	{
		return m_glowDivide[index];
	};

	void computeGlowWeights(uint index, float sigma);

	virtual bool operator<(const IFireSource& other) const;

	/** Opérateur de comparaison des flammes basées sur leur distance par rapport
	 * à la caméra. On est obligé de définir cet opérateur car on utilise des
	 * vecteurs de pointeurs pour stocker les flammes. */
	static bool cmp( const IFireSource* a, const IFireSource* b )
	{
		return a->m_dist < b->m_dist;
	}

protected:

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
	CPoint m_center;

	/** Direction principale de la flamme, recalculée à chaque itération dans build() */
	CVector m_direction;

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
	IDetachableFireSource (const FlameConfig& a_rFlameConfig,
												Field3D& a_rField,
												uint a_uiNbFlames,
												CharCPtrC a_szTexname,
												const CShader& a_rGenShadowCubeMapShader,
												const CRenderTarget& a_rShadowRenderTarget);
	virtual ~IDetachableFireSource();

	virtual void build();
	virtual void drawFlame(bool display, bool displayParticle, u_char boundingVolume=0) const;

	virtual void setLOD(u_char value)
	{
		u_char tolerance=4;
		switch (value)
		{
			case 5:
				for (uint i = 0; i < m_nbFlames; i++)
				{
					m_flames[i]->setSamplingTolerance(4);
					m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
					m_flames[i]->setFlatFlame(false);
					tolerance = 4;
				}
				break;
			case 4:
				for (uint i = 0; i < m_nbFlames; i++)
				{
					m_flames[i]->setSamplingTolerance(3);
					m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
					m_flames[i]->setFlatFlame(false);
					tolerance = 3;
				}
				break;
			case 3:
				for (uint i = 0; i < m_nbFlames; i++)
				{
					m_flames[i]->setSamplingTolerance(2);
					m_flames[i]->setSkeletonsLOD(HALF_SKELETON);
					m_flames[i]->setFlatFlame(false);
				}
				tolerance = 2;
				break;
			case 2:
				for (uint i = 0; i < m_nbFlames; i++)
				{
					m_flames[i]->setSamplingTolerance(1);
					m_flames[i]->setSkeletonsLOD(HALF_SKELETON);
					m_flames[i]->setFlatFlame(false);
				}
				tolerance = 1;
				break;
			case 1:
			case 0:
				for (uint i = 0; i < m_nbFlames; i++)
				{
					m_flames[i]->setSamplingTolerance(1);
					m_flames[i]->setSkeletonsLOD(HALF_SKELETON);
					m_flames[i]->setFlatFlame(true);
				}
				tolerance = 1;
				break;
			default:
				cerr << "Bad NURBS LOD parameter" << endl;
		}
		for (list < CDetachedFlame* >::const_iterator flamesIterator = m_detachedFlamesList.begin ();
		        flamesIterator != m_detachedFlamesList.end();  flamesIterator++)
			(*flamesIterator)->setSamplingTolerance(tolerance);
	};
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

	virtual void computeVisibility(const CCamera &view, bool forceSpheresBuild=false);

	void computeIntensityPositionAndDirection(void);
private:
	/** Construit la bounding box utilisée pour l'affichage */
	virtual void buildBoundingBox ();

	/** Liste des flammes détachées */
	list<CDetachedFlame *> m_detachedFlamesList;
	CPoint m_BBmin, m_BBmax;
};

#endif

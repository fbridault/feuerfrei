#if !defined(FIRESOURCES_H)
#define FIRESOURCES_H

#include <engine/Utility/UObjImporter.hpp>

#include "abstractFires.hpp"
#include "cloneFlames.hpp"

class CPointFlame;
class CLineFlame;
class IFireSource;

/** La classe Candle permet la définition d'une bougie.
 *
 * @author	Flavien Bridault
 */
class Candle : public IFireSource
{
public:
	/** Constructeur d'une bougie.
	 * @param flameConfig Configuration de la flamme.
	 * @param s CPointeur sur le solveur de fluides.
	 * @param scene CPointeur sur la scène.
	 * @param rayon Rayon de la flamme.
	 * @param wickFileName nom du fichier contenant la mèche. Si NULL, alors wick doit être fourni.
	 * @param wick Optionnel, objet représentant la mèche. Si NULL, un cylindre simple est utilisé.
	 */
	Candle(	const FlameConfig& a_rFlameConfig,
					Field3D * a_pField,
					CScene& a_rScene,
					float a_fRayon,
					CharCPtrC a_szWickFileName,
					const CShader& a_rGenShadowCubeMapShader,
					const CRenderTarget& a_rShadowRenderTarget,
					CWick *a_pWick=NULL);
	/** Destructeur */
	virtual ~Candle(){};
};


/** La classe Firmalampe permet la définition d'une firmalampe.
 *
 * @author	Flavien Bridault
 */
class Firmalampe : public IFireSource
{
public:
	/** Constructeur d'une torche.
	 * @param flameConfig Configuration de la flamme.
	 * @param s CPointeur sur le solveur de fluides.
	 * @param scene CPointeur sur la scène.
	 * @param wickFileName nom du fichier contenant la mèche
	 */
	Firmalampe(const FlameConfig& a_rFlameConfig,
							Field3D * a_pField,
							CScene& a_rScene,
							CharCPtrC a_szWickFileName,
							const CShader& a_rGenShadowCubeMapShader,
							const CRenderTarget& a_rShadowRenderTarget);
	/** Destructeur */
	virtual ~Firmalampe(){};
};

template <bool t_bDrawWick>
class CTFire : public IDetachableFireSource
{
public:
	/** Constructeur d'une torche.
	 * @param flameConfig Configuration de la flamme.
	 * @param s CPointeur sur le solveur de fluides.
	 * @param scene CPointeur sur la scène.
	 * @param torchName nom du fichier contenant le luminaire.
	 */
	CTFire(	const FlameConfig& a_rFlameConfig,
					Field3D * a_pField,
					CScene& a_rScene,
					CharCPtrC a_szTorchName,
					CharCPtrC a_rTextureName,
					const CShader& a_rGenShadowCubeMapShader,
					const CRenderTarget& a_rShadowRenderTarget,
					float a_fWidth,
					float a_fDetachedWidth) :
			IDetachableFireSource (a_rFlameConfig, a_pField, 0, a_rTextureName, a_rGenShadowCubeMapShader, a_rShadowRenderTarget)
	{
		vector<CWick *> objList;

		bool bStatus = UObjImporter::import(a_rScene, a_szTorchName, objList, WICK_NAME_PREFIX);
		assert(bStatus == true);

		m_nbFlames = objList.size();
		m_flames = new IRealFlame* [m_nbFlames];

		CPoint rPosition = GrabPosition();
		/* Calcul de la position et recentrage des mèches */
		for (vector <CWick *>::iterator objListIterator = objList.begin ();
		        objListIterator != objList.end (); objListIterator++)
		{
			(*objListIterator)->buildBoundingBox();
			rPosition += (*objListIterator)->getCenter();
		}
		rPosition = CPoint(0.5f,0.0f,0.5f) - rPosition/m_nbFlames;
		for (vector <CWick *>::iterator objListIterator = objList.begin ();
		        objListIterator != objList.end (); objListIterator++)
			(*objListIterator)->translate(rPosition);

		int i=0;
		for (vector <CWick *>::iterator objListIterator = objList.begin ();
		        objListIterator != objList.end (); objListIterator++, i++)
			m_flames[i] = new CLineFlame( a_rFlameConfig, m_oTexture, a_pField, (*objListIterator), a_fWidth, a_fDetachedWidth, this);
	}

	/** Destructeur */
	virtual ~CTFire(){};

	/** Dessine la mèche de la flamme. Les mèches des IRealFlame sont définies en (0,0,0), une translation
	 * est donc effectuée pour tenir compte du placement du feu dans le monde.
	 */
	virtual void drawWick(bool displayBoxes) const
	{
		if (t_bDrawWick)
		{
			IDetachableFireSource::drawWick(displayBoxes);
		}
	}
};


/** La classe Torche permet la définition d'une flamme de type torche.
 * Le fichier OBJ représentant le luminaire contient des mèches qui doivent avoir un nom
 * en CWick*. Celle-ci ne sont pas affichées à l'écran. Les objets composant le luminaire
 * doivent s'appeler Torch.*
 *
 * @author	Flavien Bridault
 */
typedef CTFire<true> CTorch;

/** La classe CampFire permet la définition d'un feu de camp.
 * Le fichier OBJ représentant le luminaire contient des mèches qui doivent avoir un nom
 * en CWick*. A la différence de la classe Torch, les mèches sont affichées. En revanche,
 * il n'existe pas de luminaire.
 *
 * @author	Flavien Bridault
 */
typedef CTFire<false> CCampFire;


/** La classe CandleStick permet la définition d'un chandelier. Elle est composée de flammes
 * clones.
 *
 * @author	Flavien Bridault
 */
class CandleStick : public IFireSource
{
public:
	/** Constructeur d'un chandelier.
	 * @param flameConfig Configuration de la flamme.
	 * @param s CPointeur sur le solveur de fluides.
	 * @param scene CPointeur sur la scène.
	 * @param filename Nom du fichier contenant le luminaire.
	 * @param rayon Rayon de la flamme.
	 */
	CandleStick(const FlameConfig& a_rFlameConfig,
							Field3D * a_rField,
							CScene& a_rScene,
							CharCPtrC a_szFilename,
							float a_fRayon,
							const CShader& a_rGenShadowCubeMapShader,
							const CRenderTarget& a_rShadowRenderTarget);

	/** Destructeur */
	virtual ~CandleStick();

	virtual void build();

	virtual void drawWick(bool displayBoxes) const
	{
		CPoint const& rPos = getPosition();
		CPoint const& rScale = m_solver->getScale();
		glPushMatrix();
		glTranslatef (rPos.x,rPos.y,rPos.z);
		glScalef (rScale.x, rScale.y, rScale.z);
		for (uint i = 0; i < m_nbFlames; i++)
			m_flames[i]->drawWick(displayBoxes);
		for (uint i = 0; i < m_nbCloneFlames; i++)
			m_cloneFlames[i]->drawWick(displayBoxes);
		glPopMatrix();
	}

	virtual void drawFlame(bool display, bool displayParticle, u_char boundingVolume=0) const
	{
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
					CPoint const& rPos = getPosition();
					CPoint const& rScale = m_solver->getScale();
					glPushMatrix();
					glTranslatef (rPos.x,rPos.y,rPos.z);
					glScalef (rScale.x, rScale.y, rScale.z);
					for (uint i = 0; i < m_nbFlames; i++)
						m_flames[i]->drawFlame(display, displayParticle);
					for (uint i = 0; i < m_nbCloneFlames; i++)
						m_cloneFlames[i]->drawFlame(display, displayParticle);
					glPopMatrix();
				}
		}
	}

	virtual void toggleSmoothShading (bool state);
//   virtual void setSamplingTolerance(u_char value){
//     IFireSource::setSamplingTolerance(value);
//     for (uint i = 0; i < m_nbCloneFlames; i++)
//       m_cloneFlames[i]->setSamplingTolerance(value);
//   };

private:
	/** Nombre de flammes clones */
	uint m_nbCloneFlames;
	/** Tableau contenant les flammes clones */
	CloneCPointFlame **m_cloneFlames;
};


#endif

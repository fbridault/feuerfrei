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
			Field3D& a_rField,
			CTransform &a_rTransform,
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
	Firmalampe(	const FlameConfig& a_rFlameConfig,
				Field3D& a_rField,
				CTransform &a_rTransform,
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
			Field3D& a_rField,
			CTransform &a_rTransform,
			CScene& a_rScene,
			CharCPtrC a_szTorchName,
			CharCPtrC a_rTextureName,
			const CShader& a_rGenShadowCubeMapShader,
			const CRenderTarget& a_rShadowRenderTarget,
			float a_fWidth,
			float a_fDetachedWidth) :
		IDetachableFireSource (	a_rTransform, a_rFlameConfig, a_rField, 0, a_rTextureName,
								a_rGenShadowCubeMapShader, a_rShadowRenderTarget)
	{
		CTransform& rLuminaryTransform = a_rTransform.GrabParent().GrabParent();
		bool bStatus = UObjImporter::import<CWick>(a_rScene, a_szTorchName, NULL, &rLuminaryTransform, WICK_NAME_PREFIX);
		assert(bStatus == true);

		m_nbFlames = rLuminaryTransform.GetNumObjects();
		assert(m_nbFlames > 0);
		m_flames = new IRealFlame* [m_nbFlames];

		CPoint oPosition = a_rTransform.GetLocalPosition();

		// Grab list of imported wicks
		CTransform::CObjectsList& rlWicks = rLuminaryTransform.GrabObjects();

		// Compute position
		ForEachIter(itObject, CTransform::CObjectsList, rlWicks)
		{
			// We know that we have only wicks
			CWick* pWick = dynamic_cast<CWick *>(*itObject);
			oPosition += pWick->GetCenter();
		}

		// Center wicks
		oPosition = CPoint(0.5f,0.0f,0.5f) - oPosition/m_nbFlames;
		ForEachIter(itObject, CTransform::CObjectsList, rlWicks)
		{
			// We know that we have only wicks
			CWick* pWick = dynamic_cast<CWick *>(*itObject);
			pWick->HardTranslate(oPosition);
		}

		// Set new position in the transform
		a_rTransform.SetPosition(oPosition);

		int i=0;
		ForEachIterC(itObject, CTransform::CObjectsList, rlWicks)
		{
			m_flames[i++] = new CLineFlame( a_rFlameConfig, m_oTexture, a_rField, (CWick *)(*itObject),
											a_fWidth, a_fDetachedWidth, this);
		}
	}

	/** Destructeur */
	virtual ~CTFire(){};

	/** Dessine la mèche de la flamme. Les mèches des IRealFlame sont définies en (0,0,0), une translation
	 * est donc effectuée pour tenir compte du placement du feu dans le monde.
	 */
	virtual void drawWickBoxes() const
	{
		if (t_bDrawWick)
		{
			IDetachableFireSource::drawWickBoxes();
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
				Field3D& a_rField,
				CTransform &a_rTransform,
				CScene& a_rScene,
				CharCPtrC a_szFilename,
				float a_fRayon,
				const CShader& a_rGenShadowCubeMapShader,
				const CRenderTarget& a_rShadowRenderTarget);

	/** Destructeur */
	virtual ~CandleStick();

	virtual void build();

	virtual void drawWickBoxes(bool displayBoxes) const
	{
		CTransform const& rTransform = m_rField.GetTransform();
		rTransform.Push();
		for (uint i = 0; i < m_nbFlames; i++)
			m_flames[i]->drawWickBoxes();
		for (uint i = 0; i < m_nbCloneFlames; i++)
			m_cloneFlames[i]->drawWickBoxes();
		rTransform.Pop();
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
					CTransform const& rTransform = m_rField.GetTransform();
					rTransform.Push();
					for (uint i = 0; i < m_nbFlames; i++)
						m_flames[i]->drawFlame(display, displayParticle);
					for (uint i = 0; i < m_nbCloneFlames; i++)
						m_cloneFlames[i]->drawFlame(display, displayParticle);
					rTransform.Pop();
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

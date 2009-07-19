#include "realFires.hpp"

#include <engine/Utility/GraphicsFn.hpp>
#include <engine/Scene/CTransform.hpp>

#include "../solvers/fakeField3D.hpp"

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
Candle::Candle (const FlameConfig& a_rFlameConfig,
				Field3D& a_rField,
				CTransform &a_rTransform,
				CScene& a_rScene,
				float a_fRayon,
				CharCPtrC a_szWickFileName,
				const CShader& a_rGenShadowCubeMapShader,
				const CRenderTarget& a_rShadowRenderTarget,
				CWick *a_pWick):
	IFireSource (	a_rTransform,
					a_rFlameConfig,
					a_rField,
					1,
					("textures/bougie2.png"),
					a_rGenShadowCubeMapShader,
					a_rShadowRenderTarget)
{
	if (a_szWickFileName != NULL)
	{
		CTransform& rLuminaryTransform = a_rTransform.GrabParent().GrabParent();
		bool bStatus = UObjImporter::import<CWick>(a_rScene, string(a_szWickFileName), NULL, &rLuminaryTransform, WICK_NAME_PREFIX);
		assert(bStatus == true);

		CWick* pWick = dynamic_cast<CWick *>(*rLuminaryTransform.GrabObjects().begin());
		assert (pWick != NULL);

		/* Recentrage de la mèche */
		CPoint oPosition = CPoint(0.5f,0.0f,0.5f) - pWick->GetCenter();
		pWick->HardTranslate(oPosition);
		m_flames[0] = new CPointFlame(a_rFlameConfig, m_oTexture, a_rField, a_fRayon, pWick);

		// Set new position in the transform
		a_rTransform.SetPosition(oPosition);
	}
	else
	{
		assert (a_pWick != NULL);

		/* Recentrage de la mèche */
		CPoint oPosition = CPoint(0.5f,0.0f,0.5f) - a_pWick->GetCenter();
		a_pWick->HardTranslate(oPosition);
		m_flames[0] = new CPointFlame(a_rFlameConfig, m_oTexture, a_rField, a_fRayon, a_pWick);

		// Set new position in the transform
		a_rTransform.SetPosition(oPosition);
	}
}


//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
Firmalampe::Firmalampe(	const FlameConfig& a_rFlameConfig,
						Field3D& a_rField,
						CTransform &a_rTransform,
						CScene& a_rScene,
						CharCPtrC a_szWickFileName,
						const CShader& a_rGenShadowCubeMapShader,
						const CRenderTarget& a_rShadowRenderTarget) :
	IFireSource (	a_rTransform,
					a_rFlameConfig,
					a_rField,
					1,
					("textures/firmalampe.png"),
					a_rGenShadowCubeMapShader,
					a_rShadowRenderTarget)
{
	vector<CWick *> objList;

	assert(a_szWickFileName != NULL);

	CTransform& rLuminaryTransform = a_rTransform.GrabParent().GrabParent();
	bool bStatus = UObjImporter::import<CWick>(a_rScene, string(a_szWickFileName), NULL, &rLuminaryTransform, WICK_NAME_PREFIX);

	assert(bStatus);
	CWick* pWick = (CWick *)(*rLuminaryTransform.GrabObjects().begin());

	/* Recentrage de la mèche */
	CPoint oPosition = CPoint(0.5f,0.0f,0.5f) - pWick->GetCenter();
	pWick->HardTranslate(oPosition);

	// Set new position in the transform
	a_rTransform.SetPosition(oPosition);

	m_flames[0] = new CLineFlame( a_rFlameConfig, m_oTexture, a_rField, pWick, 0.03f, 0.01f);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CandleStick::CandleStick (	const FlameConfig& a_rFlameConfig,
							Field3D& a_rField,
							CTransform &a_rTransform,
							CScene& a_rScene,
							CharCPtrC a_szFilename,
							float a_fRayon,
							const CShader& a_rGenShadowCubeMapShader,
							const CRenderTarget& a_rShadowRenderTarget):
		IFireSource (	a_rTransform,
						a_rFlameConfig,
						a_rField,
						1,
						("textures/bougie2.png"),
						a_rGenShadowCubeMapShader,
						a_rShadowRenderTarget)
{
	assert(false);
/*	vector<CWick *> objList;

	CTransform *pTransform = new CTransform();
	a_rGraph.addTransform(pTransform);
	UObjImporter::import(a_rScene, string(a_szFilename), objList, a_rGraph, pTransform, WICK_NAME_PREFIX);

	if (objList.size() > 0)
		m_flames[0] = new CPointFlame(a_rFlameConfig, m_oTexture, a_rField, a_fRayon, (*objList.begin()));

	m_nbCloneFlames = 20;
	m_cloneFlames = new CloneCPointFlame* [m_nbCloneFlames];

	m_cloneFlames[0]  = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(.5f,0.0f,0.0f));
	m_cloneFlames[1]  = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(-.5f,0.0f,0.0f));
	m_cloneFlames[2]  = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(0.1f,0.0f,0.5f));
	m_cloneFlames[3]  = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(0.0f,0.0f,-0.5f));
	m_cloneFlames[4]  = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(.5f,0.0f,0.5f));
	m_cloneFlames[5]  = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(-.5f,0.0f,0.4f));
	m_cloneFlames[6]  = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(0.5f,0.0f,-0.5f));
	m_cloneFlames[7]  = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(-0.3f,0.0f,-0.5f));
	m_cloneFlames[8]  = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(1.1f,0.0f,0.0f));
	m_cloneFlames[9]  = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(-1.0f,0.0f,0.0f));
	m_cloneFlames[10] = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(0.1f,0.0f,1.1f));
	m_cloneFlames[11] = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(-0.14f,0.0f,-1.0f));
	m_cloneFlames[12] = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(1.0f,0.0f,1.0f));
	m_cloneFlames[13] = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(-.9f,0.0f,1.3f));
	m_cloneFlames[14] = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(1.0f,0.0f,-1.0f));
	m_cloneFlames[15] = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(-1.0f,0.0f,-1.0f));
	m_cloneFlames[16] = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(1.33f,0.0f,0.0f));
	m_cloneFlames[17] = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(-1.5f,0.0f,0.0f));
	m_cloneFlames[18] = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(0.0f,0.0f,1.4f));
	m_cloneFlames[19] = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(0.0f,0.0f,-1.5f));*/
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CandleStick::~CandleStick()
{
	for (uint i = 0; i < m_nbCloneFlames; i++)
		delete m_cloneFlames[i];
	delete[]m_cloneFlames;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CandleStick::build()
{
//	CPoint averagePos, tmp;
//
//	for (uint i = 0; i < m_nbFlames; i++)
//	{
//		averagePos +=  m_flames[i]->getCenter ();
//		m_flames[i]->build();
//	}
//
//	for (uint i = 0; i < m_nbCloneFlames; i++)
//	{
//		averagePos += m_cloneFlames[i]->getCenter ();
//		m_cloneFlames[i]->build();
//	}
//
//	CTransform &rTransform = GrabTransform();
//
//	averagePos = averagePos/(m_nbFlames+m_nbCloneFlames);
//	averagePos += rTransform.GetLocalPosition();
//	rTransform.SetPosition(averagePos);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CandleStick::toggleSmoothShading(bool state)
{
	IFireSource::setSmoothShading(state);
	for (uint i = 0; i < m_nbCloneFlames; i++)
		m_cloneFlames[i]->setSmoothShading(state);
}

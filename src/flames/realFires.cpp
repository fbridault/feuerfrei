#include "realFires.hpp"

#include <engine/Utility/GraphicsFn.hpp>

#include "../solvers/fakeField3D.hpp"

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
Candle::Candle (const FlameConfig& a_rFlameConfig,
				Field3D& a_rField,
				CScene& a_rScene,
				float a_fRayon,
				CharCPtrC a_szWickFileName,
				const CShader& a_rGenShadowCubeMapShader,
				const CRenderTarget& a_rShadowRenderTarget,
				CWick *a_pWick):
	IFireSource (	a_rFlameConfig,
					a_rField,
					1,
					("textures/bougie2.png"),
					a_rGenShadowCubeMapShader,
					a_rShadowRenderTarget)
{
	if (a_szWickFileName != NULL)
	{
		vector<CWick *> objList;

		UObjImporter::import(a_rScene, string(a_szWickFileName), objList, WICK_NAME_PREFIX);

		assert (objList.size() > 0);

		CWick* pWick = *objList.begin();
		/* Calcul de la position et recentrage de la mèche */
		pWick->buildBoundingBox();

		CPoint& rPosition = GrabPosition();
		rPosition = CPoint(0.5f,0.0f,0.5f) - pWick->getCenter();
		pWick->HardTranslate(rPosition);
		m_flames[0] = new CPointFlame(a_rFlameConfig, m_oTexture, a_rField, a_fRayon, pWick);
	}
	else
	{
		assert (a_pWick != NULL);

		/* Calcul de la position et recentrage de la mèche */
		a_pWick->buildBoundingBox();

		CPoint& rPosition = GrabPosition();
		rPosition = CPoint(0.5f,0.0f,0.5f) - a_pWick->getCenter();
		a_pWick->HardTranslate(rPosition);
		m_flames[0] = new CPointFlame(a_rFlameConfig, m_oTexture, a_rField, a_fRayon, a_pWick);
	}
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
Firmalampe::Firmalampe(	const FlameConfig& a_rFlameConfig,
						Field3D& a_rField,
						CScene& a_rScene,
						CharCPtrC a_szWickFileName,
						const CShader& a_rGenShadowCubeMapShader,
						const CRenderTarget& a_rShadowRenderTarget) :
	IFireSource (	a_rFlameConfig,
					a_rField,
					1,
					("textures/firmalampe.png"),
					a_rGenShadowCubeMapShader,
					a_rShadowRenderTarget)
{
	vector<CWick *> objList;

	assert(a_szWickFileName != NULL);
	UObjImporter::import(a_rScene, string(a_szWickFileName), objList, WICK_NAME_PREFIX);

	assert(objList.size() > 0);

	CPoint& rPosition = GrabPosition();
	/* Calcul de la position et recentrage de la mèche */
	(*objList.begin())->buildBoundingBox();
	rPosition = CPoint(0.5f,0.0f,0.5f) - (*objList.begin())->getCenter();
	(*objList.begin())->HardTranslate(rPosition);

	m_flames[0] = new CLineFlame( a_rFlameConfig, m_oTexture, a_rField, (*objList.begin()), 0.03f, 0.01f);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
CandleStick::CandleStick (	const FlameConfig& a_rFlameConfig,
													Field3D& a_rField,
													CScene& a_rScene,
													CharCPtrC a_szFilename,
													float a_fRayon,
													const CShader& a_rGenShadowCubeMapShader,
													const CRenderTarget& a_rShadowRenderTarget):
		IFireSource (a_rFlameConfig,
								a_rField,
								1,
								("textures/bougie2.png"),
								a_rGenShadowCubeMapShader,
								a_rShadowRenderTarget)
{
	vector<CWick *> objList;

	UObjImporter::import(a_rScene, string(a_szFilename), objList, WICK_NAME_PREFIX);

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
	m_cloneFlames[12]  = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(1.0f,0.0f,1.0f));
	m_cloneFlames[13]  = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(-.9f,0.0f,1.3f));
	m_cloneFlames[14] = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(1.0f,0.0f,-1.0f));
	m_cloneFlames[15] = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(-1.0f,0.0f,-1.0f));
	m_cloneFlames[16]  = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(1.33f,0.0f,0.0f));
	m_cloneFlames[17]  = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(-1.5f,0.0f,0.0f));
	m_cloneFlames[18] = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(0.0f,0.0f,1.4f));
	m_cloneFlames[19] = new CloneCPointFlame(a_rFlameConfig, (CPointFlame *) m_flames[0], CPoint(0.0f,0.0f,-1.5f));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
CandleStick::~CandleStick()
{
	for (uint i = 0; i < m_nbCloneFlames; i++)
		delete m_cloneFlames[i];
	delete[]m_cloneFlames;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CandleStick::build()
{
	CPoint averagePos, tmp;

	for (uint i = 0; i < m_nbFlames; i++)
	{
		averagePos +=  m_flames[i]->getCenter ();
		m_flames[i]->build();
	}

	for (uint i = 0; i < m_nbCloneFlames; i++)
	{
		averagePos += m_cloneFlames[i]->getCenter ();
		m_cloneFlames[i]->build();
	}

	averagePos = averagePos/(m_nbFlames+m_nbCloneFlames);
	averagePos += getPosition();
	SetPosition(averagePos);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CandleStick::toggleSmoothShading(bool state)
{
	IFireSource::setSmoothShading(state);
	for (uint i = 0; i < m_nbCloneFlames; i++)
		m_cloneFlames[i]->setSmoothShading(state);
}

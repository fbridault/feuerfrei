#include "realFires.hpp"

#include <engine/Utility/GraphicsFn.hpp>
#include <engine/Utility/UObjImporter.hpp>

#include "../solvers/fakeField3D.hpp"

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
Candle::Candle (const FlameConfig& flameConfig, Field3D * s, CScene* const scene, float rayon,
								const char *wickFileName, CWick *wick):
		FireSource (flameConfig, s, 1, scene, _("textures/bougie2.png"))
{
	if (wickFileName)
	{
		vector<CWick *> objList;

		UObjImporter::import(scene, string(wickFileName), objList, WICK_NAME_PREFIX);

		assert (objList.size() > 0);

		CWick* pWick = *objList.begin();
		/* Calcul de la position et recentrage de la mèche */
		pWick->buildBoundingBox();
		m_position = CPoint(0.5f,0.0f,0.5f) - pWick->getCenter();
		pWick->translate(m_position);
		m_flames[0] = new CPointFlame(flameConfig, &m_texture, s, rayon, pWick);
	}
	else
	{
		assert (wick != NULL);

		/* Calcul de la position et recentrage de la mèche */
		wick->buildBoundingBox();
		m_position = CPoint(0.5f,0.0f,0.5f) - wick->getCenter();
		wick->translate(m_position);
		m_flames[0] = new CPointFlame(flameConfig, &m_texture, s, rayon, wick);
	}
	m_nbLights = 7;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void Candle::setLightPosition (const CPoint& pos)
{
	m_lightPosition[0] = pos.x;
	m_lightPosition[1] = pos.y;
	m_lightPosition[2] = pos.z;
	m_centreSP = pos;
	((CPointFlame *)m_flames[0])->getLightPositions(m_lightPositions, m_nbLights);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
Firmalampe::Firmalampe(const FlameConfig& flameConfig, Field3D * s, CScene *scene, const char *wickFileName):
		FireSource (flameConfig, s, 1, scene, _("textures/firmalampe.png"))
{
	vector<CWick *> objList;

	UObjImporter::import(scene, string(wickFileName), objList, WICK_NAME_PREFIX);

	assert(objList.size() > 0);

	/* Calcul de la position et recentrage de la mèche */
	(*objList.begin())->buildBoundingBox();
	m_position = CPoint(0.5f,0.0f,0.5f) - (*objList.begin())->getCenter();
	(*objList.begin())->translate(m_position);

	m_flames[0] = new CLineFlame( flameConfig, &m_texture, s, (*objList.begin()), 0.03f, 0.01f);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
Torch::Torch(const FlameConfig& flameConfig, Field3D * s, CScene *scene, const char *torchName) :
		DetachableFireSource (flameConfig, s, 0, scene, _("textures/torch6.png"))
{
	vector<CWick *> objList;

	bool bStatus = UObjImporter::import(scene, string(torchName), objList, WICK_NAME_PREFIX);
	assert(bStatus == true);

	m_nbFlames = objList.size();
	m_flames = new IRealFlame* [m_nbFlames];

	/* Calcul de la position et recentrage des mèches */
	for (vector <CWick *>::iterator objListIterator = objList.begin ();
	     objListIterator != objList.end (); objListIterator++)
	{
		(*objListIterator)->buildBoundingBox();
		m_position += (*objListIterator)->getCenter();
	}
	m_position = CPoint(0.5f,0.0f,0.5f)-m_position/m_nbFlames;
	for (vector <CWick *>::iterator objListIterator = objList.begin ();
	     objListIterator != objList.end (); objListIterator++)
		(*objListIterator)->translate(m_position);

	int i=0;
	for (vector <CWick *>::iterator objListIterator = objList.begin ();
	     objListIterator != objList.end (); objListIterator++, i++)
		m_flames[i] = new CLineFlame( flameConfig, &m_texture, s, (*objListIterator), 0.03f, 0.04f, this);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
CampFire::CampFire(const FlameConfig& flameConfig, Field3D * s, CScene *scene, const char *fireName):
		DetachableFireSource (flameConfig, s, 0, scene, _("textures/torch4.png"))
{
	vector<CWick *> objList;

	UObjImporter::import(scene, string(fireName), objList, WICK_NAME_PREFIX);

	m_nbFlames = objList.size();
	m_flames = new IRealFlame* [m_nbFlames];

	/* Calcul de la position et recentrage des mèches */
	for (vector <CWick *>::iterator objListIterator = objList.begin ();
	     objListIterator != objList.end (); objListIterator++)
	{
		(*objListIterator)->buildBoundingBox();
		m_position += (*objListIterator)->getCenter();
	}
	m_position = CPoint(0.5f,0.0f,0.5f)-m_position/m_nbFlames;
	for (vector <CWick *>::iterator objListIterator = objList.begin ();
	     objListIterator != objList.end (); objListIterator++)
		(*objListIterator)->translate(m_position);

	int i=0;
	for (vector <CWick *>::iterator objListIterator = objList.begin ();
	     objListIterator != objList.end (); objListIterator++, i++)
		m_flames[i] = new CLineFlame(flameConfig, &m_texture, s, (*objListIterator), 0.05f, 0.02f, this);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
CandleStick::CandleStick (const FlameConfig& flameConfig, Field3D * s, CScene *scene, const char *filename, float rayon):
		FireSource (flameConfig, s, 1, scene, _("textures/bougie2.png"))
{
	vector<CWick *> objList;

	UObjImporter::import(scene, string(filename), objList, WICK_NAME_PREFIX);

	if (objList.size() > 0)
		m_flames[0] = new CPointFlame(flameConfig, &m_texture, s, rayon, (*objList.begin()));

	m_nbCloneFlames = 20;
	m_cloneFlames = new CloneCPointFlame* [m_nbCloneFlames];

	m_cloneFlames[0]  = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(.5f,0.0f,0.0f));
	m_cloneFlames[1]  = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(-.5f,0.0f,0.0f));
	m_cloneFlames[2]  = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(0.1f,0.0f,0.5f));
	m_cloneFlames[3]  = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(0.0f,0.0f,-0.5f));
	m_cloneFlames[4]  = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(.5f,0.0f,0.5f));
	m_cloneFlames[5]  = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(-.5f,0.0f,0.4f));
	m_cloneFlames[6]  = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(0.5f,0.0f,-0.5f));
	m_cloneFlames[7]  = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(-0.3f,0.0f,-0.5f));
	m_cloneFlames[8]  = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(1.1f,0.0f,0.0f));
	m_cloneFlames[9]  = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(-1.0f,0.0f,0.0f));
	m_cloneFlames[10] = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(0.1f,0.0f,1.1f));
	m_cloneFlames[11] = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(-0.14f,0.0f,-1.0f));
	m_cloneFlames[12]  = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(1.0f,0.0f,1.0f));
	m_cloneFlames[13]  = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(-.9f,0.0f,1.3f));
	m_cloneFlames[14] = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(1.0f,0.0f,-1.0f));
	m_cloneFlames[15] = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(-1.0f,0.0f,-1.0f));
	m_cloneFlames[16]  = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(1.33f,0.0f,0.0f));
	m_cloneFlames[17]  = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(-1.5f,0.0f,0.0f));
	m_cloneFlames[18] = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(0.0f,0.0f,1.4f));
	m_cloneFlames[19] = new CloneCPointFlame(flameConfig, (CPointFlame *) m_flames[0], CPoint(0.0f,0.0f,-1.5f));
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
	setLightPosition(averagePos);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CandleStick::toggleSmoothShading(bool state)
{
	FireSource::setSmoothShading(state);
	for (uint i = 0; i < m_nbCloneFlames; i++)
		m_cloneFlames[i]->setSmoothShading(state);
}

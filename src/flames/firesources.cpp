#include "firesources.hpp"

#include "../scene/graphicsFn.hpp"
#include "../scene/scene.hpp"

#include <vector>
#include <string>

#define WICK_NAME_PREFIX "Wick"
#define TORCH_NAME "Torch"

Candle::Candle (FlameConfig *flameConfig, Solver * s, Scene *scene, const char *filename, uint index, 
		CgSVShader * shader, double rayon):
  FireSource (flameConfig, s, 1, scene, filename, index, shader)
	       //   cgCandleVertexShader (_("bougieShader.cg"),_("vertCandle"),context),
	       //   cgCandleFragmentShader (_("bougieShader.cg"),_("fragCandle"),context)
{
  m_flames[0] = new PointFlame(flameConfig, s, rayon);
}

Firmalampe::Firmalampe(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *filename, uint index,
		       CgSVShader * shader, const char *wickFileName):
  FireSource (flameConfig, s, 1, scene, filename, index, shader)
{
  m_flames[0] = new LineFlame( flameConfig, scene, _("textures/firmalampe.png"), s, wickFileName);
}

Torch::Torch(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *torchName, uint index,
	     CgSVShader * shader):
  FireSource (flameConfig, s, 0, scene, torchName, index, shader, TORCH_NAME)
{
  vector<string> objList;
  int i=0;
  
  scene->getObjectsNameFromOBJ(torchName, objList, WICK_NAME_PREFIX);
  
  m_nbFlames = objList.size();
  m_flames = new BasicFlame* [m_nbFlames];
  
  for (vector < string >::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      m_flames[i] = new LineFlame( flameConfig, scene, _("textures/torch2.png"), s, torchName, (*objListIterator).c_str());
    }
}

CampFire::CampFire(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *fireName, uint index, 
		   CgSVShader * shader):
  FireSource (flameConfig, s, 0, scene, fireName, index, shader, TORCH_NAME)
{
  vector<string> objList;
  int i=0;
  
  scene->getObjectsNameFromOBJ(fireName, objList, WICK_NAME_PREFIX);
  
  m_nbFlames = objList.size();
  m_flames = new BasicFlame* [m_nbFlames];
  
  for (vector < string >::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      m_flames[i] = new LineFlame(flameConfig, scene, _("textures/torch2.png"), s, fireName, (*objListIterator).c_str());
    }
}

CandleStick::CandleStick (FlameConfig *flameConfig, Solver * s, Scene *scene, const char *filename, uint index, 
			  CgSVShader * shader, double rayon):
  FireSource (flameConfig, s, 1, scene, filename, index, shader)
{
  m_flames[0] = new PointFlame(flameConfig, s, rayon);

  m_nbCloneFlames = 20;
  m_cloneFlames = new ClonePointFlame* [m_nbCloneFlames];
  
  m_cloneFlames[0]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(.5,0,0));
  m_cloneFlames[1]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-.5,0,0));
  m_cloneFlames[2]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0.1,0,0.5));
  m_cloneFlames[3]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0,0,-0.5));
  m_cloneFlames[4]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(.5,0,0.5));
  m_cloneFlames[5]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-.5,0,0.4));
  m_cloneFlames[6]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0.5,0,-0.5));
  m_cloneFlames[7]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-0.3,0,-0.5));
  m_cloneFlames[8]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(1.1,0,0));
  m_cloneFlames[9]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-1,0,0));
  m_cloneFlames[10] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0.1,0,1.1));
  m_cloneFlames[11] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-0.14,0,-1));
  m_cloneFlames[12]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(1,0,1));
  m_cloneFlames[13]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-.9,0,1.3));
  m_cloneFlames[14] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(1,0,-1));
  m_cloneFlames[15] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-1,0,-1));
  m_cloneFlames[16]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(1.33,0,0));
  m_cloneFlames[17]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-1.5,0,0));
  m_cloneFlames[18] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0,0,1.4));
  m_cloneFlames[19] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0,0,-1.5));
}

CandleStick::~CandleStick()
{
  for (uint i = 0; i < m_nbCloneFlames; i++)
    delete m_cloneFlames[i];
  delete[]m_cloneFlames;
}

void CandleStick::build()
{
  Point averagePos, tmp;
  
  for (uint i = 0; i < m_nbFlames; i++){
    averagePos +=  m_flames[i]->getCenter ();
    m_flames[i]->build();
  }
  
  for (uint i = 0; i < m_nbCloneFlames; i++){
    averagePos += m_cloneFlames[i]->getCenter ();
    m_cloneFlames[i]->build();
  }
  
  averagePos = averagePos/(m_nbFlames+m_nbCloneFlames);
  averagePos += getPosition();
  setLightPosition(averagePos);
}

void CandleStick::toggleSmoothShading(void)
{
  FireSource::toggleSmoothShading();
  for (uint i = 0; i < m_nbCloneFlames; i++)
    m_cloneFlames[i]->toggleSmoothShading();
}

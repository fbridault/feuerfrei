#include "realFires.hpp"

#include "../scene/graphicsFn.hpp"
#include "../scene/scene.hpp"
#include "../solvers/fakeField3D.hpp"

#include <vector>
#include <string>

#define WICK_NAME_PREFIX "Wick"
#define TORCH_NAME "Torch"

Candle::Candle (FlameConfig *flameConfig, Field3D * s, Scene *scene, const char *filename, uint index, 
		CgSVShader * shader, double rayon):
  FireSource (flameConfig, s, 1, scene, filename, _("textures/bougie2.png"), index, shader)
	       //   cgCandleVertexShader (_("bougieShader.cg"),_("vertCandle"),context),
	       //   cgCandleFragmentShader (_("bougieShader.cg"),_("fragCandle"),context)
{
  m_flames[0] = new PointFlame(flameConfig, &m_texture, s, rayon);
}

Firmalampe::Firmalampe(FlameConfig *flameConfig, Field3D * s, Scene *scene, const char *filename, uint index,
		       CgSVShader * shader, const char *wickFileName):
  FireSource (flameConfig, s, 1, scene, filename, _("textures/firmalampe.png"), index, shader)
{
  m_flames[0] = new LineFlame( flameConfig, scene, &m_texture, s, wickFileName, 0.01);
}

Torch::Torch(FlameConfig *flameConfig, Field3D * s, Scene *scene, const char *torchName, uint index,
	     CgSVShader * shader):
  DetachableFireSource (flameConfig, s, 0, scene, torchName, _("textures/torch6.png"), index, shader, TORCH_NAME)
{
  list<string> objList;
  int i=0;
  
  scene->getObjectsNameFromOBJ(torchName, objList, WICK_NAME_PREFIX);
  
  m_nbFlames = objList.size();
  m_flames = new RealFlame* [m_nbFlames];
  
  for (list < string >::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      m_flames[i] = new LineFlame( flameConfig, scene, &m_texture, s, torchName, 0.1, (*objListIterator).c_str(), this);
    }
}

CampFire::CampFire(FlameConfig *flameConfig, Field3D * s, Scene *scene, const char *fireName, uint index, 
		   CgSVShader * shader):
  DetachableFireSource (flameConfig, s, 0, scene, fireName, _("textures/torch4.png"), index, shader, TORCH_NAME)
{
  list<string> objList;
  int i=0;
  
  scene->getObjectsNameFromOBJ(fireName, objList, WICK_NAME_PREFIX);
  
  m_nbFlames = objList.size();
  m_flames = new RealFlame* [m_nbFlames];
  
  for (list < string >::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      m_flames[i] = new LineFlame(flameConfig, scene, &m_texture, s, fireName, 0.04, (*objListIterator).c_str(), this);
    }
}

CandlesSet::CandlesSet(FlameConfig *flameConfig, Field3D *s, vector <Field3D *>& flameSolvers, Scene *scene,
		       const char *lampName, uint index, CgSVShader * shader, Point scale):
  FireSource (flameConfig, s, 0, scene, lampName, _("textures/bougie2.png"), index, shader, "Lamp")
{
  list<string> objList;
  int i=0;
  
  scene->getObjectsNameFromOBJ(lampName, objList, WICK_NAME_PREFIX);
  
  m_nbFlames = objList.size();
  m_flames = new RealFlame* [m_nbFlames];
  
  for (list < string >::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      Point pt;
      Field3D *field =  new FakeField3D(pt, 10, 10, 10, 1.0, Point(.08,.08,.08), .4, 0.3);
      flameSolvers.push_back( field );
      m_flames[i] = new PointFlame( flameConfig, &m_texture, field, .4, scene, lampName, (*objListIterator).c_str());
    }
}

CandleStick::CandleStick (FlameConfig *flameConfig, Field3D * s, Scene *scene, const char *filename, uint index, 
			  CgSVShader * shader, double rayon):
  FireSource (flameConfig, s, 1, scene, filename, _("textures/bougie2.png"), index, shader)
{
  m_flames[0] = new PointFlame(flameConfig, &m_texture, s, rayon);

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

void CandleStick::toggleSmoothShading(bool state)
{
  FireSource::setSmoothShading(state);
  for (uint i = 0; i < m_nbCloneFlames; i++)
    m_cloneFlames[i]->setSmoothShading(state);
}

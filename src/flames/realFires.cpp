#include "realFires.hpp"

#include "../scene/graphicsFn.hpp"
#include "../scene/scene.hpp"
#include "../solvers/fakeField3D.hpp"

Candle::Candle (const FlameConfig& flameConfig, Field3D * s, Scene* const scene, uint index, 
		const GLSLProgram * const program, float rayon, Object *wick):
  FireSource (flameConfig, s, 1, scene, _("textures/bougie2.png"), index, program)
{
  m_flames[0] = new PointFlame(flameConfig, &m_texture, s, rayon, wick);
}

Firmalampe::Firmalampe(const FlameConfig& flameConfig, Field3D * s, Scene *scene, uint index,
		       const GLSLProgram * const program, const char *wickFileName):
  FireSource (flameConfig, s, 1, scene, _("textures/firmalampe.png"), index, program)
{
  list<Wick *> objList;
  
  scene->importOBJ(wickFileName, NULL, &objList, WICK_NAME_PREFIX);
  
  if(objList.size() > 0)
    m_flames[0] = new LineFlame( flameConfig, &m_texture, s, (*objList.begin()), 0.01f);
}

Torch::Torch(const FlameConfig& flameConfig, Field3D * s, Scene *scene, const char *torchName, uint index,
	     const GLSLProgram * const program):
  DetachableFireSource (flameConfig, s, 0, scene, _("textures/torch6.png"), index, program)
{
  list<Wick *> objList;
  int i=0;
  
  scene->importOBJ(torchName, NULL, &objList, WICK_NAME_PREFIX);
  
  m_nbFlames = objList.size();
  m_flames = new RealFlame* [m_nbFlames];
  
  for (list <Wick *>::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      m_flames[i] = new LineFlame( flameConfig, &m_texture, s, (*objListIterator), 0.1f, this);
    }
}

CampFire::CampFire(const FlameConfig& flameConfig, Field3D * s, Scene *scene, const char *fireName, uint index, 
		   const GLSLProgram * const program):
  DetachableFireSource (flameConfig, s, 0, scene, _("textures/torch4.png"), index, program)
{
  list<Wick *> objList;
  int i=0;
  
  scene->importOBJ(fireName, NULL, &objList, WICK_NAME_PREFIX);
  
  m_nbFlames = objList.size();
  m_flames = new RealFlame* [m_nbFlames];
  
  for (list <Wick *>::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      m_flames[i] = new LineFlame(flameConfig, &m_texture, s, (*objListIterator), 0.04f, this);
    }
}

CandleStick::CandleStick (const FlameConfig& flameConfig, Field3D * s, Scene *scene, const char *filename, uint index, 
			  const GLSLProgram * const program, float rayon):
  FireSource (flameConfig, s, 1, scene, _("textures/bougie2.png"), index, program)
{
  m_flames[0] = new PointFlame(flameConfig, &m_texture, s, rayon);

  m_nbCloneFlames = 20;
  m_cloneFlames = new ClonePointFlame* [m_nbCloneFlames];
  
  m_cloneFlames[0]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(.5f,0.0f,0.0f));
  m_cloneFlames[1]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-.5f,0.0f,0.0f));
  m_cloneFlames[2]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0.1f,0.0f,0.5f));
  m_cloneFlames[3]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0.0f,0.0f,-0.5f));
  m_cloneFlames[4]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(.5f,0.0f,0.5f));
  m_cloneFlames[5]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-.5f,0.0f,0.4f));
  m_cloneFlames[6]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0.5f,0.0f,-0.5f));
  m_cloneFlames[7]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-0.3f,0.0f,-0.5f));
  m_cloneFlames[8]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(1.1f,0.0f,0.0f));
  m_cloneFlames[9]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-1.0f,0.0f,0.0f));
  m_cloneFlames[10] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0.1f,0.0f,1.1f));
  m_cloneFlames[11] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-0.14f,0.0f,-1.0f));
  m_cloneFlames[12]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(1.0f,0.0f,1.0f));
  m_cloneFlames[13]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-.9f,0.0f,1.3f));
  m_cloneFlames[14] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(1.0f,0.0f,-1.0f));
  m_cloneFlames[15] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-1.0f,0.0f,-1.0f));
  m_cloneFlames[16]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(1.33f,0.0f,0.0f));
  m_cloneFlames[17]  = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(-1.5f,0.0f,0.0f));
  m_cloneFlames[18] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0.0f,0.0f,1.4f));
  m_cloneFlames[19] = new ClonePointFlame(flameConfig, (PointFlame *) m_flames[0], Point(0.0f,0.0f,-1.5f));
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

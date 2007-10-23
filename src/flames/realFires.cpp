#include "realFires.hpp"

#include "../scene/graphicsFn.hpp"
#include "../scene/scene.hpp"
#include "../solvers/fakeField3D.hpp"

Candle::Candle (const FlameConfig& flameConfig, Field3D * s, Scene* const scene, uint index, 
		const GLSLProgram * const program, float rayon, Object *wick):
  FireSource (flameConfig, s, 1, scene, _("textures/bougie2.png"), index, program)
{
  m_flames[0] = new PointFlame(flameConfig, &m_texture, s, rayon, wick);
  m_nbLights = 7;
}

void Candle::setLightPosition (const Point& pos)
{
  m_lightPosition[0] = pos.x;
  m_lightPosition[1] = pos.y;
  m_lightPosition[2] = pos.z;
  m_centreSP = pos;
  ((PointFlame *)m_flames[0])->getLightPositions(m_lightPositions, m_nbLights);
}

void Candle::switchOffMulti()
{
  int n,light=0;
  
  for( n = 0 ; n < 8 ; n++){
    switch(n){
    case 0 : light = GL_LIGHT0; break;
    case 1 : light = GL_LIGHT1; break;
    case 2 : light = GL_LIGHT2; break;
    case 3 : light = GL_LIGHT3; break;
    case 4 : light = GL_LIGHT4; break;
    case 5 : light = GL_LIGHT5; break;
    case 6 : light = GL_LIGHT6; break;
    case 7 : light = GL_LIGHT7; break;
    }
    glDisable(light);
  }
}

void Candle::switchOnMulti()
{
  int n,light=0;
  double coef;
  
  for( n = 0 ; n < m_nbLights ; n++){
    switch(n){
    case 0 : light = GL_LIGHT0; break;
    case 1 : light = GL_LIGHT1; break;
    case 2 : light = GL_LIGHT2; break;
    case 3 : light = GL_LIGHT3; break;
    case 4 : light = GL_LIGHT4; break;
    case 5 : light = GL_LIGHT5; break;
    case 6 : light = GL_LIGHT6; break;
    case 7 : light = GL_LIGHT7; break;
    }
    //    coef = 2/(double)(n+1);
    coef = (-(n - m_nbLights/2)*(n - m_nbLights/2)+16)/40.0;
    //x    cout << coef << " ";
    GLfloat val_diffuse[]={1.0f*coef,0.5f*coef,0.0f,1.0f};
    //GLfloat val_ambiant[]={0.05*coef,0.05*coef,0.05*coef,1.0};
    GLfloat val_null[]={0.0f,0.0f,0.0f,1.0f};
    GLfloat val_specular[]={.1f,.1f,.1f,1.0f};

    m_lightPositions[n][0] = m_lightPositions[n][0] * m_solver->getScale().x + getPosition().x;
    m_lightPositions[n][1] = m_lightPositions[n][1] * m_solver->getScale().y + getPosition().y;
    m_lightPositions[n][2] = m_lightPositions[n][2] * m_solver->getScale().z + getPosition().z;

    glLightfv(light,GL_POSITION,m_lightPositions[n]);
    glLightfv(light,GL_DIFFUSE,val_diffuse);
    glLightfv(light,GL_SPECULAR,val_specular);
    glLightfv(light,GL_AMBIENT,val_null);
    glLightf(light,GL_QUADRATIC_ATTENUATION,0.05f);
    glEnable(light);
  }
  //cout << endl;
}

Firmalampe::Firmalampe(const FlameConfig& flameConfig, Field3D * s, Scene *scene, uint index,
		       const GLSLProgram * const program, const char *wickFileName):
  FireSource (flameConfig, s, 1, scene, _("textures/firmalampe.png"), index, program)
{
  list<Wick *> objList;
  
  scene->importOBJ(wickFileName, NULL, &objList, WICK_NAME_PREFIX);
  
  if(objList.size() > 0)
    m_flames[0] = new LineFlame( flameConfig, &m_texture, s, (*objList.begin()), 0.03f, 0.01f);
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
      m_flames[i] = new LineFlame( flameConfig, &m_texture, s, (*objListIterator), 0.03f, 0.04f, this);
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
      m_flames[i] = new LineFlame(flameConfig, &m_texture, s, (*objListIterator), 0.05f, 0.04f, this);
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

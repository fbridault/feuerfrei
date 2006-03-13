#include "fire.hpp"

#include "../scene/scene.hpp"

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FLAMELIGHT ****************************************/
/**********************************************************************************************************************/

FlameLight::FlameLight(Scene *scene, int index, CgSVShader * shader)
{  
  m_scene = scene;
  
  switch(index){
    case 0 : m_light = GL_LIGHT0; break;
    case 1 : m_light = GL_LIGHT1; break;
    case 2 : m_light = GL_LIGHT2; break;
    case 3 : m_light = GL_LIGHT3; break;
    case 4 : m_light = GL_LIGHT4; break;
    case 5 : m_light = GL_LIGHT5; break;
    case 6 : m_light = GL_LIGHT6; break;
    case 7 : m_light = GL_LIGHT7; break;
  }
  
  m_lightPosition[3] = 1.0;
  
  m_cgShader = shader;
}

FlameLight::~FlameLight()
{
}

void FlameLight::switchOff()
{ 
  glDisable(m_light);
}

void FlameLight::switchOn(double coef)
{  
  GLfloat val_diffuse[]={1*coef,0.5*coef,0.0,1.0};
  //GLfloat val_ambiant[]={0.05*coef,0.05*coef,0.05*coef,1.0};
  GLfloat val_null[]={0.0,0.0,0.0,1.0};
  GLfloat val_specular[]={.1*coef,.1*coef,.1*coef,1.0};
  
  glLightfv(m_light,GL_POSITION,m_lightPosition);
  glLightfv(m_light,GL_DIFFUSE,val_diffuse);
  glLightfv(m_light,GL_SPECULAR,val_specular);
  glLightfv(m_light,GL_AMBIENT,val_null);
  glEnable(m_light);
}

void FlameLight::resetDiffuseLight()
{
  glDisable(m_light);
}

void FlameLight::drawShadowVolume ()
{
  m_cgShader->setLightPos (m_lightPosition);
  
  m_cgShader->enableProfile ();
  m_cgShader->bindProgram ();
  
  glPushMatrix ();
  glLoadIdentity ();
  m_cgShader->setModelViewMatrixToInverse ();
  glPopMatrix ();
  m_cgShader->setModelViewProjectionMatrix ();
  
  m_scene->draw_sceneWSV();
  
  m_cgShader->disableProfile ();
}

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FIRESOURCE ****************************************/
/**********************************************************************************************************************/

FireSource::FireSource(Solver * s, int nbFlames, Point& posRel, Scene *scene, double innerForce,  const char *filename, 
		       int index, CgSVShader * shader) : FlameLight(scene, index, shader)
{  
  m_solver = s;
  
  m_nbFlames=nbFlames;
  if(m_nbFlames) m_flames = new BasicFlame* [m_nbFlames];
  
  m_luminary = new Object(scene);
  scene->importOBJ(filename, m_luminary, true);
  m_luminaryDL=glGenLists(1);
  glNewList(m_luminaryDL,GL_COMPILE);
  m_luminary->draw();
  glEndList();
  
  m_position=posRel;
}

FireSource::~FireSource()
{
  for (int i = 0; i < m_nbFlames; i++)
    delete m_flames[i];
  delete[]m_flames;
  /* On efface le luminaire, il n'appartient pas à la scène */
  delete m_luminary;
  glDeleteLists(m_luminaryDL,1);
}

void FireSource::build()
{
  Point averagePos, tmp;
  
  for (int i = 0; i < m_nbFlames; i++){
    averagePos = (averagePos*i + m_flames[i]->getCenter ())/(i+1);
    m_flames[i]->build();
  }
  averagePos += getPosition();
  setLightPosition(averagePos);
}

Vector FireSource::getMainDirection()
{
  Vector averageVec, tmp;
  
  for (int i = 0; i < m_nbFlames; i++)
    averageVec = (averageVec*i + m_flames[i]->getMainDirection ())/(i+1);
  
  return(averageVec);
}

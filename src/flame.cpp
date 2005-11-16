#include "flame.hpp"

#include "scene.hpp"

Flame::Flame(Solver *s, int nb, CPoint *centre, CPoint *pos, const char *filename, CScene *scene)
{  
  m_solver = s;
  m_scene = scene;
  
  m_nbSkeletons = nb;
  
  m_startPosition = m_position = *pos;
  pos->y -= m_solver->getDimY()/24.0;
  
  m_skeletons = new PeriSkeleton* [m_nbSkeletons];
  
  m_uorder = 4;
  m_vorder = 4;
  
  m_nurbs = gluNewNurbsRenderer();
  gluNurbsProperty(m_nurbs, GLU_SAMPLING_TOLERANCE, 20.0);
  gluNurbsProperty(m_nurbs, GLU_DISPLAY_MODE, GLU_FILL);
  gluNurbsCallback(m_nurbs, GLU_NURBS_ERROR, (void(*)())nurbsError);
  
  m_toggle=false;
  
  m_nbLights=0;
  
  m_perturbateCount=0;
  
  m_luminary = new CObject(m_scene);
  m_scene->loadObject(filename, m_luminary, true);
  m_luminaryDL=glGenLists(1);
  glNewList(m_luminaryDL,GL_COMPILE);
  m_luminary->draw();
  glEndList();
}

Flame::Flame(Solver *s, CPoint *centre, CPoint *pos, const char *filename, CScene *scene)
{  
  m_solver = s;
  m_scene = scene;

  m_position = *pos;

  m_uorder = 4;
  m_vorder = 4;
  
  m_nurbs = gluNewNurbsRenderer();
  gluNurbsProperty(m_nurbs, GLU_SAMPLING_TOLERANCE, 10.0);
  gluNurbsProperty(m_nurbs, GLU_DISPLAY_MODE, GLU_FILL);
  gluNurbsCallback(m_nurbs, GLU_NURBS_ERROR, (void(*)())nurbsError);
  
  m_toggle=false;
  
  m_nbLights=0;
  
  m_perturbateCount=0;
    
  m_luminary = new CObject(m_scene);
  m_scene->loadObject(filename, m_luminary, true);
  m_luminaryDL=glGenLists(1);
  glNewList(m_luminaryDL,GL_COMPILE);
  m_luminary->draw();
  glEndList();
}

Flame::~Flame()
{  
  gluDeleteNurbsRenderer(m_nurbs);
  /* On efface le luminaire, il n'appartient pas à la scène */
  delete m_luminary;
}

void Flame::moveTo(CPoint& position)
{
  int i,j;
  CPoint move = position - m_position;
  double strength=1.5;
  m_position=m_startPosition + position;
  
  /* Ajouter des forces externes */
  if(move.x)
    if( move.x > 0)
      for (i = -m_solver->getZRes() / 4 - 1; i <= m_solver->getZRes() / 4 + 1; i++)
	for (j = -m_solver->getYRes() / 4 - 1; j <= m_solver->getYRes() / 4 + 1; j++)
	  m_solver->addUsrc (m_solver->getXRes() - 1,
			     ((int) (ceil (m_solver->getYRes() / 2.0))) + j,
			     ((int) (ceil (m_solver->getZRes() / 2.0))) + i, -strength);
    else
      for (i = -m_solver->getZRes() / 4 - 1; i <= m_solver->getZRes() / 4 + 1; i++)
	for (j = -m_solver->getYRes() / 4 - 1; j <= m_solver->getYRes() / 4 + 1; j++)
	  m_solver->addUsrc (2,
			     ((int) (ceil (m_solver->getYRes() / 2.0))) + j,
			     ((int) (ceil (m_solver->getZRes() / 2.0))) + i, strength);  
  if(move.y)
    if( move.y > 0)
      for (i = -m_solver->getXRes() / 4 - 1; i <= m_solver->getXRes() / 4 + 1; i++)
	for (j = -m_solver->getZRes() / 4 - 1; j < m_solver->getZRes() / 4 + 1; j++)
	  m_solver->addVsrc (((int) (ceil (m_solver->getXRes() / 2.0))) + i,
			     m_solver->getYRes() - 1,
			     ((int) (ceil (m_solver->getZRes() / 2.0))) + j, -strength);
    else
      for (i = -m_solver->getXRes() / 4 - 1; i <= m_solver->getXRes() / 4 + 1; i++)
	for (j = -m_solver->getZRes() / 4 - 1; j <= m_solver->getZRes() / 4 + 1; j++)
	  m_solver->addVsrc (((int) (ceil (m_solver->getXRes() / 2.0))) + i, 
			     2,
			     ((int) (ceil (m_solver->getZRes() / 2.0))) + j, strength/10.0);
  if(move.z)
    if( move.z > 0)
      for (i = -m_solver->getXRes() / 4 - 1; i <= m_solver->getXRes() / 4 + 1; i++)
	for (j = -m_solver->getYRes() / 4 - 1; j <= m_solver->getYRes() / 4 - 1; j++)
	  m_solver->addWsrc (((int) (ceil (m_solver->getXRes() / 2.0))) + i,
			     ((int) (ceil (m_solver->getYRes() / 2.0))) + j,
			     m_solver->getZRes() - 1, -strength);
    else
      for (i = -m_solver->getXRes() / 4 - 1; i <= m_solver->getXRes() / 4 + 1; i++)
	for (j = -m_solver->getYRes() / 4 - 1; j <= m_solver->getYRes() / 4 - 1; j++)
	  m_solver->addWsrc (((int) (ceil (m_solver->getXRes() / 2.0))) + i,
			     ((int) (ceil (m_solver->getYRes() / 2.0))) + j,
			     2, strength);
}

void CALLBACK Flame::nurbsError(GLenum errorCode)
{
  const GLubyte *estring;
  
  estring = gluErrorString(errorCode);
  fprintf(stderr, "Erreur Nurbs : %s\n", estring);
  exit(0);
}

void Flame::toggleSmoothShading()
{
  m_toggle = !m_toggle;
  if(m_toggle)
    gluNurbsProperty(m_nurbs, GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON);
  else
    gluNurbsProperty(m_nurbs, GLU_DISPLAY_MODE, GLU_FILL);
}

void Flame::switch_off_lights()
{
  int n,light=0;
  
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
    glDisable(light);
  }
}

void Flame::switch_on_lights()
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
    coef = (-(n - m_nbLights/2)*(n - m_nbLights/2)+16)/10.0;
    //cout << coef << endl;
    GLfloat val_diffuse[]={0.13*coef,0.09*coef,0.06*coef,1.0};
    //GLfloat val_ambiant[]={0.2,0.2,0.2,1.0};
    GLfloat null[]={0.0,0.0,0.0,1.0};
    GLfloat val_position[]={m_lightPositions[n][0],m_lightPositions[n][1],m_lightPositions[n][2],1.0};
  
    glLightfv(light,GL_POSITION,val_position);
    glLightfv(light,GL_DIFFUSE,val_diffuse);
    glLightfv(light,GL_SPECULAR,null);
    glLightfv(light,GL_AMBIENT,null);
    glEnable(light);
  }
}

void Flame::enable_only_ambient_light(int i)
{
  int light=0;
  double coef;

  //coef = 4/(double)(i+1);
  coef = (-(i - m_nbLights/2)*(i - m_nbLights/2)+16)/10.0;
  GLfloat val_ambiant[]={0.25*coef,0.25*coef,0.25*coef,1.0};
  // GLdouble val_ambiant[]={0.4,0.4,0.4,1.0};
  GLfloat null[]={0.0,0.0,0.0,1.0};
  
  switch(i){
  case 0 : light = GL_LIGHT0; break;
  case 1 : light = GL_LIGHT1; break;
  case 2 : light = GL_LIGHT2; break;
  case 3 : light = GL_LIGHT3; break;
  case 4 : light = GL_LIGHT4; break;
  case 5 : light = GL_LIGHT5; break;
  case 6 : light = GL_LIGHT6; break;
  case 7 : light = GL_LIGHT7; break;
  }

  glLightfv(light,GL_DIFFUSE,null);
  //    glLightfv(light,GL_SPECULAR,null);
  glLightfv(light,GL_AMBIENT,val_ambiant);

  glEnable(light);
}

void Flame::reset_diffuse_light(int i)
{
  int light=0;
  double coef;
  
  //coef = 2/(double)(i+1);
  coef = (-(i - m_nbLights/2)*(i - m_nbLights/2)+16)/10.0;
  GLfloat val_diffuse[]={0.13*coef,0.09*coef,0.06*coef,1.0};
  //GLfloat val_ambiant[]={0.2,0.2,0.2,1.0};
  GLfloat null[]={0.0,0.0,0.0,1.0};
    
  switch(i){
  case 0 : light = GL_LIGHT0; break;
  case 1 : light = GL_LIGHT1; break;
  case 2 : light = GL_LIGHT2; break;
  case 3 : light = GL_LIGHT3; break;
  case 4 : light = GL_LIGHT4; break;
  case 5 : light = GL_LIGHT5; break;
  case 6 : light = GL_LIGHT6; break;
  case 7 : light = GL_LIGHT7; break;
  }
  
  glLightfv(light,GL_DIFFUSE,val_diffuse);
  //    glLightfv(light,GL_SPECULAR,null);
  glLightfv(light,GL_AMBIENT,null);
  glDisable(light);
}

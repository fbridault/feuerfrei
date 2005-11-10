#include "flame.hpp"

#include <stdlib.h>

Flame::Flame(Solver *s, int nb, CPoint *centre, CPoint *pos, const char *filename, CScene *scene)
{  
  solveur = s;
  sc = scene;
  
  nb_squelettes = nb;
  
  position = *pos;
  pos->setY(pos->getY() - solveur->getDimY()/24.0);
  
  squelettes = new PeriSkeleton* [nb_squelettes];
  
  uorder = 4;
  vorder = 4;
  
  nurbs = gluNewNurbsRenderer();
  gluNurbsProperty(nurbs, GLU_SAMPLING_TOLERANCE, 20.0);
  gluNurbsProperty(nurbs, GLU_DISPLAY_MODE, GLU_FILL);
  gluNurbsCallback(nurbs, GLU_NURBS_ERROR, (void(*)())nurbsError);
  
  toggle=false;
  
  nb_lights=0;
  
  perturbate_count=0;

  scene->loadObject(filename, new CObject(scene,pos));
}

Flame::Flame(Solver *s, CPoint *centre, CPoint *pos, const char *filename, CScene *scene)
{  
  solveur = s;
  sc = scene;

  position = *pos;

  uorder = 4;
  vorder = 4;
  
  nurbs = gluNewNurbsRenderer();
  gluNurbsProperty(nurbs, GLU_SAMPLING_TOLERANCE, 10.0);
  gluNurbsProperty(nurbs, GLU_DISPLAY_MODE, GLU_FILL);
  gluNurbsCallback(nurbs, GLU_NURBS_ERROR, (void(*)())nurbsError);
  
  toggle=false;
  
  nb_lights=0;
  
  perturbate_count=0;
    
  scene->loadObject(filename, new CObject(scene,pos));
}

Flame::~Flame()
{  
  gluDeleteNurbsRenderer(nurbs);
  /* On efface pas le luminaire, il appartient à la scène */
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
  toggle = !toggle;
  if(toggle)
    gluNurbsProperty(nurbs, GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON);
  else
    gluNurbsProperty(nurbs, GLU_DISPLAY_MODE, GLU_FILL);
}

void Flame::switch_off_lights()
{
  int n,light=0;
  
  for( n = 0 ; n < nb_lights ; n++){
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

  for( n = 0 ; n < nb_lights ; n++){
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
    coef = (-(n - nb_lights/2)*(n - nb_lights/2)+16)/10.0;
    //cout << coef << endl;
    GLfloat val_diffuse[]={0.13*coef,0.09*coef,0.06*coef,1.0};
    //GLfloat val_ambiant[]={0.2,0.2,0.2,1.0};
    GLfloat null[]={0.0,0.0,0.0,1.0};
    GLfloat val_position[]={lightPositions[n][0],lightPositions[n][1],lightPositions[n][2],1.0};
  
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
  coef = (-(i - nb_lights/2)*(i - nb_lights/2)+16)/10.0;
  GLfloat val_ambiant[]={0.25*coef,0.25*coef,0.25*coef,1.0};
  // GLfloat val_ambiant[]={0.4,0.4,0.4,1.0};
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
  coef = (-(i - nb_lights/2)*(i - nb_lights/2)+16)/10.0;
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

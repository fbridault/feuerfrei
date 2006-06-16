#include "fire.hpp"

#include "../scene/scene.hpp"

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FLAMELIGHT ****************************************/
/**********************************************************************************************************************/

FlameLight::FlameLight(Scene *scene, uint index, CgSVShader * shader, const char* const IESFilename)
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
  
  m_orientationSPtheta = 0.0;

  m_iesFile = new IES(IESFilename);
}

FlameLight::~FlameLight()
{
  delete m_iesFile;
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

FireSource::FireSource(FlameConfig *flameConfig, Solver *s, uint nbFlames,  Scene *scene, const char *filename, uint index, 
		       CgSVShader *shader, const char *objName) : 
  FlameLight(scene, index, shader, flameConfig->IESFileName.ToAscii())
{  
  char mtlName[255];
  m_solver = s;
  
  m_nbFlames=nbFlames;
  if(m_nbFlames) m_flames = new BasicFlame* [m_nbFlames];
  
  if(objName != NULL && scene->getMTLFileNameFromOBJ(filename, mtlName)){
    cerr << filename << " utilise le fichier MTL " << mtlName << endl;
    AS_ERROR(chdir("./scenes"),"chdir scenes dans getMTLFileNameFromOBJ");
    scene->importMTL(mtlName);
    chdir("..");
  }
  
  m_luminary = new Object(scene);
  hasLuminary = scene->importOBJ(filename, m_luminary, true, objName);
  if(hasLuminary){
    m_luminaryDL=glGenLists(1);
    glNewList(m_luminaryDL,GL_COMPILE);
    m_luminary->draw();
    glEndList();
  }
  
  m_position=flameConfig->position;
}

FireSource::~FireSource()
{
  for (uint i = 0; i < m_nbFlames; i++)
    delete m_flames[i];
  delete[]m_flames;
  /* On efface le luminaire, il n'appartient pas à la scène */
  delete m_luminary;
  
  if(hasLuminary) glDeleteLists(m_luminaryDL,1);
}

void FireSource::build()
{
  Point averagePos, tmp;
  
  for (uint i = 0; i < m_nbFlames; i++){
    averagePos +=  m_flames[i]->getCenter ();
    m_flames[i]->build();
  }
  averagePos = averagePos/m_nbFlames;
  averagePos += getPosition();
  setLightPosition(averagePos);
}

Vector FireSource::getMainDirection()
{
  Vector averageVec, tmp;
  
  for (uint i = 0; i < m_nbFlames; i++)
    averageVec = (averageVec*i + m_flames[i]->getMainDirection ())/(i+1);
  
  return(averageVec);
}

void FireSource::computeIntensityPositionAndDirection()
{
  double r,y;
  
  Vector o = getMainDirection();
  
  // l'intensité est calculée à partir du rapport de la longeur de la flamme (o)
  // et de la taille en y de la grille fois un coeff correcteur
  m_intensity=o.length()/(m_solver->getDimY());
//   m_intensity = log(m_intensity)/6.0+1;
//   m_intensity = sin(m_intensity * PI/2.0);
  /* Fonction de smoothing pour éviter d'avoir trop de fluctuation */
  m_intensity = sqrt(m_intensity);
  // le centre du SP est la position de la flamme + la moitié du vecteur orientation
  // (orientation = vecteur position vers dernière particule)
  m_centreSP= getPosition()+(o/2.0);
  
  // l'axe de rotation est dans le plan x0z perpendiculaire aux coordonnées
  // de o projeté perpendiculairement dans ce plan
//   m_axeRotation.set(-o.z,0.0,o.x);
  
//   // l'angle de rotation theta est la coordonnée sphérique correspondante
//   y=o.y;
//   r = (double)o.length();
//   if(r - fabs(y) < EPSILON)
//     m_orientationSPtheta = 0.0;
//   else
//     m_orientationSPtheta=acos(y / r)*180.0/M_PI;
}

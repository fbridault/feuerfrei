#include "abstractFires.hpp"

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

void FlameLight::switchOn()
{  
  double coef = 1.5*m_intensity;
//   GLfloat val_diffuse[]={1,1,1,1.0};
  GLfloat val_diffuse[]={1*coef,0.5*coef,0.0,1.0};
  //GLfloat val_ambiant[]={0.05*coef,0.05*coef,0.05*coef,1.0};
  GLfloat val_null[]={0.0,0.0,0.0,1.0};
  GLfloat val_specular[]={.1*coef,.1*coef,.1*coef,1.0};
  
  /* Définition de l'intensité lumineuse de chaque flamme en fonction de la hauteur de celle-ci */
  glLightfv(m_light,GL_POSITION,m_lightPosition);
  glLightfv(m_light,GL_DIFFUSE,val_diffuse);
  glLightfv(m_light,GL_SPECULAR,val_specular);
  glLightfv(m_light,GL_AMBIENT,val_null);
  glLightf(m_light,GL_QUADRATIC_ATTENUATION,0.005);
  glEnable(m_light);
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
  
  m_scene->drawSceneWSV(*m_cgShader);
  
  m_cgShader->disableProfile ();
}

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FIRESOURCE ****************************************/
/**********************************************************************************************************************/

FireSource::FireSource(FlameConfig *flameConfig, Field3D *s, uint nbFlames,  Scene *scene, const char *filename, 
		       const wxString &texname,  uint index, CgSVShader *shader, const char *objName) : 
  FlameLight(scene, index, shader, flameConfig->IESFileName.ToAscii()),
  m_texture(texname, GL_CLAMP, GL_REPEAT)
{  
  list<string> objList;
  char mtlName[255];
  /** Luminaire */
  Object **m_luminary;
  uint i;
  uint nbObj;
  
  m_solver = s;
  
  m_nbFlames=nbFlames;
  if(m_nbFlames) m_flames = new RealFlame* [m_nbFlames];
    
  if(objName != NULL){
    if(scene->getMTLFileNameFromOBJ(filename, mtlName))
      scene->importMTL(mtlName);
    
    scene->getObjectsNameFromOBJ(filename, objList, objName);
    m_luminary = new Object* [objList.size()];
    nbObj=objList.size();
    
    i=0;
    for (list < string >::iterator objListIterator = objList.begin ();
	 objListIterator != objList.end (); objListIterator++, i++)
      {      
	m_luminary[i] = new Object(scene);
	scene->importOBJ(filename, m_luminary[i], true, (*objListIterator).c_str());
      }
    
    m_luminaryDL=glGenLists(1);
    glNewList(m_luminaryDL,GL_COMPILE);
    for (i=0; i < objList.size(); i++)
      m_luminary[i]->draw();
    glEndList();
    m_hasLuminary=true;
  }
  else
    {
      nbObj=1;
      m_luminary = new Object* [1];
      m_luminary[0] = new Object(scene);
      m_hasLuminary = scene->importOBJ(filename, m_luminary[0], true, NULL);
      if(m_hasLuminary){
	m_luminaryDL=glGenLists(1);
	glNewList(m_luminaryDL,GL_COMPILE);
	m_luminary[0]->draw();
	glEndList();
      }
    }
  m_breakable=flameConfig->breakable;
  
  /* On efface le luminaire, il n'appartient pas à la scène */
  for (uint i = 0; i < nbObj; i++)
    delete m_luminary[i];
  delete []m_luminary;
  
  m_intensityCoef = flameConfig->intensityCoef;
}

FireSource::~FireSource()
{
  for (uint i = 0; i < m_nbFlames; i++)
    delete m_flames[i];
  delete[]m_flames;
  
  if(m_hasLuminary) glDeleteLists(m_luminaryDL,1);
}

void FireSource::build()
{
  Point averagePos;
  
  for (uint i = 0; i < m_nbFlames; i++){
    m_flames[i]->build();
    averagePos += m_flames[i]->getCenter ();
  }
  averagePos *= m_solver->getScale();
  averagePos /= m_nbFlames;
  averagePos += getPosition();
  setLightPosition(averagePos);
}

void FireSource::computeIntensityPositionAndDirection()
{
  //  double r,y;
  
  Vector o = getMainDirection();
  
  // l'intensité est calculée à partir du rapport de la longeur de la flamme (o)
  // et de la taille en y de la grille fois un coeff correcteur
  m_intensity=o.length()/(m_solver->getScale().y)*m_intensityCoef;
  
   //  m_intensity = log(m_intensity)/6.0+1;
//   m_intensity = sin(m_intensity * PI/2.0);
  /* Fonction de smoothing pour éviter d'avoir trop de fluctuation */
  m_intensity = sqrt(m_intensity);
  
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

DetachableFireSource::DetachableFireSource(FlameConfig *flameConfig, Field3D *s, uint nbFlames, 
					   Scene *scene, const char *filename, const wxString &texname, 
					   uint index, CgSVShader *shader, const char *objName) : 
  FireSource (flameConfig, s, nbFlames, scene, filename, texname, index, shader, objName)
{
}

DetachableFireSource::~DetachableFireSource()
{
  for (list < DetachedFlame* >::iterator flamesIterator = m_detachedFlamesList.begin ();
       flamesIterator != m_detachedFlamesList.end();  flamesIterator++)
    delete (*flamesIterator);
  m_detachedFlamesList.clear ();
}

void DetachableFireSource::drawFlame(bool display, bool displayParticle)
{
  Point pt(m_solver->getPosition());
  Point scale(m_solver->getScale());
  glPushMatrix();
  glTranslatef (pt.x, pt.y, pt.z);
  glScalef (scale.x, scale.y, scale.z);
  for (uint i = 0; i < m_nbFlames; i++)
    m_flames[i]->drawFlame(display, displayParticle);
  for (list < DetachedFlame* >::iterator flamesIterator = m_detachedFlamesList.begin ();
       flamesIterator != m_detachedFlamesList.end();  flamesIterator++)
    (*flamesIterator)->drawFlame(display, displayParticle);
  glPopMatrix();
}

void DetachableFireSource::build()
{
  Point averagePos, tmp;
  DetachedFlame* flame;
  
  for (uint i = 0; i < m_nbFlames; i++){
    m_flames[i]->breakCheck();
    m_flames[i]->build();
    averagePos += m_flames[i]->getCenter ();
  }
  list < DetachedFlame* >::iterator flamesIterator = m_detachedFlamesList.begin ();
  while( flamesIterator != m_detachedFlamesList.end()){
    if(!(*flamesIterator)->build()){
      flame = *flamesIterator;
      flamesIterator = m_detachedFlamesList.erase(flamesIterator);
      delete flame;
    }else
      flamesIterator++;
  }
  averagePos *= m_solver->getScale();
  averagePos /= m_nbFlames;
  averagePos += getPosition();
  setLightPosition(averagePos);
}

#include "abstractFires.hpp"

#include "../scene/scene.hpp"

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FLAMELIGHT ****************************************/
/**********************************************************************************************************************/

FlameLight::FlameLight(const Scene* const scene, uint index, const CgSVShader* const shader, const char* const IESFilename)
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

FireSource::FireSource(const FlameConfig* const flameConfig, Field3D* const s, uint nbFlames,  Scene *scene, const char *filename, 
		       const wxString &texname,  uint index, const CgSVShader* const shader, const char *objName) : 
  FlameLight(scene, index, shader, flameConfig->IESFileName.ToAscii()),
  m_texture(texname, GL_CLAMP, GL_REPEAT)
{  
  char mtlName[255];
  uint i;
  uint nbObj;
  
  m_solver = s;
  
  m_nbFlames=nbFlames;
  if(m_nbFlames) m_flames = new RealFlame* [m_nbFlames];
    
  if(objName != NULL){
    if(scene->getMTLFileNameFromOBJ(filename, mtlName))
      scene->importMTL(mtlName);
    
    scene->importOBJ(filename, &m_luminary, NULL, objName);
    
    for (list < Object* >::iterator luminaryIterator = m_luminary.begin ();
	 luminaryIterator  != m_luminary.end (); luminaryIterator++)
      (*luminaryIterator)->buildVBO();
    m_hasLuminary=true;
  }
  else
    {
      m_hasLuminary = scene->importOBJ(filename, &m_luminary);
      if(m_hasLuminary)
	for (list < Object* >::iterator luminaryIterator = m_luminary.begin ();
	     luminaryIterator  != m_luminary.end (); luminaryIterator++)
	  (*luminaryIterator)->buildVBO();
    }
  m_breakable=flameConfig->breakable;  
  
  m_intensityCoef = 0.3;
  m_visibility = true;
  m_dist=0;
  buildBoundingSphere();
}

FireSource::~FireSource()
{
  for (uint i = 0; i < m_nbFlames; i++)
    delete m_flames[i];
  delete[]m_flames;
  /* On efface le luminaire, il n'appartient pas à la scène */
  
  for (list < Object* >::iterator luminaryIterator = m_luminary.begin ();
       luminaryIterator  != m_luminary.end (); luminaryIterator++)
    delete (*luminaryIterator);
  m_luminary.clear();
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
  
  // l'intensité est calculée à partir du rapport de la longueur de la flamme (o)
  // et de la taille en y de la grille fois un coeff correcteur
  m_intensity=o.length()*(m_solver->getScale().y)*m_intensityCoef;
  
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

void FireSource::buildBoundingSphere ()
{
  Point p;
  double t,k;
  p = (m_solver->getScale() * m_solver->getDim())/2.0;
  t = p.max();
  k = t*t;
  m_boundingSphere.radius = sqrt(k+k);
  m_boundingSphere.centre = m_solver->getPosition() + p;
  //  m_boundingSphere.radius = ((getMainDirection()-getCenter()).scaleBy(m_solver->getScale())).length()+.1;
  //  m_boundingSphere.centre = getCenterSP();
}

void FireSource::computeVisibility(const Camera &view, bool forceSpheresBuild)
{  
  bool save=m_visibility;
  
  if(forceSpheresBuild) buildBoundingSphere();
  
  m_dist=m_boundingSphere.visibleDistance(view);
  m_visibility = (m_dist);
  
  if(m_visibility){
    /* Il faut prendre en compte la taille de l'objet */
    m_dist = m_dist - m_boundingSphere.radius;
    if(m_dist > 5){
//       cerr << 2000 << endl;
      setSamplingTolerance(2000);
      if(m_solver->isRealSolver())
	m_solver->switchToFakeField();
    }else{
      if(!m_solver->isRealSolver())
	m_solver->switchToRealSolver();
      if(m_dist > 3){
//  	cerr << 500 << endl;
	setSamplingTolerance(500);
      }else
	if(m_dist > 2){
//  	  cerr << 60 << endl;
	  setSamplingTolerance(40);
	}else{
//  	  cerr << 25 << endl;
	  setSamplingTolerance(20);
	}
    }
    if(!save)
      m_solver->setRunningState(true);
  }else
    if(!m_visibility && save)
      m_solver->setRunningState(false);
}

DetachableFireSource::DetachableFireSource(const FlameConfig* const flameConfig, Field3D* const s, uint nbFlames, 
					   Scene* const scene, const char *filename, const wxString &texname, 
					   uint index, const CgSVShader* const shader, const char *objName) : 
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

void DetachableFireSource::drawFlame(bool display, bool displayParticle, bool displayBoundingSphere) const
{
  if(m_visibility)
    if(displayBoundingSphere)
      m_boundingSphere.draw();
    else{
      Point pt(m_solver->getPosition());
      Point scale(m_solver->getScale());
      glPushMatrix();
      glTranslatef (pt.x, pt.y, pt.z);
      glScalef (scale.x, scale.y, scale.z);
      for (uint i = 0; i < m_nbFlames; i++)
	m_flames[i]->drawFlame(display, displayParticle);
      for (list < DetachedFlame* >::const_iterator flamesIterator = m_detachedFlamesList.begin ();
	   flamesIterator != m_detachedFlamesList.end();  flamesIterator++)
	(*flamesIterator)->drawFlame(display, displayParticle);
      glPopMatrix();
    }
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

void DetachableFireSource::setSmoothShading (bool state)
{
  FireSource::setSmoothShading(state);
  for (list < DetachedFlame* >::const_iterator flamesIterator = m_detachedFlamesList.begin ();
       flamesIterator != m_detachedFlamesList.end();  flamesIterator++)
    (*flamesIterator)->setSmoothShading(state);
}

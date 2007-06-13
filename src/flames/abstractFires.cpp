#include "abstractFires.hpp"

#include "../scene/scene.hpp"

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FLAMELIGHT ****************************************/
/**********************************************************************************************************************/

FlameLight::FlameLight(const Scene* const scene, uint index, const GLSLProgram* const program, const char* const IESFilename)
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
  default : m_light = GL_LIGHT0; break;
  }
  
  m_lightPosition[3] = 1.0;
  m_SVProgram = program;  
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
  float coef = 1.5*m_intensity;
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

void FlameLight::drawShadowVolume (GLfloat fatness[4], GLfloat extrudeDist[4])
{
  m_SVProgram->enable();
  m_SVProgram->setUniform4f("LightPos",m_lightPosition);
  m_SVProgram->setUniform4f("Fatness",fatness);
  m_SVProgram->setUniform4f("ShadowExtrudeDist",extrudeDist);
  
  m_scene->drawSceneWSV();
  m_SVProgram->disable();
}

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FIRESOURCE ****************************************/
/**********************************************************************************************************************/

FireSource::FireSource(const FlameConfig& flameConfig, Field3D* const s, uint nbFlames, Scene* const scene,
		       const wxString &texname, uint index, const GLSLProgram* const program) : 
  FlameLight(scene, index, program, flameConfig.IESFileName.ToAscii()),
  m_texture(texname, GL_CLAMP, GL_REPEAT)
{ 
  m_solver = s;
  
  m_nbFlames=nbFlames;
  if(m_nbFlames) m_flames = new RealFlame* [m_nbFlames];
  
  m_intensityCoef = 0.3;
  m_visibility = true;
  m_dist=0;
  buildBoundingSphere();
  m_flickSave=-1;
  m_moduloSave=0;
}

FireSource::~FireSource()
{
  for (uint i = 0; i < m_nbFlames; i++)
    delete m_flames[i];
  delete[]m_flames;
}

void FireSource::build()
{
  Point averagePos;
  Vector averageVec;
  
  if(!m_visibility) return;
  
  for (uint i = 0; i < m_nbFlames; i++){
    m_flames[i]->build();
    averagePos += m_flames[i]->getCenter ();
    averageVec += m_flames[i]->getMainDirection ();
  }
  averagePos *= m_solver->getScale();
  m_center = averagePos/m_nbFlames;
  averagePos = m_center + getPosition();
  setLightPosition(averagePos);
  
  m_direction = averageVec/m_nbFlames;
}

void FireSource::computeIntensityPositionAndDirection()
{
  //  float r,y;
  
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
//   r = (float)o.length();
//   if(r - fabs(y) < EPSILON)
//     m_orientationSPtheta = 0.0;
//   else
//     m_orientationSPtheta=acos(y / r)*180.0/M_PI;
}

void FireSource::buildBoundingSphere ()
{
  Point p;
  float t,k,s;
  p = (m_solver->getScale() * m_solver->getDim())/2.0;
  t = p.max();
  k = t*t;
  s = sqrt(k+k);
  
  m_boundingSphere.radius = sqrt(s+k);
  /* Augmentation de 10% du rayon pour prévenir l'apparition des flammes */
//   m_boundingSphere.radius *= 1.1;
  m_boundingSphere.centre = m_solver->getPosition() + p;
  //  m_boundingSphere.radius = ((getMainDirection()-getCenter()).scaleBy(m_solver->getScale())).length()+.1;
  //  m_boundingSphere.centre = getCenterSP();
}

void FireSource::computeVisibility(const Camera &view, bool forceSpheresBuild)
{  
  bool vis_save=m_visibility;
  uint modulo, remainder;
  int mod;
  const uint INCREMENT=4;
  
  if(forceSpheresBuild)
    buildBoundingSphere();
  
  m_dist=m_boundingSphere.visibleDistance(view);
  m_visibility = (m_dist);
  
  if(m_visibility){
    if(!vis_save){
      cerr << "solver " << m_light - GL_LIGHT0 << " launched" << endl;
      m_solver->setRunningState(true);
    }
    /* Il faut prendre en compte la taille de l'objet */
    m_dist = m_dist - m_boundingSphere.radius;
    
    remainder = ((uint)nearbyint(m_dist)) % INCREMENT;
    
    if(!remainder){
      modulo = ((uint)floor(m_dist))/INCREMENT;
      
      if(modulo > m_moduloSave)
	{
	  mod=modulo-m_moduloSave;
	  do
	    {
	      /* On change le niveau de détail des solveurs à mi-distance */
	      if( (modulo-mod) == 1 ){
		cerr << "simplified skeletons" << endl;
		for (uint i = 0; i < m_nbFlames; i++)
		  m_flames[i]->setSkeletonsLOD(SIMPLIFIED);
		setSamplingTolerance(1);
	      }
	      
	      /* On passe en FakeField */
	      if( (modulo-mod) == 3 ){
		m_solver->switchToFakeField();
		setSamplingTolerance(2);
	      }
	      mod--;
	    }while(mod>0);
	}
      else
	if(modulo < m_moduloSave)
	  {
	    mod=m_moduloSave-modulo;
	    do
	      {
		/* On change le niveau de détail des solveurs à mi-distance */
		if( (m_moduloSave-mod) == 1 ){
		  cerr << "normal skeletons" << endl;
		  for (uint i = 0; i < m_nbFlames; i++)
		    m_flames[i]->setSkeletonsLOD(NORMAL);
		  setSamplingTolerance(0);
		}
	      
		/* On repasse en solveur */
		if( (m_moduloSave-mod) == 3 ){
		  m_solver->switchToRealSolver();
		  setSamplingTolerance(1);
		}
		mod--;
	      }while(mod>0);
	  }
      m_moduloSave=modulo;
    }
  }else
    if(vis_save){
      cerr << "solver " << m_light - GL_LIGHT0 << " stopped" << endl;
      m_solver->setRunningState(false);
    }
}

bool FireSource::operator<(const FireSource& other) const{
  return (m_dist < other.m_dist);
}

/**********************************************************************************************************************/
/******************************** IMPLEMENTATION DE LA CLASSE DETACHABLEFIRESOURCE ************************************/
/**********************************************************************************************************************/

DetachableFireSource::DetachableFireSource(const FlameConfig& flameConfig, Field3D* const s, uint nbFlames, 
					   Scene* const scene, 
					   const wxString &texname, uint index, const GLSLProgram* const program) : 
  FireSource (flameConfig, s, nbFlames, scene, texname, index, program)
{
}

DetachableFireSource::~DetachableFireSource()
{
  for (list < DetachedFlame* >::iterator flamesIterator = m_detachedFlamesList.begin ();
       flamesIterator != m_detachedFlamesList.end();  flamesIterator++)
    delete (*flamesIterator);
  m_detachedFlamesList.clear ();
}

void DetachableFireSource::drawFlame(bool display, bool displayParticle, u_char boundingVolume) const
{
  switch(boundingVolume){
  case 1 : drawBoundingSphere(); break;
  case 2 : drawBoundingBox(); break;
  default : 
    if(m_visibility)
      {
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
    break;
  }
}

void DetachableFireSource::build()
{
  Point averagePos, tmp;
  Vector averageVec;
  DetachedFlame* flame;
  
  if(!m_visibility) return;
  
  for (uint i = 0; i < m_nbFlames; i++){
    m_flames[i]->breakCheck();
    m_flames[i]->build();
    averagePos += m_flames[i]->getCenter ();
    averageVec += m_flames[i]->getMainDirection ();
  }
  averagePos *= m_solver->getScale();
  m_center = averagePos/m_nbFlames;
  averagePos = m_center + getPosition();
  setLightPosition(averagePos);
  
  m_direction = averageVec/m_nbFlames;
  
  /* Destruction des flammes détachées en fin de vie */
  list < DetachedFlame* >::iterator flamesIterator = m_detachedFlamesList.begin ();
  while( flamesIterator != m_detachedFlamesList.end()){
    if(!(*flamesIterator)->build()){
      flame = *flamesIterator;
      flamesIterator = m_detachedFlamesList.erase(flamesIterator);
      delete flame;
    }else
      flamesIterator++;
  }

  Point ptMax(DBL_MIN, DBL_MIN, DBL_MIN), ptMin(DBL_MAX, DBL_MAX, DBL_MAX);
  Point pt;
  Point p;
  float t,k;
  p = (m_solver->getScale() * m_solver->getDim())/2.0;
  t = p.max();
  k = t*t;
  
  /* Calcul de la bouding sphere pour les flammes détachées */
  if( m_detachedFlamesList.size () )
    for (list < DetachedFlame* >::const_iterator flamesIterator = m_detachedFlamesList.begin ();
	 flamesIterator != m_detachedFlamesList.end();  flamesIterator++){
      pt = (*flamesIterator)->getTop();
      if(pt.x > ptMax.x)
	ptMax.x = pt.x;
      if(pt.y > ptMax.y)
	ptMax.y = pt.y;
      if(pt.z > ptMax.z)
	ptMax.z = pt.z;
      pt = (*flamesIterator)->getBottom();
      if(pt.x < ptMin.x)
      ptMin.x = pt.x;
      if(pt.y < ptMin.y)
	ptMin.y = pt.y;
      if(pt.z < ptMin.z)
	ptMin.z = pt.z;
      ptMin *= m_solver->getScale();
      ptMax *= m_solver->getScale();
      m_boundingSphere.radius = (sqrt(k+k) + ptMax.distance(ptMin));
      m_boundingSphere.centre = m_solver->getPosition() + (p + (ptMax + ptMin)/2.0)/2.0;
    }else{
    m_boundingSphere.radius = sqrt(k+k);
    m_boundingSphere.centre = m_solver->getPosition() + p;
  }
  /* Calcul de la bouding box pour affichage */
  buildBoundingBox ();
}

void DetachableFireSource::setSmoothShading (bool state)
{
  FireSource::setSmoothShading(state);
  for (list < DetachedFlame* >::const_iterator flamesIterator = m_detachedFlamesList.begin ();
       flamesIterator != m_detachedFlamesList.end();  flamesIterator++)
    (*flamesIterator)->setSmoothShading(state);
}

void DetachableFireSource::computeVisibility(const Camera &view, bool forceSpheresBuild)
{  
  bool vis_save=m_visibility;
  uint modulo, remainder;
  int mod;
  const uint INCREMENT=4;
  
  if(forceSpheresBuild)
    buildBoundingSphere();
  
  m_dist=m_boundingSphere.visibleDistance(view);
  m_visibility = (m_dist);
  
  if(m_visibility){
    if(!vis_save){
      cerr << "solver " << m_light - GL_LIGHT0 << " launched" << endl;
      m_solver->setRunningState(true);
    }
    /* Il faut prendre en compte la taille de l'objet */
    m_dist = m_dist - m_boundingSphere.radius;
    
    remainder = ((uint)nearbyint(m_dist)) % INCREMENT;
    
    if(!remainder){
      modulo = ((uint)floor(m_dist))/INCREMENT;
      
      if(modulo > m_moduloSave)
	{      
	  cerr << "solver " << m_light - GL_LIGHT0 << " : ";
	  mod=modulo-m_moduloSave;
	  do
	    {
	      /* On change le niveau de détail des solveurs à mi-distance */
	      if( (modulo-mod) == 1 ){
		cerr << "simplified skeletons" << endl;
		for (uint i = 0; i < m_nbFlames; i++)
		  m_flames[i]->setSkeletonsLOD(SIMPLIFIED);
		setSamplingTolerance(2);
	      }
	    
	      /* On passe en FakeField */
	      if( (modulo-mod) == m_solver->getNbMaxDiv() ){
		if(getPerturbateMode() != FLICKERING_NOISE){
		  m_flickSave = getPerturbateMode();
		  setPerturbateMode(FLICKERING_NOISE);
		}
		m_solver->switchToFakeField();
	      }else
		if( (modulo-mod) >= 0 && (modulo-mod) < m_solver->getNbMaxDiv())
		  m_solver->decreaseRes();
	    
	      mod--;
	    }while(mod>0);
	}
      else
	if(modulo < m_moduloSave){
	  mod=m_moduloSave-modulo;
	  cerr << "solver " << m_light - GL_LIGHT0 << " : ";
	  do
	    {
	      /* On change le niveau de détail des solveurs à mi-distance */
	      if( (modulo+mod) == 2 ){
		cerr << "normal skeletons" << endl;
		for (uint i = 0; i < m_nbFlames; i++)
		  m_flames[i]->setSkeletonsLOD(NORMAL);
		setSamplingTolerance(1);
	      }
	      
	      if( (m_moduloSave-mod) < m_solver->getNbMaxDiv() )
		m_solver->increaseRes();
	      else
		if( (m_moduloSave-mod) == m_solver->getNbMaxDiv() ){
		  if(m_flickSave > -1){
		    setPerturbateMode(m_flickSave);
		    m_flickSave = -1;
		  }
		  m_solver->switchToRealSolver();
		}
	      mod--;
	    }while(mod>0);
	}
      m_moduloSave=modulo;
    }
  }else
    if(vis_save){
      cerr << "solver " << m_light - GL_LIGHT0 << " stopped" << endl;
      m_solver->setRunningState(false);
    }
}

void DetachableFireSource::buildBoundingBox ()
{
  Point ptMax(DBL_MIN, DBL_MIN, DBL_MIN), ptMin(DBL_MAX, DBL_MAX, DBL_MAX);
  Point pt;
  Point pos(m_solver->getPosition());
  
  if( m_detachedFlamesList.size () )
    for (list < DetachedFlame* >::const_iterator flamesIterator = m_detachedFlamesList.begin ();
	 flamesIterator != m_detachedFlamesList.end();  flamesIterator++){
      pt = (*flamesIterator)->getTop();
      if(pt.x > ptMax.x)
	ptMax.x = pt.x;
      if(pt.y > ptMax.y)
	ptMax.y = pt.y;
      if(pt.z > ptMax.z)
	ptMax.z = pt.z;
      pt = (*flamesIterator)->getBottom();
      if(pt.x < ptMin.x)
	ptMin.x = pt.x;
      if(pt.y < ptMin.y)
	ptMin.y = pt.y;
      if(pt.z < ptMin.z)
	ptMin.z = pt.z;
    }
  pt = m_solver->getDim();
  if(pt.x > ptMax.x)
    ptMax.x = pt.x;
  if(pt.y > ptMax.y)
    ptMax.y = pt.y;
  if(pt.z > ptMax.z)
    ptMax.z = pt.z;
  if(0.0 < ptMin.x)
    ptMin.x = 0.0;
  if(0.0 < ptMin.y)
    ptMin.y = 0.0;
  if(0.0 < ptMin.z)
    ptMin.z = 0.0;
  
  m_BBmin = ptMin * m_solver->getScale();
  m_BBmax = ptMax * m_solver->getScale();
}

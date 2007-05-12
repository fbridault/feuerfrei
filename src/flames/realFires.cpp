#include "realFires.hpp"

#include "../scene/graphicsFn.hpp"
#include "../scene/scene.hpp"
#include "../solvers/fakeField3D.hpp"

#define WICK_NAME_PREFIX "Wick"
#define TORCH_NAME "Torch"

Candle::Candle (FlameConfig* const flameConfig, Field3D * s, Scene *scene, const char *filename, uint index, 
		const GLSLProgram * const program, double rayon):
  FireSource (flameConfig, s, 1, scene, filename, _("textures/bougie2.png"), index, program)
{
  m_flames[0] = new PointFlame(flameConfig, &m_texture, s, rayon);
}

Firmalampe::Firmalampe(FlameConfig* const flameConfig, Field3D * s, Scene *scene, const char *filename, uint index,
		       const GLSLProgram * const program, const char *wickFileName):
  FireSource (flameConfig, s, 1, scene, filename, _("textures/firmalampe.png"), index, program)
{
  list<Wick *> objList;
  
  scene->importOBJ(wickFileName, NULL, &objList);
  
  if(objList.size() > 0)
    m_flames[0] = new LineFlame( flameConfig, scene, &m_texture, s, (*objList.begin()), 0.01);
}

Torch::Torch(FlameConfig* const flameConfig, Field3D * s, Scene *scene, const char *torchName, uint index,
	     const GLSLProgram * const program):
  DetachableFireSource (flameConfig, s, 0, scene, torchName, _("textures/torch6.png"), index, program, TORCH_NAME)
{
  list<Wick *> objList;
  int i=0;
  
  scene->importOBJ(torchName, NULL, &objList, WICK_NAME_PREFIX);
  
  m_nbFlames = objList.size();
  m_flames = new RealFlame* [m_nbFlames];
  
  for (list <Wick *>::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      m_flames[i] = new LineFlame( flameConfig, scene, &m_texture, s, (*objListIterator), 0.1, this);
    }
}

CampFire::CampFire(FlameConfig* const flameConfig, Field3D * s, Scene *scene, const char *fireName, uint index, 
		   const GLSLProgram * const program):
  DetachableFireSource (flameConfig, s, 0, scene, fireName, _("textures/torch4.png"), index, program, TORCH_NAME)
{
  list<Wick *> objList;
  int i=0;
  
  scene->importOBJ(fireName, NULL, &objList, WICK_NAME_PREFIX);
  
  m_nbFlames = objList.size();
  m_flames = new RealFlame* [m_nbFlames];
  
  for (list <Wick *>::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      m_flames[i] = new LineFlame(flameConfig, scene, &m_texture, s, (*objListIterator), 0.04, this);
    }
}

CandlesSet::CandlesSet(FlameConfig* const flameConfig, Field3D *s, list <FieldFlamesThread *>& fieldThreads, Scene *scene,
		       const char *lampName, uint index, const GLSLProgram * const program, Point scale):
  FireSource (flameConfig, s, 0, scene, lampName, _("textures/bougie2.png"), index, program, "Lamp")
{
  list<Object *> objList;
  int i=0;
  
  scene->importOBJ(lampName, &objList, NULL, WICK_NAME_PREFIX);
  
  m_nbFlames = objList.size();
  m_flames = new RealFlame* [m_nbFlames];
  
  for (list < Object *>::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      Point pt;
      FieldFlamesAssociation *fieldFlamesAssociation;
      
      /* Le field sera supprimé par le destructeur de FieldFlamesAssociation */
      Field3D *field =  new FakeField3D(pt, 10, 10, 10, 1.0, Point(.08,.08,.08), .4, 0.3);
      
      fieldFlamesAssociation = new FieldFlamesAssociation(field);
      m_flames[i] = new PointFlame( flameConfig, &m_texture, field, .4, scene, (*objListIterator));
      m_flames[i]->buildBoundingSphere( s->getPosition() );
      fieldFlamesAssociation->addFlameSource(m_flames[i]);    
      m_fieldFlamesAssociations.push_back(fieldFlamesAssociation);
      /* Instanciation des threads : 1 solveur = 1 thread */
      fieldThreads.push_back(new FieldFlamesThread(fieldFlamesAssociation));
    }
}

CandlesSet::~CandlesSet()
{
  for (list < FieldFlamesAssociation* >::iterator ffaIterator = m_fieldFlamesAssociations.begin ();
       ffaIterator != m_fieldFlamesAssociations.end (); ffaIterator++)
    delete (*ffaIterator);
  m_fieldFlamesAssociations.clear();
}

void CandlesSet::build()
{
  Point averagePos;
  
  /* On ne construit pas les flammes, ceci est fait dans des threads séparés. */
  for (uint i = 0; i < m_nbFlames; i++)
    averagePos += m_flames[i]->getCenter ();
  
  averagePos *= m_solver->getScale();
  averagePos /= m_nbFlames;
  averagePos += getPosition();
  setLightPosition(averagePos);
}

void CandlesSet::computeVisibility(const Camera &view, bool forceSpheresBuild)
{  
  //  bool save=m_visibility;
  
  /* Si la flamme n'est pas visible, il ne faut pas recalculer la sphère car le solveur est arrêté ! */
  /* On est assuré de calculer la sphère la première fois car m_visibility est initialisé à true */
  // if(m_visibility || forceSpheresBuild) buildBoundingSphere();
  
//   m_dist=m_boundingSphere.visibleDistance(view);
//   m_visibility = (m_dist);
  
//   if(m_visibility){
//     if(!save)
//       m_solver->setRunningState(true);
//   }else
//     if(!m_visibility && save)
//       m_solver->setRunningState(false);
  for (uint i = 0; i < m_nbFlames; i++)
    m_flames[i]->computeVisibility(view, m_solver->getPosition(), forceSpheresBuild);
  
  sort(m_flames,m_flames+m_nbFlames,RealFlame::cmp);
}
  

CandleStick::CandleStick (FlameConfig* const flameConfig, Field3D * s, Scene *scene, const char *filename, uint index, 
			  const GLSLProgram * const program, double rayon):
  FireSource (flameConfig, s, 1, scene, filename, _("textures/bougie2.png"), index, program)
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

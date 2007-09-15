#include "fakeField3D.hpp"

#include <math.h>
#include "../scene/graphicsFn.hpp"

FakeField3D::FakeField3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, 
			  float timeStep, float buoyancy) : 
  Field3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy)
{
  m_forceCoef = 3.0f;
  m_coef = m_dt * m_dt * m_forceCoef;
  buildDLGrid();
}

FakeField3D::~FakeField3D ()
{
}

void FakeField3D::iterate ()
{
  float coef=.5;
  if(!m_run)
    return;
  
  if(m_permanentExternalForces.x || m_permanentExternalForces.y || m_permanentExternalForces.z){
    if(m_permanentExternalForces.x > 0.0f)
      if(m_latentForces.x < m_permanentExternalForces.x)
	m_latentForces.x = m_latentForces.x + m_permanentExternalForces.x * coef;
      else
	m_latentForces.x = m_latentForces.x - m_permanentExternalForces.x * coef;
    if(m_permanentExternalForces.y > 0.0f)
      if(m_latentForces.y < m_permanentExternalForces.y)
	m_latentForces.y = m_latentForces.y + m_permanentExternalForces.y * coef;
      else
	m_latentForces.y = m_latentForces.y - m_permanentExternalForces.y * coef;	
    if(m_permanentExternalForces.z > 0.0f)
      if(m_latentForces.z < m_permanentExternalForces.z)
	m_latentForces.z = m_latentForces.z + m_permanentExternalForces.z * coef;
      else
	m_latentForces.z = m_latentForces.z - m_permanentExternalForces.z * coef;
    
    if(m_permanentExternalForces.x < 0.0f)
      if(m_latentForces.x > m_permanentExternalForces.x)
	m_latentForces.x = m_latentForces.x + m_permanentExternalForces.x * coef;
      else
	m_latentForces.x = m_latentForces.x - m_permanentExternalForces.x * coef;
    if(m_permanentExternalForces.y < 0.0f)
       if(m_latentForces.y > m_permanentExternalForces.y)
	 m_latentForces.y = m_latentForces.y + m_permanentExternalForces.y * coef;
       else
	 m_latentForces.y = m_latentForces.y - m_permanentExternalForces.y * coef;
    if(m_permanentExternalForces.z < 0.0f)
       if(m_latentForces.z > m_permanentExternalForces.z)
	 m_latentForces.z = m_latentForces.z + m_permanentExternalForces.z * coef;
       else
	 m_latentForces.z = m_latentForces.z - m_permanentExternalForces.z * coef;
    
    addExternalForces(m_latentForces,false);
  }else{
    if(m_latentForces.x > 0.0f) m_latentForces.x = m_latentForces.x - coef;
    if(m_latentForces.y > 0.0f) m_latentForces.y = m_latentForces.y - coef;
    if(m_latentForces.z > 0.0f) m_latentForces.z = m_latentForces.z - coef;
    if(m_latentForces.x < 0.0f) m_latentForces.x = m_latentForces.x + coef;
    if(m_latentForces.y < 0.0f) m_latentForces.y = m_latentForces.y + coef;
    if(m_latentForces.z < 0.0f) m_latentForces.z = m_latentForces.z + coef;
    
    if(m_latentForces.x || m_latentForces.y || m_latentForces.z) addExternalForces(m_latentForces,false);
  }
  
  if(m_temporaryExternalForces.x || m_temporaryExternalForces.y || m_temporaryExternalForces.z)
    {
      addExternalForces(m_temporaryExternalForces,false);
      m_temporaryExternalForces.resetToNull();
    }
  
  if(m_movingForces.x || m_movingForces.y || m_movingForces.z)
    {
      addExternalForces(m_movingForces,true);
      m_movingForces.resetToNull();
    }
  m_nbIter++;
}

void FakeField3D::cleanSources ()
{
  m_src.x = 0.0f;
  m_src.y = 0.0f;
  m_src.z = 0.0f;
}

void FakeField3D::addExternalForces(const Point& position, bool move)
{
  Point strength;
  Point force;
  
  if(move){
    force = position;
    strength.x = force.x > 0.0f ? .2f : -.2f;
    strength.y = force.y > 0.0f ? .2f : -.2f;
    strength.z = force.z > 0.0f ? .2f : -.2f;
    setPosition(m_position + position);
  }else{
    force = position;
    strength = position * .025f;
  }
  
  /* Ajouter des forces externes */
  if(force.x)
    m_src.x -= strength.x;
  if(force.y)
    m_src.y -= strength.y;
  if(force.z)
    m_src.z -= strength.z;
}

void FakeField3D::buildDLGrid ()
{
  m_gridDisplayList=glGenLists(1);
  glNewList (m_gridDisplayList, GL_COMPILE);
  glColor4f (0.5f, 0.5f, 0.5f, 0.5f);
  glBegin (GL_LINE_LOOP);
  glVertex3f (0.0f, 0.0f, 0.0f);
  glVertex3f (0.0f, m_dim.y, 0.0f);
  glVertex3f (m_dim.x, m_dim.y, 0.0f);
  glVertex3f (m_dim.x, 0.0f, 0.0f);
  glEnd();
  glBegin (GL_LINE_LOOP);
  glVertex3f (0.0f, 0.0f, m_dim.z);
  glVertex3f (0.0f, m_dim.y, m_dim.z);
  glVertex3f (m_dim.x, m_dim.y, m_dim.z);
  glVertex3f (m_dim.x, 0.0f, m_dim.z);
  glEnd();
  glBegin (GL_LINES);
  glVertex3f (0.0f, 0.0f, 0.0f);
  glVertex3f (0.0f, 0.0f, m_dim.z);
  glVertex3f (0.0f, m_dim.y, 0.0f);
  glVertex3f (0.0f, m_dim.y, m_dim.z);
  glVertex3f (m_dim.x, m_dim.y, 0.0f);
  glVertex3f (m_dim.x, m_dim.y, m_dim.z);
  glVertex3f (m_dim.x, 0.0f, 0.0f);
  glVertex3f (m_dim.x, 0.0f, m_dim.z);
  glEnd();
  glEndList ();
}

void FakeField3D::displayVelocityField (void)
{
  float inc_x = m_dim.x / (float) m_nbVoxelsX;
  float inc_y = m_dim.y / (float) m_nbVoxelsY;
  float inc_z = m_dim.z / (float) m_nbVoxelsZ;
  
  for (uint i = 0; i <= m_nbVoxelsX; i++)
    for (uint j = 0; j <= m_nbVoxelsY; j++)
      for (uint k = 0; k <= m_nbVoxelsZ; k++)
	{
	  Vector vect;
	  Point pt(inc_x * i - inc_x/2.0f , inc_y * j - inc_y/2.0f, inc_z * k - inc_z/2.0f);
	  
	  /* Affichage du champ de vélocité */
	  glPushMatrix ();	  
	  glTranslatef (pt.x, pt.y, pt.z);
	  //SDL_mutexP (lock);
	  vect = getUVW (pt,0);
	  //SDL_mutexV (lock);
	  displayArrow (vect);
	  glPopMatrix ();
	}
}

void FakeField3D::addForcesOnFace(unsigned char face, const Point& BLStrength, const Point& TLStrength,
				  const Point& TRStrength, const Point& BRStrength)
{
  switch(face){
  case LEFT_FACE : m_temporaryExternalForces.x -= 5*(BLStrength.x+TLStrength.x+TRStrength.x+BRStrength.x)/4.0f; break;
  case RIGHT_FACE : m_temporaryExternalForces.x -= 5*(BLStrength.x+TLStrength.x+TRStrength.x+BRStrength.x)/4.0f; break;
  case BACK_FACE : m_temporaryExternalForces.z -= 5*(BLStrength.z+TLStrength.z+TRStrength.z+BRStrength.z)/4.0f; break;
  case FRONT_FACE : m_temporaryExternalForces.z -= 5*(BLStrength.z+TLStrength.z+TRStrength.z+BRStrength.z)/4.0f; break;
  }
}

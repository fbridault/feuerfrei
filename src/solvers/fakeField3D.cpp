#include "fakeField3D.hpp"

#include <math.h>
#include "../scene/graphicsFn.hpp"

FakeField3D::FakeField3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale, 
			  double timeStep, double buoyancy) : 
  Field3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy)
{
  m_forceCoef = 3;
  m_coef = m_dt * m_dt * m_forceCoef;
}

FakeField3D::~FakeField3D ()
{
}

void FakeField3D::iterate ()
{
  double coef=.5;
  if(!m_run)
    return;
  
  if(m_permanentExternalForces.x || m_permanentExternalForces.y || m_permanentExternalForces.z){
    if(m_permanentExternalForces.x > 0)
      if(m_latentForces.x < m_permanentExternalForces.x)
	m_latentForces.x = m_latentForces.x + m_permanentExternalForces.x * coef;
      else
	m_latentForces.x = m_latentForces.x - m_permanentExternalForces.x * coef;
    if(m_permanentExternalForces.y > 0)
      if(m_latentForces.y < m_permanentExternalForces.y)
	m_latentForces.y = m_latentForces.y + m_permanentExternalForces.y * coef;
      else
	m_latentForces.y = m_latentForces.y - m_permanentExternalForces.y * coef;	
    if(m_permanentExternalForces.z > 0)
      if(m_latentForces.z < m_permanentExternalForces.z)
	m_latentForces.z = m_latentForces.z + m_permanentExternalForces.z * coef;
      else
	m_latentForces.z = m_latentForces.z - m_permanentExternalForces.z * coef;
    
    if(m_permanentExternalForces.x < 0)
      if(m_latentForces.x > m_permanentExternalForces.x)
	m_latentForces.x = m_latentForces.x + m_permanentExternalForces.x * coef;
      else
	m_latentForces.x = m_latentForces.x - m_permanentExternalForces.x * coef;
    if(m_permanentExternalForces.y < 0)
       if(m_latentForces.y > m_permanentExternalForces.y)
	 m_latentForces.y = m_latentForces.y + m_permanentExternalForces.y * coef;
       else
	 m_latentForces.y = m_latentForces.y - m_permanentExternalForces.y * coef;
    if(m_permanentExternalForces.z < 0)
       if(m_latentForces.z > m_permanentExternalForces.z)
	 m_latentForces.z = m_latentForces.z + m_permanentExternalForces.z * coef;
       else
	 m_latentForces.z = m_latentForces.z - m_permanentExternalForces.z * coef;
    
    addExternalForces(m_latentForces,false);
  }else{
    if(m_latentForces.x > 0) m_latentForces.x = m_latentForces.x - coef;
    if(m_latentForces.y > 0) m_latentForces.y = m_latentForces.y - coef;
    if(m_latentForces.z > 0) m_latentForces.z = m_latentForces.z - coef;
    if(m_latentForces.x < 0) m_latentForces.x = m_latentForces.x + coef;
    if(m_latentForces.y < 0) m_latentForces.y = m_latentForces.y + coef;
    if(m_latentForces.z < 0) m_latentForces.z = m_latentForces.z + coef;
    
    if(m_latentForces.x || m_latentForces.y || m_latentForces.z) addExternalForces(m_latentForces,false);
  }
  
  if(m_temporaryExternalForces.x || m_temporaryExternalForces.y || m_temporaryExternalForces.z)
    {
      addExternalForces(m_temporaryExternalForces,true);
      m_temporaryExternalForces.resetToNull();
    }
  
  m_nbIter++;
}

void FakeField3D::cleanSources ()
{
  m_src.x = 0;
  m_src.y = 0;
  m_src.z = 0;
}

void FakeField3D::addExternalForces(const Point& position, bool move)
{
  Point strength;
  Point force;
  
  if(move){
    force = position - m_position;
    strength.x = force.x > 0 ? .2 : -.2;
    strength.y = force.y > 0 ? .2 : -.2;
    strength.z = force.z > 0 ? .2 : -.2;
    m_position=position;
  }else{
    force = position;
    strength = position * .025;
  }
  
  /* Ajouter des forces externes */
  if(force.x)
    m_src.x -= strength.x;
  if(force.y)
    m_src.y -= strength.y;
  if(force.z)
    m_src.z -= strength.z;
}

void FakeField3D::displayVelocityField (void)
{
  double inc_x = m_dim.x / (double) m_nbVoxelsX;
  double inc_y = m_dim.y / (double) m_nbVoxelsY;
  double inc_z = m_dim.z / (double) m_nbVoxelsZ;
  
  for (uint i = 0; i <= m_nbVoxelsX; i++)
    for (uint j = 0; j <= m_nbVoxelsY; j++)
      for (uint k = 0; k <= m_nbVoxelsZ; k++)
	{
	  Vector vect;
	  Point pt(inc_x * i - inc_x/2.0 , inc_y * j - inc_y/2.0, inc_z * k - inc_z/2.0);
	  
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
  case LEFT_FACE : m_src.x += (BLStrength.x+TLStrength.x+TRStrength.x+BRStrength.x)/4.0; break;
  case RIGHT_FACE : m_src.x += (BLStrength.x+TLStrength.x+TRStrength.x+BRStrength.x)/4.0; break;
  case BACK_FACE : m_src.z += (BLStrength.z+TLStrength.z+TRStrength.z+BRStrength.z)/4.0; break;
  case FRONT_FACE : m_src.z += (BLStrength.z+TLStrength.z+TRStrength.z+BRStrength.z)/4.0; break;
  }
}

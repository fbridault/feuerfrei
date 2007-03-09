#include "fakeField3D.hpp"

#include <math.h>
#include "../scene/graphicsFn.hpp"

FakeField3D::FakeField3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale, 
			  double timeStep, double buoyancy) : 
  Field3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy)
{
  m_uSrc = 0;
  m_vSrc = 0;
  m_wSrc = 0;
}

FakeField3D::~FakeField3D ()
{
}

void FakeField3D::iterate ()
{
  if(!m_run)
    return;
  
  if(arePermanentExternalForces)
    addExternalForces(permanentExternalForces,false);
  
  m_nbIter++;
}

void FakeField3D::cleanSources ()
{
  m_uSrc = 0;
  m_vSrc = 0;
  m_wSrc = 0;
}

void FakeField3D::addExternalForces(const Point& position, bool move)
{
  Point strength;
  Point force;
  
  if(move){
    force = position - m_position;
    strength.x = strength.y = strength.z = .001;
    m_position=position;
  }else{
    force = position;
    strength = position * .1;
  }
  
  /* Ajouter des forces externes */
  if(force.x)
    m_uSrc -= strength.x;
  if(force.y)
    m_vSrc -= strength.y;
  if(force.z)
    m_wSrc -= strength.z;
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

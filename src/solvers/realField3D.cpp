#include "realField3D.hpp"

#include <math.h>

RealField3D::RealField3D ()
{
}

RealField3D::RealField3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale,
			  double timeStep, double buoyancy) : 
  Field3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy)
{
  m_u = new double[m_nbVoxels];
  m_v = new double[m_nbVoxels];
  m_w = new double[m_nbVoxels];
  m_uSrc = new double[m_nbVoxels];
  m_vSrc = new double[m_nbVoxels];
  m_wSrc = new double[m_nbVoxels];
  
  memset (m_u, 0, m_nbVoxels * sizeof (double));
  memset (m_v, 0, m_nbVoxels * sizeof (double));
  memset (m_w, 0, m_nbVoxels * sizeof (double));
  memset (m_uSrc, 0, m_nbVoxels * sizeof (double));
  memset (m_vSrc, 0, m_nbVoxels * sizeof (double));
  memset (m_wSrc, 0, m_nbVoxels * sizeof (double));
}

RealField3D::~RealField3D ()
{
  delete[]m_u;
  delete[]m_v;
  delete[]m_w;

  delete[]m_uSrc;
  delete[]m_vSrc;
  delete[]m_wSrc;
}

void RealField3D::vel_step ()
{
  add_source (m_u, m_uSrc);
  add_source (m_v, m_vSrc);
  add_source (m_w, m_wSrc);
}

void RealField3D::iterate ()
{ 
  double tmp;
  
  if(!m_run)
    return;
  m_u = (double *) memset (m_u, 0, m_nbVoxels * sizeof (double));
  m_v = (double *) memset (m_v, 0, m_nbVoxels * sizeof (double));
  m_w = (double *) memset (m_w, 0, m_nbVoxels * sizeof (double));
  /* Cellule(s) génératrice(s) */
  for (uint j = 1; j < m_nbVoxelsY + 1; j++){
    tmp = m_buoyancy * j/m_nbVoxelsY;
    for (uint i = 1; i < m_nbVoxelsX + 1; i++)
      for (uint k = 1; k < m_nbVoxelsZ + 1; k++)
	m_vSrc[IX(i,j,k)] += tmp;
  }
  
  if(m_permanentExternalForces.x || m_permanentExternalForces.y || m_permanentExternalForces.z)
    addExternalForces(m_permanentExternalForces,false);
  
  if(m_movingForces.x || m_movingForces.y || m_movingForces.z)
    {
      addExternalForces(m_movingForces,true);
      m_movingForces.resetToNull();
    }
  if(m_temporaryExternalForces.x || m_temporaryExternalForces.y || m_temporaryExternalForces.z)
    {
      addExternalForces(m_temporaryExternalForces,true);
      m_temporaryExternalForces.resetToNull();
    }
  
  vel_step ();
  
  m_nbIter++;
}

void RealField3D::cleanSources ()
{
  m_uSrc = (double *) memset (m_uSrc, 0, m_nbVoxels * sizeof (double));
  m_vSrc = (double *) memset (m_vSrc, 0, m_nbVoxels * sizeof (double));
  m_wSrc = (double *) memset (m_wSrc, 0, m_nbVoxels * sizeof (double));
}

void RealField3D::addExternalForces(const Point& position, bool move)
{
  Point strength;
  Point force;
  
  if(move){
    force = position;
    strength.x = strength.y = strength.z = 1;
    setPosition(m_position + position);
  }else{
    force = position;
    strength = position * .1;
  }
    
  /* Ajouter des forces externes */
  if(force.x)
    for (uint i = 1; i < m_nbVoxelsX + 1; i++)
      for (uint j = 1; j < m_nbVoxelsY + 1; j++)
	for (uint k = 1; k < m_nbVoxelsZ + 1; k++)
	  m_uSrc[IX(i, j, k)] += strength.x*j/(double)m_nbVoxelsY;
  if(force.y)
    for (uint i = 1; i < m_nbVoxelsX + 1; i++)
      for (uint j = 1; j < m_nbVoxelsY + 1; j++)
	for (uint k = 1; k < m_nbVoxelsZ + 1; k++)
	  m_vSrc[IX(i, j, k)] += strength.y;
  if(force.z)
    for (uint i = 1; i < m_nbVoxelsX + 1; i++)
      for (uint j = 1; j < m_nbVoxelsY + 1; j++)
	for (uint k = 1; k < m_nbVoxelsZ + 1; k++)
	  m_wSrc[IX(i, j, k)] += strength.z*j/(double)m_nbVoxelsY;
}

void RealField3D::addForcesOnFace(unsigned char face, const Point& BLStrength, const Point& TLStrength,
				  const Point& TRStrength, const Point& BRStrength)
{
  switch(face){
  case LEFT_FACE : ;
  case RIGHT_FACE : ;
  case BACK_FACE : ;
  case FRONT_FACE : ;
  }
}

void RealField3D::displayVelocityField (void)
{
  double inc_x = m_dim.x / (double) m_nbVoxelsX;
  double inc_y = m_dim.y / (double) m_nbVoxelsY;
  double inc_z = m_dim.z / (double) m_nbVoxelsZ;
  
  for (uint i = 0; i <= m_nbVoxelsX; i++)
    for (uint j = 0; j <= m_nbVoxelsY; j++)
      for (uint k = 0; k <= m_nbVoxelsZ; k++)
	{
	  uint index = IX(i,j,k);
	  /* Affichage du champ de vélocité */
	  glPushMatrix ();
	  glTranslatef (inc_x * i - inc_x/2.0 , inc_y * j - inc_y/2.0, inc_z * k - inc_z/2.0);
	  //SDL_mutexP (lock);
	  //SDL_mutexV (lock);
	  displayArrow (Vector (m_u[index], m_v[index], m_w[index]));
	  glPopMatrix ();
	}
}

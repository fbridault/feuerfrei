#include "field.hpp"

Field::Field()
{
}

Field::Field (const Point& position, double timeStep, double buoyancy) : m_position(position)
{
  m_dt = timeStep;
  
  m_nbIter = 0;
  
  m_buoyancy=buoyancy;

  arePermanentExternalForces = false;
  m_forceCoef = 2;
  m_forceRatio = 1/m_forceCoef;
}
 
Field::~Field ()
{
  glDeleteLists(m_baseDisplayList,1);
  glDeleteLists(m_gridDisplayList,1);
}

void Field::add_source (double *const x, double *const src)
{
  uint i;
  
  for (i = 0; i < m_nbVoxels; i++)
    x[i] += m_dt * src[i] * m_forceCoef;
}
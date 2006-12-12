#include "field.hpp"

Field::Field()
{
}

Field::Field (Point& position, double timeStep, double buoyancy) : m_position(position)
{
  m_dt = timeStep;
  
  m_nbIter = 0;
  
  m_buoyancy=buoyancy;

  arePermanentExternalForces = false;
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
    x[i] += m_dt * src[i] * 10;
}

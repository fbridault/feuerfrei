#include "solver2D.hpp"

#include <math.h>
#include "../scene/graphicsFn.hpp"

Solver::Solver ()
{
}

Solver::Solver (Point& position, double timeStep, double buoyancy) : m_position(position)
{
  m_dt = timeStep;
  
  m_nbSteps = 20;
  m_nbIter = 0;
  
  m_buoyancy=buoyancy;

  arePermanentExternalForces = false;
}

Solver::~Solver ()
{
  glDeleteLists(m_baseDisplayList,1);
  glDeleteLists(m_gridDisplayList,1);
}

void Solver::add_source (double *const x, double *const src)
{
  uint i;
  
  for (i = 0; i < m_nbVoxels; i++)
    x[i] += m_dt * src[i];
}

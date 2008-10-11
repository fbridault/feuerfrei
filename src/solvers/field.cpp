#include "field.hpp"

Field::Field()
{
}

Field::Field (const CPoint& position, float timeStep, float buoyancy) : m_position(position)
{
  m_dt = timeStep;

  m_nbIter = 0;

  m_buoyancy=buoyancy;

  m_forceCoef = 2;
  m_forceRatio = 1/m_forceCoef;
  m_run = true;
  /* Affectation surchargée dans Solver3D::Solver3D(...) */
  m_vorticityConfinement = 0.0f;
}

Field::~Field ()
{
  glDeleteLists(m_baseDisplayList,1);
  glDeleteLists(m_gridDisplayList,1);
}

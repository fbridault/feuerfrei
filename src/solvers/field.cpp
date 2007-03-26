#include "field.hpp"

Field::Field()
{
}

Field::Field (const Point& position, double timeStep, double buoyancy) : m_position(position)
{
  m_dt = timeStep;
  
  m_nbIter = 0;
  
  m_buoyancy=buoyancy;
  
  m_forceCoef = 2;
  m_forceRatio = 1/m_forceCoef;
  m_run = true;
}
 
Field::~Field ()
{
  glDeleteLists(m_baseDisplayList,1);
  glDeleteLists(m_gridDisplayList,1);
}

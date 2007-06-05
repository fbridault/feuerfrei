#include "LODField3D.hpp"

LODField3D::LODField3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, float timeStep,
			float buoyancy, float omegaDiff, float omegaProj, float epsilon) : 
  Field3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy),
  m_fakeField(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy),
  m_solver(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy, omegaDiff, omegaProj, epsilon)
{
  m_currentField = &m_solver;
}

LODHybridField::LODHybridField (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, float timeStep,
			    float buoyancy, float omegaDiff, float omegaProj, float epsilon) : 
  LODField3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy, omegaDiff, omegaProj, epsilon)
{
  m_switch = 0;
  m_increaseCount = 0;
  m_decreaseCount = 0;
}

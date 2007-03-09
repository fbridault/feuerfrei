#include "LODField3D.hpp"

LODField3D::LODField3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale, double timeStep,
			double buoyancy, double omegaDiff, double omegaProj, double epsilon) : 
  Field3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy),
  m_fakeField(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy),
  m_solver(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy, omegaDiff, omegaProj, epsilon)
{
  m_currentField = &m_solver;
}

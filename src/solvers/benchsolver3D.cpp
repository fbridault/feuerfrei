#include "benchsolver3D.hpp"

BenchSolver3D::BenchSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, float timeStep,
			      float buoyancy, float vorticityConfinement, uint nbTimeSteps, float omegaDiff, float omegaProj, 
			      float epsilon) : 
  Solver3D (position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy, vorticityConfinement),
  GCSSORSolver3D(omegaDiff, omegaProj, epsilon)
{
  m_save = new float[m_nbVoxels];
  m_save2 = new float[m_nbVoxels];
  
  fill_n(m_save, m_nbVoxels, 0.0f);
  fill_n(m_save2, m_nbVoxels, 0.0f);
  
  m_nbSteps = 40;
  m_nbMaxIter = nbTimeSteps;
}

/* Le constructeur de GSsolver n'a pas de paramètre, il n'est donc pas appelé explicitement */
BenchSolver3D::BenchSolver3D (uint nbTimeSteps, float omegaDiff, float omegaProj, float epsilon) : 
  GCSSORSolver3D(omegaDiff, omegaProj, epsilon)
{
  m_save = new float[m_nbVoxels];
  m_save2 = new float[m_nbVoxels];
  
  fill_n(m_save, m_nbVoxels, 0.0f);
  fill_n(m_save2, m_nbVoxels, 0.0f);
  
  m_nbSteps = 40;
  m_nbMaxIter = nbTimeSteps;
}

BenchSolver3D::~BenchSolver3D ()
{  
  delete[]m_save;
  delete[]m_save2;
}

void BenchSolver3D::saveState (const float *const x, const float *const x2)
{
  copy(x, &x[m_nbVoxels], m_save);
  copy(x2, &x2[m_nbVoxels], m_save2);
}

void BenchSolver3D::setPreviousState (float *const x, float *const x2)
{
  copy(m_save, &m_save[m_nbVoxels], x);
  copy(m_save2, &m_save2[m_nbVoxels], x2);
}

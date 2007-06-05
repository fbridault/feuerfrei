#include "benchsolver3D.hpp"

BenchSolver3D::BenchSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, float timeStep,
			      float buoyancy, uint nbTimeSteps, float omegaDiff, float omegaProj, float epsilon) : 
  Solver3D (position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy), GCSSORSolver3D(omegaDiff, omegaProj, epsilon)
{
  m_save = new float[m_nbVoxels];
  m_save2 = new float[m_nbVoxels];
  
  memset (m_save, 0, m_nbVoxels * sizeof (float));
  memset (m_save2, 0, m_nbVoxels * sizeof (float));
  
  m_nbSteps = 20;
  m_nbMaxIter = nbTimeSteps;
}

/* Le constructeur de GSsolver n'a pas de paramètre, il n'est donc pas appelé explicitement */
BenchSolver3D::BenchSolver3D (uint nbTimeSteps, float omegaDiff, float omegaProj, float epsilon) : 
  GCSSORSolver3D(omegaDiff, omegaProj, epsilon)
{
  m_save = new float[m_nbVoxels];
  m_save2 = new float[m_nbVoxels];
  
  memset (m_save, 0, m_nbVoxels * sizeof (float));
  memset (m_save2, 0, m_nbVoxels * sizeof (float));
  
  m_nbSteps = 20;
  m_nbMaxIter = nbTimeSteps;
}

BenchSolver3D::~BenchSolver3D ()
{  
  delete[]m_save;
  delete[]m_save2;
}

void BenchSolver3D::saveState (const float *const x, const float *const x2)
{
  memcpy (m_save, x, m_nbVoxels * sizeof (float));
  memcpy (m_save2, x2, m_nbVoxels * sizeof (float));
}

void BenchSolver3D::setPreviousState (float *const x, float *const x2)
{
  memcpy (x, m_save, m_nbVoxels * sizeof (float));
  memcpy (x2, m_save2, m_nbVoxels * sizeof (float));
}

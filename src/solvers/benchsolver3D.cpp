#include "benchsolver3D.hpp"

BenchSolver3D::BenchSolver3D (Point& position, uint n_x, uint n_y, uint n_z, double dim, double timeStep,
			  double buoyancy, uint nbTimeSteps, double omegaDiff, double omegaProj, double epsilon) : 
  Solver3D (position, n_x, n_y, n_z, dim, timeStep, buoyancy), GCSSORSolver3D(omegaDiff, omegaProj, epsilon)
{
  m_save = new double[m_nbVoxels];
  m_save2 = new double[m_nbVoxels];
  
  memset (m_save, 0, m_nbVoxels * sizeof (double));
  memset (m_save2, 0, m_nbVoxels * sizeof (double));
  
  m_nbSteps = 20;
  m_nbMaxIter = nbTimeSteps;
}

/* Le constructeur de GSsolver n'a pas de paramètre, il n'est donc pas appelé explicitement */
BenchSolver3D::BenchSolver3D (uint nbTimeSteps, double omegaDiff, double omegaProj, double epsilon) : 
  GCSSORSolver3D(omegaDiff, omegaProj, epsilon)
{
  m_save = new double[m_nbVoxels];
  m_save2 = new double[m_nbVoxels];
  
  memset (m_save, 0, m_nbVoxels * sizeof (double));
  memset (m_save2, 0, m_nbVoxels * sizeof (double));
  
  m_nbSteps = 20;
  m_nbMaxIter = nbTimeSteps;
}

BenchSolver3D::~BenchSolver3D ()
{  
  delete[]m_save;
  delete[]m_save2;
}

void BenchSolver3D::saveState (const double *const x, const double *const x2)
{
  memcpy (m_save, x, m_nbVoxels * sizeof (double));
  memcpy (m_save2, x2, m_nbVoxels * sizeof (double));
}

void BenchSolver3D::setPreviousState (double *const x, double *const x2)
{
  memcpy (x, m_save, m_nbVoxels * sizeof (double));
  memcpy (x2, m_save2, m_nbVoxels * sizeof (double));
}

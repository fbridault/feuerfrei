#include "benchsolver.hpp"

BenchSolver::BenchSolver (Point& position, int n_x, int n_y, int n_z, double dim, double pas_de_temps,
			  double buoyancy, double nbTimeSteps, double omegaDiff, double omegaProj, double epsilon) : 
  Solver (position, n_x, n_y, n_z, dim, pas_de_temps, buoyancy), GCSSORsolver(omegaDiff, omegaProj, epsilon)
{
  m_save = new double[m_nbVoxels];
  m_save2 = new double[m_nbVoxels];
  
  memset (m_save, 0, m_nbVoxels * sizeof (double));
  memset (m_save2, 0, m_nbVoxels * sizeof (double));
  
  m_nbSteps = 20;
  m_nbMaxIter = nbTimeSteps;
}

/* Le constructeur de GSsolver n'a pas de paramètre, il n'est donc pas appelé explicitement */
BenchSolver::BenchSolver (double nbTimeSteps, double omegaDiff, double omegaProj, double epsilon) : 
  GCSSORsolver(omegaDiff, omegaProj, epsilon)
{
  m_save = new double[m_nbVoxels];
  m_save2 = new double[m_nbVoxels];
  
  memset (m_save, 0, m_nbVoxels * sizeof (double));
  memset (m_save2, 0, m_nbVoxels * sizeof (double));
  
  m_nbSteps = 20;
  m_nbMaxIter = nbTimeSteps;
}

BenchSolver::~BenchSolver ()
{  
  delete[]m_save;
  delete[]m_save2;
}

void BenchSolver::saveState (const double *const x, const double *const x2)
{
  memcpy (m_save, x, m_nbVoxels * sizeof (double));
  memcpy (m_save2, x2, m_nbVoxels * sizeof (double));
}

void BenchSolver::setPreviousState (double *const x, double *const x2)
{
  memcpy (x, m_save, m_nbVoxels * sizeof (double));
  memcpy (x2, m_save2, m_nbVoxels * sizeof (double));
}

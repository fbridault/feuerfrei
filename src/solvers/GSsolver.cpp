#include "GSsolver.hpp"

GSsolver::GSsolver (CPoint& position, int n_x, int n_y, int n_z, double dim, double timeStep) : 
  Solver(position, n_x, n_y, n_z, dim, timeStep)
{
}

GSsolver::GSsolver ()
{
}

GSsolver::~GSsolver ()
{
}

void GSsolver::GS_solve(int b, double *const x, const double *const x0, double a, double div, double nb_steps)
{
  int i, j, k, l;
  
  for (l = 0; l < nb_steps; l++){
    for (i = 1; i <= m_nbVoxelsX; i++)
      for (j = 1; j <= m_nbVoxelsY; j++)
	for (k = 1; k <= m_nbVoxelsZ; k++)
	  x[IX (i, j, k)] = (x0[IX (i, j, k)] +
			     a * (x[IX (i - 1, j, k)] + x[IX (i + 1, j, k)] +
				  x[IX (i, j - 1, k)] +	x[IX (i, j + 1, k)] +
				  x[IX (i, j, k - 1)] +	x[IX (i, j, k + 1)])) * div;
    //set_bnd (b, x);
  }
}

/* Pas de diffusion */
void GSsolver::diffuse (int b, double *const x, double *const x0, double a, double diff_visc)
{
  GS_solve(b,x,x0,a, 1/(1.0 + 6.0 * a), 2);
}

void GSsolver::project (double *const p, double *const div)
{
  double h_x = 1.0 / m_nbVoxelsX, h_y = 1.0 / m_nbVoxelsY, h_z = 1.0 / m_nbVoxelsZ;
  int i, j, k;
  
  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++){
		div[IX (i, j, k)] =
		  -0.5 * (h_x * (m_u[IX (i + 1, j, k)] - m_u[IX (i - 1, j, k)]) +
			  h_y * (m_v[IX (i, j + 1, k)] - m_v[IX (i, j - 1, k)]) +
			  h_z * (m_w[IX (i, j, k + 1)] - m_w[IX (i, j, k - 1)]));
		//p[IX (i, j, k)] = 0;
      }
  
  set_bnd (0, div);
  memset (p, 0, m_nbVoxels * sizeof (double));
//  set_bnd (0, p);
  
  GS_solve(0,p,div,1, 1/6.0, m_nbSteps); 
  
  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++){
		m_u[IX (i, j, k)] -= 0.5 * (p[IX (i + 1, j, k)] - p[IX (i - 1, j, k)]) / h_x;
		m_v[IX (i, j, k)] -= 0.5 * (p[IX (i, j + 1, k)] - p[IX (i, j - 1, k)]) / h_y;
		m_w[IX (i, j, k)] -= 0.5 * (p[IX (i, j, k + 1)] - p[IX (i, j, k - 1)]) / h_z;
      }
  //set_bnd (1, u);
  //set_bnd (2, v);
  //set_bnd (3, w);
}

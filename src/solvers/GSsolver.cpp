#include "GSsolver.hpp"

GSsolver::GSsolver (Point& position, uint n_x, uint n_y, uint n_z, double dim, double timeStep, double buoyancy) : 
  Solver(position, n_x, n_y, n_z, dim, timeStep, buoyancy)
{
}

GSsolver::GSsolver ()
{
}

GSsolver::~GSsolver ()
{
}

void GSsolver::GS_solve(unsigned char b, double *const x, const double *const x0, double a, double div, uint nb_steps)
{
  uint i, j, k, l;
  
  for (l = 0; l < nb_steps; l++){
    t=t1;
    for (k = 1; k <= m_nbVoxelsZ; k++){
      for (j = 1; j <= m_nbVoxelsY; j++){
	for (i = 1; i <= m_nbVoxelsZ; i++){
	  x[t] = ( x0[t] + a * (x[t-1] + x[t+1] + x[t-nx] + x[t+nx] + x[t-n2] +x[t+n2]) ) * div;
	  //set_bnd (b, x);
	  t++;
	}//for i
	t+=2;
      }//for j
      t+=t2nx;
    }//for k
  }//for l
}//GS_solve

/* Pas de diffusion */
void GSsolver::diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc)
{
  GS_solve(b,x,x0,a, 1/(1.0 + 6.0 * a), 2);
}

void GSsolver::project (double *const p, double *const div)
{
  double h_x = 1.0 / m_nbVoxelsX, h_y = 1.0 / m_nbVoxelsY, h_z = 1.0 / m_nbVoxelsZ;
  uint i, j, k;
  
  t=t1;
  for (k = 1; k <= m_nbVoxelsZ; k++){
    for (j = 1; j <= m_nbVoxelsY; j++){
      for (i = 1; i <= m_nbVoxelsX; i++){
	div[t] = -0.5 * (
			 h_x * (m_u[t+1] - m_u[t-1]) +
			 h_y * (m_v[t+nx] - m_v[t-nx]) +
			 h_z * (m_w[t+n2] - m_w[t-n2])
			 );
	t++;
      }//for i
      t+=2;
    }//for j
    t+=t2nx;
    //p[IX (i, j, k)] = 0;
  }// for k
  
  //  set_bnd (0, div);
  memset (p, 0, m_nbVoxels * sizeof (double));
  //  set_bnd (0, p);
  
  GS_solve(0,p,div,1, 1/6.0, m_nbSteps); 
  
  t=t1;
  for (k = 1; k <= m_nbVoxelsZ; k++){
    for (j = 1; j <= m_nbVoxelsY; j++){
      for (i = 1; i <= m_nbVoxelsX; i++){
	m_u[t] -= 0.5 * (p[t+1] - p[t-1]) / h_x;
	m_v[t] -= 0.5 * (p[t+nx] - p[t-nx]) / h_y;
	m_w[t] -= 0.5 * (p[t+n2] - p[t-n2]) / h_z;
	t++;
      }//for i
      t+=2;
    }//for j
    t+=t2nx;
  }//for k
  //set_bnd (1, u);
  //set_bnd (2, v);
  //set_bnd (3, w);
}//project

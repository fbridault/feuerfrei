#include "GSSolver2D.hpp"

GSSolver2D::GSSolver2D (Point& position, uint n_x, uint n_y, double dim, double timeStep, double buoyancy) : 
  Solver2D(position, n_x, n_y, dim, timeStep, buoyancy)
{
}

GSSolver2D::GSSolver2D ()
{
}

GSSolver2D::~GSSolver2D ()
{
}

void GSSolver2D::GS_solve(unsigned char b, double *const x, const double *const x0, double a, double div, uint nb_steps)
{
  uint i, j, k, l;
  
  for (l = 0; l < nb_steps; l++){
    t=t1;
    for (j = 1; j <= m_nbVoxelsY; j++){
      for (i = 1; i <= m_nbVoxelsX; i++){
	x[t] = ( x0[t] + a * (x[t-1] + x[t+1] + x[t-nx] + x[t+nx]) ) * div;
	//set_bnd (b, x);
	t++;
      }//for i
      t+=2;
    }//for j
  }//for l
}//GS_solve

/* Pas de diffusion */
void GSSolver2D::diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc)
{
  GS_solve(b,x,x0,a, 1/(1.0 + 6.0 * a), 2);
}

void GSSolver2D::project (double *const p, double *const div)
{
  double h_x = 1.0 / m_nbVoxelsX, h_y = 1.0 / m_nbVoxelsY;
  uint i, j;
  
  t=t1;
  for (j = 1; j <= m_nbVoxelsY; j++){
    for (i = 1; i <= m_nbVoxelsX; i++){
      div[t] = -0.5 * ( h_x * (m_u[t+1] - m_u[t-1]) + h_y * (m_v[t+nx] - m_v[t-nx]) );
      t++;
    }//for i
    t+=2;
  }//for j
  //p[IX (i, j, k)] = 0;
  
  //  set_bnd (0, div);
  memset (p, 0, m_nbVoxels * sizeof (double));
  //  set_bnd (0, p);
  
  GS_solve(0,p,div,1, 1/6.0, m_nbSteps); 
  
  t=t1;
  for (j = 1; j <= m_nbVoxelsY; j++){
    for (i = 1; i <= m_nbVoxelsX; i++){
      m_u[t] -= 0.5 * (p[t+1] - p[t-1]) / h_x;
      m_v[t] -= 0.5 * (p[t+nx] - p[t-nx]) / h_y;
      t++;
    }//for i
    t+=2;
  }//for j
  
  //set_bnd (1, u);
  //set_bnd (2, v);
  //set_bnd (3, w);
}//project

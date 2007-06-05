#include "GSSolver3D.hpp"

GSSolver3D::GSSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale,
			float timeStep, float buoyancy) : 
  Solver3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy)
{
}

GSSolver3D::GSSolver3D ()
{
}

GSSolver3D::~GSSolver3D ()
{
}

void GSSolver3D::GS_solve(unsigned char b, float *const x, const float *const x0, float a, float div, uint nb_steps)
{
  uint i, j, k, l;
  
  for (l = 0; l < nb_steps; l++){
    m_t=m_t1;
    for (k = 1; k <= m_nbVoxelsZ; k++){
      for (j = 1; j <= m_nbVoxelsY; j++){
	for (i = 1; i <= m_nbVoxelsX; i++){
	  x[m_t] = ( x0[m_t] + a * (x[m_t-1] + x[m_t+1] + x[m_t-m_nx] + x[m_t+m_nx] + x[m_t-m_n2] +x[m_t+m_n2]) ) * div;
	  //set_bnd (b, x);
	  m_t++;
	}//for i
	m_t+=2;
      }//for j
      m_t+=m_t2nx;
    }//for k
  }//for l
}//GS_solve

/* Pas de diffusion */
void GSSolver3D::diffuse (unsigned char b, float *const x, float *const x0, float a, float diff_visc)
{
  GS_solve(b,x,x0,a, 1/(1.0 + 6.0 * a), 2);
}

void GSSolver3D::project (float *const p, float *const div)
{
  float h_x = 1.0 / m_nbVoxelsX, h_y = 1.0 / m_nbVoxelsY, h_z = 1.0 / m_nbVoxelsZ;
  uint i, j, k;
  
  m_t=m_t1;
  for (k = 1; k <= m_nbVoxelsZ; k++){
    for (j = 1; j <= m_nbVoxelsY; j++){
      for (i = 1; i <= m_nbVoxelsX; i++){
	div[m_t] = -0.5 * (
			 h_x * (m_u[m_t+1] - m_u[m_t-1]) +
			 h_y * (m_v[m_t+m_nx] - m_v[m_t-m_nx]) +
			 h_z * (m_w[m_t+m_n2] - m_w[m_t-m_n2])
			 );
	m_t++;
      }//for i
      m_t+=2;
    }//for j
    m_t+=m_t2nx;
    //p[IX (i, j, k)] = 0;
  }// for k
  
  //  set_bnd (0, div);
  memset (p, 0, m_nbVoxels * sizeof (float));
  //  set_bnd (0, p);
  
  GS_solve(0,p,div,1, 1/6.0, m_nbSteps); 
  
  m_t=m_t1;
  for (k = 1; k <= m_nbVoxelsZ; k++){
    for (j = 1; j <= m_nbVoxelsY; j++){
      for (i = 1; i <= m_nbVoxelsX; i++){
	m_u[m_t] -= 0.5 * (p[m_t+1] - p[m_t-1]) / h_x;
	m_v[m_t] -= 0.5 * (p[m_t+m_nx] - p[m_t-m_nx]) / h_y;
	m_w[m_t] -= 0.5 * (p[m_t+m_n2] - p[m_t-m_n2]) / h_z;
	m_t++;
      }//for i
      m_t+=2;
    }//for j
    m_t+=m_t2nx;
  }//for k
  //set_bnd (1, u);
  //set_bnd (2, v);
  //set_bnd (3, w);
}//project

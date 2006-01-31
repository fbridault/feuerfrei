#include "HybridSolver.hpp"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

/* Le constructeur de GSsolver n'a pas de paramètre, il n'est donc pas appelé explicitement */
HybridSolver::HybridSolver (CPoint& position, int n_x, int n_y, int n_z, double dim, double pas_de_temps,
			    double buoyancy, double omegaDiff, double omegaProj, double epsilon) : 
  Solver (position, n_x, n_y, n_z, dim, pas_de_temps, buoyancy), GCSSORsolver(omegaDiff, omegaProj, epsilon)
{
  m_time = 0.0;
}

HybridSolver::~HybridSolver ()
{  
}

/* Pas de diffusion */
void HybridSolver::diffuse (int b, double *const x, double *const x0, double a, double diff_visc)
{
  GS_solve(b,x,x0,a, 1/(1.0 + 6.0 * a), 2);
}

void HybridSolver::project (double *const p, double *const div)
{
  double h_x = 1.0 / m_nbVoxelsX, 
		h_y = 1.0 / m_nbVoxelsY, 
		h_z = 1.0 / m_nbVoxelsZ;
  int i, j, k;

  int t;
  int n2= (m_nbVoxelsX+2) * (m_nbVoxelsY+2);
  int nx = m_nbVoxelsX+2;
  int t1=n2 + nx +1;
  int t2nx=2*nx;

  t = t1;
  for ( k = 1; k <= m_nbVoxelsZ; k++){
    for ( j = 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	div[t] = -0.5 * (h_x * (m_u[t+1] - m_u[t-1]) + h_y * (m_v[t+nx] - m_v[t-nx]) + h_z * (m_w[t+n2] - m_w[t-n2]));
	t++;
      }//for i
      t+=2;
    }//for j
    t+=t2nx;
    //p[IX (i, j, k)] = 0;
  }// for k
  
  set_bnd (0, div);
  memset (p, 0, m_nbVoxels * sizeof (double));
  //set_bnd (0, p);
    
  //GS_solve(0,p,div,1, 1/6.0, 15); 
  GCSSOR(p,div,1, 6.0, m_omegaProj,4);
  
  t = t1;
  for (k = 1; k <= m_nbVoxelsZ; k++){
    for (j = 1; j <= m_nbVoxelsY; j++){
      for (i = 1; i <= m_nbVoxelsX; i++){
	m_u[t] -= 0.5 * (p[t+1] - p[t-1]) / h_x;
	m_v[t] -= 0.5 * (p[t+nx] - p[t-nx]) / h_y;
	m_w[t] -= 0.5 * (p[t+n2] - p[t-n2]) / h_z;
	t++;
      }// for i
      t+=2;
    }// for j
    t+=t2nx;
  }//for k
  
  //set_bnd (1, u);
  //set_bnd (2, v);
  //set_bnd (3, w);
}

// void HybridSolver::iterate ()
// { 
//   ::wxStartTimer();
//   vel_step ();
//   //  dens_step();

//   cleanSources ();
// //   set_bnd (0, m_u);
// //   set_bnd (0, m_v);
// //   set_bnd (0, m_w);
//   m_time = (m_time*m_nbIter + ::wxGetElapsedTime (false))/(double) (++m_nbIter);
  
//   cout << m_time << "      \r"; cout.flush();
// }

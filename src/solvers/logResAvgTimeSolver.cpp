#include "logResAvgTimeSolver.hpp"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

/* Le constructeur de GSsolver n'a pas de paramètre, il n'est donc pas appelé explicitement */
LogResAvgTimeSolver::LogResAvgTimeSolver (CPoint& position, int n_x, int n_y, int n_z, double dim, double pas_de_temps,
					  double buoyancy, double nbTimeSteps, double omegaDiff, double omegaProj, double epsilon) : 
  Solver (position, n_x, n_y, n_z, dim, pas_de_temps, buoyancy),
  LogResAvgSolver (nbTimeSteps, omegaDiff, omegaProj, epsilon)
{
  m_times = new double[m_nbAverages];
  
  memset (m_times, 0, m_nbAverages * sizeof (double));
}

LogResAvgTimeSolver::~LogResAvgTimeSolver ()
{
  delete[]m_times;
}

void LogResAvgTimeSolver::vel_step ()
{
  if(m_nbIter > m_nbMaxIter)
    return;
  add_source (m_u, m_uSrc);
  add_source (m_v, m_vSrc);
  add_source (m_w, m_wSrc);
  SWAP (m_uPrev, m_u);
  diffuse (1, m_u, m_uPrev, m_aVisc, m_visc);
  SWAP (m_vPrev, m_v);
  diffuse (2, m_v, m_vPrev, m_aVisc, m_visc);
  SWAP (m_wPrev, m_w);
  diffuse (3, m_w, m_wPrev, m_aVisc, m_visc);
  
  m_index = NB_DIFF_LOGS;
  project (m_uPrev, m_vPrev);
  SWAP (m_uPrev, m_u);
  SWAP (m_vPrev, m_v);
  SWAP (m_wPrev, m_w);
  advect (1, m_u, m_uPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (2, m_v, m_vPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (3, m_w, m_wPrev, m_uPrev, m_vPrev, m_wPrev);
  
  m_index = NB_DIFF_LOGS+1;
  project (m_uPrev, m_vPrev);
  
  if(m_nbIter == m_nbMaxIter)
    for(int i=0; i < m_nbSteps; i++){
      m_file << i << " ";
      
      for(int j=0; j < (NB_PROJ_LOGS+NB_DIFF_LOGS) ; j++)
	m_file << m_times[j*m_nbSteps + i] << " " << m_averages[j*m_nbSteps + i] << " ";
      
      m_file << endl;
    } 
}

void LogResAvgTimeSolver::GS_solve(int b, double *const x, double *const x0, double a, double div, double nb_steps)
{  
  double t=0;
  ::wxStartTimer();

  int i, j, k, l;
  double diagonal = 1/div;
  double norm2;
  
  for (l = 0; l < nb_steps; l++){
    for (i = 1; i <= m_nbVoxelsX; i++)
      for (j = 1; j <= m_nbVoxelsY; j++)
	for (k = 1; k <= m_nbVoxelsZ; k++)
	  x[IX (i, j, k)] = (x0[IX (i, j, k)] +
			     a * (x[IX (i - 1, j, k)] + x[IX (i + 1, j, k)] +
				  x[IX (i, j - 1, k)] +	x[IX (i, j + 1, k)] +
				  x[IX (i, j, k - 1)] +	x[IX (i, j, k + 1)])) * div;
    
    // calcul du résidu
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  m_r[IX (i, j, k)] = x0[IX (i, j, k)] - diagonal * x[IX (i, j, k)] + 
	    a * (x[IX (i - 1, j, k)] + x[IX (i + 1, j, k)] +
		 x[IX (i, j - 1, k)] + x[IX (i, j + 1, k)] +
		 x[IX (i, j, k - 1)] + x[IX (i, j, k + 1)]);
    // calcul du carré de la norme du résidu
    norm2=0.0;
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  norm2+=m_r[IX(i,j,k)]*m_r[IX(i,j,k)];
    t += ::wxGetElapsedTime (false);
    
    computeAverage(l,norm2,t);
    ::wxStartTimer();
  }
  //set_bnd (b, x);
}

void LogResAvgTimeSolver::GCSSOR(double *const x0, const double *const b, double a, double diagonal, double omega, int maxiter)
{
  double t=0;
  ::wxStartTimer();
  
  double f=omega/diagonal;
  double d=f*a;
  double e=2.0-omega;
  int i,j,k;
  
  double rho0, rho1, alpha, beta,norm2,normb2,eb2;
  
  // calcul du carré de la norme de b
  normb2=0.0;
  for ( k = 1; k <= m_nbVoxelsZ; k++)
    for ( j = 1; j <= m_nbVoxelsY; j++)
      for ( i = 1; i <= m_nbVoxelsX; i++)
	normb2+=b[IX(i,j,k)]*b[IX(i,j,k)];
  
  // calcul de eb2 le second membre du test d'arrêt
  eb2=m_epsilon*normb2;
  // calcul du premier résidu r
  //calcul de r = b - A*x0
  
  for ( k = 1; k <= m_nbVoxelsZ; k++)
    for ( j = 1; j <= m_nbVoxelsY; j++)
      for ( i = 1; i <= m_nbVoxelsX; i++)
	m_r[IX (i, j, k)] = b[IX (i, j, k)] - diagonal * x0[IX (i, j, k)] + 
	  a * (x0[IX (i - 1, j, k)] + x0[IX (i + 1, j, k)] +
	       x0[IX (i, j - 1, k)] + x0[IX (i, j + 1, k)] +
	       x0[IX (i, j, k - 1)] + x0[IX (i, j, k + 1)]);
  
  // calcul de z tel que Cz=r
  
  // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
  for ( k = 1; k <= m_nbVoxelsZ; k++)
    for ( j = 1; j <= m_nbVoxelsY; j++)
      for ( i = 1; i <= m_nbVoxelsX; i++)
	m_z[IX(i,j,k)] = e*m_r[IX(i,j,k)]+d*(m_z[IX(i-1,j,k)]+m_z[IX(i,j-1,k)]+m_z[IX(i,j,k-1)]);
  
  // calcul de z tel que (D/w -TL)z=u
  for ( k = m_nbVoxelsZ; k>= 1 ; k--)
    for ( j = m_nbVoxelsY; j>=1 ; j--)
      for ( i = m_nbVoxelsX; i>=1; i--)
	m_z[IX(i,j,k)] = f*m_z[IX(i,j,k)]+d*(m_z[IX(i+1,j,k)]+m_z[IX(i,j+1,k)]+m_z[IX(i,j,k+1)]);
  
  // p=z
  memcpy (m_p, m_z, m_nbVoxels * sizeof (double));
  
  // calcul de r.z
  rho0=0.0;
  for ( k = 1; k <= m_nbVoxelsZ; k++)
    for ( j = 1; j <= m_nbVoxelsY; j++)
      for ( i = 1; i <= m_nbVoxelsX; i++)
	rho0+=m_r[IX(i,j,k)]*m_z[IX(i,j,k)];
  
  // début des itérations
  for( int numiter=0;numiter<maxiter;numiter++){
    //calcul de q =  A.p
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  m_q[IX (i, j, k)] = diagonal * m_p[IX (i, j, k)] - 
	    a * (m_p[IX (i - 1, j, k)] + m_p[IX (i + 1, j, k)] +
		 m_p[IX (i, j - 1, k)] + m_p[IX (i, j + 1, k)] +
		 m_p[IX (i, j, k - 1)] + m_p[IX (i, j, k + 1)]);
    
    //calcul du produit scalaire p.q
    alpha=0.0;
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  alpha+=m_p[IX(i,j,k)]*m_q[IX(i,j,k)];
    
    //calcul de alpha
//    alpha= rho0/alpha;
    alpha=(alpha) ? rho0/alpha : 0;
    // calcul de x = x + alpha.p
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  x0[IX(i,j,k)]+=alpha*m_p[IX(i,j,k)];
    
    // calcul de r = r -alpha*q
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  m_r[IX(i,j,k)]-=alpha*m_q[IX(i,j,k)];
    
    // calcul du carré de la norme du résidu
    norm2=0.0;
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  norm2+=m_r[IX(i,j,k)]*m_r[IX(i,j,k)];
    
    t += ::wxGetElapsedTime (false);
    
    computeAverage(numiter,norm2,t);
    ::wxStartTimer();
    
    //test d'arrÃªt
//     if(norm2 < eb2){
//       //cerr<<"précision atteinte : nbiter = "<<numiter<<endl;
//       break;
//     }
    // calcul de z tel que Cz =r
    
    // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
    
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  m_z[IX(i,j,k)] = e*m_r[IX(i,j,k)]+d*(m_z[IX(i-1,j,k)]+m_z[IX(i,j-1,k)]+m_z[IX(i,j,k-1)]);
    
    // calcul de z tel que (D/w -TL)z=u
    for ( k = m_nbVoxelsZ; k>= 1 ; k--)
      for ( j = m_nbVoxelsY; j>=1 ; j--)
	for ( i = m_nbVoxelsX; i>=1; i--)
	  m_z[IX(i,j,k)] = f*m_z[IX(i,j,k)]+d*(m_z[IX(i+1,j,k)]+m_z[IX(i,j+1,k)]+m_z[IX(i,j,k+1)]);
    
    //calcul de rho1 = r.z
    rho1=0.0;
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  rho1+=m_r[IX(i,j,k)]*m_z[IX(i,j,k)];
    
    //calcul de beta =rho1/rho0
    //    beta= rho1/rho0;
    beta=(rho0) ? rho1/rho0 : 0;
    
    rho0=rho1;
    //calcul de p = z+ beta.p
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  m_p[IX(i,j,k)]=m_z[IX(i,j,k)] + beta*m_p[IX(i,j,k)];
    
  }//for numiter
  if(norm2 > eb2){
    cerr<<"précision non atteinte !!!"<<endl;
  }
  
  return;
}//GCSSOR

void LogResAvgTimeSolver::computeAverage (int iter, double value, double time)
{
  int i = m_index*m_nbSteps + iter;
  
  m_averages[i] = (m_averages[i] * m_nbIter + sqrt(value))/(double)(m_nbIter+1);
  m_times[i] = (m_times[i] * m_nbIter + time)/(double)(m_nbIter+1);
}

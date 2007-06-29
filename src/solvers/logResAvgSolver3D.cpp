#include "logResAvgSolver3D.hpp"

LogResAvgSolver3D::LogResAvgSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale,
				      float timeStep, uint nbTimeSteps, float buoyancy, 
				      float omegaDiff, float omegaProj, float epsilon) : 
  Solver3D (position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy),
  BenchSolver3D (nbTimeSteps, omegaDiff, omegaProj, epsilon)
{
  m_file.open ("logs/residualsAverage.log", ios::out | ios::trunc);
  
  m_nbAverages = (NB_PROJ_LOGS+NB_DIFF_LOGS)*m_nbSteps;
  m_averages = new float[m_nbAverages];
  fill_n(m_averages, m_nbAverages, 0.0f);
}

LogResAvgSolver3D::LogResAvgSolver3D (uint nbTimeSteps, float omegaDiff, float omegaProj, float epsilon) : 
  BenchSolver3D (nbTimeSteps, omegaDiff, omegaProj, epsilon)
{
  m_file.open ("logs/residualsAverage.log", ios::out | ios::trunc);
  
  m_nbAverages = (NB_PROJ_LOGS+NB_DIFF_LOGS)*m_nbSteps;
  m_averages = new float[m_nbAverages];  
  fill_n(m_averages, m_nbAverages, 0.0f);
}

LogResAvgSolver3D::~LogResAvgSolver3D ()
{
  delete[]m_averages;
  m_file.close ();
}

void LogResAvgSolver3D::vel_step ()
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
    for(uint i=0; i < m_nbSteps; i++){
      m_file << i << " ";
      
      for(uint j=0; j < (NB_PROJ_LOGS+NB_DIFF_LOGS) ; j++)
	m_file << m_averages[j*m_nbSteps + i] << " ";

      m_file << endl;
    } 
}

/* Pas de diffusion */
void LogResAvgSolver3D::diffuse (unsigned char b, float *const x, float *const x0, float a, float diff_visc)
{
  m_index = b-1;
  saveState(x, x0);
  GS_solve(b,x,x0,a, 1/(1.0f + 6.0f * a), m_nbSteps);
  
  m_index = b+2;
  setPreviousState(x, x0);
  GCSSOR(x,x0,a, (1.0f + 6.0f * a), m_omegaDiff,m_nbSteps);
}


void LogResAvgSolver3D::project (float *const p, float *const div)
{
  float h_x = 1.0f / m_nbVoxelsX, h_y = 1.0f / m_nbVoxelsY, h_z = 1.0f / m_nbVoxelsZ;
  uint i, j, k;
  
  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++){
		div[IX (i, j, k)] =
		  -0.5f * (h_x * (m_u[IX (i + 1, j, k)] - m_u[IX (i - 1, j, k)]) +
			   h_y * (m_v[IX (i, j + 1, k)] - m_v[IX (i, j - 1, k)]) +
			   h_z * (m_w[IX (i, j, k + 1)] - m_w[IX (i, j, k - 1)]));
		//p[IX (i, j, k)] = 0;
      }
//   set_bnd (0, div);
  fill_n(p, m_nbVoxels, 0.0f);
//   set_bnd (0, p);
  
  saveState(p, div);
  GS_solve(0,p,div,1, 1/6.0f, m_nbSteps);
  
  m_index +=2;
  setPreviousState(p, div);  
  GCSSOR(p,div,1, 6.0f, m_omegaProj,m_nbSteps);
  
  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++){
		m_u[IX (i, j, k)] -= 0.5f * (p[IX (i + 1, j, k)] - p[IX (i - 1, j, k)]) / h_x;
		m_v[IX (i, j, k)] -= 0.5f * (p[IX (i, j + 1, k)] - p[IX (i, j - 1, k)]) / h_y;
		m_w[IX (i, j, k)] -= 0.5f * (p[IX (i, j, k + 1)] - p[IX (i, j, k - 1)]) / h_z;
      }
  //set_bnd (1, u);
  //set_bnd (2, v);
  //set_bnd (3, w);
}


void LogResAvgSolver3D::GS_solve(unsigned char b, float *const x, float *const x0, float a, float div, uint nb_steps)
{
  uint i, j, k, l;
  float diagonal = 1/div;
  float norm2;
  
  for (l = 0; l < nb_steps; l++){
    for (k = 1; k <= m_nbVoxelsZ; k++)
      for (j = 1; j <= m_nbVoxelsY; j++)
	for (i = 1; i <= m_nbVoxelsX; i++)
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
    norm2=0.0f;
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  norm2+=m_r[IX(i,j,k)]*m_r[IX(i,j,k)];
    
    computeAverage(l,norm2);
  }
  //set_bnd (b, x);
}

void LogResAvgSolver3D::GCSSOR(float *const x0, const float *const b, float a, float diagonal, float omega, uint maxiter)
{
  float f=omega/diagonal;
  float d=f*a;
  float e=2.0f-omega;
  uint i,j,k;
  
  float rho0, rho1, alpha, beta,norm2,normb2,eb2;
  
  // calcul du carré de la norme de b
  normb2=0.0f;
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
  copy(m_z, &m_z[m_nbVoxels], m_p);
    
  // calcul de r.z
  rho0=0.0f;
  for ( k = 1; k <= m_nbVoxelsZ; k++)
    for ( j = 1; j <= m_nbVoxelsY; j++)
      for ( i = 1; i <= m_nbVoxelsX; i++)
	rho0+=m_r[IX(i,j,k)]*m_z[IX(i,j,k)];
  
  // début des itérations
  for( uint numiter=0;numiter<maxiter;numiter++){
    //calcul de q =  A.p
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  m_q[IX (i, j, k)] = diagonal * m_p[IX (i, j, k)] - 
	    a * (m_p[IX (i - 1, j, k)] + m_p[IX (i + 1, j, k)] +
		 m_p[IX (i, j - 1, k)] + m_p[IX (i, j + 1, k)] +
		 m_p[IX (i, j, k - 1)] + m_p[IX (i, j, k + 1)]);
    
    //calcul du produit scalaire p.q
    alpha=0.0f;
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  alpha+=m_p[IX(i,j,k)]*m_q[IX(i,j,k)];
    
    //calcul de alpha
//    alpha= rho0/alpha;
    alpha=(alpha) ? rho0/alpha : 0.0f;
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
    norm2=0.0f;
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  norm2+=m_r[IX(i,j,k)]*m_r[IX(i,j,k)];

    computeAverage(numiter,norm2);

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
    rho1=0.0f;
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  rho1+=m_r[IX(i,j,k)]*m_z[IX(i,j,k)];
    
    //calcul de beta =rho1/rho0
    //    beta= rho1/rho0;
    beta=(rho0) ? rho1/rho0 : 0.0f;

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

void LogResAvgSolver3D::computeAverage (uint iter, float value)
{
  int i = m_index*m_nbSteps + iter;
  m_averages[i] = (m_averages[i] * m_nbIter + sqrtf(value))/(float)(m_nbIter+1);
}

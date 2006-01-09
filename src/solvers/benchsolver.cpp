#include "benchsolver.hpp"

/* Le constructeur de GSsolver n'a pas de paramètre, ils n'est donc pas appelé explicitement */
BenchSolver::BenchSolver (CPoint& position, int n_x, int n_y, int n_z, double dim, double pas_de_temps,
			  double nbTimeSteps, double omegaDiff, double omegaProj, double epsilon) : 
  Solver (position, n_x, n_y, n_z, dim, pas_de_temps), GCSSORsolver(omegaDiff, omegaProj, epsilon)
{
  m_save = new double[m_nbVoxels];
  m_save2 = new double[m_nbVoxels];
  
  m_save = (double *) memset (m_save, 0, m_nbVoxels * sizeof (double));
  m_save2 = (double *) memset (m_save2, 0, m_nbVoxels * sizeof (double));
  
  m_fileDiff[0].open ("solvers/GSsolverDiff.u.log", ios::out | ios::trunc);
  m_fileDiff[3].open ("solvers/GCSSORsolverDiff.u.log", ios::out | ios::trunc);
  m_fileDiff[1].open ("solvers/GSsolverDiff.v.log", ios::out | ios::trunc);
  m_fileDiff[4].open ("solvers/GCSSORsolverDiff.v.log", ios::out | ios::trunc);
  m_fileDiff[2].open ("solvers/GSsolverDiff.w.log", ios::out | ios::trunc);
  m_fileDiff[5].open ("solvers/GCSSORsolverDiff.w.log", ios::out | ios::trunc);
  
  m_fileProj[0].open ("solvers/GSsolverProj1.log", ios::out | ios::trunc);
  m_fileProj[2].open ("solvers/GCSSORsolverProj1.log", ios::out | ios::trunc);
  m_fileProj[1].open ("solvers/GSsolverProj2.log", ios::out | ios::trunc);
  m_fileProj[3].open ("solvers/GCSSORsolverProj2.log", ios::out | ios::trunc);

  m_nbSteps = 100;
  m_nbMaxIter = nbTimeSteps;
}

BenchSolver::~BenchSolver ()
{
  for(int i=0; i<6; i++)
    m_fileDiff[i].close ();
  
  for(int i=0; i<4; i++)
    m_fileProj[i].close ();

  delete[]m_save;
  delete[]m_save2;
}

void BenchSolver::vel_step ()
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
  m_index = 0;
  project (m_uPrev, m_vPrev);
  SWAP (m_uPrev, m_u);
  SWAP (m_vPrev, m_v);
  SWAP (m_wPrev, m_w);
  advect (1, m_u, m_uPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (2, m_v, m_vPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (3, m_w, m_wPrev, m_uPrev, m_vPrev, m_wPrev);
  m_index = 1;
  project (m_uPrev, m_vPrev);
}

/* Pas de diffusion */
void BenchSolver::diffuse (int b, double *const x, double *const x0, double a, double diff_visc)
{
  m_file = &m_fileDiff[b-1];
  
  saveState(x, x0);
  GS_solve(b,x,x0,a, 1/(1.0 + 6.0 * a), m_nbSteps);

  m_file = &m_fileDiff[b-1+3];

  setPreviousState(x, x0);
  GCSSOR(x,x0,a, (1.0 + 6.0 * a), m_omegaDiff);
}


void BenchSolver::project (double *const p, double *const div)
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
  
  m_file = &m_fileProj[m_index];
  
  saveState(p, div);
  GS_solve(0,p,div,1, 1/6.0, m_nbSteps);
  
  m_file = &m_fileProj[m_index+2];
  
  setPreviousState(p, div);  
  GCSSOR(p,div,1, 6.0, m_omegaProj);
  
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


void BenchSolver::GS_solve(int b, double *const x, double *const x0, double a, double div, double nb_steps)
{
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
    
    logResidu(norm2);
    
  }
  *m_file << endl;
  //set_bnd (b, x);
}

void BenchSolver::GCSSOR(double *const x0, const double *const b, double a, double diagonal, double omega)
{
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
  for( int numiter=0;numiter<MAXITER;numiter++){
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

    logResidu(norm2);

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
  
  *m_file << endl;
  
  return;
}//GCSSOR

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

void BenchSolver::logResidu (double value)
{
  *m_file << value << " " << endl;
}

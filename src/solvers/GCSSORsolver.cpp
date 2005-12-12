#include "GCSSORsolver.hpp"

#define MAXITER 100
#define EPSILON2 1e-5

GCSSORsolver::GCSSORsolver (CPoint& position, int n_x, int n_y, int n_z, double dim, double timeStep) : 
  Solver(position, n_x, n_y, n_z, dim, timeStep)
{
  m_r=new double[m_nbVoxels];
  m_z=new double[m_nbVoxels];
  m_p=new double[m_nbVoxels];
  m_q=new double[m_nbVoxels];

  memset (m_r, 0, m_nbVoxels * sizeof (double));
  memset (m_z, 0, m_nbVoxels * sizeof (double));
  memset (m_p, 0, m_nbVoxels * sizeof (double));
  memset (m_q, 0, m_nbVoxels * sizeof (double));
}

GCSSORsolver::~GCSSORsolver ()
{
  delete[]m_r;
  delete[]m_z;
  delete[]m_p;
  delete[]m_q;
}

void GCSSORsolver::GCSSOR(double *const x0, const double *const b, double a, double diagonal, double omega )
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
  eb2=EPSILON2*normb2;
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
    norm2=0.0f;
    for ( k = 1; k <= m_nbVoxelsZ; k++)
      for ( j = 1; j <= m_nbVoxelsY; j++)
	for ( i = 1; i <= m_nbVoxelsX; i++)
	  norm2+=m_r[IX(i,j,k)]*m_r[IX(i,j,k)];

    //test d'arrÃªt
    if(norm2 < eb2){
      //cerr<<"précision atteinte : nbiter = "<<numiter<<endl;
      break;
    }
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


/* Pas de diffusion */
void
GCSSORsolver::diffuse (int b, double *const x, const double *const x0,
		       double a, double diff_visc)
{
  GCSSOR(x,x0,a, (1.0 + 6.0 * a), 1.815);
}

void
GCSSORsolver::project (double *const p, double *const div)
{
  double h_x = 1.0 / m_nbVoxelsX, h_y = 1.0 / m_nbVoxelsY, h_z = 1.0 / m_nbVoxelsZ;
  int i, j, k;
  
  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++)
	{
	  div[IX (i, j, k)] = -0.5 * (h_x * (m_u[IX (i + 1, j, k)] - m_u[IX (i - 1, j, k)]) +
				      h_y * (m_v[IX (i, j + 1, k)] - m_v[IX (i, j - 1, k)]) +
				      h_z * (m_w[IX (i, j, k + 1)] - m_w[IX (i, j, k - 1)]));
	  //p[IX (i, j, k)] = 0;
	}
  
  set_bnd (0, div);
  memset (p, 0, m_nbVoxels * sizeof (double));
  //set_bnd (0, p);
  
  GCSSOR(p,div,1, 6.0, 1.815);
  
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

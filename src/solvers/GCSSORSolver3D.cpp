#include "GCSSORSolver3D.hpp"

GCSSORSolver3D::GCSSORSolver3D (double omegaDiff, double omegaProj, double epsilon)
{
  m_r=new double[m_nbVoxels];
  m_z=new double[m_nbVoxels];
  m_p=new double[m_nbVoxels];
  m_q=new double[m_nbVoxels];

  memset (m_r, 0, m_nbVoxels * sizeof (double));
  memset (m_z, 0, m_nbVoxels * sizeof (double));
  memset (m_p, 0, m_nbVoxels * sizeof (double));
  memset (m_q, 0, m_nbVoxels * sizeof (double));
  
  m_omegaDiff = omegaDiff;
  m_omegaProj = omegaProj;
  m_epsilon = epsilon;
}

GCSSORSolver3D::GCSSORSolver3D (Point& position, uint n_x, uint n_y, uint n_z, double dim, 
				double timeStep, double buoyancy, double omegaDiff, double omegaProj, double epsilon) : 
  Solver3D(position, n_x, n_y, n_z, dim, timeStep, buoyancy)
{
  m_r=new double[m_nbVoxels];
  m_z=new double[m_nbVoxels];
  m_p=new double[m_nbVoxels];
  m_q=new double[m_nbVoxels];

  memset (m_r, 0, m_nbVoxels * sizeof (double));
  memset (m_z, 0, m_nbVoxels * sizeof (double));
  memset (m_p, 0, m_nbVoxels * sizeof (double));
  memset (m_q, 0, m_nbVoxels * sizeof (double));

  m_omegaDiff = omegaDiff;
  m_omegaProj = omegaProj;
  m_epsilon = epsilon;
}

GCSSORSolver3D::~GCSSORSolver3D ()
{
  delete[]m_r;
  delete[]m_z;
  delete[]m_p;
  delete[]m_q;
}

void GCSSORSolver3D::GCSSOR(double *const x0, const double *const b, double a, double diagonal, double omega, uint maxiter)
{
  double f=omega/diagonal;
  double d=f*a;
  double e=2.0-omega;
  uint i,j,k;
  
  double rho0, rho1, alpha, beta,norm2,normb2,eb2;
  double *sav1, *sav2;
  
  // calcul du carré de la norme de b
//   t = t1;
//   normb2=0.0;
//   //traiter les couches de voxels de la grille
//   for (int k = 1; k <= m_nbVoxelsZ; k++){
//     //traiter une couche de voxels
//     for (int j = 1; j <= m_nbVoxelsY; j++){
//       //traiter une ligne de voxels
//       for (int i = 1; i<= m_nbVoxelsX; i++){
// 	normb2+=b[t]*b[t];
// 	t++;
//       }//for i
//       t+=2;
//     }//for j
//     t+=t2nx;
//   }//for k
  
//   // calcul de eb2 le second membre du test d'arrêt
//   eb2=m_epsilon*normb2;

  // calcul du premier résidu r
  //calcul de r = b - A*x0
  t = t1; 
  
  for ( k = 1; k <= m_nbVoxelsZ; k++){
    for ( j= 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	m_r[t] = b[t] - diagonal * x0[t] + a * (x0[t-nx] + x0[t+nx] + x0[t-1] + x0[t+1] + x0[t-n2] + x0[t+n2]);
	t++;
      }//for i
      t+=2;
    }//for j
    t+=t2nx;
  }// for k  

  // calcul de z tel que Cz=r
  
  // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
  t = t1; 
  for ( k = 1; k <= m_nbVoxelsZ; k++){
    for ( j = 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	m_z[t] = e*m_r[t]+d*(m_z[t-nx]+m_z[t-1]+m_z[t-n2]);
	t++;
      }//for i
      t+=2;
    }//for j
    t+=t2nx;
  }//for k

  // calcul de z tel que (D/w -TL)z=u

  t=IX(m_nbVoxelsY,m_nbVoxelsX,m_nbVoxelsZ);
  for ( k = m_nbVoxelsZ; k>= 1 ; k--){
    for ( j = m_nbVoxelsY; j>=1; j--){
      for ( i = m_nbVoxelsX; i>=1 ; i--){
	m_z[t] = f*m_z[t]+d*(m_z[t+nx]+m_z[t+1]+m_z[t+n2]);
	t--;
      }//for i
      t-=2;
    }//for j
    t-=t2nx;
  }//for k
	
  // p=z
  memcpy (m_p, m_z, m_nbVoxels * sizeof (double));
    
  // calcul de r.z
  t = t1;
  rho0=0.0;
  for ( k = 1; k <= m_nbVoxelsZ; k++){   
    for ( j= 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	rho0+=m_r[t]*m_z[t];
	t++;
      }//for i
      t+=2;
    }//for j
    t+=t2nx;
  }//for k
  
  // début des itérations
  for( uint numiter=0;numiter<maxiter;numiter++){
    //calcul de q =  A.p
    t = t1;
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  m_q[t] = diagonal * m_p[t] - 
	    a * (m_p[t-nx] + m_p[t+nx] +
		 m_p[t-1] + m_p[t+1] +
		 m_p[t-n2] + m_p[t+n2]);
	  t++;
	}//for i
	t+=2;
      }//for j
      t+=t2nx;
    }//for k
    
    //calcul du produit scalaire p.q
    alpha=0.0;
    t = t1;
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  alpha+=m_p[t]*m_q[t];
	  t++;
	}//for i
	t+=2;
      }//for j
      t+=t2nx;
    }//for k
    
    //calcul de alpha
    // alpha= rho0/alpha;
    alpha=(alpha) ? rho0/alpha : 0;
		
    // calcul de x = x + alpha.p
    // calcul de r = r -alpha*q
    t = t1;
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  x0[t]+=alpha*m_p[t];
	  m_r[t]-=alpha*m_q[t];
	  t++;
	}//for i
	t+=2;
      }//for j
      t+=2*nx;
    }//for k
    
    // calcul du carré de la norme du résidu
//     norm2=0.0f;
//     t = t1;
//     for ( k = 1; k <= m_nbVoxelsZ; k++){
//       for ( j = 1; j <= m_nbVoxelsY; j++){
// 	for ( i = 1; i <= m_nbVoxelsX; i++){
// 	  norm2+=m_r[t]*m_r[t];
// 	  t++;
// 	}//for i
// 	t+=2;
//       }//for j
//       t+=t2nx;
//     }//for k

//     //test d'arrêt
//     if(norm2 < eb2){
//       cerr<<"précision atteinte : nbiter = "<<numiter<<endl;
//       break;
//     }
    // calcul de z tel que Cz =r

    // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
    t = t1;
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  m_z[t] = e*m_r[t]+d*(m_z[t-nx]+m_z[t-1]+m_z[t-n2]);
	  t++;
	}//for i
	t+=2;
      }//for j
      t+=t2nx;
    }//for k
    
    // calcul de z tel que (D/w -TL)z=u

    t =IX(m_nbVoxelsY,m_nbVoxelsX,m_nbVoxelsZ) ;
    for ( k = m_nbVoxelsZ; k>= 1 ; k--){
      for ( j = m_nbVoxelsY; j>=1 ; j--){
	for ( i = m_nbVoxelsX; i>=1; i--){
	  m_z[t] = f*m_z[t]+d*(m_z[t+nx]+m_z[t+1]+m_z[t+n2]);
	  t--;
	}//for i
	t-=2;
      }//for j
      t-=t2nx;
    }//for k

    //calcul de rho1 = r.z
    rho1=0.0;
    t = t1;
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  rho1+=m_r[t]*m_z[t];
	  t++;
	}//for i
	t+=2;
      }//for j
      t+=t2nx;
    }//for k
    
    //calcul de beta =rho1/rho0
    //    beta= rho1/rho0;
    beta=(rho0) ? rho1/rho0 : 0;

    rho0=rho1;
    //calcul de p = z+ beta.p
    t = t1;
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  m_p[t]=m_z[t] + beta*m_p[t];
	  t++;
	}//for i
	t+=2;
      }//for j
      t+=t2nx;
    }//for k
  }//for numiter
//   if(norm2 > eb2){
//     cerr<<"précision non atteinte !!!"<<endl;
//   }
  
  return;
}//GCSSOR

/* Pas de diffusion */
void
GCSSORSolver3D::diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc)
{
  GCSSOR(x,x0,a, (1.0 + 6.0 * a), m_omegaDiff,100);
}

void
GCSSORSolver3D::project (double *const p, double *const div)
{
  double h_x = 1.0 / m_nbVoxelsX, 
		h_y = 1.0 / m_nbVoxelsY, 
		h_z = 1.0 / m_nbVoxelsZ;
  uint i, j, k;
  
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
  
  //set_bnd (0, div);
  memset (p, 0, m_nbVoxels * sizeof (double));
  //set_bnd (0, p);
  
  GCSSOR(p,div,1, 6.0, m_omegaProj,100);
  
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

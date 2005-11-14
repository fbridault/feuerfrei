#include "solver.hpp"

#include <math.h>
#include "graphicsFn.hpp"

#define MAXITER 100
#define EPSILON2 1e-10

Solver::Solver (int n_x, int n_y, int n_z, double dim, double timeStep)
{
  int n3=n_x * n_y * n_z;
  
  m_nbVoxelsX = n_x;
  m_nbVoxelsY = n_y;
  m_nbVoxelsZ = n_z;
  
  /* D�termination de la taille du solveur de mani�re � ce que */
  /*  le plus grand c�t� soit de dimension dim */
  if (m_nbVoxelsX > m_nbVoxelsY){
    if (m_nbVoxelsX > m_nbVoxelsZ){
      m_dimX = dim;
      m_dimY = m_dimX * m_nbVoxelsY / m_nbVoxelsX;
      m_dimZ = m_dimX * m_nbVoxelsZ / m_nbVoxelsX;
    }else{
      m_dimZ = dim;
      m_dimX = m_dimZ * m_nbVoxelsX / m_nbVoxelsZ;
      m_dimY = m_dimZ * m_nbVoxelsY / m_nbVoxelsZ;
    }
  }else{
    if (m_nbVoxelsY > m_nbVoxelsZ){
      m_dimY = dim;
      m_dimX = m_dimY * m_nbVoxelsX / m_nbVoxelsY;
      m_dimZ = m_dimY * m_nbVoxelsZ / m_nbVoxelsY;
    }else{
      m_dimZ = dim;
      m_dimX = m_dimZ * m_nbVoxelsX / m_nbVoxelsZ;
      m_dimY = m_dimZ * m_nbVoxelsY / m_nbVoxelsZ;
    }
  }
  
  m_nbVoxels = (m_nbVoxelsX + 2) * (m_nbVoxelsY + 2) * (m_nbVoxelsZ + 2);
  m_dt = timeStep;
  
  m_u = new double[m_nbVoxels];
  m_v = new double[m_nbVoxels];
  m_w = new double[m_nbVoxels];
  m_uPrev = new double[m_nbVoxels];
  m_vPrev = new double[m_nbVoxels];
  m_wPrev = new double[m_nbVoxels];
  m_dens = new double[m_nbVoxels];
  m_densPrev = new double[m_nbVoxels];
  m_densSrc = new double[m_nbVoxels];
  m_uSrc = new double[m_nbVoxels];
  m_vSrc = new double[m_nbVoxels];
  m_wSrc = new double[m_nbVoxels];
  
  memset (m_u, 0, m_nbVoxels * sizeof (double));
  memset (m_v, 0, m_nbVoxels * sizeof (double));
  memset (m_w, 0, m_nbVoxels * sizeof (double));
  memset (m_uPrev, 0, m_nbVoxels * sizeof (double));
  memset (m_vPrev, 0, m_nbVoxels * sizeof (double));
  memset (m_wPrev, 0, m_nbVoxels * sizeof (double));
  memset (m_dens, 0, m_nbVoxels * sizeof (double));
  memset (m_densPrev, 0, m_nbVoxels * sizeof (double));
  memset (m_densSrc, 0, m_nbVoxels * sizeof (double));
  memset (m_uSrc, 0, m_nbVoxels * sizeof (double));
  memset (m_vSrc, 0, m_nbVoxels * sizeof (double));
  memset (m_wSrc, 0, m_nbVoxels * sizeof (double));

  m_uResidu = new double[m_nbVoxels];
  memset (m_uResidu, 0, m_nbVoxels * sizeof (double));
  m_uPrevResidu = new double[m_nbVoxels];
  memset (m_uPrevResidu, 0, m_nbVoxels * sizeof (double));
  m_vResidu = new double[m_nbVoxels];
  memset (m_vResidu, 0, m_nbVoxels * sizeof (double));
  m_vPrevResidu = new double[m_nbVoxels];
  memset (m_vPrevResidu, 0, m_nbVoxels * sizeof (double));
  m_wResidu = new double[m_nbVoxels];
  memset (m_wResidu, 0, m_nbVoxels * sizeof (double));
  m_wPrevResidu = new double[m_nbVoxels];
  memset (m_wPrevResidu, 0, m_nbVoxels * sizeof (double));

  m_visc = 0.00000015;
  m_diff = 0.001;
  m_nbStepsGS = 15;
  m_nbIter = 0;
  
  m_aDiff = m_dt * m_diff * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
  m_aVisc = m_dt * m_visc * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
  
  m_r=new double[n3];
  m_z=new double[n3];
  m_p=new double[n3];
  m_q=new double[n3];
  
  /* Construction des display lists */
  buildDLBase ();
  buildDLGrid ();
}

Solver::~Solver ()
{
  delete[]m_u;
  delete[]m_v;
  delete[]m_w;

  delete[]m_uPrev;
  delete[]m_vPrev;
  delete[]m_wPrev;
  delete[]m_dens;

  delete[]m_densPrev;
  delete[]m_densSrc;

  delete[]m_uSrc;
  delete[]m_vSrc;
  delete[]m_wSrc;

  delete[]m_uResidu;
  delete[]m_vResidu;
  delete[]m_wResidu;
  delete[]m_uPrevResidu;
  delete[]m_vPrevResidu;
  delete[]m_wPrevResidu;
  
  delete[]m_r;
  delete[]m_z;
  delete[]m_p;
  delete[]m_q;

  glDeleteLists(REPERE,1);
  glDeleteLists(GRILLE,1);
}

void
Solver::set_bnd (int b, double *const x)
{
  int i, j;

  for (i = 1; i <= m_nbVoxelsY; i++)
    {
      for (j = 1; j <= m_nbVoxelsZ; j++)
	{
	  x[IX (0, i, j)] = 0;	//x[IX(i,j,1)];
	  x[IX (m_nbVoxelsX + 1, i, j)] = 0;	//x[IX(i,j,N)];
	}
    }

  for (i = 1; i <= m_nbVoxelsX; i++)
    {
      for (j = 1; j <= m_nbVoxelsZ; j++)
	{
	  x[IX (i, 0, j)] = 0;	//x[IX(i, 1, j)];
	  //x[IX(i, N+1, j)] = 0;//x[IX(i,N,j)];
	  x[IX (i, m_nbVoxelsY + 1, j)] = 0;//-- x[IX (i, m_nbVoxelsY, j)];
	}
    }

  for (i = 1; i <= m_nbVoxelsX; i++)
    {
      for (j = 1; j <= m_nbVoxelsY; j++)
	{
	  x[IX (i, j, 0)] = 0;	//x[IX(i,j,1)];
	  x[IX (i, j, m_nbVoxelsZ + 1)] = 0;	//x[IX(i,j,N)];
	}
    }
}

/* Ajout des forces externes */
void
Solver::add_source (double *const x, double *const src)
{
  int i;

  for (i = 0; i < m_nbVoxels; i++)
    x[i] += m_dt * src[i];
}

void Solver::GS_solve(int b, double *const x, const double *const x0,
			  double a, double div, double nb_steps)
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

void Solver::GCSSOR(double *const x0, const double *const b, double a, double diagonal, 
			   double nb_steps, double omega )
{
  double f=omega/diagonal;
  double d=f*a;
  double e=2.0-omega;

  int taille=m_nbVoxelsX;
  int n2=m_nbVoxelsX * m_nbVoxelsY; // Problème là !!!!!!!!!!!!!!!!!!!!!
  int n3=m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;

  double rho0, rho1, alpha, beta,norm2,normb2,eb2;
  
  // calcul du carré de la norme de b
  normb2=0.0;
  for(int i=0;i<3;i++){
    normb2+=b[IX2(i)]*b[IX2(i)];
  }
  // calculde eb2 le second membre du test d'arrêt
  eb2=EPSILON2*normb2;
  // calcul du premier résidu r
  //calcul de r = b - A*x0
  m_r[0]=b[IX2(0)]-diagonal*x0[IX2(0)]+a*(x0[IX2(1)]+x0[IX2(taille)]+x0[IX2(n2)]);
  for(int i= 1; i<taille ; i++){
    m_r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-1)]+x0[IX2(i+1)]+x0[IX2(i+taille)]+x0[IX2(i+n2)]);
  }
  for(int i= taille; i<n2 ; i++){
    m_r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]+x0[IX2(i+taille)]+
					  x0[IX2(i+n2)]);
  }
  for(int i= n2; i<n3-n2 ; i++){
    m_r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]+
					  x0[IX2(i+taille)]+x0[IX2(i+n2)]);
  }
  for(int i= n3-n2; i<n3-taille ; i++){
    m_r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]+
					  x0[IX2(i+taille)]);
  }
  for(int i= n3-taille; i<n3-1 ; i++){
    m_r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]);
  }
  int i=n3-1;
  m_r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]);


  // calcul de z tel que Cz=r

  // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
  m_z[0]=e*m_r[0];
  for(int i=1;i<taille;i++){
	m_z[i]=e*m_r[i]+d*m_z[i-1];
  }
  for(int i=taille;i<n2;i++){
	m_z[i]=e*m_r[i]+d*(m_z[i-1]+m_z[i-taille]);
  }
  for(int i=n2;i<n3;i++){
	m_z[i]=e*m_r[i]+d*(m_z[i-1]+m_z[i-taille]+m_z[i-n2]);
  }

  // calcul de z tel que (D/w -TL)z=u
  m_z[n3-1]=f*m_z[n3-1];
  for(int i=n3-2;i>=n3-taille;i--){
	m_z[i]=f*m_z[i]+d*m_z[i+1];
  }
  for(int i=n3-taille-1;i>=n3-n2;i--){
	m_z[i]=f*m_z[i]+d*(m_z[i+1]+m_z[i+taille]);
  }
  for(int i=n3-n2-1;i>=0;i--){
	m_z[i]=f*m_z[i]+d*(m_z[i+1]+m_z[i+taille]+m_z[i+n2]);
  }
	
  // p=z
  for(int i=0; i< n3;i++){
	m_p[i]=m_z[i];
  }

  // calcul de r.z
  rho0=0.0;
  for(int i = 0; i<n3;i++){
	rho0+=m_r[i]*m_z[i];
  }
  // début des itérations
  for(int numiter=0;numiter<MAXITER;numiter++){
	//calcul de q =  A.p
	m_q[0]=diagonal*m_p[0]-a*(m_p[1]+m_p[taille]+m_p[n2]);
	for(int i= 1; i<taille ; i++){
	  m_q[i]=diagonal*m_p[i]-a*(m_p[i-1]+m_p[i+1]+m_p[i+taille]+m_p[i+n2]);
	}
	for(int i= taille; i<n2 ; i++){
	  m_q[i]=diagonal*m_p[i]-a*(m_p[i-taille]+m_p[i-1]+m_p[i+1]+m_p[i+taille]+m_p[i+n2]);
	}
	for(int i= n2; i<n3-n2 ; i++){
	  m_q[i]=diagonal*m_p[i]-a*(m_p[i-n2]+m_p[i-taille]+m_p[i-1]+m_p[i+1]+m_p[i+taille]+m_p[i+n2]);
	}
	for(int i= n3-n2; i<n3-taille ; i++){
	  m_q[i]=diagonal*m_p[i]-a*(m_p[i-n2]+m_p[i-taille]+m_p[i-1]+m_p[i+1]+m_p[i+taille]);
	}
	for(int i= n3-taille; i<n3-1 ; i++){
	  m_q[i]=diagonal*m_p[i]-a*(m_p[i-n2]+m_p[i-taille]+m_p[i-1]+m_p[i+1]);
	}
	int i=n3-1;
	m_q[i]=diagonal*m_p[i]-a*(m_p[i-n2]+m_p[i-taille]+m_p[i-1]);


	//calcul du produit scalaire p.q
	alpha=0.0f;
	for(int i=0;i<n3;i++){
	  alpha+=m_p[i]*m_q[i];
	}
	//calcul de alpha
	alpha=rho0/alpha;
	// calcul de x = x + alpha.p
	for(int i=0;i<n3;i++){
	  x0[IX2(i)]+=alpha*m_p[i];
	}
	// calcul de r = r -alpha*q
	for(int i=0;i<n3;i++){
	  m_r[i]-=alpha*m_q[i];
	}
	// calcul du carré de la norme du résidu
	norm2=0.0f;
	for(int i=0;i<n3;i++){
	  norm2+=m_r[i]*m_r[i];
	}
	//test d'arrêt
	if(norm2 < eb2){
	  //cout<<"précision atteinte : nbiter = "<<numiter<<endl;
	  break;
	}
	// calcul de z tel que Cz =r

	// calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
	m_z[0]=e*m_r[0];
	for(int i=1;i<taille;i++){
	  m_z[i]=e*m_r[i]+d*m_z[i-1];
	}
	for(int i=taille;i<n2;i++){
	  m_z[i]=e*m_r[i]+d*(m_z[i-1]+m_z[i-taille]);
	}
	for(int i=n2;i<n3;i++){
	  m_z[i]=e*m_r[i]+d*(m_z[i-1]+m_z[i-taille]+m_z[i-n2]);
	}

	// calcul de z tel que (D/w -TL)z=u
	m_z[n3-1]=f*m_z[n3-1];
	for(int i=n3-2;i>=n3-taille;i--){
	  m_z[i]=f*m_z[i]+d*m_z[i+1];
	}
	for(int i=n3-taille-1;i>=n3-n2;i--){
	  m_z[i]=f*m_z[i]+d*(m_z[i+1]+m_z[i+taille]);
	}
	for(int i=n3-n2-1;i>=0;i--){
	  m_z[i]=f*m_z[i]+d*(m_z[i+1]+m_z[i+taille]+m_z[i+n2]);
	}

	//calcul de rho1 = r.z
	rho1=0.0;
	for(int i=0; i <n3;i++){
	  rho1+=m_r[i]*m_z[i];
	}
	//calcul de beta =rho1/rho0
	beta=rho1/rho0;
	rho0=rho1;
	//calcul de p = z+ beta.p
	for(int i=0; i <n3;i++){
	  m_p[i]=m_z[i] + beta*m_p[i];
	}

  }//for numiter
  if(norm2 > eb2){
	cout<<"précision non atteinte !!!"<<endl;
  }
  
  for(int i=0; i <n3;i++)
    x0[IX2(i)]=m_q[i];
	
  return;
}//GCSSOR

// void Solver::GCSSOR(double *const x0, const double *const b, double a, double diagonal, 
// 			   double nb_steps, double omega )
// {
//   double *r;// pour le résidu
//   double *z;// pour SSOR
//   double *p;//direction de descente
//   double *q;

//   double f=omega/diagonal;
//   double d=f*a;
//   double e=2.0f-omega;

//   int taille=m_nbVoxelsX;
//   int n2=m_nbVoxelsX * m_nbVoxelsY; // Problème là !!!!!!!!!!!!!!!!!!!!!
//   int n3=m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;

//   double rho0, rho1, alpha, beta;

//   r=new double[n3];
//   z=new double[n3];
//   p=new double[n3];
//   q=new double[n3];

//   // calcul du premier résidu r
//   //calcul de r = b - A*x0
//   m_r[0]=b[IX2(0)]-diagonal*x0[IX2(0)]+a*(x0[IX2(1)]+x0[IX2(taille)]+x0[IX2(n2)]);
//   for(int i= 1; i<taille ; i++){
//     m_r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-1)]+x0[IX2(i+1)]+x0[IX2(i+taille)]+x0[IX2(i+n2)]);
//   }
//   for(int i= taille; i<n2 ; i++){
//     m_r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]+x0[IX2(i+taille)]+x0[IX2(i+n2)]);
//   }
//   for(int i= n2; i<n3-n2 ; i++){
//     m_r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]+x0[IX2(i+taille)]+x0[IX2(i+n2)]);
//   }
//   for(int i= n3-n2; i<n3-taille ; i++){
//     m_r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]+x0[IX2(i+taille)]);
//   }
//   for(int i= n3-taille; i<n3-1 ; i++){
//     m_r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]);
//   }
//   int i=n3-1;
//   m_r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]);


//   // calcul de z tel que Cz=r

//   // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
//   m_z[0]=e*m_r[0];
//   for(int i=1;i<taille;i++){
//     m_z[i]=e*m_r[i]+d*m_z[i-1];
//   }
//   for(int i=taille;i<n2;i++){
//     m_z[i]=e*m_r[i]+d*(m_z[i-1]+m_z[i-taille]);
//   }
//   for(int i=n2;i<n3;i++){
//     m_z[i]=e*m_r[i]+d*(m_z[i-1]+m_z[i-taille]+m_z[i-n2]);
//   }

//   // calcul de z tel que (D/w -TL)z=u
//   m_z[n3-1]=f*m_z[n3-1];
//   for(int i=n3-2;i>=n3-taille;i--){
//     m_z[i]=f*m_z[i]+d*m_z[i+1];
//   }
//   for(int i=n3-taille-1;i>=n3-n2;i--){
//     m_z[i]=f*m_z[i]+d*(m_z[i+1]+m_z[i+taille]);
//   }
//   for(int i=n3-n2-1;i>=0;i--){
//     m_z[i]=f*m_z[i]+d*(m_z[i+1]+m_z[i+taille]+m_z[i+n2]);
//   }
	
//   // p=z
//   for(int i=0; i< n3;i++){
//     m_p[i]=m_z[i];
//   }

//   // calcul de r.z
//   rho0=0.0f;
//   for(int i = 0; i<n3;i++){
//     rho0+=m_r[i]*m_z[i];
//   }
//   for(int numiter=0;numiter<13;numiter++){
//     //calcul de q =  A.p
//     m_q[0]=diagonal*m_p[0]-a*(m_p[1]+m_p[taille]+m_p[n2]);
//     for(int i= 1; i<taille ; i++){
//       m_q[i]=diagonal*m_p[i]-a*(m_p[i-1]+m_p[i+1]+m_p[i+taille]+m_p[i+n2]);
//     }
//     for(int i= taille; i<n2 ; i++){
//       m_q[i]=diagonal*m_p[i]-a*(m_p[i-taille]+m_p[i-1]+m_p[i+1]+m_p[i+taille]+m_p[i+n2]);
//     }
//     for(int i= n2; i<n3-n2 ; i++){
//       m_q[i]=diagonal*m_p[i]-a*(m_p[i-n2]+m_p[i-taille]+m_p[i-1]+m_p[i+1]+m_p[i+taille]+m_p[i+n2]);
//     }
//     for(int i= n3-n2; i<n3-taille ; i++){
//       m_q[i]=diagonal*m_p[i]-a*(m_p[i-n2]+m_p[i-taille]+m_p[i-1]+m_p[i+1]+m_p[i+taille]);
//     }
//     for(int i= n3-taille; i<n3-1 ; i++){
//       m_q[i]=diagonal*m_p[i]-a*(m_p[i-n2]+m_p[i-taille]+m_p[i-1]+m_p[i+1]);
//     }
//     int i=n3-1;
//     m_q[i]=diagonal*m_p[i]-a*(m_p[i-n2]+m_p[i-taille]+m_p[i-1]);


//     //calcul du produit scalaire p.q
//     alpha=0.0f;
//     for(int i=0;i<n3;i++){
//       alpha+=m_p[i]*m_q[i];
//     }
//     //calcul de alpha
//     alpha=rho0/alpha;
//     // calcul de x = x + alpha.p
//     for(int i=0;i<n3;i++){
//       x0[IX2(i)]+=alpha*m_p[i];
//     }
//     // calcul de r = r -alpha*q
//     for(int i=0;i<n3;i++){
//       m_r[i]-=alpha*m_q[i];
//     }

//     // calcul de z tel que Cz =r

//     // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
//     m_z[0]=e*m_r[0];
//     for(int i=1;i<taille;i++){
//       m_z[i]=e*m_r[i]+d*m_z[i-1];
//     }
//     for(int i=taille;i<n2;i++){
//       m_z[i]=e*m_r[i]+d*(m_z[i-1]+m_z[i-taille]);
//     }
//     for(int i=n2;i<n3;i++){
//       m_z[i]=e*m_r[i]+d*(m_z[i-1]+m_z[i-taille]+m_z[i-n2]);
//     }

//     // calcul de z tel que (D/w -TL)z=u
//     m_z[n3-1]=f*m_z[n3-1];
//     for(int i=n3-2;i>=n3-taille;i--){
//       m_z[i]=f*m_z[i]+d*m_z[i+1];
//     }
//     for(int i=n3-taille-1;i>=n3-n2;i--){
//       m_z[i]=f*m_z[i]+d*(m_z[i+1]+m_z[i+taille]);
//     }
//     for(int i=n3-n2-1;i>=0;i--){
//       m_z[i]=f*m_z[i]+d*(m_z[i+1]+m_z[i+taille]+m_z[i+n2]);
//     }

//     //calcul de rho1 = r.z
//     rho1=0.0f;
//     for(int i=0; i <n3;i++){
//       rho1+=m_r[i]*m_z[i];
//     }
//     //calcul de beta =rho1/rho0
//     beta=rho1/rho0;
//     rho0=rho1;
//     //calcul de p = z+ beta.p
//     for(int i=0; i <n3;i++){
//       m_p[i]=m_z[i] + beta*m_p[i];
//     }

//   }//for numiter
//   delete(r);
//   delete(z);
//   delete(p);
//   delete(q);
//   return;
// }//GCSSOR

/* Pas de diffusion */
void
Solver::diffuse (int b, double *const x, const double *const x0,
				 double a, double diff_visc)
{
  GS_solve(b,x,x0,a, 1/(1.0 + 6.0 * a), 2);
  //GCSSOR(x,x0,a, (1.0 + 6.0 * a),  m_nbStepsGS, 1.815);
}

/* Pas de diffusion */
void
Solver::diffuse_hybride (int b, double *const x, const double *const x0,
			 double *const residu, double *const residu0,
			 double diff_visc)
{
  double alpha;
  double num = 0.0;
  double den = 0.0;
  double diff;
  /*double num=0.0, ei=0.0, sei=0.0,  den=0.0, dtmp, etmp *//*,ntmp */ ;
  double a = m_dt * diff_visc * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
  int i, j, k, l;

  for (l = 0; l < m_nbStepsGS; l++)
    {

      for (i = 1; i <= m_nbVoxelsX; i++)
	for (j = 1; j <= m_nbVoxelsY; j++)
	  for (k = 1; k <= m_nbVoxelsZ; k++)
	    {
	      x[IX (i, j, k)] =
		(x0[IX (i, j, k)] +
		 a * (x[IX (i - 1, j, k)] +
		      x[IX (i + 1, j, k)] +
		      x[IX (i, j - 1, k)] +
		      x[IX (i, j + 1, k)] +
		      x[IX (i, j, k - 1)] +
		      x[IX (i, j, k + 1)])) /
		(1.0 + 6.0 * a);
	      residu[IX (i, j, k)] =
		x0[IX (i, j, k)] -
		(x[IX (i, j, k)] -
		 a * (x[IX (i - 1, j, k)] +
		      x[IX (i + 1, j, k)] +
		      x[IX (i, j - 1, k)] +
		      x[IX (i, j + 1, k)] +
		      x[IX (i, j, k - 1)] +
		      x[IX (i, j, k + 1)] -
		      6 * x[IX (i, j, k)]));
	    }
      //set_bnd (b, x);

      /* calcul du terme alpha */
      for (i = 1; i <= m_nbVoxelsX; i++)
	for (j = 1; j <= m_nbVoxelsY; j++)
	  for (k = 1; k <= m_nbVoxelsZ; k++)
	    {
	      diff = residu0[IX (i, j, k)] -
		residu[IX (i, j, k)];
	      //cout << residu[IX(i,j,k)] << " " << residu0[IX(i,j,k)] << " " << diff << endl;
	      //      getchar();
	      num += (residu[IX (i, j, k)] * diff);
	      den += (diff * diff);
	    }	/* for alpha */

      alpha = den ? -num / den : 0.0;

      /* test alpha */
      /*if(alpha >-1.0 && alpha < 1.0) alpha = 0.0;
       */

      /*  printf("numerateur : %f %f %f  \ndenominateur : %f %f %f\n",
       * num_r, num_v, num_b, den_r, den_v, den_b); */
      //printf("alpha : %f\n", alpha);

      /* hybridation des radiosites et des residus */
      for (i = 1; i <= m_nbVoxelsX; i++)
	for (j = 1; j <= m_nbVoxelsY; j++)
	  for (k = 1; k <= m_nbVoxelsZ; k++)
	    {
	      x[IX (i, j, k)] = alpha * x[IX (i, j, k)] + (1 - alpha) * x0[IX (i, j, k)];
	      residu0[IX (i, j, k)] = alpha * residu[IX (i, j, k)] + (1 - alpha) * residu0[IX (i, j, k)];
	      // if(residu0[IX(i,j,k)])
	      //           cout << "residu0 " << residu0[IX(i,j,k)] << endl;
	    }
    }
}


/* Pas d'advection => dÃ©placement du fluide sur lui-mÃªme */
void
Solver::advect (int b, double *const d, const double *const d0,
		const double *const u, const double *const v,
		const double *const w)
{
  int i, j, k, i0, j0, k0, i1, j1, k1;
  double x, y, z, r0, s0, t0, r1, s1, t1, dt0_x, dt0_y, dt0_z;

  dt0_x = m_dt * m_nbVoxelsX;
  dt0_y = m_dt * m_nbVoxelsY;
  dt0_z = m_dt * m_nbVoxelsZ;
  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++)
	{
	  x = i - dt0_x * u[IX (i, j, k)];
	  y = j - dt0_y * v[IX (i, j, k)];
	  z = k - dt0_z * w[IX (i, j, k)];

	  if (x < 0.5)
	    x = 0.5;
	  if (x > m_nbVoxelsX + 0.5)
	    x = m_nbVoxelsX + 0.5;
	  i0 = (int) x;
	  i1 = i0 + 1;
	  if (y < 0.5)
	    y = 0.5;
	  if (y > m_nbVoxelsY + 0.5)
	    y = m_nbVoxelsY + 0.5;
	  j0 = (int) y;
	  j1 = j0 + 1;
	  if (z < 0.5)
	    z = 0.5;
	  if (z > m_nbVoxelsZ + 0.5)
	    z = m_nbVoxelsZ + 0.5;
	  k0 = (int) z;
	  k1 = k0 + 1;

	  r1 = x - i0;
	  r0 = 1 - r1;
	  s1 = y - j0;
	  s0 = 1 - s1;
	  t1 = z - k0;
	  t0 = 1 - t1;
	  d[IX (i, j, k)] =
	    r0 * (s0 * (t0 * d0[IX (i0, j0, k0)] +
			t1 * d0[IX (i0, j0, k1)]) +
		  s1 * (t0 * d0[IX (i0, j1, k0)] +
			t1 * d0[IX (i0, j1, k1)])) +
	    r1 * (s0 * (t0 * d0[IX (i1, j0, k0)] +
			t1 * d0[IX (i1, j0, k1)]) +
		  s1 * (t0 * d0[IX (i1, j1, k0)] +
			t1 * d0[IX (i1, j1, k1)]));
	}
  //set_bnd (b, d);
}

void
Solver::project (double *const p, double *const div)
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
		p[IX (i, j, k)] = 0;
      }
	  
  set_bnd (0, div);
  set_bnd (0, p);
  GS_solve(0,p,div,1, 1/6.0, m_nbStepsGS); 
  //GCSSOR(p,div,1, 6.0,  m_nbStepsGS, 1.815);

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

// void Solver::dens_step()
// {
//   add_source ( m_dens, m_densSrc);
//   SWAP (m_densPrev, m_dens); diffuse ( 0, m_dens, m_densPrev, a_diff, diff);
//   SWAP (m_densPrev, m_dens); advect ( 0, m_dens, m_densPrev, m_u, v, w);
// }

void
Solver::vel_step ()
{
  add_source (m_u, m_uSrc);
  add_source (m_v, m_vSrc);
  add_source (m_w, m_wSrc);
  SWAP (m_uPrev, m_u);
  diffuse (1, m_u, m_uPrev, m_aVisc, m_visc);
  SWAP (m_vPrev, m_v);
  diffuse (2, m_v, m_vPrev, m_aVisc, m_visc);
  SWAP (m_wPrev, m_w);
  diffuse (3, m_w, m_wPrev, m_aVisc, m_visc);
  project (m_uPrev, m_vPrev);
  SWAP (m_uPrev, m_u);
  SWAP (m_vPrev, m_v);
  SWAP (m_wPrev, m_w);
  advect (1, m_u, m_uPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (2, m_v, m_vPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (3, m_w, m_wPrev, m_uPrev, m_vPrev, m_wPrev);
  project (m_uPrev, m_vPrev);
}

void
Solver::vel_step_hybride ()
{
  add_source (m_u, m_uSrc);
  add_source (m_v, m_vSrc);
  add_source (m_w, m_wSrc);
  SWAP (m_uPrev, m_u);
  diffuse_hybride (1, m_u, m_uPrev, m_uResidu, m_uPrevResidu, m_visc);
  SWAP (m_vPrev, m_v);
  diffuse_hybride (2, m_v, m_vPrev, m_vResidu, m_vPrevResidu, m_visc);
  SWAP (m_wPrev, m_w);
  diffuse_hybride (3, m_w, m_wPrev, m_wResidu, m_wPrevResidu, m_visc);
  project (m_uPrev, m_vPrev);
  SWAP (m_uPrev, m_u);
  SWAP (m_vPrev, m_v);
  SWAP (m_wPrev, m_w);
  advect (1, m_u, m_uPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (2, m_v, m_vPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (3, m_w, m_wPrev, m_uPrev, m_vPrev, m_wPrev);
  project (m_uPrev, m_vPrev);
}

void
Solver::iterate (bool flickering)
{
  /* Temporaire : ajout de forces pÃ©riodiques */
  //~ if ((nb_iter % nb_iter_flickering) == 5)
  //~ {
  //~ cleanSources ();
  //~ flickering = false;
  //~ }
  //~ else if ((nb_iter % nb_iter_flickering) < 5)
  //~ flickering = true;
  //~ else
  //~ flickering = false;

  //cout << IX2(3374) << "  " << IX2(3375) << " " << IX2(1) << " " << IX2(225) << endl;
  for (int i = 0; i < m_nbFlames; i++)
    m_flames[i]->add_forces (flickering);

  vel_step ();
  //  dens_step();

  m_nbIter++;

  set_bnd (0, m_u);
  set_bnd (0, m_v);
  set_bnd (0, m_w);
  
  cleanSources ();
}

void
Solver::cleanSources ()
{
  m_uSrc = (double *) memset (m_uSrc, 0, m_nbVoxels * sizeof (double));
  m_vSrc = (double *) memset (m_vSrc, 0, m_nbVoxels * sizeof (double));
  m_wSrc = (double *) memset (m_wSrc, 0, m_nbVoxels * sizeof (double));
}

void
Solver::buildDLGrid ()
{
  double interx = m_dimX / (double) m_nbVoxelsX;
  double intery = m_dimY / (double) m_nbVoxelsY;
  double interz = m_dimZ / (double) m_nbVoxelsZ;
  double i, j;

  glNewList (GRILLE, GL_COMPILE);
  glPushMatrix ();
  glTranslatef (-m_dimX / 2.0, 0, m_dimZ / 2.0);
  glBegin (GL_LINES);

  glColor4f (0.5, 0.5, 0.5, 0.5);

  for (j = 0.0; j <= m_dimZ; j += interz)
    {
      for (i = 0.0; i <= m_dimX + interx / 2; i += interx)
	{
	  glVertex3d (i, 0.0, -j);
	  glVertex3d (i, m_dimY, -j);
	}
      for (i = 0.0; i <= m_dimY + intery / 2; i += intery)
	{
	  glVertex3d (0.0, i, -j);
	  glVertex3d (m_dimX, i, -j);
	}
    }
  glEnd ();
  glPopMatrix ();
  glEndList ();
}

void
Solver::buildDLBase ()
{
  double interx = m_dimX / (double) m_nbVoxelsX;
  double interz = m_dimZ / (double) m_nbVoxelsZ;
  double i;

  glNewList (REPERE, GL_COMPILE);
  glPushMatrix ();
  glTranslatef (-m_dimX / 2.0, 0.0, m_dimZ / 2.0);
  glBegin (GL_LINES);

  glLineWidth (1.0);
  glColor4f (0.5, 0.5, 0.5, 0.5);
  for (i = 0.0; i <= m_dimX + interx / 2; i += interx)
    {
      glVertex3d (i, 0.0, -m_dimZ);
      glVertex3d (i, 0.0, 0.0);
    }
  for (i = 0.0; i <= m_dimZ + interz / 2; i += interz)
    {
      glVertex3d (0.0, 0.0, i - m_dimZ);
      glVertex3d (m_dimX, 0.0, i - m_dimZ);
    }
  glEnd ();
  glPopMatrix ();
  glEndList ();
}

void 
Solver::displayVelocityField (void)
{
  double inc_x = m_dimX / (double) m_nbVoxelsX;
  double inc_y = m_dimY / (double) m_nbVoxelsY;
  double inc_z = m_dimZ / (double) m_nbVoxelsZ;
  
  for (int i = 1; i <= m_nbVoxelsX; i++)
    {
      for (int j = 1; j <= m_nbVoxelsY; j++)
	{
	  for (int k = 1; k <= m_nbVoxelsZ; k++)
	    {
	      CVector vect;
	      /* Affichage du champ de v�locit� */
	      glPushMatrix ();
	      glTranslatef (inc_x * i - inc_x / 2.0 - m_dimX / 2.0,
			    inc_y * j - inc_y / 2.0, 
			    inc_z * k - inc_z / 2.0 - m_dimZ / 2.0);
	      //    printf("v�locit� %d %d %d %f %f %f\n",i,j,k,getU(i,j,k)],getV(i,j,k),getW(i,j,k));
	      //SDL_mutexP (lock);
	      vect.x = getU (i, j, k);
	      vect.y = getV (i, j, k);
	      vect.z = getW (i, j, k);
	      //SDL_mutexV (lock);
	      displayArrow (&vect);
	      glPopMatrix ();
	    }
	}
    }
}

void
Solver::displayArrow (CVector * const direction)
{
  double norme_vel = sqrt (direction->x * direction->x +
			   direction->y * direction->z +
			   direction->z * direction->z);
  double taille = m_dimX * m_dimY * m_dimZ * norme_vel / 2.5;
  double angle;
  CVector axeRot, axeCone (0.0, 0.0, 1.0);

  direction->normalize ();

  /* On obtient un vecteur perpendiculaire au plan défini par l'axe du cône et la direction souhaitée */
  axeRot = axeCone ^ *direction;

  /* On récupère l'angle de rotation entre les deux vecteurs */
  angle = acos (axeCone * *direction);

  glRotatef (angle * RAD_TO_DEG, axeRot.x, axeRot.y, axeRot.z);
  /***********************************************************************************/

  /* Dégradé de couleur bleu vers rouge */
  /* Problème : on ne connaît pas l'échelle des valeurs */
  /* On va donc tenter de prendre une valeur max suffisamment grande */
  /* pour pouvoir discerner au mieux les variations de la vélocité */

//  printf("%f\n",norme_vel);
  glColor4f (norme_vel / VELOCITE_MAX, 0.0, (VELOCITE_MAX - norme_vel) / VELOCITE_MAX, 0.75);

  GraphicsFn::SolidCone (taille / 4, taille, 3, 3);
}

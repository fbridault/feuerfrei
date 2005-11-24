#include "GCSSORsolver.hpp"

#define MAXITER 100
#define EPSILON2 1e-20

GCSSORsolver::GCSSORsolver (CPoint& position, int n_x, int n_y, int n_z, double dim, double timeStep) : 
  Solver(position, n_x, n_y, n_z, dim, timeStep)
{
  int n3 = m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
  m_r=new double[n3];
  m_z=new double[n3];
  m_p=new double[n3];
  m_q=new double[n3];
}

GCSSORsolver::~GCSSORsolver ()
{
  delete[]m_r;
  delete[]m_z;
  delete[]m_p;
  delete[]m_q;
}

void GCSSORsolver::GCSSOR(double *const x0, const double *const b, double a, double diagonal, 
			   double nb_steps, double omega )
{
  double f=omega/diagonal;
  double d=f*a;
  double e=2.0-omega;
  
  int taille=m_nbVoxelsX;
  int n2=m_nbVoxelsX * m_nbVoxelsY; // Problème là !!???????!!???!!
  int n3=m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
  
  double rho0, rho1, alpha, beta,norm2,normb2,eb2;
  
  // calcul du carré de la norme de b
  normb2=0.0;
  for(int i=0;i<n3;i++){
    normb2+=b[IX2(i)]*b[IX2(i)];
  }
  
  // calcul de eb2 le second membre du test d'arrêt
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
	alpha=0.0;
	for(int i=0;i<n3;i++){
	  alpha+=m_p[i]*m_q[i];
	}
	//calcul de alpha
	alpha=(alpha) ? rho0/alpha : 0;
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
	//test d'arrÃªt
	if(norm2 < eb2){
	  //cerr<<"précision atteinte : nbiter = "<<numiter<<endl;
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
	beta=(rho1) ? rho1/rho0 : 0;
	rho0=rho1;
	//calcul de p = z+ beta.p
	for(int i=0; i <n3;i++){
	  m_p[i]=m_z[i] + beta*m_p[i];
	}	
  }//for numiter
  if(norm2 > eb2){
	cerr<<"précision non atteinte !!!"<<endl;
  }
 //  for(int i=0; i <n3;i++){
//     cerr << i << " " << x0[IX2(i)] << endl;
//   }
  return;
}//GCSSOR

// void GCSSORsolver::GCSSOR(double *const x0, const double *const b, double a, double diagonal, 
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
//   int n2=m_nbVoxelsX * m_nbVoxelsY; // ProblÃ¨me lÃ  !!!!!!!!!!!!!!!!!!!!!
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
GCSSORsolver::diffuse (int b, double *const x, const double *const x0,
				 double a, double diff_visc)
{
  GCSSOR(x,x0,a, (1.0 + 6.0 * a), m_nbSteps, 1.815);
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
  
  GCSSOR(p,div,1, 6.0,  m_nbSteps, 1.815);
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

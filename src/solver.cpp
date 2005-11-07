#include "solver.hpp"

#include <math.h>
#include "graphicsFn.hpp"

#define MAXITER 100
#define EPSILON2 1e-10

Solver::Solver (int n_x, int n_y, int n_z, double dim, double pas_de_temps)
{
  int n3=n_x * n_y * n_z;
  
  N_x = n_x;
  N_y = n_y;
  N_z = n_z;
  
  /* D�termination de la taille du solveur de mani�re � ce que */
  /*  le plus grand c�t� soit de dimension dim */
  if (N_x > N_y){
    if (N_x > N_z){
      dim_x = dim;
      dim_y = dim_x * N_y / N_x;
      dim_z = dim_x * N_z / N_x;
    }else{
      dim_z = dim;
      dim_x = dim_z * N_x / N_z;
      dim_y = dim_z * N_y / N_z;
    }
  }else{
    if (N_y > N_z){
      dim_y = dim;
      dim_x = dim_y * N_x / N_y;
      dim_z = dim_y * N_z / N_y;
    }else{
      dim_z = dim;
      dim_x = dim_z * N_x / N_z;
      dim_y = dim_z * N_y / N_z;
    }
  }
  
  size = (N_x + 2) * (N_y + 2) * (N_z + 2);
  dt = pas_de_temps;

  cout << "pouetnew " << size << endl;
  u = new double[size];
  cout << "pouetpasnew" << endl;
  v = new double[size];
  w = new double[size];
  u_prev = new double[size];
  v_prev = new double[size];
  w_prev = new double[size];
  dens = new double[size];
  dens_prev = new double[size];
  dens_src = new double[size];
  u_src = new double[size];
  v_src = new double[size];
  w_src = new double[size];
  
  memset (u, 0, size * sizeof (double));
  memset (v, 0, size * sizeof (double));
  memset (w, 0, size * sizeof (double));
  memset (u_prev, 0, size * sizeof (double));
  memset (v_prev, 0, size * sizeof (double));
  memset (w_prev, 0, size * sizeof (double));
  memset (dens, 0, size * sizeof (double));
  memset (dens_prev, 0, size * sizeof (double));
  memset (dens_src, 0, size * sizeof (double));
  memset (u_src, 0, size * sizeof (double));
  memset (v_src, 0, size * sizeof (double));
  memset (w_src, 0, size * sizeof (double));

  cout << "pouet" << endl;
  residu_u = new double[size];
  memset (residu_u, 0, size * sizeof (double));
  residu_u_prev = new double[size];
  memset (residu_u_prev, 0, size * sizeof (double));
  residu_v = new double[size];
  memset (residu_v, 0, size * sizeof (double));
  residu_v_prev = new double[size];
  memset (residu_v_prev, 0, size * sizeof (double));
  residu_w = new double[size];
  memset (residu_w, 0, size * sizeof (double));
  residu_w_prev = new double[size];
  memset (residu_w_prev, 0, size * sizeof (double));

  cout << "pouet" << endl;
  visc = 0.00000015;
  diff = 0.001;
  nb_step_gauss_seidel = 15;
  nb_iter = 0;
  
  a_diff = dt * diff * N_x * N_y * N_z;
  a_visc = dt * visc * N_x * N_y * N_z;
  
  r=new double[n3];
  z=new double[n3];
  p=new double[n3];
  q=new double[n3];
  
  /* Construction des display lists */
  buildDLBase ();
  buildDLGrid ();
}

Solver::~Solver ()
{
  delete[]u;
  delete[]v;
  delete[]w;
  delete[]u_prev;
  delete[]v_prev;
  delete[]w_prev;
  delete[]dens;
  delete[]dens_prev;
  delete[]dens_src;
  delete[]u_src;
  delete[]v_src;
  delete[]w_src;

  delete[]residu_u;
  delete[]residu_v;
  delete[]residu_w;
  delete[]residu_u_prev;
  delete[]residu_v_prev;
  delete[]residu_w_prev;
  
  delete[]r;
  delete[]z;
  delete[]p;
  delete[]q;

  glDeleteLists(REPERE,1);
  glDeleteLists(GRILLE,1);
}

void
Solver::set_bnd (int b, double *const x)
{
  int i, j;

  for (i = 1; i <= N_y; i++)
    {
      for (j = 1; j <= N_z; j++)
	{
	  x[IX (0, i, j)] = 0;	//x[IX(i,j,1)];
	  x[IX (N_x + 1, i, j)] = 0;	//x[IX(i,j,N)];
	}
    }

  for (i = 1; i <= N_x; i++)
    {
      for (j = 1; j <= N_z; j++)
	{
	  x[IX (i, 0, j)] = 0;	//x[IX(i, 1, j)];
	  //x[IX(i, N+1, j)] = 0;//x[IX(i,N,j)];
	  x[IX (i, N_y + 1, j)] = 0;//-- x[IX (i, N_y, j)];
	}
    }

  for (i = 1; i <= N_x; i++)
    {
      for (j = 1; j <= N_y; j++)
	{
	  x[IX (i, j, 0)] = 0;	//x[IX(i,j,1)];
	  x[IX (i, j, N_z + 1)] = 0;	//x[IX(i,j,N)];
	}
    }
}

/* Ajout des forces externes */
void
Solver::add_source (double *const x, double *const src)
{
  int i;

  for (i = 0; i < size; i++)
    x[i] += dt * src[i];
}

void Solver::GS_solve(int b, double *const x, const double *const x0,
			  double a, double div, double nb_steps)
{
  int i, j, k, l;
  
  for (l = 0; l < nb_steps; l++){
    for (i = 1; i <= N_x; i++)
      for (j = 1; j <= N_y; j++)
	for (k = 1; k <= N_z; k++)
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

  int taille=N_x;
  int n2=N_x * N_y; // Problème là !!!!!!!!!!!!!!!!!!!!!
  int n3=N_x * N_y * N_z;

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
  r[0]=b[IX2(0)]-diagonal*x0[IX2(0)]+a*(x0[IX2(1)]+x0[IX2(taille)]+x0[IX2(n2)]);
  for(int i= 1; i<taille ; i++){
    r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-1)]+x0[IX2(i+1)]+x0[IX2(i+taille)]+x0[IX2(i+n2)]);
  }
  for(int i= taille; i<n2 ; i++){
    r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]+x0[IX2(i+taille)]+
					  x0[IX2(i+n2)]);
  }
  for(int i= n2; i<n3-n2 ; i++){
    r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]+
					  x0[IX2(i+taille)]+x0[IX2(i+n2)]);
  }
  for(int i= n3-n2; i<n3-taille ; i++){
    r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]+
					  x0[IX2(i+taille)]);
  }
  for(int i= n3-taille; i<n3-1 ; i++){
    r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]);
  }
  int i=n3-1;
  r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]);


  // calcul de z tel que Cz=r

  // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
  z[0]=e*r[0];
  for(int i=1;i<taille;i++){
	z[i]=e*r[i]+d*z[i-1];
  }
  for(int i=taille;i<n2;i++){
	z[i]=e*r[i]+d*(z[i-1]+z[i-taille]);
  }
  for(int i=n2;i<n3;i++){
	z[i]=e*r[i]+d*(z[i-1]+z[i-taille]+z[i-n2]);
  }

  // calcul de z tel que (D/w -TL)z=u
  z[n3-1]=f*z[n3-1];
  for(int i=n3-2;i>=n3-taille;i--){
	z[i]=f*z[i]+d*z[i+1];
  }
  for(int i=n3-taille-1;i>=n3-n2;i--){
	z[i]=f*z[i]+d*(z[i+1]+z[i+taille]);
  }
  for(int i=n3-n2-1;i>=0;i--){
	z[i]=f*z[i]+d*(z[i+1]+z[i+taille]+z[i+n2]);
  }
	
  // p=z
  for(int i=0; i< n3;i++){
	p[i]=z[i];
  }

  // calcul de r.z
  rho0=0.0;
  for(int i = 0; i<n3;i++){
	rho0+=r[i]*z[i];
  }
  // début des itérations
  for(int numiter=0;numiter<MAXITER;numiter++){
	//calcul de q =  A.p
	q[0]=diagonal*p[0]-a*(p[1]+p[taille]+p[n2]);
	for(int i= 1; i<taille ; i++){
	  q[i]=diagonal*p[i]-a*(p[i-1]+p[i+1]+p[i+taille]+p[i+n2]);
	}
	for(int i= taille; i<n2 ; i++){
	  q[i]=diagonal*p[i]-a*(p[i-taille]+p[i-1]+p[i+1]+p[i+taille]+p[i+n2]);
	}
	for(int i= n2; i<n3-n2 ; i++){
	  q[i]=diagonal*p[i]-a*(p[i-n2]+p[i-taille]+p[i-1]+p[i+1]+p[i+taille]+p[i+n2]);
	}
	for(int i= n3-n2; i<n3-taille ; i++){
	  q[i]=diagonal*p[i]-a*(p[i-n2]+p[i-taille]+p[i-1]+p[i+1]+p[i+taille]);
	}
	for(int i= n3-taille; i<n3-1 ; i++){
	  q[i]=diagonal*p[i]-a*(p[i-n2]+p[i-taille]+p[i-1]+p[i+1]);
	}
	int i=n3-1;
	q[i]=diagonal*p[i]-a*(p[i-n2]+p[i-taille]+p[i-1]);


	//calcul du produit scalaire p.q
	alpha=0.0f;
	for(int i=0;i<n3;i++){
	  alpha+=p[i]*q[i];
	}
	//calcul de alpha
	alpha=rho0/alpha;
	// calcul de x = x + alpha.p
	for(int i=0;i<n3;i++){
	  x0[IX2(i)]+=alpha*p[i];
	}
	// calcul de r = r -alpha*q
	for(int i=0;i<n3;i++){
	  r[i]-=alpha*q[i];
	}
	// calcul du carré de la norme du résidu
	norm2=0.0f;
	for(int i=0;i<n3;i++){
	  norm2+=r[i]*r[i];
	}
	//test d'arrêt
	if(norm2 < eb2){
	  //cout<<"précision atteinte : nbiter = "<<numiter<<endl;
	  break;
	}
	// calcul de z tel que Cz =r

	// calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
	z[0]=e*r[0];
	for(int i=1;i<taille;i++){
	  z[i]=e*r[i]+d*z[i-1];
	}
	for(int i=taille;i<n2;i++){
	  z[i]=e*r[i]+d*(z[i-1]+z[i-taille]);
	}
	for(int i=n2;i<n3;i++){
	  z[i]=e*r[i]+d*(z[i-1]+z[i-taille]+z[i-n2]);
	}

	// calcul de z tel que (D/w -TL)z=u
	z[n3-1]=f*z[n3-1];
	for(int i=n3-2;i>=n3-taille;i--){
	  z[i]=f*z[i]+d*z[i+1];
	}
	for(int i=n3-taille-1;i>=n3-n2;i--){
	  z[i]=f*z[i]+d*(z[i+1]+z[i+taille]);
	}
	for(int i=n3-n2-1;i>=0;i--){
	  z[i]=f*z[i]+d*(z[i+1]+z[i+taille]+z[i+n2]);
	}

	//calcul de rho1 = r.z
	rho1=0.0;
	for(int i=0; i <n3;i++){
	  rho1+=r[i]*z[i];
	}
	//calcul de beta =rho1/rho0
	beta=rho1/rho0;
	rho0=rho1;
	//calcul de p = z+ beta.p
	for(int i=0; i <n3;i++){
	  p[i]=z[i] + beta*p[i];
	}

  }//for numiter
  if(norm2 > eb2){
	cout<<"précision non atteinte !!!"<<endl;
  }
  
  for(int i=0; i <n3;i++)
    x0[IX2(i)]=q[i];
	
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

//   int taille=N_x;
//   int n2=N_x * N_y; // Problème là !!!!!!!!!!!!!!!!!!!!!
//   int n3=N_x * N_y * N_z;

//   double rho0, rho1, alpha, beta;

//   r=new double[n3];
//   z=new double[n3];
//   p=new double[n3];
//   q=new double[n3];

//   // calcul du premier résidu r
//   //calcul de r = b - A*x0
//   r[0]=b[IX2(0)]-diagonal*x0[IX2(0)]+a*(x0[IX2(1)]+x0[IX2(taille)]+x0[IX2(n2)]);
//   for(int i= 1; i<taille ; i++){
//     r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-1)]+x0[IX2(i+1)]+x0[IX2(i+taille)]+x0[IX2(i+n2)]);
//   }
//   for(int i= taille; i<n2 ; i++){
//     r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]+x0[IX2(i+taille)]+x0[IX2(i+n2)]);
//   }
//   for(int i= n2; i<n3-n2 ; i++){
//     r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]+x0[IX2(i+taille)]+x0[IX2(i+n2)]);
//   }
//   for(int i= n3-n2; i<n3-taille ; i++){
//     r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]+x0[IX2(i+taille)]);
//   }
//   for(int i= n3-taille; i<n3-1 ; i++){
//     r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]+x0[IX2(i+1)]);
//   }
//   int i=n3-1;
//   r[i]=b[IX2(i)]-diagonal*x0[IX2(i)]+a*(x0[IX2(i-n2)]+x0[IX2(i-taille)]+x0[IX2(i-1)]);


//   // calcul de z tel que Cz=r

//   // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
//   z[0]=e*r[0];
//   for(int i=1;i<taille;i++){
//     z[i]=e*r[i]+d*z[i-1];
//   }
//   for(int i=taille;i<n2;i++){
//     z[i]=e*r[i]+d*(z[i-1]+z[i-taille]);
//   }
//   for(int i=n2;i<n3;i++){
//     z[i]=e*r[i]+d*(z[i-1]+z[i-taille]+z[i-n2]);
//   }

//   // calcul de z tel que (D/w -TL)z=u
//   z[n3-1]=f*z[n3-1];
//   for(int i=n3-2;i>=n3-taille;i--){
//     z[i]=f*z[i]+d*z[i+1];
//   }
//   for(int i=n3-taille-1;i>=n3-n2;i--){
//     z[i]=f*z[i]+d*(z[i+1]+z[i+taille]);
//   }
//   for(int i=n3-n2-1;i>=0;i--){
//     z[i]=f*z[i]+d*(z[i+1]+z[i+taille]+z[i+n2]);
//   }
	
//   // p=z
//   for(int i=0; i< n3;i++){
//     p[i]=z[i];
//   }

//   // calcul de r.z
//   rho0=0.0f;
//   for(int i = 0; i<n3;i++){
//     rho0+=r[i]*z[i];
//   }
//   for(int numiter=0;numiter<13;numiter++){
//     //calcul de q =  A.p
//     q[0]=diagonal*p[0]-a*(p[1]+p[taille]+p[n2]);
//     for(int i= 1; i<taille ; i++){
//       q[i]=diagonal*p[i]-a*(p[i-1]+p[i+1]+p[i+taille]+p[i+n2]);
//     }
//     for(int i= taille; i<n2 ; i++){
//       q[i]=diagonal*p[i]-a*(p[i-taille]+p[i-1]+p[i+1]+p[i+taille]+p[i+n2]);
//     }
//     for(int i= n2; i<n3-n2 ; i++){
//       q[i]=diagonal*p[i]-a*(p[i-n2]+p[i-taille]+p[i-1]+p[i+1]+p[i+taille]+p[i+n2]);
//     }
//     for(int i= n3-n2; i<n3-taille ; i++){
//       q[i]=diagonal*p[i]-a*(p[i-n2]+p[i-taille]+p[i-1]+p[i+1]+p[i+taille]);
//     }
//     for(int i= n3-taille; i<n3-1 ; i++){
//       q[i]=diagonal*p[i]-a*(p[i-n2]+p[i-taille]+p[i-1]+p[i+1]);
//     }
//     int i=n3-1;
//     q[i]=diagonal*p[i]-a*(p[i-n2]+p[i-taille]+p[i-1]);


//     //calcul du produit scalaire p.q
//     alpha=0.0f;
//     for(int i=0;i<n3;i++){
//       alpha+=p[i]*q[i];
//     }
//     //calcul de alpha
//     alpha=rho0/alpha;
//     // calcul de x = x + alpha.p
//     for(int i=0;i<n3;i++){
//       x0[IX2(i)]+=alpha*p[i];
//     }
//     // calcul de r = r -alpha*q
//     for(int i=0;i<n3;i++){
//       r[i]-=alpha*q[i];
//     }

//     // calcul de z tel que Cz =r

//     // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
//     z[0]=e*r[0];
//     for(int i=1;i<taille;i++){
//       z[i]=e*r[i]+d*z[i-1];
//     }
//     for(int i=taille;i<n2;i++){
//       z[i]=e*r[i]+d*(z[i-1]+z[i-taille]);
//     }
//     for(int i=n2;i<n3;i++){
//       z[i]=e*r[i]+d*(z[i-1]+z[i-taille]+z[i-n2]);
//     }

//     // calcul de z tel que (D/w -TL)z=u
//     z[n3-1]=f*z[n3-1];
//     for(int i=n3-2;i>=n3-taille;i--){
//       z[i]=f*z[i]+d*z[i+1];
//     }
//     for(int i=n3-taille-1;i>=n3-n2;i--){
//       z[i]=f*z[i]+d*(z[i+1]+z[i+taille]);
//     }
//     for(int i=n3-n2-1;i>=0;i--){
//       z[i]=f*z[i]+d*(z[i+1]+z[i+taille]+z[i+n2]);
//     }

//     //calcul de rho1 = r.z
//     rho1=0.0f;
//     for(int i=0; i <n3;i++){
//       rho1+=r[i]*z[i];
//     }
//     //calcul de beta =rho1/rho0
//     beta=rho1/rho0;
//     rho0=rho1;
//     //calcul de p = z+ beta.p
//     for(int i=0; i <n3;i++){
//       p[i]=z[i] + beta*p[i];
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
  //GCSSOR(x,x0,a, (1.0 + 6.0 * a),  nb_step_gauss_seidel, 1.815);
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
  double a = dt * diff_visc * N_x * N_y * N_z;
  int i, j, k, l;

  for (l = 0; l < nb_step_gauss_seidel; l++)
    {

      for (i = 1; i <= N_x; i++)
	for (j = 1; j <= N_y; j++)
	  for (k = 1; k <= N_z; k++)
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
      for (i = 1; i <= N_x; i++)
	for (j = 1; j <= N_y; j++)
	  for (k = 1; k <= N_z; k++)
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
      for (i = 1; i <= N_x; i++)
	for (j = 1; j <= N_y; j++)
	  for (k = 1; k <= N_z; k++)
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

  dt0_x = dt * N_x;
  dt0_y = dt * N_y;
  dt0_z = dt * N_z;
  for (i = 1; i <= N_x; i++)
    for (j = 1; j <= N_y; j++)
      for (k = 1; k <= N_z; k++)
	{
	  x = i - dt0_x * u[IX (i, j, k)];
	  y = j - dt0_y * v[IX (i, j, k)];
	  z = k - dt0_z * w[IX (i, j, k)];

	  if (x < 0.5)
	    x = 0.5;
	  if (x > N_x + 0.5)
	    x = N_x + 0.5;
	  i0 = (int) x;
	  i1 = i0 + 1;
	  if (y < 0.5)
	    y = 0.5;
	  if (y > N_y + 0.5)
	    y = N_y + 0.5;
	  j0 = (int) y;
	  j1 = j0 + 1;
	  if (z < 0.5)
	    z = 0.5;
	  if (z > N_z + 0.5)
	    z = N_z + 0.5;
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
  double h_x = 1.0 / N_x, h_y = 1.0 / N_y, h_z = 1.0 / N_z;
  int i, j, k, l;
  
  for (i = 1; i <= N_x; i++)
    for (j = 1; j <= N_y; j++)
      for (k = 1; k <= N_z; k++){
		div[IX (i, j, k)] =
		-0.5 * (h_x * (u[IX (i + 1, j, k)] - u[IX (i - 1, j, k)]) +
				h_y * (v[IX (i, j + 1, k)] - v[IX (i, j - 1, k)]) +
				h_z * (w[IX (i, j, k + 1)] - w[IX (i, j, k - 1)]));
		p[IX (i, j, k)] = 0;
      }
	  
  set_bnd (0, div);
  set_bnd (0, p);
  GS_solve(0,p,div,1, 1/6.0, nb_step_gauss_seidel); 
  //GCSSOR(p,div,1, 6.0,  nb_step_gauss_seidel, 1.815);

  for (i = 1; i <= N_x; i++)
    for (j = 1; j <= N_y; j++)
      for (k = 1; k <= N_z; k++){
		u[IX (i, j, k)] -= 0.5 * (p[IX (i + 1, j, k)] - p[IX (i - 1, j, k)]) / h_x;
		v[IX (i, j, k)] -= 0.5 * (p[IX (i, j + 1, k)] - p[IX (i, j - 1, k)]) / h_y;
		w[IX (i, j, k)] -= 0.5 * (p[IX (i, j, k + 1)] - p[IX (i, j, k - 1)]) / h_z;
      }
  //set_bnd (1, u);
  //set_bnd (2, v);
  //set_bnd (3, w);
}

// void Solver::dens_step()
// {
//   add_source ( dens, dens_src);
//   SWAP (dens_prev, dens); diffuse ( 0, dens, dens_prev, a_diff, diff);
//   SWAP (dens_prev, dens); advect ( 0, dens, dens_prev, u, v, w);
// }

void
Solver::vel_step ()
{
  add_source (u, u_src);
  add_source (v, v_src);
  add_source (w, w_src);
  SWAP (u_prev, u);
  diffuse (1, u, u_prev, a_visc, visc);
  SWAP (v_prev, v);
  diffuse (2, v, v_prev, a_visc, visc);
  SWAP (w_prev, w);
  diffuse (3, w, w_prev, a_visc, visc);
  project (u_prev, v_prev);
  SWAP (u_prev, u);
  SWAP (v_prev, v);
  SWAP (w_prev, w);
  advect (1, u, u_prev, u_prev, v_prev, w_prev);
  advect (2, v, v_prev, u_prev, v_prev, w_prev);
  advect (3, w, w_prev, u_prev, v_prev, w_prev);
  project (u_prev, v_prev);
}

void
Solver::vel_step_hybride ()
{
  add_source (u, u_src);
  add_source (v, v_src);
  add_source (w, w_src);
  SWAP (u_prev, u);
  diffuse_hybride (1, u, u_prev, residu_u, residu_u_prev, visc);
  SWAP (v_prev, v);
  diffuse_hybride (2, v, v_prev, residu_v, residu_v_prev, visc);
  SWAP (w_prev, w);
  diffuse_hybride (3, w, w_prev, residu_w, residu_w_prev, visc);
  project (u_prev, v_prev);
  SWAP (u_prev, u);
  SWAP (v_prev, v);
  SWAP (w_prev, w);
  advect (1, u, u_prev, u_prev, v_prev, w_prev);
  advect (2, v, v_prev, u_prev, v_prev, w_prev);
  advect (3, w, w_prev, u_prev, v_prev, w_prev);
  project (u_prev, v_prev);
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

  for (int i = 0; i < nb_flammes; i++)
    flammes[i]->add_forces (flickering);

  vel_step ();
  //  dens_step();

  nb_iter++;

  set_bnd (0, u);
  set_bnd (0, v);
  set_bnd (0, w);
}

void
Solver::cleanSources ()
{
  u_src = (double *) memset (u_src, 0, size * sizeof (double));
  v_src = (double *) memset (v_src, 0, size * sizeof (double));
  w_src = (double *) memset (w_src, 0, size * sizeof (double));
}

void
Solver::buildDLGrid ()
{
  double interx = dim_x / (double) N_x;
  double intery = dim_y / (double) N_y;
  double interz = dim_z / (double) N_z;
  double i, j;

  glNewList (GRILLE, GL_COMPILE);
  glPushMatrix ();
  glTranslatef (-dim_x / 2.0, 0, dim_z / 2.0);
  glBegin (GL_LINES);

  glColor4f (0.5, 0.5, 0.5, 0.5);

  for (j = 0.0; j <= dim_z; j += interz)
    {
      for (i = 0.0; i <= dim_x + interx / 2; i += interx)
	{
	  glVertex3f (i, 0.0, -j);
	  glVertex3f (i, dim_y, -j);
	}
      for (i = 0.0; i <= dim_y + intery / 2; i += intery)
	{
	  glVertex3f (0.0, i, -j);
	  glVertex3f (dim_x, i, -j);
	}
    }
  glEnd ();
  glPopMatrix ();
  glEndList ();
}

void
Solver::buildDLBase ()
{
  double interx = dim_x / (double) N_x;
  double interz = dim_z / (double) N_z;
  double i;

  glNewList (REPERE, GL_COMPILE);
  glPushMatrix ();
  glTranslatef (-dim_x / 2.0, 0.0, dim_z / 2.0);
  glBegin (GL_LINES);

  glLineWidth (1.0);
  glColor4f (0.5, 0.5, 0.5, 0.5);
  for (i = 0.0; i <= dim_x + interx / 2; i += interx)
    {
      glVertex3f (i, 0.0, -dim_z);
      glVertex3f (i, 0.0, 0.0);
    }
  for (i = 0.0; i <= dim_z + interz / 2; i += interz)
    {
      glVertex3f (0.0, 0.0, i - dim_z);
      glVertex3f (dim_x, 0.0, i - dim_z);
    }
  glEnd ();
  glPopMatrix ();
  glEndList ();
}

void 
Solver::displayVelocityField (void)
{
  double inc_x = dim_x / (double) N_x;
  double inc_y = dim_y / (double) N_y;
  double inc_z = dim_z / (double) N_z;
  
  for (int i = 1; i <= N_x; i++)
    {
      for (int j = 1; j <= N_y; j++)
	{
	  for (int k = 1; k <= N_z; k++)
	    {
	      CVector vect;
	      /* Affichage du champ de v�locit� */
	      glPushMatrix ();
	      glTranslatef (inc_x * i - inc_x / 2.0 - N_x / 2.0,
			    inc_y * j - inc_y / 2.0, 
			    inc_z * k - inc_z / 2.0 -  N_z / 2.0);
	      //    printf("v�locit� %d %d %d %f %f %f\n",i,j,k,getU(i,j,k)],getV(i,j,k),getW(i,j,k));
	      //SDL_mutexP (lock);
	      vect.setX (getU (i, j, k));
	      vect.setY (getV (i, j, k));
	      vect.setZ (getW (i, j, k));
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
  double norme_vel =
    sqrt (direction->getX () * direction->getX () +
	  direction->getY () * direction->getY () +
	  direction->getZ () * direction->getZ ());
  double taille = dim_x * dim_y * dim_z * norme_vel / 2.5;
  double angle;
  CVector axeRot, axeCone (0.0, 0.0, 1.0);

  direction->normalize ();

  /* On obtient un vecteur perpendiculaire au plan défini par l'axe du cône et la direction souhaitée */
  axeRot = axeCone ^ *direction;

  /* On récupère l'angle de rotation entre les deux vecteurs */
  angle = acos (axeCone * *direction);

  glRotatef (angle * RAD_TO_DEG, axeRot.getX (), axeRot.getY (), axeRot.getZ ());
  /***********************************************************************************/

  /* Dégradé de couleur bleu vers rouge */
  /* Problème : on ne connaît pas l'échelle des valeurs */
  /* On va donc tenter de prendre une valeur max suffisamment grande */
  /* pour pouvoir discerner au mieux les variations de la vélocité */

//  printf("%f\n",norme_vel);
  glColor4f (norme_vel / VELOCITE_MAX, 0.0, (VELOCITE_MAX - norme_vel) / VELOCITE_MAX, 0.75);

  GraphicsFn::SolidCone (taille / 4, taille, 3, 3);
}

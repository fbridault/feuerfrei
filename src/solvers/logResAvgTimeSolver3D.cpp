#include "logResAvgTimeSolver3D.hpp"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

/* Le constructeur de GSsolver n'a pas de paramètre, il n'est donc pas appelé explicitement */
LogResAvgTimeSolver3D::LogResAvgTimeSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale,
					      float timeStep, float buoyancy, float vorticityConfinement, 
					      float omegaDiff, float omegaProj, float epsilon, uint nbTimeSteps) : 
  Solver3D (position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy, vorticityConfinement),
  LogResAvgSolver3D (nbTimeSteps, omegaDiff, omegaProj, epsilon)
{
  m_times = new float[m_nbAverages];
  
  fill_n(m_times, m_nbAverages, 0.0f);
}

LogResAvgTimeSolver3D::~LogResAvgTimeSolver3D ()
{
  delete[]m_times;
}

void LogResAvgTimeSolver3D::vel_step ()
{
  if(m_nbIter > m_nbMaxIter)
    return;
  add_source (m_u, m_uSrc);
  add_source (m_v, m_vSrc);
  add_source (m_w, m_wSrc);
  addVorticityConfinement(m_u,m_v,m_w);
  SWAP (m_uPrev, m_u);
  diffuse (1, m_u, m_uPrev, m_aVisc);
  SWAP (m_vPrev, m_v);
  diffuse (2, m_v, m_vPrev, m_aVisc);
  SWAP (m_wPrev, m_w);
  diffuse (3, m_w, m_wPrev, m_aVisc);
  
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
  
  if(m_nbIter == m_nbMaxIter){
    for(uint i=0; i <= m_nbSteps; i++){
      m_file << i << " ";
      
      for(uint j=0; j < (NB_PROJ_LOGS+NB_DIFF_LOGS) ; j++)
	m_file << m_times[j*(m_nbSteps+1) + i] << " " << m_averages[j*(m_nbSteps+1) + i] << " ";
      
      m_file << endl;
    }
    cout << "Simulation over" << endl;
  }
}

// void LogResAvgTimeSolver3D::GS_solve(unsigned char b, float *const x, const float *const x0, float a, float div, uint nb_steps)
// {
//   float t=0.0f;
//   unsigned long long int start;
//   uint i, j, k, l;
//   float diagonal = 1/div;
//   float norm2;
  
//   // calcul du résidu initial
//   for ( k = 1; k <= m_nbVoxelsZ; k++)
//     for ( j = 1; j <= m_nbVoxelsY; j++)
//       for ( i = 1; i <= m_nbVoxelsX; i++)
// 	m_r[IX (i, j, k)] = x0[IX (i, j, k)] - diagonal * x[IX (i, j, k)] + 
// 	  a * (x[IX (i - 1, j, k)] + x[IX (i + 1, j, k)] +
// 	       x[IX (i, j - 1, k)] + x[IX (i, j + 1, k)] +
// 	       x[IX (i, j, k - 1)] + x[IX (i, j, k + 1)]);
  
//   // calcul du carré de la norme du résidu
//   norm2=0.0f;
//   for ( k = 1; k <= m_nbVoxelsZ; k++)
//     for ( j = 1; j <= m_nbVoxelsY; j++)
//       for ( i = 1; i <= m_nbVoxelsX; i++)
// 	norm2+=m_r[IX(i,j,k)]*m_r[IX(i,j,k)];
  
//   computeAverage(0,norm2,0.0f);
//   start = rdtsc();
  
//   for (l = 0; l < nb_steps; l++){
//     for (k = 1; k <= m_nbVoxelsZ; k++)
//       for (j = 1; j <= m_nbVoxelsY; j++)
// 	for (i = 1; i <= m_nbVoxelsX; i++)
// 	  x[IX (i, j, k)] = (x0[IX (i, j, k)] +
// 			     a * (x[IX (i - 1, j, k)] + x[IX (i + 1, j, k)] +
// 				  x[IX (i, j - 1, k)] +	x[IX (i, j + 1, k)] +
// 				  x[IX (i, j, k - 1)] +	x[IX (i, j, k + 1)])) * div;

//     t += (rdtsc() - start)/CPU_FREQ;
    
//     // calcul du résidu
//     for ( k = 1; k <= m_nbVoxelsZ; k++)
//       for ( j = 1; j <= m_nbVoxelsY; j++)
// 	for ( i = 1; i <= m_nbVoxelsX; i++)
// 	  m_r[IX (i, j, k)] = x0[IX (i, j, k)] - diagonal * x[IX (i, j, k)] + 
// 	    a * (x[IX (i - 1, j, k)] + x[IX (i + 1, j, k)] +
// 		 x[IX (i, j - 1, k)] + x[IX (i, j + 1, k)] +
// 		 x[IX (i, j, k - 1)] + x[IX (i, j, k + 1)]);
//     // calcul du carré de la norme du résidu
//     norm2=0.0f;
//     for ( k = 1; k <= m_nbVoxelsZ; k++)
//       for ( j = 1; j <= m_nbVoxelsY; j++)
// 	for ( i = 1; i <= m_nbVoxelsX; i++)
// 	  norm2+=m_r[IX(i,j,k)]*m_r[IX(i,j,k)];
    
//     computeAverage(l+1,norm2,t);
    
//      start = rdtsc();
//   }
//   //set_bnd (b, x);
// }

// void LogResAvgTimeSolver3D::GS_solve(unsigned char b, float *const x, const float *const x0, float a, float div, uint nb_steps)
// {  
//   float t=0.0f;
//   unsigned long long int start;
//   uint i, j, k, l;
//   float diagonal = 1/div;
//   float norm2;
  
//   // calcul du résidu initial  
//   m_t=m_t1;
//   for (k = 1; k <= m_nbVoxelsZ; k++){
//     for (j = 1; j <= m_nbVoxelsY; j++){
//       for (i = 1; i <= m_nbVoxelsX; i++){
// 	m_r[m_t] = x0[m_t] - diagonal * x[m_t] + 
// 	  a * (x[m_t-1] + x[m_t+1] +  x[m_t-m_nx] + x[m_t+m_nx] + x[m_t-m_n2] + x[m_t+m_n2]);
// 	m_t++;
//       }//for i
//       m_t+=2;
//     }//for j
//     m_t+=m_t2nx;
//   }//for k
    
//   // calcul du carré de la norme du résidu
//   m_t=m_t1;
//   norm2=0.0f;
//   for (k = 1; k <= m_nbVoxelsZ; k++){
//     for (j = 1; j <= m_nbVoxelsY; j++){
//       for (i = 1; i <= m_nbVoxelsX; i++){
// 	norm2+=m_r[m_t]*m_r[m_t];
// 	m_t++;
//       }//for i
//       m_t+=2;
//     }//for j
//     m_t+=m_t2nx;
//   }//for k
  
//   computeAverage(0,norm2,0.0f);
//   start = rdtsc();

//   for (l = 0; l < nb_steps; l++){
//     m_t=m_t1;
//     for (k = 1; k <= m_nbVoxelsZ; k++){
//       for (j = 1; j <= m_nbVoxelsY; j++){
//         for (i = 1; i <= m_nbVoxelsX; i++){
// 	  x[m_t] = ( x0[m_t] + a * (x[m_t-1] + x[m_t+1] + x[m_t-m_nx] + x[m_t+m_nx] + x[m_t-m_n2] +x[m_t+m_n2]) ) * div;
//           //set_bnd (b, x);
// 	  m_t++;
//         }//for i
// 	m_t+=2;
//       }//for j
//       m_t+=m_t2nx;
//     }//for k
 
//     t += (rdtsc() - start)/CPU_FREQ;
//     // calcul du résidu
//     m_t=m_t1;
//     for (k = 1; k <= m_nbVoxelsZ; k++){
//       for (j = 1; j <= m_nbVoxelsY; j++){
//         for (i = 1; i <= m_nbVoxelsX; i++){
// 	  m_r[m_t] = x0[m_t] - diagonal * x[m_t] + 
// 	    a * (x[m_t-1] + x[m_t+1] +  x[m_t-m_nx] + x[m_t+m_nx] + x[m_t-m_n2] + x[m_t+m_n2]);
// 	  m_t++;
// 	}//for i
// 	m_t+=2;
//       }//for j
//       m_t+=m_t2nx;
//     }//for k
    
//     // calcul du carré de la norme du résidu
//     m_t=m_t1;
//     norm2=0.0f;
//     for (k = 1; k <= m_nbVoxelsZ; k++){
//       for (j = 1; j <= m_nbVoxelsY; j++){
// 	for (i = 1; i <= m_nbVoxelsX; i++){
// 	  norm2+=m_r[m_t]*m_r[m_t];
// 	  m_t++;
//         }//for i
// 	m_t+=2;
//       }//for j
//       m_t+=m_t2nx;
//     }//for k
//     computeAverage(l+1,norm2,t);
    
//     start = rdtsc();
//   }
// }

void LogResAvgTimeSolver3D::GS_solve(unsigned char b, float *const x, const float *const x0, float a, float div, uint nb_steps)
{  
  float t=0.0f;
  unsigned long long int start;
  uint i, j, k, l;
  float diagonal = 1/div;
  float norm2;
  float *mx,*mxmnx,* mxpnx,* mxm1,* mxp1,* mxmn2,* mxpn2,*mr;
  const float *mx0;
  
  // calcul du résidu initial  
  mx0=&x0[m_t1]; mx=&x[m_t1]; mr=m_r;
  mxm1=mx-1; mxp1=mx+1; mxmnx=mx-m_nx; mxpnx=mx+m_nx; mxmn2=mx-m_n2; mxpn2=mx+m_n2;
  for (k = 1; k <= m_nbVoxelsZ; k++){
    for (j = 1; j <= m_nbVoxelsY; j++){
      for (i = 1; i <= m_nbVoxelsX; i++){
	*mr = (*mx0 - diagonal * *mx + 
		    a * (*mxm1 + *mxp1 + *mxmnx + *mxpnx + *mxmn2 + *mxpn2));
	mx0++; mx++; mxm1++; mxp1++; mxmnx++; mxpnx++; mxmn2++; mxpn2++; mr++;
      }//for i
      mx0+=2; mx+=2; mxm1+=2; mxp1+=2; mxmnx+=2; mxpnx+=2; mxmn2+=2; mxpn2+=2; mr+=2;
    }//for j
    mx0+=m_t2nx; mx+=m_t2nx; mxm1+=m_t2nx; mxp1+=m_t2nx; mxmnx+=m_t2nx; mxpnx+=m_t2nx; mxmn2+=m_t2nx; mxpn2+=m_t2nx; mr+=m_t2nx;
  }//for k
  
  // calcul du carré de la norme du résidu    
  norm2=0.0f; mr=&m_r[m_t1];    
  for ( k = 1; k <= m_nbVoxelsZ; k++){
    for ( j = 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	norm2+=(*mr)*(*mr);
	mr++;
      }//for i
      mr+=2;
    }//for j
    mr+=m_t2nx;
  }//for k
  
  computeAverage(0,norm2,0.0f);
  start = rdtsc();

  for (l = 0; l < nb_steps; l++){
    mx0=&x0[m_t1]; mx=&x[m_t1]; 
    mxm1=mx-1; mxp1=mx+1; mxmnx=mx-m_nx; mxpnx=mx+m_nx; mxmn2=mx-m_n2; mxpn2=mx+m_n2;
    for (k = 1; k <= m_nbVoxelsZ; k++){
      for (j = 1; j <= m_nbVoxelsY; j++){
	for (i = 1; i <= m_nbVoxelsX; i++){
	  *mx = ( *mx0 + a * (*mxm1 + *mxp1 + *mxmnx + *mxpnx + *mxmn2 + *mxpn2) ) * div;
	  //set_bnd (b, x);
	  mx0++; mx++; mxm1++; mxp1++; mxmnx++; mxpnx++; mxmn2++; mxpn2++;	  
	}//for i
	mx0+=2; mx+=2; mxm1+=2; mxp1+=2; mxmnx+=2; mxpnx+=2; mxmn2+=2; mxpn2+=2;
      }//for j
      mx0+=m_t2nx; mx+=m_t2nx; mxm1+=m_t2nx; mxp1+=m_t2nx; mxmnx+=m_t2nx; mxpnx+=m_t2nx; mxmn2+=m_t2nx; mxpn2+=m_t2nx;
    }//for k

    t += (rdtsc() - start)/CPU_FREQ;
    
    mx0=&x0[m_t1]; mx=&x[m_t1];  mr=&m_r[m_t1];
    mxm1=mx-1; mxp1=mx+1; mxmnx=mx-m_nx; mxpnx=mx+m_nx; mxmn2=mx-m_n2; mxpn2=mx+m_n2;
    for (k = 1; k <= m_nbVoxelsZ; k++){
      for (j = 1; j <= m_nbVoxelsY; j++){
	for (i = 1; i <= m_nbVoxelsX; i++){
	  *mr = (*mx0 - diagonal * *mx + 
		 a * (*mxm1 + *mxp1 + *mxmnx + *mxpnx + *mxmn2 + *mxpn2));
	  mx0++; mx++; mxm1++; mxp1++; mxmnx++; mxpnx++; mxmn2++; mxpn2++; mr++;
	}//for i
	mx0+=2; mx+=2; mxm1+=2; mxp1+=2; mxmnx+=2; mxpnx+=2; mxmn2+=2; mxpn2+=2; mr+=2;
      }//for j
      mx0+=m_t2nx; mx+=m_t2nx; mxm1+=m_t2nx; mxp1+=m_t2nx; mxmnx+=m_t2nx; mxpnx+=m_t2nx; mxmn2+=m_t2nx; mxpn2+=m_t2nx; mr+=m_t2nx;
    }//for k    

    // calcul du carré de la norme du résidu    
    norm2=0.0f; mr=&m_r[m_t1];    
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  norm2+=(*mr)*(*mr);
	  mr++;
	}//for i
	mr+=2;
      }//for j
      mr+=m_t2nx;
    }//for k
    
    computeAverage(l+1,norm2,t);
    
    start = rdtsc();
  }//for l
  //set_bnd (b, x);
}

// void LogResAvgTimeSolver3D::GCSSOR(float *const x0, const float *const b, float a, float diagonal, float omega, uint maxiter)
// {
//   float t=0.0f;
//   unsigned long long int start;
//   float f=omega/diagonal;
//   float d=f*a;
//   float e=2.0f-omega;
//   uint i,j,k;
  
//   float rho0, rho1, alpha, beta,norm2,normb2,eb2;
  
//   // calcul du carré de la norme de b
//   normb2=0.0f;
//   for ( k = 1; k <= m_nbVoxelsZ; k++)
//     for ( j = 1; j <= m_nbVoxelsY; j++)
//       for ( i = 1; i <= m_nbVoxelsX; i++)
// 	normb2+=b[IX(i,j,k)]*b[IX(i,j,k)];
  
//   // calcul de eb2 le second membre du test d'arrêt
//   eb2=m_epsilon*normb2;
//   // calcul du premier résidu r
//   //calcul de r = b - A*x0
  
//   for ( k = 1; k <= m_nbVoxelsZ; k++)
//     for ( j = 1; j <= m_nbVoxelsY; j++)
//       for ( i = 1; i <= m_nbVoxelsX; i++)
// 	m_r[IX (i, j, k)] = b[IX (i, j, k)] - diagonal * x0[IX (i, j, k)] + 
// 	  a * (x0[IX (i - 1, j, k)] + x0[IX (i + 1, j, k)] +
// 	       x0[IX (i, j - 1, k)] + x0[IX (i, j + 1, k)] +
// 	       x0[IX (i, j, k - 1)] + x0[IX (i, j, k + 1)]);
  
//   // calcul du carré de la norme du résidu pour stockage
//   norm2=0.0f;
//   for ( k = 1; k <= m_nbVoxelsZ; k++)
//     for ( j = 1; j <= m_nbVoxelsY; j++)
//       for ( i = 1; i <= m_nbVoxelsX; i++)
// 	norm2+=m_r[IX(i,j,k)]*m_r[IX(i,j,k)];
//   computeAverage(0,norm2,0.0f);
//   start=rdtsc();
  
//   // calcul de z tel que Cz=r
  
//   // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
//   for ( k = 1; k <= m_nbVoxelsZ; k++)
//     for ( j = 1; j <= m_nbVoxelsY; j++)
//       for ( i = 1; i <= m_nbVoxelsX; i++)
// 	m_z[IX(i,j,k)] = e*m_r[IX(i,j,k)]+d*(m_z[IX(i-1,j,k)]+m_z[IX(i,j-1,k)]+m_z[IX(i,j,k-1)]);
  
//   // calcul de z tel que (D/w -TL)z=u
//   for ( k = m_nbVoxelsZ; k>= 1 ; k--)
//     for ( j = m_nbVoxelsY; j>=1 ; j--)
//       for ( i = m_nbVoxelsX; i>=1; i--)
// 	m_z[IX(i,j,k)] = f*m_z[IX(i,j,k)]+d*(m_z[IX(i+1,j,k)]+m_z[IX(i,j+1,k)]+m_z[IX(i,j,k+1)]);
  
//   // p=z
//   copy(m_z, &m_z[m_nbVoxels], m_p);
    
//   // calcul de r.z
//   rho0=0.0f;
//   for ( k = 1; k <= m_nbVoxelsZ; k++)
//     for ( j = 1; j <= m_nbVoxelsY; j++)
//       for ( i = 1; i <= m_nbVoxelsX; i++)
// 	rho0+=m_r[IX(i,j,k)]*m_z[IX(i,j,k)];
  
//   // début des itérations
//   for( uint numiter=0;numiter<maxiter;numiter++){
//     //calcul de q =  A.p
//     for ( k = 1; k <= m_nbVoxelsZ; k++)
//       for ( j = 1; j <= m_nbVoxelsY; j++)
// 	for ( i = 1; i <= m_nbVoxelsX; i++)
// 	  m_q[IX (i, j, k)] = diagonal * m_p[IX (i, j, k)] - 
// 	    a * (m_p[IX (i - 1, j, k)] + m_p[IX (i + 1, j, k)] +
// 		 m_p[IX (i, j - 1, k)] + m_p[IX (i, j + 1, k)] +
// 		 m_p[IX (i, j, k - 1)] + m_p[IX (i, j, k + 1)]);
    
//     //calcul du produit scalaire p.q
//     alpha=0.0f;
//     for ( k = 1; k <= m_nbVoxelsZ; k++)
//       for ( j = 1; j <= m_nbVoxelsY; j++)
// 	for ( i = 1; i <= m_nbVoxelsX; i++)
// 	  alpha+=m_p[IX(i,j,k)]*m_q[IX(i,j,k)];
    
//     //calcul de alpha
//     alpha= rho0/alpha;
//     //alpha=(alpha) ? rho0/alpha : 0.0f;
//     // calcul de x = x + alpha.p
//     for ( k = 1; k <= m_nbVoxelsZ; k++)
//       for ( j = 1; j <= m_nbVoxelsY; j++)
// 	for ( i = 1; i <= m_nbVoxelsX; i++)
// 	  x0[IX(i,j,k)]+=alpha*m_p[IX(i,j,k)];
    
//     // calcul de r = r -alpha*q
//     for ( k = 1; k <= m_nbVoxelsZ; k++)
//       for ( j = 1; j <= m_nbVoxelsY; j++)
// 	for ( i = 1; i <= m_nbVoxelsX; i++)
// 	  m_r[IX(i,j,k)]-=alpha*m_q[IX(i,j,k)];
    
//     t += (rdtsc() - start)/CPU_FREQ;
//     // calcul du carré de la norme du résidu
//     norm2=0.0f;
//     for ( k = 1; k <= m_nbVoxelsZ; k++)
//       for ( j = 1; j <= m_nbVoxelsY; j++)
// 	for ( i = 1; i <= m_nbVoxelsX; i++)
// 	  norm2+=m_r[IX(i,j,k)]*m_r[IX(i,j,k)];
    
//     computeAverage(numiter+1,norm2,t);

//     start=rdtsc();
//     //test d'arrÃªt
// //     if(norm2 < eb2){
// //       //cerr<<"précision atteinte : nbiter = "<<numiter<<endl;
// //       break;
// //     }
//     // calcul de z tel que Cz =r

//     // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r

//     for ( k = 1; k <= m_nbVoxelsZ; k++)
//       for ( j = 1; j <= m_nbVoxelsY; j++)
// 	for ( i = 1; i <= m_nbVoxelsX; i++)
// 	  m_z[IX(i,j,k)] = e*m_r[IX(i,j,k)]+d*(m_z[IX(i-1,j,k)]+m_z[IX(i,j-1,k)]+m_z[IX(i,j,k-1)]);
    
//     // calcul de z tel que (D/w -TL)z=u
//     for ( k = m_nbVoxelsZ; k>= 1 ; k--)
//       for ( j = m_nbVoxelsY; j>=1 ; j--)
// 	for ( i = m_nbVoxelsX; i>=1; i--)
// 	  m_z[IX(i,j,k)] = f*m_z[IX(i,j,k)]+d*(m_z[IX(i+1,j,k)]+m_z[IX(i,j+1,k)]+m_z[IX(i,j,k+1)]);
    
//     //calcul de rho1 = r.z
//     rho1=0.0f;
//     for ( k = 1; k <= m_nbVoxelsZ; k++)
//       for ( j = 1; j <= m_nbVoxelsY; j++)
// 	for ( i = 1; i <= m_nbVoxelsX; i++)
// 	  rho1+=m_r[IX(i,j,k)]*m_z[IX(i,j,k)];
    
//     //calcul de beta =rho1/rho0
//     beta= rho1/rho0;
//     //beta=(rho0) ? rho1/rho0 : 0.0f;

//     rho0=rho1;
//     //calcul de p = z+ beta.p
//     for ( k = 1; k <= m_nbVoxelsZ; k++)
//       for ( j = 1; j <= m_nbVoxelsY; j++)
// 	for ( i = 1; i <= m_nbVoxelsX; i++)
// 	  m_p[IX(i,j,k)]=m_z[IX(i,j,k)] + beta*m_p[IX(i,j,k)];
    
//   }//for numiter
//   if(norm2 > eb2){
//     cerr<<"précision non atteinte !!!"<<endl;
//   }
  
//   return;
// }//GCSSOR

// void LogResAvgTimeSolver3D::GCSSOR(float *const x0, const float *const b, float a, float diagonal, float omega, uint maxiter)
// {
//   float t=0.0f;
//   unsigned long long int start;
//   double f=omega/diagonal;
//   double d=f*a;
//   double e=2.0-omega;
//   uint i,j,k;
  
//   double rho0, rho1, alpha, beta,norm2,normb2,eb2;
  
//   // calcul du carré de la norme de b
//   m_t = m_t1;
//   normb2=0.0;
  
//   for (k = 1; k <= m_nbVoxelsZ; k++){
//     for (j = 1; j <= m_nbVoxelsY; j++){
//       for (i = 1; i<= m_nbVoxelsX; i++){
// 	normb2+=b[m_t]*b[m_t];
// 	m_t++;
//       }//for i
//       m_t+=2;
//     }//for j
//     m_t+=m_t2nx;
//   }//for k
  
//   // calcul de eb2 le second membre du test d'arrêt
//   eb2=m_epsilon*normb2;

//   // calcul du premier résidu r
//   //calcul de r = b - A*x0
//   m_t = m_t1; 
//   for ( k = 1; k <= m_nbVoxelsZ; k++){
//     for ( j= 1; j <= m_nbVoxelsY; j++){
//       for ( i = 1; i <= m_nbVoxelsX; i++){
// 	m_r[m_t] = b[m_t] - diagonal * x0[m_t] + a * (x0[m_t-m_nx] + x0[m_t+m_nx] + x0[m_t-1] + x0[m_t+1] + x0[m_t-m_n2] + x0[m_t+m_n2]);
// 	m_t++;
//       }//for i
//       m_t+=2;
//     }//for j
//     m_t+=m_t2nx;
//   }// for k  

//   // calcul du carré de la norme du résidu
//   norm2=0.0f;
//   m_t = m_t1; 
//   for (k = 1; k <= m_nbVoxelsZ; k++){
//     for (j = 1; j <= m_nbVoxelsY; j++){
//       for (i = 1; i<= m_nbVoxelsX; i++){
// 	norm2+=m_r[m_t]*m_r[m_t];
// 	m_t++;
//       }//for i
//       m_t+=2;
//     }//for j
//     m_t+=m_t2nx;
//   }//for k
  
//   computeAverage(0,norm2,0.0f);
  
//   start=rdtsc();
//   // calcul de z tel que Cz=r
  
//   // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
//   m_t = m_t1; 
//   for ( k = 1; k <= m_nbVoxelsZ; k++){
//     for ( j = 1; j <= m_nbVoxelsY; j++){
//       for ( i = 1; i <= m_nbVoxelsX; i++){
// 	m_z[m_t] = e*m_r[m_t]+d*(m_z[m_t-m_nx]+m_z[m_t-1]+m_z[m_t-m_n2]);
// 	m_t++;
//       }//for i
//       m_t+=2;
//     }//for j
//     m_t+=m_t2nx;
//   }//for k

//   // calcul de z tel que (D/w -TL)z=u

//   m_t=IX(m_nbVoxelsY,m_nbVoxelsX,m_nbVoxelsZ);
//   for ( k = m_nbVoxelsZ; k>= 1 ; k--){
//     for ( j = m_nbVoxelsY; j>=1; j--){
//       for ( i = m_nbVoxelsX; i>=1 ; i--){
// 	m_z[m_t] = f*m_z[m_t]+d*(m_z[m_t+m_nx]+m_z[m_t+1]+m_z[m_t+m_n2]);
// 	m_t--;
//       }//for i
//       m_t-=2;
//     }//for j
//     m_t-=m_t2nx;
//   }//for k
	
//   // p=z
//   copy(m_z, &m_z[m_nbVoxels], m_p);
    
//   // calcul de r.z
//   m_t = m_t1;
//   rho0=0.0;
//   for ( k = 1; k <= m_nbVoxelsZ; k++){   
//     for ( j= 1; j <= m_nbVoxelsY; j++){
//       for ( i = 1; i <= m_nbVoxelsX; i++){
// 	rho0+=m_r[m_t]*m_z[m_t];
// 	m_t++;
//       }//for i
//       m_t+=2;
//     }//for j
//     m_t+=m_t2nx;
//   }//for k
  
//   // début des itérations
//   for( uint numiter=0;numiter<maxiter;numiter++){
//     //calcul de q =  A.p
//     m_t = m_t1;
//     for ( k = 1; k <= m_nbVoxelsZ; k++){
//       for ( j = 1; j <= m_nbVoxelsY; j++){
// 	for ( i = 1; i <= m_nbVoxelsX; i++){
// 	  m_q[m_t] = diagonal * m_p[m_t] - 
// 	    a * (m_p[m_t-m_nx] + m_p[m_t+m_nx] +
// 		 m_p[m_t-1] + m_p[m_t+1] +
// 		 m_p[m_t-m_n2] + m_p[m_t+m_n2]);
// 	  m_t++;
// 	}//for i
// 	m_t+=2;
//       }//for j
//       m_t+=m_t2nx;
//     }//for k
    
//     //calcul du produit scalaire p.q
//     alpha=0.0;
//     m_t = m_t1;
//     for ( k = 1; k <= m_nbVoxelsZ; k++){
//       for ( j = 1; j <= m_nbVoxelsY; j++){
// 	for ( i = 1; i <= m_nbVoxelsX; i++){
// 	  alpha+=m_p[m_t]*m_q[m_t];
// 	  m_t++;
// 	}//for i
// 	m_t+=2;
//       }//for j
//       m_t+=m_t2nx;
//     }//for k
    
//     //calcul de alpha
//     alpha= rho0/alpha;
//     //alpha=(alpha) ? rho0/alpha : 0;
		
//     // calcul de x = x + alpha.p
//     // calcul de r = r -alpha*q
//     m_t = m_t1;
//     for ( k = 1; k <= m_nbVoxelsZ; k++){
//       for ( j = 1; j <= m_nbVoxelsY; j++){
// 	for ( i = 1; i <= m_nbVoxelsX; i++){
// 	  x0[m_t]+=alpha*m_p[m_t];
// 	  m_r[m_t]-=alpha*m_q[m_t];
// 	  m_t++;
// 	}//for i
// 	m_t+=2;
//       }//for j
//       m_t+=2*m_nx;
//     }//for k
    
//     t += (rdtsc() - start)/CPU_FREQ;
//     // calcul du carré de la norme du résidu
//     norm2=0.0f;
//     m_t = m_t1;
//     for ( k = 1; k <= m_nbVoxelsZ; k++){
//       for ( j = 1; j <= m_nbVoxelsY; j++){
// 	for ( i = 1; i <= m_nbVoxelsX; i++){
// 	  norm2+=m_r[m_t]*m_r[m_t];
// 	  m_t++;
// 	}//for i
// 	m_t+=2;
//       }//for j
//       m_t+=m_t2nx;
//     }//for k

//     computeAverage(numiter+1,norm2,t);
    
//     start=rdtsc();
//     //test d'arrêt
// //     if(norm2 < eb2){
// //       cerr<<"précision atteinte : nbiter = "<<numiter<<endl;
// //       break;
// //     }
//     // calcul de z tel que Cz =r

//     // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
//     m_t = m_t1;
//     for ( k = 1; k <= m_nbVoxelsZ; k++){
//       for ( j = 1; j <= m_nbVoxelsY; j++){
// 	for ( i = 1; i <= m_nbVoxelsX; i++){
// 	  m_z[m_t] = e*m_r[m_t]+d*(m_z[m_t-m_nx]+m_z[m_t-1]+m_z[m_t-m_n2]);
// 	  m_t++;
// 	}//for i
// 	m_t+=2;
//       }//for j
//       m_t+=m_t2nx;
//     }//for k
    
//     // calcul de z tel que (D/w -TL)z=u

//     m_t =IX(m_nbVoxelsY,m_nbVoxelsX,m_nbVoxelsZ) ;
//     for ( k = m_nbVoxelsZ; k>= 1 ; k--){
//       for ( j = m_nbVoxelsY; j>=1 ; j--){
// 	for ( i = m_nbVoxelsX; i>=1; i--){
// 	  m_z[m_t] = f*m_z[m_t]+d*(m_z[m_t+m_nx]+m_z[m_t+1]+m_z[m_t+m_n2]);
// 	  m_t--;
// 	}//for i
// 	m_t-=2;
//       }//for j
//       m_t-=m_t2nx;
//     }//for k

//     //calcul de rho1 = r.z
//     rho1=0.0;
//     m_t = m_t1;
//     for ( k = 1; k <= m_nbVoxelsZ; k++){
//       for ( j = 1; j <= m_nbVoxelsY; j++){
// 	for ( i = 1; i <= m_nbVoxelsX; i++){
// 	  rho1+=m_r[m_t]*m_z[m_t];
// 	  m_t++;
// 	}//for i
// 	m_t+=2;
//       }//for j
//       m_t+=m_t2nx;
//     }//for k
    
//     //calcul de beta =rho1/rho0
//     beta= rho1/rho0;
//     //beta=(rho0) ? rho1/rho0 : 0;

//     rho0=rho1;
//     //calcul de p = z+ beta.p
//     m_t = m_t1;
//     for ( k = 1; k <= m_nbVoxelsZ; k++){
//       for ( j = 1; j <= m_nbVoxelsY; j++){
// 	for ( i = 1; i <= m_nbVoxelsX; i++){
// 	  m_p[m_t]=m_z[m_t] + beta*m_p[m_t];
// 	  m_t++;
// 	}//for i
// 	m_t+=2;
//       }//for j
//       m_t+=m_t2nx;
//     }//for k
//   }//for numiter
// //   if(norm2 > eb2){
// //     cerr<<"précision non atteinte !!!"<<endl;
// //   }
  
//   return;
// }//GCSSOR


void LogResAvgTimeSolver3D::GCSSOR(float *const x0, const float *const b, float a, float diagonal, float omega, uint maxiter)
{
  float t=0.0f;
  unsigned long long int start;
  float f=omega/diagonal;
  float d=f*a;
  float e=2.0f-omega;
  uint i,j,k;
  
  float rho0, rho1, alpha, beta,norm2,normb2,eb2;
  
  float * mp,* mpmnx,* mppnx,* mpm1,* mpp1,* mpmn2,* mppn2, * mq;
  float * mz,* mzmnx,* mzpnx,* mzm1,* mzp1,* mzmn2,* mzpn2, *xx, *mr;
  const float * mb;
  // calcul du carré de la norme de b
  normb2=0.0f;
  mb=&b[m_t1];
  for ( k = 1; k <= m_nbVoxelsZ; k++){
    for ( j = 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
  	normb2+=(*mb) * (*mb);
	mb++;
      }//for i
      mb+=2;
    }//for j
    mb+=m_t2nx;
  }//for k
  
  // calcul de eb2 le second membre du test d'arrêt
  eb2=m_epsilon*normb2;
  
  // calcul du premier résidu r
  // calcul de r = b - A*x0
  // calcul de z tel que Cz=r
  // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
  mb=&b[m_t1]; mr=&m_r[m_t1]; mz=&m_z[m_t1]; mzmnx=mz-m_nx; mzm1=mz-1; mzmn2=mz-m_n2;
  mp=&x0[m_t1]; mpmnx=mp-m_nx; mppnx=mp+m_nx; mpm1=mp-1; mpp1=mp+1; mpmn2=mp-m_n2; mppn2=mp+m_n2;
  
  for ( k = 1; k <= m_nbVoxelsZ; k++){
    for ( j= 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	*mr = *mb - diagonal * (*mp) + a * ( (*mpmnx) + (*mppnx) + (*mpm1) + (*mpp1) + (*mpmn2) + (*mppn2) );
	*mz = e*(*mr)+d*( (*mzmnx) + (*mzm1) + (*mzmn2) );
	
	mb++; mr++; mp++; mpmn2++; mpmnx++; mpm1++; mpp1++; mppnx++; mppn2++; mz++; mzmn2++; mzmnx++; mzm1++;
      }//for i
      mb+=2; mr+=2; mp+=2; mpmn2+=2; mpmnx+=2; mpm1+=2; mpp1+=2; mppnx+=2; mppn2+=2; mz+=2; mzmn2+=2; mzmnx+=2; mzm1+=2;
    }//for j
    mb+=m_t2nx; mr+=m_t2nx; mp+=m_t2nx; mpmn2+=m_t2nx; mpmnx+=m_t2nx; mpm1+=m_t2nx; mpp1+=m_t2nx;
    mppnx+=m_t2nx; mppn2+=m_t2nx; mz+=m_t2nx; mzmn2+=m_t2nx; mzmnx+=m_t2nx; mzm1+=m_t2nx;
  }// for k  
  
  // calcul du carré de la norme du résidu
  norm2=0.0f;
  for ( k = 1; k <= m_nbVoxelsZ; k++)
    for ( j = 1; j <= m_nbVoxelsY; j++)
      for ( i = 1; i <= m_nbVoxelsX; i++)
	norm2+=m_r[IX(i,j,k)]*m_r[IX(i,j,k)];
  computeAverage(0,norm2,0.0f);
  
  start=rdtsc();
  //  m_swatch.Start();
  // calcul de z tel que (D/w -TL)z=u
  // pour calcul de z tel que Cz =r    
  // calcul de z tel que (D/w -TL)z=u

  m_t =IX(m_nbVoxelsX,m_nbVoxelsY,m_nbVoxelsZ) ;
  mz=&m_z[m_t]; mzpnx=mz+m_nx; mzp1=mz+1; mzpn2=mz+m_n2;
  
  for ( k = m_nbVoxelsZ; k>= 1 ; k--){
    for ( j = m_nbVoxelsY; j>=1 ; j--){
      for ( i = m_nbVoxelsX; i>=1; i--){
	(*mz) = f*(*mz)+d*( (*mzpnx) + (*mzp1) + (*mzpn2) );
	
	mz--; mzpnx--; mzp1--; mzpn2--;
      }//for i
      mz-=2; mzpnx-=2; mzp1-=2; mzpn2-=2;
    }//for j
    mz-=m_t2nx; mzpnx-=m_t2nx; mzp1-=m_t2nx; mzpn2-=m_t2nx;

  }//for k
  // p=z
  copy(m_z, &m_z[m_nbVoxels], m_p);
  
  // calcul de r.z
  rho0=0.0f;
  mr=&m_r[m_t1]; mz=&m_z[m_t1];
  for ( k = 1; k <= m_nbVoxelsZ; k++){
    for ( j = 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	rho0+=(*mr)*(*mz);
	
	mr++; mz++;
      }//for i
      mr+=2; mz+=2;
    }//for j
    mr+=m_t2nx; mz+=m_t2nx;
  }//for k
  
  // début des itérations
  for( uint numiter=0;numiter<maxiter;numiter++){
    //calcul de q =  A.p
    //calcul du produit scalaire p.q
    alpha=0.0f;
    mq=&m_q[m_t1]; mp=&m_p[m_t1]; mpmnx=mp-m_nx; mppnx=mp+m_nx; 
    mpm1=mp-1; mpp1=mp+1; mpmn2=mp-m_n2; mppn2=mp+m_n2;
    
    for (k = 1; k <= m_nbVoxelsZ; k++){
      for (j = 1; j <= m_nbVoxelsY; j++){
	for (i = 1; i <= m_nbVoxelsX; i++){
	  *mq = diagonal * (*mp) - a * ( (*mpmnx) + (*mppnx) + (*mpm1) + (*mpp1) + (*mpmn2) + (*mppn2) );
	  alpha+=(*mp)*(*mq);
	  
	  mq++; mp++; mpmnx++; mppnx++; mpm1++; mpp1++; mpmn2++; mppn2++;
	}//for i
	mq+=2; mp+=2; mpmnx+=2; mppnx+=2; mpm1+=2; mpp1+=2; mpmn2+=2; mppn2+=2;
      }//for j
      mq+=m_t2nx; mp+=m_t2nx; mpmnx+=m_t2nx; mppnx+=m_t2nx; mpm1+=m_t2nx; mpp1+=m_t2nx; mpmn2+=m_t2nx; mppn2+=m_t2nx;
    }//for k   
    
    //calcul de alpha
    alpha= rho0/alpha;
    //alpha=(alpha) ? rho0/alpha : 0.0f;
    //alpha=0.0f;
    
    // calcul de x = x + alpha.p
    // calcul de r = r -alpha*q
    xx=&x0[m_t1]; mp=&m_p[m_t1]; mq=&m_q[m_t1]; mr=&m_r[m_t1];
    
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  (*xx)+=alpha*(*mp);
	  (*mr)-=alpha*(*mq);
	  xx++; mp++; mq++; mr++;
	}//for i
	xx+=2; mp+=2; mq+=2; mr+=2;
      }//for j
      xx+=m_t2nx; mp+=m_t2nx; mq+=m_t2nx; mr+=m_t2nx;
    }//for k
    
//     if(norm2 < eb2){
//       //cerr << ((a == 1) ? "proj : " : "diff : ") << numiter << endl;
//       return;
//     }
    
    t += (rdtsc() - start)/CPU_FREQ;
    // calcul du carré de la norme du résidu    
    norm2=0.0f; mr=&m_r[m_t1];    
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  norm2+=(*mr)*(*mr);
	  mr++;
	}//for i
	mr+=2;
      }//for j
      mr+=m_t2nx;
    }//for k
    
    computeAverage(numiter+1,norm2,t);
    
    start=rdtsc();
    mr=&m_r[m_t1]; mz=&m_z[m_t1];
    mzmnx=mz-m_nx; mzm1=mz-1; mzmn2=mz-m_n2;
    
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  norm2+=(*mr)*(*mr);
	  // pour calcul de z tel que Cz =r
	  // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
	  // ce calcul sera fait inutilement une fois quand le critère d'arrêt
	  // sera satisfait
	  (*mz) = e*(*mr)+d*( (*mzmnx) + (*mzm1) + (*mzmn2) );
	  
	  mr++; mz++; mzmnx++; mzm1++; mzmn2++;
	}//for i
	mr+=2; mz+=2; mzmnx+=2; mzm1+=2; mzmn2+=2;
      }//for j
      mr+=m_t2nx; mz+=m_t2nx; mzmnx+=m_t2nx; mzm1+=m_t2nx; mzmn2+=m_t2nx;
    }//for k
    
    // pour calcul de z tel que Cz =r    
    // calcul de z tel que (D/w -TL)z=u
    
    m_t =IX(m_nbVoxelsX,m_nbVoxelsY,m_nbVoxelsZ) ;
    mz=&m_z[m_t]; mzpnx=mz+m_nx; mzp1=mz+1; mzpn2=mz+m_n2;
    
    for ( k = m_nbVoxelsZ; k>= 1 ; k--){
      for ( j = m_nbVoxelsY; j>=1 ; j--){
	for ( i = m_nbVoxelsX; i>=1; i--){
	  (*mz) = f*(*mz) + d*( (*mzpnx) + (*mzp1) + (*mzpn2) );
	  mz--; mzpnx--; mzp1--; mzpn2--;
	}//for i
	mz-=2; mzpnx-=2; mzp1-=2; mzpn2-=2;
      }//for j
      mz-=m_t2nx; mzpnx-=m_t2nx; mzp1-=m_t2nx; mzpn2-=m_t2nx;
      
    }//for k
    //calcul de rho1 = r.z
    rho1=0.0f;
    mr=&m_r[m_t1]; mz=&m_z[m_t1];
    
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  rho1+=(*mr)*(*mz);
	  
	  mr++; mz++;
	}//for i
	mr+=2; mz+=2;
      }//for j
      mr+=m_t2nx; mz+=m_t2nx;
    }//for k
    
    //calcul de beta =rho1/rho0
    beta= rho1/rho0;
    //beta=(rho0) ? rho1/rho0 : 0.0f;
    rho0=rho1;
    
    //calcul de p = z+ beta.p
    mp=&m_p[m_t1]; mz=&m_z[m_t1];
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  (*mp)=(*mz)+beta*(*mp);
	  
	  mp++;mz++;
	}//for i
	mp+=2; mz+=2;
      }//for j
      mp+=m_t2nx; mz+=m_t2nx;
    }//for k
  }//for numiter
  //  if(norm2 > eb2){
  //    cerr<<"précision non atteinte !!!"<<endl;
  //}
  
  return;
}//GCSSOR

void LogResAvgTimeSolver3D::computeAverage (uint iter, float value, float time)
{
  uint i = m_index*(m_nbSteps+1) + iter;
  
  m_averages[i] = (m_averages[i] * m_nbIter + sqrt(value))/(float)(m_nbIter+1);
  m_times[i] = (m_times[i] * m_nbIter + time)/(float)(m_nbIter+1);
}

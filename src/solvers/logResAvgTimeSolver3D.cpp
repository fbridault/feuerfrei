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
  
  if(m_nbIter == m_nbMaxIter)
    for(uint i=0; i < m_nbSteps; i++){
      m_file << i << " ";
      
      for(uint j=0; j < (NB_PROJ_LOGS+NB_DIFF_LOGS) ; j++)
	m_file << m_times[j*m_nbSteps + i] << " " << m_averages[j*m_nbSteps + i] << " ";
      
      m_file << endl;
    } 
}

void LogResAvgTimeSolver3D::GS_solve(unsigned char b, float *const x, float *const x0, float a, float div, uint nb_steps)
{  
  float t=0.0f;
  ::wxStartTimer();

  uint i, j, k, l;
  float diagonal = 1/div;
  float norm2;
  
  for (l = 0; l < nb_steps; l++){
    m_t=m_t1;
    for (k = 1; k <= m_nbVoxelsZ; k++){
      for (j = 1; j <= m_nbVoxelsY; j++){
	for (i = 1; i <= m_nbVoxelsX; i++){
	  x[m_t] = ( x0[m_t] + a * (x[m_t-1] + x[m_t+1] + x[m_t-m_nx] + x[m_t+m_nx] + x[m_t-m_n2] +x[m_t+m_n2]) ) * div;
	  //set_bnd (b, x);
	  m_t++;
	}//for i
	m_t+=2;
      }//for j
      m_t+=m_t2nx;
    }//for k

    // calcul du résidu
    m_t=m_t1;
    for (k = 1; k <= m_nbVoxelsZ; k++){
      for (j = 1; j <= m_nbVoxelsY; j++){
	for (i = 1; i <= m_nbVoxelsX; i++){
	  m_r[m_t] = x0[m_t] - diagonal * x[m_t] + 
	    a * (x[m_t-1] + x[m_t+1] +  x[m_t-m_nx] + x[m_t+m_nx] + x[m_t-m_n2] + x[m_t+m_n2]);
	  m_t++;
	}//for i
	m_t+=2;
      }//for j
      m_t+=m_t2nx;
    }//for k
    
    // calcul du carré de la norme du résidu
    m_t=m_t1;
    norm2=0.0f;
    for (k = 1; k <= m_nbVoxelsZ; k++){
      for (j = 1; j <= m_nbVoxelsY; j++){
	for (i = 1; i <= m_nbVoxelsX; i++){
	  norm2+=m_r[m_t]*m_r[m_t];
	  m_t++;
	}//for i
	m_t+=2;
      }//for j
      m_t+=m_t2nx;
    }//for k
    
    t += ::wxGetElapsedTime (false);
    
    computeAverage(l,norm2,t);
    ::wxStartTimer();
  }//for l
  //set_bnd (b, x);
}

void LogResAvgTimeSolver3D::GCSSOR(float *const x0, const float *const b, float a, float diagonal, float omega, uint maxiter)
{
  float t=0.0f;
  ::wxStartTimer();


  float f=omega/diagonal;
  float d=f*a;
  float e=2.0f-omega;
  uint i,j,k;
  
  float rho0, rho1, alpha, beta,norm2,normb2,eb2;
  //float *sav1, *sav2;
  
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
  //calcul de r = b - A*x0
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
    // alpha= rho0/alpha;
    alpha=(alpha) ? rho0/alpha : 0.0f;
    // calcul de x = x + alpha.p
    // calcul de r = r -alpha*q
    // calcul du carré de la norme du résidu

    norm2=0.0f;
    
    xx=&x0[m_t1]; mp=&m_p[m_t1]; mq=&m_q[m_t1]; mr=&m_r[m_t1]; mz=&m_z[m_t1];
    mzmnx=mz-m_nx; mzm1=mz-1; mzmn2=mz-m_n2;
    
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  (*xx)+=alpha*(*mp);
	  (*mr)-=alpha*(*mq);
	  norm2+=(*mr)*(*mr);
	  // pour calcul de z tel que Cz =r
	  // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
	  // ce calcul sera fait inutilement une fois quand le critère d'arrêt
	  // sera satisfait
	  (*mz) = e*(*mr)+d*( (*mzmnx) + (*mzm1) + (*mzmn2) );
	  
	  xx++; mp++; mq++; mr++; mz++; mzmnx++; mzm1++; mzmn2++;
	}//for i
	xx+=2; mp+=2; mq+=2; mr+=2; mz+=2; mzmnx+=2; mzm1+=2; mzmn2+=2;
      }//for j
      xx+=m_t2nx; mp+=m_t2nx; mq+=m_t2nx; mr+=m_t2nx; mz+=m_t2nx; mzmnx+=m_t2nx; mzm1+=m_t2nx; mzmn2+=m_t2nx;
    }//for k
    
//     if(norm2 < eb2){
//       //cerr << ((a == 1) ? "proj : " : "diff : ") << numiter << endl;
//       return;
//     }
    t += ::wxGetElapsedTime (false);
    
    computeAverage(numiter,norm2,t);
    ::wxStartTimer();    
  
    // calcul de z tel que (D/w -TL)z=u
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
    //    beta= rho1/rho0;
    beta=(rho0) ? rho1/rho0 : 0;

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
  if(norm2 > eb2){
    cerr<<"précision non atteinte !!!"<<endl;
  }
  
  return;
}//GCSSOR

void LogResAvgTimeSolver3D::computeAverage (uint iter, float value, float time)
{
  uint i = m_index*m_nbSteps + iter;
  
  m_averages[i] = (m_averages[i] * m_nbIter + sqrt(value))/(float)(m_nbIter+1);
  m_times[i] = (m_times[i] * m_nbIter + time)/(float)(m_nbIter+1);
}

#include "GCSSORSolver3D.hpp"

GCSSORSolver3D::GCSSORSolver3D (float omegaDiff, float omegaProj, float epsilon)
{
  m_r=new float[m_nbVoxels];
  m_z=new float[m_nbVoxels];
  m_p=new float[m_nbVoxels];
  m_q=new float[m_nbVoxels];

  fill_n(m_r, m_nbVoxels, 0.0f);
  fill_n(m_z, m_nbVoxels, 0.0f);
  fill_n(m_p, m_nbVoxels, 0.0f);
  fill_n(m_q, m_nbVoxels, 0.0f);
  
  m_omegaDiff = omegaDiff;
  m_omegaProj = omegaProj;
  m_epsilon = epsilon;
}

GCSSORSolver3D::GCSSORSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale,
				float timeStep, float buoyancy, float omegaDiff, float omegaProj, float epsilon) : 
  Solver3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy)
{
  m_r=new float[m_nbVoxels];
  m_z=new float[m_nbVoxels];
  m_p=new float[m_nbVoxels];
  m_q=new float[m_nbVoxels];

  fill_n(m_r, m_nbVoxels, 0.0f);
  fill_n(m_z, m_nbVoxels, 0.0f);
  fill_n(m_p, m_nbVoxels, 0.0f);
  fill_n(m_q, m_nbVoxels, 0.0f);

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

void GCSSORSolver3D::GCSSOR(float *const x0, const float *const b, float a,
			    float diagonal, float omega, uint maxiter)
{
  float f=omega/diagonal;
  float d=f*a;
  float e=2.0f-omega;
  uint i,j,k;
  
  float rho0, rho1, alpha, beta;//,norm2,normb2,eb2;
  //float *sav1, *sav2;
  
  float * mp,* mpmnx,* mppnx,* mpm1,* mpp1,* mpmn2,* mppn2, * mq;
  float * mz,* mzmnx,* mzpnx,* mzm1,* mzp1,* mzmn2,* mzpn2, *xx, *mr;
  const float * mb;
  // calcul du carr� de la norme de b
  //   t = t1;
  //   normb2=0.0f;
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
  
  //   // calcul de eb2 le second membre du test d'arr�t
  //   eb2=m_epsilon*normb2;

  // calcul du premier r�sidu r
  //calcul de r = b - A*x0
  // calcul de z tel que Cz=r
  // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
  mb=&b[m_t1];
  mr=&m_r[m_t1];
  mz=&m_z[m_t1];
  mzmnx=mz-m_nx;
  mzm1=mz-1;
  mzmn2=mz-m_n2;
  mp=&x0[m_t1];
  mpmnx=mp-m_nx;
  mppnx=mp+m_nx;
  mpm1=mp-1;
  mpp1=mp+1;
  mpmn2=mp-m_n2;
  mppn2=mp+m_n2;
  for ( k = 1; k <= m_nbVoxelsZ; k++){
    for ( j= 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	*mr = *mb - diagonal * (*mp) + a * (
					       (*mpmnx)
					       + (*mppnx)
					       + (*mpm1) 
					       +	(*mpp1)
					       + (*mpmn2)
					       + (*mppn2)	
					       );
	*mz = e*(*mr)+d*(
			 (*mzmnx)
			 +(*mzm1)
			 +(*mzmn2)
			 );
	mb++;
	mr++;
	mp++;
	mpmn2++;
	mpmnx++;
	mpm1++;
	mpp1++;
	mppnx++;
	mppn2++;
	mz++;
	mzmn2++;
	mzmnx++;
	mzm1++;
      }//for i
      mb+=2;
      mr+=2;
      mp+=2;
      mpmn2+=2;
      mpmnx+=2;
      mpm1+=2;
      mpp1+=2;
      mppnx+=2;
      mppn2+=2;
      mz+=2;
      mzmn2+=2;
      mzmnx+=2;
      mzm1+=2;
    }//for j
    mb+=m_t2nx;
    mr+=m_t2nx;
    mp+=m_t2nx;
    mpmn2+=m_t2nx;
    mpmnx+=m_t2nx;
    mpm1+=m_t2nx;
    mpp1+=m_t2nx;
    mppnx+=m_t2nx;
    mppn2+=m_t2nx;
    mz+=m_t2nx;
    mzmn2+=m_t2nx;
    mzmnx+=m_t2nx;
    mzm1+=m_t2nx;
  }// for k  
  
  // calcul de z tel que (D/w -TL)z=u
  // pour calcul de z tel que Cz =r    
  // calcul de z tel que (D/w -TL)z=u

  m_t =IX(m_nbVoxelsX,m_nbVoxelsY,m_nbVoxelsZ) ;
  mz=&m_z[m_t];
  mzpnx=mz+m_nx;
  mzp1=mz+1;
  mzpn2=mz+m_n2;
  for ( k = m_nbVoxelsZ; k>= 1 ; k--){
    for ( j = m_nbVoxelsY; j>=1 ; j--){
      for ( i = m_nbVoxelsX; i>=1; i--){
	(*mz) = f*(*mz)+d*(
			   (*mzpnx)
			   + (*mzp1)
			   + (*mzpn2)
			   );
	mz--;
	mzpnx--;
	mzp1--;
	mzpn2--;
      }//for i
      mz-=2;
      mzpnx-=2;
      mzp1-=2;
      mzpn2-=2;
    }//for j
    mz-=m_t2nx;
    mzpnx-=m_t2nx;
    mzp1-=m_t2nx;
    mzpn2-=m_t2nx;

  }//for k
  // p=z
  copy(m_z, &m_z[m_nbVoxels], m_p);
  
  // calcul de r.z
  rho0=0.0f;
  mr=&m_r[m_t1];
  mz=&m_z[m_t1];
  for ( k = 1; k <= m_nbVoxelsZ; k++){
    for ( j = 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	rho0+=(*mr)*(*mz);
	mr++;
	mz++;
      }//for i
      mr+=2;
      mz+=2;
    }//for j
    mr+=m_t2nx;
    mz+=m_t2nx;
  }//for k
  
  // d�but des it�rations
  for( uint numiter=0;numiter<maxiter;numiter++){
    //calcul de q =  A.p
    //calcul du produit scalaire p.q
    alpha=0.0f;
    mq=&m_q[m_t1];
    mp=&m_p[m_t1];
    mpmnx=mp-m_nx;
    mppnx=mp+m_nx;
    mpm1=mp-1;
    mpp1=mp+1;
    mpmn2=mp-m_n2;
    mppn2=mp+m_n2;
		
    for (k = 1; k <= m_nbVoxelsZ; k++){
      for (j = 1; j <= m_nbVoxelsY; j++){
	for (i = 1; i <= m_nbVoxelsX; i++){
	  *mq = diagonal * (*mp)
	    -	a * (
		     (*mpmnx)
		     + (*mppnx)
		     + (*mpm1)
		     + (*mpp1)
		     + (*mpmn2)
		     + (*mppn2)
		     );
	  alpha+=(*mp)*(*mq);
	  mq++;
	  mp++;
	  mpmnx++;
	  mppnx++;
	  mpm1++;
	  mpp1++;
	  mpmn2++;
	  mppn2++;
	}//for i
	mq+=2;
	mp+=2;
	mpmnx+=2;
	mppnx+=2;
	mpm1+=2;
	mpp1+=2;
	mpmn2+=2;
	mppn2+=2;
      }//for j
      mq+=m_t2nx;
      mp+=m_t2nx;
      mpmnx+=m_t2nx;
      mppnx+=m_t2nx;
      mpm1+=m_t2nx;
      mpp1+=m_t2nx;
      mpmn2+=m_t2nx;
      mppn2+=m_t2nx;
    }//for k   
  
    //calcul de alpha
    // alpha= rho0/alpha;
    alpha=(alpha) ? rho0/alpha : 0.0f;
    // calcul de x = x + alpha.p
    // calcul de r = r -alpha*q
    // calcul du carr� de la norme du r�sidu
    //	norm2=0.0f;
    xx=&x0[m_t1];
    mp=&m_p[m_t1];
    mq=&m_q[m_t1];
    mr=&m_r[m_t1];
    mz=&m_z[m_t1];
    mzmnx=mz-m_nx;
    mzm1=mz-1;
    mzmn2=mz-m_n2;
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  (*xx)+=alpha*(*mp);
	  (*mr)-=alpha*(*mq);
	  //	norm2+=(*mr)*(*mr);
	  // pour calcul de z tel que Cz =r
	  // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
	  // ce calcul sera fait inutilement une fois quand le crit�re d'arr�t
	  // sera satisfait
	  (*mz) = e*(*mr)+d*(
			     (*mzmnx)+
			     (*mzm1)+
			     (*mzmn2)
			     );
	  xx++;
	  mp++;
	  mq++;
	  mr++;
	  mz++;
	  mzmnx++;
	  mzm1++;
	  mzmn2++;
	}//for i
	xx+=2;
	mp+=2;
	mq+=2;
	mr+=2;
	mz+=2;
	mzmnx+=2;
	mzm1+=2;
	mzmn2+=2;
      }//for j
      xx+=m_t2nx;
      mp+=m_t2nx;
      mq+=m_t2nx;
      mr+=m_t2nx;
      mz+=m_t2nx;
      mzmnx+=m_t2nx;
      mzm1+=m_t2nx;
      mzmn2+=m_t2nx;
    }//for k
   
    // calcul de z tel que (D/w -TL)z=u
    // pour calcul de z tel que Cz =r    
    // calcul de z tel que (D/w -TL)z=u

    m_t =IX(m_nbVoxelsX,m_nbVoxelsY,m_nbVoxelsZ) ;
    mz=&m_z[m_t];
    mzpnx=mz+m_nx;
    mzp1=mz+1;
    mzpn2=mz+m_n2;
    for ( k = m_nbVoxelsZ; k>= 1 ; k--){
      for ( j = m_nbVoxelsY; j>=1 ; j--){
	for ( i = m_nbVoxelsX; i>=1; i--){
	  (*mz) = f*(*mz) + d*(
			       (*mzpnx)
			       + (*mzp1)
			       + (*mzpn2)
			       );
	  mz--;
	  mzpnx--;
	  mzp1--;
	  mzpn2--;
	}//for i
	mz-=2;
	mzpnx-=2;
	mzp1-=2;
	mzpn2-=2;
      }//for j
      mz-=m_t2nx;
      mzpnx-=m_t2nx;
      mzp1-=m_t2nx;
      mzpn2-=m_t2nx;

    }//for k
    //calcul de rho1 = r.z
    rho1=0.0f;
    mr=&m_r[m_t1];
    mz=&m_z[m_t1];
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  rho1+=(*mr)*(*mz);
	  mr++;
	  mz++;
	}//for i
	mr+=2;
	mz+=2;
      }//for j
      mr+=m_t2nx;
      mz+=m_t2nx;
    }//for k

    //calcul de beta =rho1/rho0
    //    beta= rho1/rho0;
    beta=(rho0) ? rho1/rho0 : 0;

    rho0=rho1;
 
    //calcul de p = z+ beta.p
    mp=&m_p[m_t1];
    mz=&m_z[m_t1];
    for ( k = 1; k <= m_nbVoxelsZ; k++){
      for ( j = 1; j <= m_nbVoxelsY; j++){
	for ( i = 1; i <= m_nbVoxelsX; i++){
	  (*mp)=(*mz)+beta*(*mp);
	  mp++;
	  mz++;
	}//for i
	mp+=2;
	mz+=2;
      }//for j
      mp+=m_t2nx;
      mz+=m_t2nx;
    }//for k
  }//for numiter
  //   if(norm2 > eb2){
  //     cerr<<"pr�cision non atteinte !!!"<<endl;
  //   }
  
  return;
}//GCSSOR

/* Pas de diffusion */
void
GCSSORSolver3D::diffuse (unsigned char b, float *const x, float *const x0, float a, float diff_visc)
{
  GCSSOR(x,x0,a, (1.0f + 6.0f * a), m_omegaDiff,100);
}

void
GCSSORSolver3D::project (float *const p, float *const div)
{
  float h_x = 1.0f / m_nbVoxelsX, 
    h_y = 1.0f / m_nbVoxelsY, 
    h_z = 1.0f / m_nbVoxelsZ;
  uint i, j, k;
  
  m_t = m_t1;
  for ( k = 1; k <= m_nbVoxelsZ; k++){
    for ( j = 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	div[m_t] = -0.5f * (
			    h_x * (m_u[m_t+1] - m_u[m_t-1]) +
			    h_y * (m_v[m_t+m_nx] - m_v[m_t-m_nx]) +
			    h_z * (m_w[m_t+m_n2] - m_w[m_t-m_n2])
			    );
	m_t++;
      }//for i
      m_t+=2;
    }//for j
    m_t+=m_t2nx;
    //p[IX (i, j, k)] = 0;
  }// for k
  
  //set_bnd (0, div);
  fill_n(p, m_nbVoxels, 0.0f);
  //set_bnd (0, p);
  
  GCSSOR(p,div,1, 6.0f, m_omegaProj,100);
  
  m_t = m_t1;
  for (k = 1; k <= m_nbVoxelsZ; k++){
    for (j = 1; j <= m_nbVoxelsY; j++){
      for (i = 1; i <= m_nbVoxelsX; i++){
	m_u[m_t] -= 0.5f * (p[m_t+1] - p[m_t-1]) / h_x;
	m_v[m_t] -= 0.5f * (p[m_t+m_nx] - p[m_t-m_nx]) / h_y;
	m_w[m_t] -= 0.5f * (p[m_t+m_n2] - p[m_t-m_n2]) / h_z;
	m_t++;
      }// for i
      m_t+=2;
    }// for j
    m_t+=m_t2nx;
  }//for k
  
  //set_bnd (1, u);
  //set_bnd (2, v);
  //set_bnd (3, w);
}

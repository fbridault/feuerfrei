#include "GCSSORSolver3D-SSE.hpp"
#include "SSE4.hpp"

GCSSORSolver3D_SSE::GCSSORSolver3D_SSE (float omegaDiff, float omegaProj, float epsilon)
{
  m_r=(float*)_mm_malloc(m_nbVoxels*sizeof(float),16);
  m_z=(float*)_mm_malloc(m_nbVoxels*sizeof(float),16);
  m_p=(float*)_mm_malloc(m_nbVoxels*sizeof(float),16);
  m_q=(float*)_mm_malloc(m_nbVoxels*sizeof(float),16);

  fill_n(m_r, m_nbVoxels, 0.0f);
  fill_n(m_z, m_nbVoxels, 0.0f);
  fill_n(m_p, m_nbVoxels, 0.0f);
  fill_n(m_q, m_nbVoxels, 0.0f);

  m_r_sse=(__m128*)(&m_r[m_debut]);
  m_z_sse=(__m128*)(&m_z[m_debut]);
  m_p_sse=(__m128*)(&m_p[m_debut]);
  m_q_sse=(__m128*)(&m_q[m_debut]);

  m_omegaDiff = omegaDiff;
  m_omegaProj = omegaProj;
  m_epsilon = epsilon;
}

GCSSORSolver3D_SSE::GCSSORSolver3D_SSE (const CPoint& position, uint n_x, uint n_y, uint n_z, float dim, const CPoint& scale,
					float timeStep, float buoyancy, float vorticityConfinement, float omegaDiff,
					float omegaProj, float epsilon) :
  Solver3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy, vorticityConfinement)
{
  m_r=(float*)_mm_malloc(m_nbVoxels*sizeof(float),16);
  m_z=(float*)_mm_malloc(m_nbVoxels*sizeof(float),16);
  m_p=(float*)_mm_malloc(m_nbVoxels*sizeof(float),16);
  m_q=(float*)_mm_malloc(m_nbVoxels*sizeof(float),16);

  fill_n(m_r, m_nbVoxels, 0.0f);
  fill_n(m_z, m_nbVoxels, 0.0f);
  fill_n(m_p, m_nbVoxels, 0.0f);
  fill_n(m_q, m_nbVoxels, 0.0f);

  m_r_sse=(__m128*)(&m_r[m_debut]);
  m_z_sse=(__m128*)(&m_z[m_debut]);
  m_p_sse=(__m128*)(&m_p[m_debut]);
  m_q_sse=(__m128*)(&m_q[m_debut]);

  m_omegaDiff = omegaDiff;
  m_omegaProj = omegaProj;
  m_epsilon = epsilon;
}

GCSSORSolver3D_SSE::~GCSSORSolver3D_SSE ()
{
  _mm_free(m_r);
  _mm_free(m_z);
  _mm_free(m_p);
  _mm_free(m_q);
}

void GCSSORSolver3D_SSE::GCSSOR(float *const x0, const float *const b, float a, float diagonal, float omega, uint maxiter)
{
  float f=omega/diagonal;
  float d=f*a;
  float e=2.0f-omega;
  uint i,j,k;
  float rho0, rho1, alpha, beta, norm2, normb2, eb2;
  //float *sav1, *sav2;
  //	int t;

  SSE4f alpha_sse;
  SSE4f beta_sse;
  SSE4f norme_sse;
  SSE4f rho1_sse;
  SSE4f rho0_sse;

  __m128 * p_sse, * q_sse, * r_sse, * z_sse, * x_sse;
  __m128 *x0_sse;

  float * mp,* mpmnx,* mppnx,* mpm1,* mpp1,* mpmn2,* mppn2, * mq;
  float * mz,* mzmnx,* mzpnx,* mzm1,* mzp1,* mzmn2,* mzpn2, *mr;
  const float * mb;

  x0_sse=(__m128*)(&x0[m_debut]);

//   // calcul du carré de la norme de b
//   normb2=0.0f;
//   mb=&b[m_t1];
//   for ( k = 1; k <= m_nbVoxelsZ; k++){
//     for ( j = 1; j <= m_nbVoxelsY; j++){
//       for ( i = 1; i <= m_nbVoxelsX; i++){
//   	normb2+=(*mb) * (*mb);
// 	mb++;
//       }//for i
//       mb+=2;
//     }//for j
//     mb+=m_t2nx;
//   }//for k
//   cerr << normb2 << endl;

  // calcul du carré de la norme du résidu
  norme_sse.m=_mm_setzero_ps();
  r_sse=(__m128*)(&b[m_debut]);
  for ( i = 0; i < m_nbgrps; i++){
    norme_sse.m=_mm_add_ps(norme_sse.m,_mm_mul_ps(*r_sse,*r_sse));
    p_sse++;
    q_sse++;
    r_sse++;
    x_sse++;
  }//for i

  normb2=norme_sse.f[0]+norme_sse.f[1]+norme_sse.f[2]+norme_sse.f[3];
//   cerr << normb2 << endl;

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

  rho0_sse.m=_mm_setzero_ps();
  r_sse=m_r_sse;
  z_sse=m_z_sse;
  for ( i = 0; i < m_nbgrps; i++){
    rho0_sse.m=_mm_add_ps(rho0_sse.m,_mm_mul_ps(*r_sse,*z_sse));
    r_sse++;
    z_sse++;
  }//for i
  rho0=rho0_sse.f[0]+rho0_sse.f[1]+rho0_sse.f[2]+ rho0_sse.f[3];

  // début des itérations
  for( uint numiter=0;numiter<maxiter;numiter++){
    //calcul de q =  A.p
    mq=&m_q[m_t1]; mp=&m_p[m_t1]; mpmnx=mp-m_nx; mppnx=mp+m_nx;
    mpm1=mp-1; mpp1=mp+1; mpmn2=mp-m_n2; mppn2=mp+m_n2;

    for (k = 1; k <= m_nbVoxelsZ; k++){
      for (j = 1; j <= m_nbVoxelsY; j++){
	for (i = 1; i <= m_nbVoxelsX; i++){
	  *mq = diagonal * (*mp) - a * ( (*mpmnx) + (*mppnx) + (*mpm1) + (*mpp1) + (*mpmn2) + (*mppn2) );

	  mq++; mp++; mpmnx++; mppnx++; mpm1++; mpp1++; mpmn2++; mppn2++;
	}//for i
	mq+=2; mp+=2; mpmnx+=2; mppnx+=2; mpm1+=2; mpp1+=2; mpmn2+=2; mppn2+=2;
      }//for j
      mq+=m_t2nx; mp+=m_t2nx; mpmnx+=m_t2nx; mppnx+=m_t2nx; mpm1+=m_t2nx; mpp1+=m_t2nx; mpmn2+=m_t2nx; mppn2+=m_t2nx;
    }//for k

    //calcul du produit scalaire p.q
    alpha_sse.m=_mm_setzero_ps();
    p_sse=m_p_sse;
    q_sse=m_q_sse;
    for (i = 0; i < m_nbgrps; i++){
      alpha_sse.m=_mm_add_ps(alpha_sse.m,_mm_mul_ps(*p_sse,*q_sse));
      p_sse++;
      q_sse++;
    }//for i
    alpha=alpha_sse.f[0]+alpha_sse.f[1]+alpha_sse.f[2]+alpha_sse.f[3];
    alpha=(alpha) ? rho0/alpha : 0.0f;
    alpha_sse.m=_mm_set_ps1(alpha);
    // calcul de x = x + alpha.p
    // calcul de r = r -alpha*q
    // calcul du carré de la norme du résidu
    norme_sse.m=_mm_setzero_ps();
    p_sse=m_p_sse;
    q_sse=m_q_sse;
    r_sse=m_r_sse;
    x_sse=x0_sse;
    for ( i = 0; i < m_nbgrps; i++){
      *x_sse=_mm_add_ps(*x_sse,_mm_mul_ps(alpha_sse.m,*p_sse));
      *r_sse=_mm_sub_ps(*r_sse,_mm_mul_ps(alpha_sse.m,*q_sse));
      norme_sse.m=_mm_add_ps(norme_sse.m,_mm_mul_ps(*r_sse,*r_sse));
      p_sse++;
      q_sse++;
      r_sse++;
      x_sse++;
    }//for i

    norm2=norme_sse.f[0]+norme_sse.f[1]+norme_sse.f[2]+norme_sse.f[3];
    //test d'arrêt
    if(norm2 < eb2){
//       cout<<"précision atteinte : nbiter = "<<numiter<<endl;
      break;
    }
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
    rho1_sse.m=_mm_setzero_ps();
    r_sse=m_r_sse;
    z_sse=m_z_sse;
    for ( i = 0; i < m_nbgrps; i++){
      rho1_sse.m=_mm_add_ps(rho1_sse.m,_mm_mul_ps(*r_sse,*z_sse));
      r_sse++;
      z_sse++;
    }//for i
    rho1=rho1_sse.f[0]+rho1_sse.f[1]+rho1_sse.f[2]+ rho1_sse.f[3];
    //calcul de beta =rho1/rho0
    //    beta= rho1/rho0;
    beta=(rho0) ? rho1/rho0 : 0.0f;
    beta_sse.m=_mm_set_ps1(beta);
    rho0=rho1;
    //calcul de p = z+ beta.p
    z_sse=m_z_sse;
    p_sse=m_p_sse;
    for ( i = 0; i < m_nbgrps; i++){
      *p_sse=_mm_add_ps(*z_sse,_mm_mul_ps(beta_sse.m,*p_sse));
      p_sse++;
      z_sse++;
    }//for i
  }//for numiter
  return;
}//GCSSOR

/* Pas de diffusion */
void
GCSSORSolver3D_SSE::diffuse (unsigned char b, float *const x, float *const x0, float a)
{
  GCSSOR(x,x0,a, (1.0f + 6.0f * a), m_omegaDiff,4);
}

void
GCSSORSolver3D_SSE::project (float *const p, float *const div)
{
  uint i, j, k;

  m_t = m_t1;
  for ( k = 1; k <= m_nbVoxelsZ; k++){
    for ( j = 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	div[m_t] = m_hx * (m_u[m_t-1] - m_u[m_t+1]) +
	  m_hy * (m_v[m_t-m_nx] - m_v[m_t+m_nx]) +
	  m_hz * (m_w[m_t-m_n2] - m_w[m_t+m_n2]);
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

  GCSSOR(p,div,1, 6.0f, m_omegaProj,4);

  m_t = m_t1;
  for (k = 1; k <= m_nbVoxelsZ; k++){
    for (j = 1; j <= m_nbVoxelsY; j++){
      for (i = 1; i <= m_nbVoxelsX; i++){
	m_u[m_t] += m_invhx * (p[m_t-1] - p[m_t+1]);
	m_v[m_t] += m_invhy * (p[m_t-m_nx] - p[m_t+m_nx]);
	m_w[m_t] += m_invhz * (p[m_t-m_n2] - p[m_t+m_n2]);
	m_t++;
      }// for i
      m_t+=2;
    }// for j
    m_t+=m_t2nx;
  }//for k

  // set_bnd (1,m_u);
  //set_bnd (2,m_v);
  //set_bnd (3,m_w);
}

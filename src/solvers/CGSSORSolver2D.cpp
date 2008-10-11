#include "CGSSORSolver2D.hpp"

CGSSORSolver2D::CGSSORSolver2D (const CPoint& position, uint n_x, uint n_y, float dim, float timeStep,
				float buoyancy, float vorticityConfinement, float omegaDiff,
				float omegaProj, float epsilon) :
  Solver2D(position, n_x, n_y, dim, timeStep, buoyancy, vorticityConfinement)
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

CGSSORSolver2D::~CGSSORSolver2D ()
{
  delete[]m_r;
  delete[]m_z;
  delete[]m_p;
  delete[]m_q;
}

void CGSSORSolver2D::CGSSOR(float *x0, float *b, float a, float diagonal, float omega, int maxiter)
{
  float f=omega/diagonal;
  float d=f*a;
  float e=2.0f-omega;
  uint i,j;

  float rho0, rho1, alpha, beta,norm2,normb2,eb2;

  float *mp, *mpmnx, *mppnx, *mpm1, *mpp1, *mq;
  float *mz, *mzmnx, *mzpnx, *mzm1, *mzp1, *xx, *mr;
  float *mb;
  uint numiter;

  // calcul du carré de la norme de b
  normb2=0.0f;
  mb=&b[m_t1];
  for ( j = 1; j <= m_nbVoxelsY; j++){
    for ( i = 1; i <= m_nbVoxelsX; i++){
      normb2+=(*mb) * (*mb);
      mb++;
    }//for i
    mb+=2;
  }//for k

  // calcul de eb2 le second membre du test d'arrêt
  eb2=m_epsilon*normb2;

  // calcul du premier résidu r
  //calcul de r = b - A*x0
  // calcul de z tel que Cz=r
  // calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
  mb=&b[m_t1]; mr=&m_r[m_t1]; mz=&m_z[m_t1]; mzmnx=mz-m_nx; mzm1=mz-1;
  mp=&x0[m_t1]; mpmnx=mp-m_nx; mppnx=mp+m_nx; mpm1=mp-1; mpp1=mp+1;

  for ( j= 1; j <= m_nbVoxelsY; j++){
    for ( i = 1; i <= m_nbVoxelsX; i++){
      *mr = *mb - diagonal * (*mp) + a * ( (*mpmnx) + (*mppnx) + (*mpm1) + (*mpp1) );
      *mz = e*(*mr)+d*( (*mzmnx) + (*mzm1) );

      mb++; mr++; mp++; mpmnx++; mpm1++; mpp1++; mppnx++; mz++; mzmnx++; mzm1++;
    }//for i
    mb+=2; mr+=2; mp+=2; mpmnx+=2; mpm1+=2; mpp1+=2; mppnx+=2; mz+=2; mzmnx+=2; mzm1+=2;
  }

  // pour calcul de z tel que Cz =r
  // calcul de z tel que (D/w -TL)z=u
  m_t =IX(m_nbVoxelsX,m_nbVoxelsY) ;
  mz=&m_z[m_t]; mzpnx=mz+m_nx; mzp1=mz+1;

  for ( j = m_nbVoxelsY; j>=1 ; j--){
    for ( i = m_nbVoxelsX; i>=1; i--){
      (*mz) = f*(*mz)+d*( (*mzpnx) + (*mzp1) );

      mz--; mzpnx--; mzp1--;
    }//for i
    mz-=2; mzpnx-=2; mzp1-=2;
  }//for j

  // p=z
  memcpy(m_p, m_z, (m_nbVoxelsX+2)*(m_nbVoxelsY+2)*sizeof(float));

  // calcul de r.z
  rho0=0.0f;
  mr=&m_r[m_t1]; mz=&m_z[m_t1];
  for ( j = 1; j <= m_nbVoxelsY; j++){
    for ( i = 1; i <= m_nbVoxelsX; i++){
      rho0+=(*mr)*(*mz);

      mr++; mz++;
    }//for i
    mr+=2; mz+=2;
  }//for j

  // début des itérations
  for(numiter=0;numiter<maxiter;numiter++){
    //calcul de q =  A.p
    //calcul du produit scalaire p.q
    alpha=0.0f;
    mq=&m_q[m_t1]; mp=&m_p[m_t1]; mpmnx=mp-m_nx; mppnx=mp+m_nx;
    mpm1=mp-1; mpp1=mp+1;

    for (j = 1; j <= m_nbVoxelsY; j++){
      for (i = 1; i <= m_nbVoxelsX; i++){
	*mq = diagonal * (*mp) - a * ( (*mpmnx) + (*mppnx) + (*mpm1) + (*mpp1) );
	alpha+=(*mp)*(*mq);

	mq++; mp++; mpmnx++; mppnx++; mpm1++; mpp1++;
      }//for i
      mq+=2; mp+=2; mpmnx+=2; mppnx+=2; mpm1+=2; mpp1+=2;
    }//for j

    //calcul de alpha
    // alpha= rho0/alpha;
    alpha=(alpha) ? rho0/alpha : 0.0f;
    // calcul de x = x + alpha.p
    // calcul de r = r -alpha*q
    // calcul du carré de la norme du résidu

    norm2=0.0f;

    xx=&x0[m_t1]; mp=&m_p[m_t1]; mq=&m_q[m_t1]; mr=&m_r[m_t1]; mz=&m_z[m_t1];
    mzmnx=mz-m_nx; mzm1=mz-1;

    for ( j = 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	(*xx)+=alpha*(*mp);
	(*mr)-=alpha*(*mq);
	norm2+=(*mr)*(*mr);
	// pour calcul de z tel que Cz =r
	// calcul de u tel que 1/(2-w)*(D/w-L)D^(-1)w.u=r
	// ce calcul sera fait inutilement une fois quand le critère d'arrêt
	// sera satisfait
	(*mz) = e*(*mr)+d*( (*mzmnx) + (*mzm1) );

	xx++; mp++; mq++; mr++; mz++; mzmnx++; mzm1++;
      }//for i
      xx+=2; mp+=2; mq+=2; mr+=2; mz+=2; mzmnx+=2; mzm1+=2;
    }//for j

    if(norm2 < eb2){
      // Précision atteinte
      //       cerr << ((a == 1) ? "proj : " : "diff : ") << numiter << endl;
      return;
    }

    // calcul de z tel que (D/w -TL)z=u
    // pour calcul de z tel que Cz =r
    m_t =IX(m_nbVoxelsX,m_nbVoxelsY) ;
    mz=&m_z[m_t]; mzpnx=mz+m_nx; mzp1=mz+1;

    for ( j = m_nbVoxelsY; j>=1 ; j--){
      for ( i = m_nbVoxelsX; i>=1; i--){
	(*mz) = f*(*mz) + d*( (*mzpnx) + (*mzp1) );
	mz--; mzpnx--; mzp1--;
      }//for i
      mz-=2; mzpnx-=2; mzp1-=2;
    }//for j
    //calcul de rho1 = r.z
    rho1=0.0f;
    mr=&m_r[m_t1]; mz=&m_z[m_t1];

    for ( j = 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	rho1+=(*mr)*(*mz);

	mr++; mz++;
      }//for i
      mr+=2; mz+=2;
    }//for j

    //calcul de beta =rho1/rho0
    beta=(rho0) ? rho1/rho0 : 0;

    rho0=rho1;

    //calcul de p = z+ beta.p
    mp=&m_p[m_t1]; mz=&m_z[m_t1];
    for ( j = 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	(*mp)=(*mz)+beta*(*mp);
	mp++;mz++;
      }//for i
      mp+=2; mz+=2;
    }//for j
  }//for numiter

  if(norm2 > eb2){
    printf("Précision non atteinte !\n");
  }

  return;
}//GCSSOR

/* Pas de diffusion */
void CGSSORSolver2D::diffuse (unsigned char b, float *const x, float *const x0, float a)
{
  CGSSOR(x, x0, a, (1.0f + 4.0f * a), 1.0f, 100);
}

void CGSSORSolver2D::project (float *const p, float *const div)
{
  float h_x = 1.0f / m_nbVoxelsX, h_y = 1.0f / m_nbVoxelsY;
  uint i, j;

  m_t=m_t1;
  for (j = 1; j <= m_nbVoxelsY; j++){
    for (i = 1; i <= m_nbVoxelsX; i++){
      div[m_t] = -0.5f * ( h_x * (m_u[m_t+1] - m_u[m_t-1]) + h_y * (m_v[m_t+m_nx] - m_v[m_t-m_nx]) );
      m_t++;
    }//for i
    m_t+=2;
  }//for j
  //p[IX (i, j, k)] = 0;

  //  set_bnd (0, div);
  fill_n(p, m_nbVoxels, 0.0f);
  //  set_bnd (0, p);

  CGSSOR(p, div, 1, 4.0f, 1.3, 100);

  m_t=m_t1;
  for (j = 1; j <= m_nbVoxelsY; j++){
    for (i = 1; i <= m_nbVoxelsX; i++){
      m_u[m_t] -= 0.5f * (p[m_t+1] - p[m_t-1]) / h_x;
      m_v[m_t] -= 0.5f * (p[m_t+m_nx] - p[m_t-m_nx]) / h_y;
      m_t++;
    }//for i
    m_t+=2;
  }//for j

  //  set_bnd (1, m_u);
  //  set_bnd (2, m_v);
}//project

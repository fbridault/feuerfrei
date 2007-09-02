#include "solver3D.hpp"
#include "SSE4.hpp"
#include <math.h>
#include "../scene/graphicsFn.hpp"

Solver3D::Solver3D ()
{
}

Solver3D::Solver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale,
		    float timeStep, float buoyancy, float vorticityConfinement) : 
  RealField3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy)
{
  m_uPrev    = (float*)_mm_malloc( m_nbVoxels*sizeof(float),16);
  m_vPrev    = (float*)_mm_malloc( m_nbVoxels*sizeof(float),16);
  m_wPrev    = (float*)_mm_malloc( m_nbVoxels*sizeof(float),16);
  m_dens     = (float*)_mm_malloc( m_nbVoxels*sizeof(float),16);
  m_densPrev = (float*)_mm_malloc( m_nbVoxels*sizeof(float),16);
  m_densSrc  = (float*)_mm_malloc( m_nbVoxels*sizeof(float),16);
  m_rotx     = (float*)_mm_malloc( m_nbVoxels*sizeof(float),16);
  m_roty     = (float*)_mm_malloc( m_nbVoxels*sizeof(float),16);
  m_rotz     = (float*)_mm_malloc( m_nbVoxels*sizeof(float),16);
  m_rot      = (float*)_mm_malloc( m_nbVoxels*sizeof(float),16);
  
  fill_n(m_uPrev, m_nbVoxels, 0.0f);
  fill_n(m_vPrev, m_nbVoxels, 0.0f);
  fill_n(m_wPrev, m_nbVoxels, 0.0f);
  fill_n(m_dens, m_nbVoxels, 0.0f);
  fill_n(m_densPrev, m_nbVoxels, 0.0f);
  fill_n(m_densSrc, m_nbVoxels, 0.0f);
  
  m_visc = 0.00000015f;
  m_diff = 0.001f;
  m_vorticityConfinement = vorticityConfinement;
  
  // Utilisé pour la densité
  // m_aDiff = m_dt * m_diff * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
  m_aVisc = m_dt * m_visc * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;

  m_n2 = (m_nbVoxelsX+2) * (m_nbVoxelsY+2);
  m_nx = m_nbVoxelsX+2;
  m_t1 = m_n2 + m_nx + 1;
  m_t2nx = 2*m_nx;
  // chercher le premier groupe de 4 contenant le premier voxel
  // de la grille initiale
  // m_t1 = 4*q1 + r1 avec 0 <= r1 < 4
  // q1 est le numéro de ce premier groupe
  // m_debut = 4*q1 est le numéro du premier élément de ce groupe
  m_debut= m_t1-m_t1%4;
  // récupérer le numéro du dernier voxel de la grille initiale
  m_dernier=IX(m_nbVoxelsX,m_nbVoxelsY,m_nbVoxelsZ);
  //chercher le nombre de groupes de 4 contenant les voxels de la
  // grille initiale
  //m_dernier = 4*q2 + r2 avec 0 <= r2 < 4
  // m_nbgrps = q2 - q1 + 1 est le nombre cherché
  m_nbgrps= (m_dernier-m_t1)/4+1;
  
//   m_forceCoef = 1;
//   m_forceRatio = 1;
}

Solver3D::~Solver3D ()
{
  _mm_free(m_uPrev);
  _mm_free(m_vPrev);
  _mm_free(m_wPrev);
  _mm_free(m_dens);
  _mm_free(m_densPrev);
  _mm_free(m_densSrc);
  _mm_free(m_rotx);
  _mm_free(m_roty);
  _mm_free(m_rotz);
  _mm_free(m_rot);

}

void Solver3D::set_bnd (unsigned char b, float *const x)
{
  uint i, j;

  /* Attention cela ne prend pas en compte les coins et les arêtes entre les coins */
  for (i = 1; i <= m_nbVoxelsY; i++)
    {
      for (j = 1; j <= m_nbVoxelsZ; j++)
	{
	  x[IX (0, i, j)] = 0.0f;	//x[IX(i,j,1)];
	  x[IX (m_nbVoxelsX + 1, i, j)] = 0.0f;	//x[IX(i,j,N)];
	}
    }

  for (i = 1; i <= m_nbVoxelsX; i++)
    {
      for (j = 1; j <= m_nbVoxelsZ; j++)
	{
	  x[IX (i, 0, j)] = 0.0f;	//x[IX(i, 1, j)];
	  //x[IX(i, N+1, j)] = 0;//x[IX(i,N,j)];
	  x[IX (i, m_nbVoxelsY + 1, j)] = 0.0f;//-- x[IX (i, m_nbVoxelsY, j)];
	}
    }

  for (i = 1; i <= m_nbVoxelsX; i++)
    {
      for (j = 1; j <= m_nbVoxelsY; j++)
	{
	  x[IX (i, j, 0)] = 0.0f;	//x[IX(i,j,1)];
	  x[IX (i, j, m_nbVoxelsZ + 1)] = 0.0f;	//x[IX(i,j,N)];
	}
    }
}

void Solver3D::advect (unsigned char b, float *const d, const float *const d0,
		       const float *const u, const float *const v,
		       const float *const w)
{
  uint i0, j0, k0, i1, j1, k1;
  float x, y, z, r0, s0, t0, r1, s1, t1, dt0_x, dt0_y, dt0_z;

  dt0_x = m_dt * m_nbVoxelsX;
  dt0_y = m_dt * m_nbVoxelsY;
  dt0_z = m_dt * m_nbVoxelsZ;
  
  m_t=m_t1;
  for (uint k = 1; k <= m_nbVoxelsZ; k++){
    for (uint j = 1; j <= m_nbVoxelsY; j++){
      for (uint i = 1; i <= m_nbVoxelsX; i++){
	
	x = i - dt0_x * u[m_t];
	y = j - dt0_y * v[m_t];
	z = k - dt0_z * w[m_t];
	  
	if (x < 0.5f) x = 0.5f;
	if (x > m_nbVoxelsX + 0.5f) x = m_nbVoxelsX + 0.5f;
	i0 = (uint) x; i1 = i0 + 1;
	  
	if (y < 0.5f) y = 0.5f;
	if (y > m_nbVoxelsY + 0.5f) y = m_nbVoxelsY + 0.5f;
	j0 = (uint) y; j1 = j0 + 1;
	  
	if (z < 0.5f) z = 0.5f;
	if (z > m_nbVoxelsZ + 0.5f) z = m_nbVoxelsZ + 0.5f;
	k0 = (uint) z; k1 = k0 + 1;

	r1 = x - i0;
	r0 = 1 - r1;
	s1 = y - j0;
	s0 = 1 - s1;
	t1 = z - k0;
	t0 = 1 - t1;
	  
	d[m_t] = r0 * (s0 * (t0 * d0[IX (i0, j0, k0)] + t1 * d0[IX (i0, j0, k1)]) +
		       s1 * (t0 * d0[IX (i0, j1, k0)] + t1 * d0[IX (i0, j1, k1)])
		       ) +
	  r1 * (s0 * (t0 * d0[IX (i1, j0, k0)] + t1 * d0[IX (i1, j0, k1)]) +
		s1 * (t0 * d0[IX (i1, j1, k0)] + t1 * d0[IX (i1, j1, k1)])
		);
	  
	m_t++;
      }//for i
      m_t+=2;
    }//for j
    m_t+=m_t2nx;
  }//for k
  
  //set_bnd (b, d);
}

// void Solver3D::dens_step()
// {
//   add_source ( m_dens, m_densSrc);
//   SWAP (m_densPrev, m_dens); diffuse ( 0, m_dens, m_densPrev, a_diff);
//   SWAP (m_densPrev, m_dens); advect ( 0, m_dens, m_densPrev, m_u, v, w);
// }
void Solver3D::addVorticityConfinement( float * const u, float *const  v,  float * const w)
{
  uint i,j,k;	
  float epsh =m_dt*m_forceCoef*m_vorticityConfinement;//epsilon*h
  float x,y,z;
  float Nx,Ny,Nz;
  float invNormeN;
  
  /** Calcul de m_rot la norme du rotationnel du champ de vélocité (m_u, m_v, m_w)
   */
  m_t = m_t1;
  for (k=1; k<=m_nbVoxelsZ; k++) {
    for (j=1; j<=m_nbVoxelsY; j++) {
      for (i=1; i<=m_nbVoxelsX; i++) {
	// m_rotx = dm_w/dy - dm_v/dz
	x = m_rotx[m_t] = (w[m_t+m_nx] - w[m_t-m_nx]) * m_invhy -
	  (v[m_t+m_n2] - v[m_t-m_n2]) * m_invhz;
	// m_roty = dm_u/dz - dm_w/dx
	
	y = m_roty[m_t] = (u[m_t+m_n2] - u[m_t-m_n2]) * m_invhz -
	  (w[m_t+1] - w[m_t-1]) * m_invhx;

	// m_rotz = dm_v/dx - dm_u/dy
	z = m_rotz[m_t] = (v[m_t+1] - v[m_t-1]) * m_invhx -
	  (u[m_t+m_nx] - u[m_t-m_nx]) * m_invhy;

	// m_rot = |m_rot|
	m_rot[m_t] = sqrtf(x*x+y*y+z*z);
	m_t++;
      }//for i
      m_t+=2;
    }//for j
    m_t+=m_t2nx;
  }//for k
  /* Calcul du gradient normalisé du rotationnel m_rot
   * Calcul du produit vectoriel N^m_rot
   * Le vecteur est multiplié par epsilon*h
   * et est ajouté au champ de vélocité
   */
  m_t=m_t1;
  for (k=1; k<=m_nbVoxelsZ; k++) {
    for (j=1; j<=m_nbVoxelsY; j++) {
      for (i=1; i<=m_nbVoxelsX; i++) {
			
	Nx = (m_rot[m_t+1] - m_rot[m_t-1]) * m_invhx;
	Ny = (m_rot[m_t+m_nx] - m_rot[m_t-m_nx]) * m_invhy;
	Nz = (m_rot[m_t+m_n2] - m_rot[m_t-m_n2]) * m_invhz;
			
	invNormeN = 1.0/(sqrtf(Nx*Nx+Ny*Ny+Nz*Nz)+0.000001f);
			
	Nx *= invNormeN;
	Ny *= invNormeN;
	Nz *= invNormeN;

	u[m_t] +=(Ny*m_rotz[m_t] - Nz*m_roty[m_t]) * epsh;
	v[m_t] +=(Nz*m_rotx[m_t] - Nx*m_rotz[m_t]) * epsh;
	w[m_t] +=(Nx*m_roty[m_t] - Ny*m_rotx[m_t]) * epsh;
	m_t++;
      }//for i
      m_t+=2;
    }//for j
    m_t+=m_t2nx;
  }//for k

}//AddVorticityConfinement

void Solver3D::vel_step ()
{
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
  project (m_uPrev, m_vPrev);
  SWAP (m_uPrev, m_u);
  SWAP (m_vPrev, m_v);
  SWAP (m_wPrev, m_w);
  advect (1, m_u, m_uPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (2, m_v, m_vPrev, m_uPrev, m_vPrev, m_wPrev);
  advect (3, m_w, m_wPrev, m_uPrev, m_vPrev, m_wPrev);
  project (m_uPrev, m_vPrev);

}

void Solver3D::iterate ()
{ 
  if(!m_run)
    return;
  
  /* Cellule(s) génératrice(s) */
  
  //   for (uint i = 1; i < m_nbVoxelsX + 1; i++)
  //     for (uint k = 1; k < m_nbVoxelsZ + 1; k++)
  //       m_vSrc[IX(i,1,k)] += m_buoyancy/20.0f;

  m_t=m_t1;
  for (uint k = 1; k <= m_nbVoxelsZ; k++){
    for (uint j = 1; j <= m_nbVoxelsY; j++){
      for (uint i = 1; i <= m_nbVoxelsX; i++){
	m_vSrc[m_t] += m_buoyancy / (float) (m_nbVoxelsY-j+1);
	m_t++;
      }//for i
      m_t+=2;
    }//for j
    m_t+=m_t2nx;
  }//for k
  
  if(m_permanentExternalForces.x || m_permanentExternalForces.y || m_permanentExternalForces.z)
    addExternalForces(m_permanentExternalForces,false);

  if(m_temporaryExternalForces.x || m_temporaryExternalForces.y || m_temporaryExternalForces.z)
    {
      addExternalForces(m_temporaryExternalForces,false);
      m_temporaryExternalForces.resetToNull();
    }
  
  if(m_movingForces.x || m_movingForces.y || m_movingForces.z)
    {
      addExternalForces(m_movingForces,true);
      m_movingForces.resetToNull();
    }
  
  vel_step ();

  m_nbIter++;
}

void Solver3D::addExternalForces(const Point& position, bool move)
{
  uint i,j;
  uint widthx, widthy, widthz;
  uint ceilx, ceily, ceilz;
  Point strength;
  Point force;
  //  float factor = m_dim.y/(m_nbVoxelsY - 1);
  if(move){
    force = position;
    strength.x = strength.y = strength.z = .2f;
    setPosition(m_position + position);
  }else{
    force = position;
    strength = position * .1f;
    strength.x = fabs(strength.x);
    strength.y = fabs(strength.y);
    strength.z = fabs(strength.z);  
  }
  
  findPointPosition(m_dim-Point(.5f,.5f,.5f),widthx,widthy,widthz);
  findPointPosition(Point(0.0f,0.0f,0.0f),ceilx,ceily,ceilz);
  
  /* Ajouter des forces externes */
  if(force.x)
    if( force.x > 0.0f)
      for (i = ceilz; i <= widthz; i++)
	for (j = ceily; j <= widthy; j++)
	  m_uSrc[IX(m_nbVoxelsX, j, i)] -= strength.x;//*(m_nbVoxelsY - j)*factor;
    else
      for (i = ceilz; i <= widthz; i++)
	for (j = ceily; j <= widthy; j++)
	  m_uSrc[IX(1, j, i)] += strength.x;//*(m_nbVoxelsY - j)*factor;
  if(force.y)
    if( force.y > 0.0f)
      for (i = ceilx; i <= widthx; i++)
	for (j = ceilz; j < widthz; j++)
	  m_vSrc[IX(i, m_nbVoxelsY, j)] -= strength.y/10.0f;
    else
      for (i = ceilx; i <= widthx; i++)
	for (j = ceilz; j <= widthz; j++)
	  m_vSrc[IX(i, 1, j)] += strength.y/10.0f;
  if(force.z)
    if( force.z > 0.0f)
      for (i = ceilx; i <= widthx; i++)
	for (j = ceily; j <= widthy; j++)
	  m_wSrc[IX(i, j, m_nbVoxelsZ)] -= strength.z;//*(m_nbVoxelsY - j)*factor;
    else
      for (i = ceilx; i <= widthx; i++)
	for (j = ceily; j <= widthy; j++)
	  m_wSrc[IX(i, j, 1)] += strength.z;//*(m_nbVoxelsY - j)*factor;
}

void Solver3D::addForcesOnFace(unsigned char face, const Point& BLStrength, const Point& TLStrength,
			       const Point& TRStrength, const Point& BRStrength)
{
  uint i,j;
  
  switch(face){
  case LEFT_FACE :
    for (i = 1; i <= m_nbVoxelsZ; i++)
      for (j = 1; j <= m_nbVoxelsY; j++)
	m_uSrc[IX(m_nbVoxelsX, j, i)] += (BLStrength.x+TLStrength.x+TRStrength.x+BRStrength.x)/4.0f;
    break;
  case RIGHT_FACE : 
    for (i = 1; i <= m_nbVoxelsZ; i++)
      for (j = 1; j <= m_nbVoxelsY; j++)
	m_uSrc[IX(1, j, i)] += (BLStrength.x+TLStrength.x+TRStrength.x+BRStrength.x)/4.0f;
    break;
  case BACK_FACE : 
    for (i = 1; i <= m_nbVoxelsX; i++)
      for (j = 1; j <= m_nbVoxelsY; j++)
	m_wSrc[IX(i, j, 1)] += (BLStrength.z+TLStrength.z+TRStrength.z+BRStrength.z)/4.0f;
    break;
  case FRONT_FACE : 
    for (i = 1; i <= m_nbVoxelsX; i++)
      for (j = 1; j <= m_nbVoxelsY; j++)
	m_wSrc[IX(i, j, m_nbVoxelsZ)] += (BLStrength.z+TLStrength.z+TRStrength.z+BRStrength.z)/4.0f;
    break;
  }
}

void Solver3D::prolonger(float  *const v2h, float *const vh)
{
  // on utilise une interpolation bilinéaire
  // récupérer la taille du solveur 

  int nx=getXRes();
  int ny=getYRes();
  int nz=getZRes();

  int I,ir,J,jr,K,kr,code;
  K=0;
  kr=1;
  for(int k = 1 ; k <= nz ; k++) {
    //déterminer l'indice K du gros voxel
    // k = 2*K + kr
    //	K=k/2;
    //	kr=k%2;
    J=0;
    jr=1;
    for(int j = 1 ; j <= ny ; j++){
      I=0;
      ir=1;
      //déterminer l'indice J du gros voxel
      // j = 2*J + jr
      //J=j/2;
      //jr=j%2;
      for(int i = 1 ; i <= nx ; i++){
	//déterminer l'indice I du gros voxel
	// i = 2*I + ir
	//	I=i/2;
	//  ir=i%2;
			
			
	// il y a 8 possibilités
	// i = 2*I ou i = 2*I +1
	// j = 2*J ou j = 2*J +1
	// k = 2*K ou k = 2*K +1
	// Chacune des possibilités est codée en positionnant les 3 bits de 
	// poids faible de l'entier code : bit2 = ir, bit1 = jr, bit0 = kr
	// Le point (i,j,k) de la grille fine est l'un des sommets du "petit"
	// voxel ayant en commun le point (2I,2J,2K) avec le "gros" voxel 
	// Le point (i,j,k) est soit le milieu d'une arête du "gros" voxel,
	// soit le centre de l'une de ses faces, soit son centre,  
	// soit le point commun des deux voxels
				 

	code=4*ir+2*jr+kr;
	switch(code){
	case 0: // point commun
	  vh[IX(i,j,k)] = v2h[IX2h(I,J,K)];
	  break;
	case 1: // milieu d'une arête
	  vh[IX(i,j,k)] = 0.5f*
	    (v2h[IX2h(I,J,K)] +
	     v2h[IX2h(I,J,K+1)]);
	  break;
	case 2: // milieu d'une arête
	  vh[IX(i,j,k)] =  0.5f*
	    (v2h[IX2h(I,J,K)] +
	     v2h[IX2h(I,J+1,K)]);
	  break;
	case 3: // centre d'une face
	  vh[IX(i,j,k)] = 0.25f*
	    (v2h[IX2h(I,J,K)]+
	     v2h[IX2h(I,J+1,K)]+
	     v2h[IX2h(I,J+1,K+1)]+
	     v2h[IX2h(I,J,K+1)]);
	  break;
	case 4: // milieu d'une arête
	  vh[IX(i,j,k)] = 0.5f*
	    (v2h[IX2h(I,J,K)] + 
	     v2h[IX2h(I+1,J,K)]);
	  break;

	case 5: // centre d'une face
	  vh[IX(i,j,k)] = 0.25f*
	    (v2h[IX2h(I,J,K)]+ 
	     v2h[IX2h(I+1,J,K)]+
	     v2h[IX2h(I+1,J,K+1)]+
	     v2h[IX2h(I,J,K+1)]);
	  break;
	case 6: // centre d'une face
	  vh[IX(i,j,k)] = 0.25f*
	    (v2h[IX2h(I,J,K)]+
	     v2h[IX2h(I+1,J,K)]+
	     v2h[IX2h(I+1,J+1,K)]+
	     v2h[IX2h(I,J+1,K)]);
	  break;
	case 7: // centre du cube
	  vh[IX(i,j,k)] = 0.125f*
	    (v2h[IX2h(I,J,K)]+ 
	     v2h[IX2h(I+1,J,K)]+
	     v2h[IX2h(I+1,J,K+1)]+
	     v2h[IX2h(I,J,K+1)] +
	     v2h[IX2h(I,J+1,K)]+ 
	     v2h[IX2h(I+1,J+1,K)]+
	     v2h[IX2h(I+1,J+1,K+1)]+
	     v2h[IX2h(I,J+1,K+1)]);
	}//switch
	// préparer l'itération suivante
	// i = 2*I + ir => i+1 = 2*(I+ir) + (1-ir)
	I=I+ir;
	ir=1-ir;
      }//for i
      J=J+jr;
      jr=1-jr;
    }// for j
    K=K+kr;
    kr=1-kr;
  }//for k
}//prolonger

void  Solver3D::restreindre(float *const vh, float *const v2h){

  // schéma en 27 points
  int nx=getXRes()/2;
  int ny=getYRes()/2;
  int nz=getZRes()/2;
  int ii,jj,kk,iiO,iiE,jjN,jjS,kkU,kkD;
  for(int k = 1 ; k <= nz ; k++) {
    for(int j = 1 ; j <= ny ; j++){
      for(int i = 1 ; i <= nx ; i++){
	//				v2h[IX2h(i,j,k)] = vh[IX(2*i,2*j,2*k)];

	// Un point de la grille grossière reçoit la somme pondérée des
	// valeurs des points voisins de la grille grossière.
	// Un point de la grille grossière a 27 voisins.
	// Les points de la grille fine situés dans le plan du point courant
	// de la grille grossière ont respectivement pour poids 1/8, 1/16, 1/32
	// en fonction de leur distance à ce point.
	// Les points de la grille fine situés dans les plans immédiatement 
	// parallèles au plan contenant le point courant de la grille grossière
	// ont respectivement pour poids 1/16, 1/32, 1/64
	// en fonction de leur distance à ce point.
	// La somme des poids est égale à 1 :
	// (1/8 + 1/16*4 + 1/32*4) *(1 + 2* 1/2) = 1
	ii=2*i;
	jj=2*j;
	kk=2*k;
	iiE = ii+1;
	iiO = ii-1;
	jjN = jj+1;
	jjS = jj-1;
	kkU = kk+1;
	kkD = kk-1;
	// multiplication par 4 car le problème n'est pas un problème de Poisson              
	//                                 |
	//                                 v

	v2h[IX2h(i,j,k)] = (
			    0.125f*(
				    vh[IX( ii ,jj , kk)]+// coïncident
				    (vh[IX(iiE,jjS,kk)]+// point  SE
				     vh[IX(iiE,jjN,kk)]+// point  NE
				     vh[IX(iiO,jjS,kk)]+// point  SO
				     vh[IX(iiO,jjN,kk)])*0.25f+// point NO
				    (vh[IX(iiE,jj ,kk)]+ // point E
				     vh[IX(ii ,jjS,kk)]+ // point S
				     vh[IX(ii ,jjN,kk)]+ // point N
				     vh[IX(iiO,jj ,kk)])*0.5f // point O
				    )+
			    0.0625f*(// idem mais pour le plan au-dessus
				     vh[IX( ii ,jj ,kkU)]+
				     (vh[IX(iiE,jjS,kkU)]+
				      vh[IX(iiE,jjN,kkU)]+
				      vh[IX(iiO,jjS,kkU)]+
				      vh[IX(iiO,jjN,kkU)])*0.25f+
				     (vh[IX(iiE,jj ,kkU)]+
				      vh[IX(ii ,jjS,kkU)]+
				      vh[IX(ii ,jjN,kkU)]+
				      vh[IX(iiO,jj ,kkU)])*0.5f
				     )+
			    0.0625f*(// idem mais pour le plan au-dessous
				     vh[IX( ii ,jj ,kkD)]+
				     (vh[IX(iiE,jjS,kkD)]+
				      vh[IX(iiE,jjN,kkD)]+
				      vh[IX(iiO,jjS,kkD)]+
				      vh[IX(iiO,jjN,kkD)])*0.25f+
				     (vh[IX(iiE,jj ,kkD)]+
				      vh[IX(ii ,jjS,kkD)]+
				      vh[IX(ii ,jjN,kkD)]+
				      vh[IX(iiO,jj ,kkD)])*0.5f
				     )
			    );

      }//for i
    }// for j
  }//for k
}//restreindre


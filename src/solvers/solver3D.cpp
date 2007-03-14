#include "solver3D.hpp"

#include <math.h>
#include "../scene/graphicsFn.hpp"

Solver3D::Solver3D ()
{
}

Solver3D::Solver3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale,
		    double timeStep, double buoyancy) : 
  RealField3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy)
{
  m_uPrev = new double[m_nbVoxels];
  m_vPrev = new double[m_nbVoxels];
  m_wPrev = new double[m_nbVoxels];
  m_dens = new double[m_nbVoxels];
  m_densPrev = new double[m_nbVoxels];
  m_densSrc = new double[m_nbVoxels];
  
  memset (m_uPrev, 0, m_nbVoxels * sizeof (double));
  memset (m_vPrev, 0, m_nbVoxels * sizeof (double));
  memset (m_wPrev, 0, m_nbVoxels * sizeof (double));
  memset (m_dens, 0, m_nbVoxels * sizeof (double));
  memset (m_densPrev, 0, m_nbVoxels * sizeof (double));
  memset (m_densSrc, 0, m_nbVoxels * sizeof (double));
  
  m_visc = 0.00000015;
  m_diff = 0.001;
  
  m_n2 = (m_nbVoxelsX+2) * (m_nbVoxelsY+2);
  m_nx = m_nbVoxelsX+2;
  m_t1 = m_n2 + m_nx +1;
  m_t2nx = 2*m_nx;

//   m_forceCoef = 1;
//   m_forceRatio = 1;
}

Solver3D::~Solver3D ()
{
  delete[]m_uPrev;
  delete[]m_vPrev;
  delete[]m_wPrev;
  
  delete[]m_dens;
  delete[]m_densPrev;
  delete[]m_densSrc;
}

void Solver3D::set_bnd (unsigned char b, double *const x)
{
  uint i, j;

  /* Attention cela ne prend pas en compte les coins et les arêtes entre les coins */
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

void Solver3D::advect (unsigned char b, double *const d, const double *const d0,
		     const double *const u, const double *const v,
		     const double *const w)
{
  uint i0, j0, k0, i1, j1, k1;
  double x, y, z, r0, s0, t0, r1, s1, t1, dt0_x, dt0_y, dt0_z;

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
	  
	  if (x < 0.5) x = 0.5;
	  if (x > m_nbVoxelsX + 0.5) x = m_nbVoxelsX + 0.5;
	  i0 = (uint) x; i1 = i0 + 1;
	  
	  if (y < 0.5) y = 0.5;
	  if (y > m_nbVoxelsY + 0.5) y = m_nbVoxelsY + 0.5;
	  j0 = (uint) y; j1 = j0 + 1;
	  
	  if (z < 0.5) z = 0.5;
	  if (z > m_nbVoxelsZ + 0.5) z = m_nbVoxelsZ + 0.5;
	  k0 = (uint) z; k1 = k0 + 1;

	  r1 = x - i0;
	  r0 = 1 - r1;
	  s1 = y - j0;
	  s0 = 1 - s1;
	  t1 = z - k0;
	  t0 = 1 - t1;
	  
	  d[m_t] = r0 * (s0 * (t0 * d0[IX (i0, j0, k0)] + t1 * d0[IX (i0, j0, k1)]) +
			 s1 * (t0 * d0[IX (i0, j1, k0)] + t1 * d0[IX (i0, j1, k1)])) +
	    r1 * (s0 * (t0 * d0[IX (i1, j0, k0)] + t1 * d0[IX (i1, j0, k1)]) +
		  s1 * (t0 * d0[IX (i1, j1, k0)] + t1 * d0[IX (i1, j1, k1)]));
	  
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
//   SWAP (m_densPrev, m_dens); diffuse ( 0, m_dens, m_densPrev, a_diff, diff);
//   SWAP (m_densPrev, m_dens); advect ( 0, m_dens, m_densPrev, m_u, v, w);
// }

void Solver3D::vel_step ()
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

void Solver3D::iterate ()
{ 
  if(!m_run)
    return;
  /* Cellule(s) génératrice(s) */

  for (uint i = 1; i < m_nbVoxelsX + 1; i++)
    for (uint k = 1; k < m_nbVoxelsZ + 1; k++)
      m_vSrc[IX(i,1,k)] += m_buoyancy/20.0;

//   m_t=m_t1;
//   for (uint k = 1; k <= m_nbVoxelsZ; k++){
//     for (uint j = 1; j <= m_nbVoxelsY; j++){
//       for (uint i = 1; i <= m_nbVoxelsX; i++){
// 	m_vSrc[m_t] += m_buoyancy/10.0; // / (double) (m_nbVoxelsY-j+1);
// 	m_t++;
//       }//for i
//       m_t+=2;
//     }//for j
//     m_t+=m_t2nx;
//   }//for k
    
  if(arePermanentExternalForces)
    addExternalForces(permanentExternalForces,false);
  
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
  
  if(move){
    force = position - m_position;
    strength.x = strength.y = strength.z = .001;  
    m_position=position;
  }else{
    force = position;
    strength = position * .1;
    strength.x = fabs(strength.x);
    strength.y = fabs(strength.y);
    strength.z = fabs(strength.z);  
  }
  
  findPointPosition(m_dim-Point(.1,.3,.1),widthx,widthy,widthz);
  findPointPosition(Point(0,0,0),ceilx,ceily,ceilz);
  
  /* Ajouter des forces externes */
  if(force.x)
    if( force.x > 0)
      for (i = ceilz; i <= widthz; i++)
	for (j = ceily; j <= widthy; j++)
	  m_uSrc[IX(m_nbVoxelsX, j, i)] -= strength.x;
    else
      for (i = ceilz; i <= widthz; i++)
	for (j = ceily; j <= widthy; j++)
	  m_uSrc[IX(1, j, i)] += strength.x;
  if(force.y)
    if( force.y > 0)
      for (i = ceilx; i <= widthx; i++)
	for (j = ceilz; j < widthz; j++)
	  m_vSrc[IX(i, m_nbVoxelsY, j)] -= strength.y/10.0;
    else
      for (i = ceilx; i <= widthx; i++)
	for (j = ceilz; j <= widthz; j++)
	  m_vSrc[IX(i, 1, j)] += strength.y/10.0;
  if(force.z)
    if( force.z > 0)
      for (i = ceilx; i <= widthx; i++)
	for (j = ceily; j <= widthy; j++)
	  m_wSrc[IX(i, j, m_nbVoxelsZ)] -= strength.z;
    else
      for (i = ceilx; i <= widthx; i++)
	for (j = ceily; j <= widthy; j++)
	  m_wSrc[IX(i, j, 1)] += strength.z;
}

void Solver3D::prolonger(double  *const v2h, double *const vh)
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
	  vh[IX(i,j,k)] = 0.5*
	    (v2h[IX2h(I,J,K)] +
	     v2h[IX2h(I,J,K+1)]);
	  break;
	case 2: // milieu d'une arête
	  vh[IX(i,j,k)] =  0.5*
	    (v2h[IX2h(I,J,K)] +
	     v2h[IX2h(I,J+1,K)]);
	  break;
	case 3: // centre d'une face
	  vh[IX(i,j,k)] = 0.25*
	    (v2h[IX2h(I,J,K)]+
	     v2h[IX2h(I,J+1,K)]+
	     v2h[IX2h(I,J+1,K+1)]+
	     v2h[IX2h(I,J,K+1)]);
	  break;
	case 4: // milieu d'une arête
	  vh[IX(i,j,k)] = 0.5*
	    (v2h[IX2h(I,J,K)] + 
	     v2h[IX2h(I+1,J,K)]);
	  break;

	case 5: // centre d'une face
	  vh[IX(i,j,k)] = 0.25*
	    (v2h[IX2h(I,J,K)]+ 
	     v2h[IX2h(I+1,J,K)]+
	     v2h[IX2h(I+1,J,K+1)]+
	     v2h[IX2h(I,J,K+1)]);
	  break;
	case 6: // centre d'une face
	  vh[IX(i,j,k)] = 0.25*
	    (v2h[IX2h(I,J,K)]+
	     v2h[IX2h(I+1,J,K)]+
	     v2h[IX2h(I+1,J+1,K)]+
	     v2h[IX2h(I,J+1,K)]);
	  break;
	case 7: // centre du cube
	  vh[IX(i,j,k)] = 0.125*
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

void  Solver3D::restreindre(double *const vh, double *const v2h){

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
			    0.125*(
				   vh[IX( ii ,jj , kk)]+// coïncident
				   (vh[IX(iiE,jjS,kk)]+// point  SE
				    vh[IX(iiE,jjN,kk)]+// point  NE
				    vh[IX(iiO,jjS,kk)]+// point  SO
				    vh[IX(iiO,jjN,kk)])*0.25+// point NO
				   (vh[IX(iiE,jj ,kk)]+ // point E
				    vh[IX(ii ,jjS,kk)]+ // point S
				    vh[IX(ii ,jjN,kk)]+ // point N
				    vh[IX(iiO,jj ,kk)])*0.5 // point O
				   )+
			    0.0625*(// idem mais pour le plan au-dessus
				    vh[IX( ii ,jj ,kkU)]+
				    (vh[IX(iiE,jjS,kkU)]+
				     vh[IX(iiE,jjN,kkU)]+
				     vh[IX(iiO,jjS,kkU)]+
				     vh[IX(iiO,jjN,kkU)])*0.25+
				    (vh[IX(iiE,jj ,kkU)]+
				     vh[IX(ii ,jjS,kkU)]+
				     vh[IX(ii ,jjN,kkU)]+
				     vh[IX(iiO,jj ,kkU)])*0.5
				    )+
			    0.0625*(// idem mais pour le plan au-dessous
				    vh[IX( ii ,jj ,kkD)]+
				    (vh[IX(iiE,jjS,kkD)]+
				     vh[IX(iiE,jjN,kkD)]+
				     vh[IX(iiO,jjS,kkD)]+
				     vh[IX(iiO,jjN,kkD)])*0.25+
				    (vh[IX(iiE,jj ,kkD)]+
				     vh[IX(ii ,jjS,kkD)]+
				     vh[IX(ii ,jjN,kkD)]+
				     vh[IX(iiO,jj ,kkD)])*0.5
				    )
			    );

      }//for i
    }// for j
  }//for k
}//restreindre


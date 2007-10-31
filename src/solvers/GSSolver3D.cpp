#include "GSSolver3D.hpp"

GSSolver3D::GSSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale,
			float timeStep, float buoyancy, float vorticityConfinement) : 
  Solver3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy, vorticityConfinement)
{
  m_res=new float[m_nbVoxels];
  fill_n(m_res, m_nbVoxels, 0.0f);
}

GSSolver3D::GSSolver3D ()
{
  m_res=new float[m_nbVoxels];
  fill_n(m_res, m_nbVoxels, 0.0f);
}

GSSolver3D::~GSSolver3D ()
{
  delete[]m_res;
}

void GSSolver3D::GS_solve(unsigned char b, float *const x, const float *const x0, float a, float div, uint nb_steps)
{  
  uint i, j, k, l;
  float diagonal = 1/div;
  float *mx,*mxmnx,* mxpnx,* mxm1,* mxp1,* mxmn2,* mxpn2,*mr;
  const float *mx0;
  float norm2, eb2;
  
  // calcul du carré de la norme du résidu    
  norm2=0.0f; mx0=&x0[m_t1];    
  for ( k = 1; k <= m_nbVoxelsZ; k++){
    for ( j = 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	norm2+=(*mx0)*(*mx0);
	mr++;
      }//for i
      mr+=2;
    }//for j
    mr+=m_t2nx;
  }//for k
  
  eb2 = 0.0001f * norm2;

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
    
    mx0=&x0[m_t1]; mx=&x[m_t1];  mr=&m_res[m_t1];
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
    norm2=0.0f; mr=&m_res[m_t1];    
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
    
    if(norm2 < eb2){
//       cerr << ((a == 1) ? "proj : " : "diff : ") << l << endl;
      return;
    }
  }//for l
  //set_bnd (b, x);
}

// void GSSolver3D::GS_solve(unsigned char b, float *const x, const float *const x0, float a, float div, uint nb_steps)
// {
//   uint i, j, k, l;
//   float diagonal = 1.0f/div;
//   float norm2, eb2;
  
//   // calcul du carré de la norme de b
//   norm2=0.0f;
//   for ( k = 1; k <= m_nbVoxelsZ; k++)
//     for ( j = 1; j <= m_nbVoxelsY; j++)
//       for ( i = 1; i <= m_nbVoxelsX; i++)
// 	norm2+=x0[IX(i,j,k)]*x0[IX(i,j,k)];
  
//   // calcul de eb2 le second membre du test d'arrêt
//   eb2=.0001f*norm2;
  
//   for (l = 0; l < nb_steps; l++){
//     for (k = 1; k <= m_nbVoxelsZ; k++)
//       for (j = 1; j <= m_nbVoxelsY; j++)
// 	for (i = 1; i <= m_nbVoxelsX; i++)
// 	  x[IX (i, j, k)] = (x0[IX (i, j, k)] +
// 			     a * (x[IX (i - 1, j, k)] + x[IX (i + 1, j, k)] +
// 				  x[IX (i, j - 1, k)] +	x[IX (i, j + 1, k)] +
// 				  x[IX (i, j, k - 1)] +	x[IX (i, j, k + 1)])) * div;
//     // calcul du résidu
//     for ( k = 1; k <= m_nbVoxelsZ; k++)
//       for ( j = 1; j <= m_nbVoxelsY; j++)
// 	for ( i = 1; i <= m_nbVoxelsX; i++)
// 	  m_res[IX (i, j, k)] = x0[IX (i, j, k)] - diagonal * x[IX (i, j, k)] + 
// 	    a * (x[IX (i - 1, j, k)] + x[IX (i + 1, j, k)] +
// 		 x[IX (i, j - 1, k)] + x[IX (i, j + 1, k)] +
// 		 x[IX (i, j, k - 1)] + x[IX (i, j, k + 1)]);
//     // calcul du carré de la norme du résidu
//     norm2=0.0f;
//     for ( k = 1; k <= m_nbVoxelsZ; k++)
//       for ( j = 1; j <= m_nbVoxelsY; j++)
// 	for ( i = 1; i <= m_nbVoxelsX; i++)
// 	  norm2+=m_res[IX(i,j,k)]*m_res[IX(i,j,k)];
   
//     if(norm2 < eb2){
//       //cerr<<"précision atteinte : nbiter = "<<l<<endl;
//       break;
//     }
//   }
//   if(norm2 > eb2){
//     //cerr<<"précision non atteinte : nbiter = "<<l<<endl;
//   }
//   //set_bnd (b, x);
// }

/* Pas de diffusion */
void GSSolver3D::diffuse (unsigned char b, float *const x, float *const x0, float a)
{
  GS_solve(b,x,x0,a, 1.0f/(1.0f + 6.0f * a), 20);
}

void GSSolver3D::project (float *const p, float *const div)
{
  float h_x = 1.0f / m_nbVoxelsX, h_y = 1.0f / m_nbVoxelsY, h_z = 1.0f / m_nbVoxelsZ;
  uint i, j, k;
  
  m_t=m_t1;
  for (k = 1; k <= m_nbVoxelsZ; k++){
    for (j = 1; j <= m_nbVoxelsY; j++){
      for (i = 1; i <= m_nbVoxelsX; i++){
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
  
  //  set_bnd (0, div);
  fill_n(p, m_nbVoxels, 0.0f);
  //  set_bnd (0, p);
  
  GS_solve(0,p,div,1, 1.0f/6.0f, 20); 
  
  m_t=m_t1;
  for (k = 1; k <= m_nbVoxelsZ; k++){
    for (j = 1; j <= m_nbVoxelsY; j++){
      for (i = 1; i <= m_nbVoxelsX; i++){
	m_u[m_t] -= 0.5f * (p[m_t+1] - p[m_t-1]) / h_x;
	m_v[m_t] -= 0.5f * (p[m_t+m_nx] - p[m_t-m_nx]) / h_y;
	m_w[m_t] -= 0.5f * (p[m_t+m_n2] - p[m_t-m_n2]) / h_z;
	m_t++;
      }//for i
      m_t+=2;
    }//for j
    m_t+=m_t2nx;
  }//for k
  //set_bnd (1, u);
  //set_bnd (2, v);
  //set_bnd (3, w);
}//project

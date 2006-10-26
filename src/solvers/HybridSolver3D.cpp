#include "HybridSolver3D.hpp"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

/* Le constructeur de GSSolver3D n'a pas de paramètre, il n'est donc pas appelé explicitement */
HybridSolver3D::HybridSolver3D (Point& position, uint n_x, uint n_y, uint n_z, double dim, double timeStep,
			    double buoyancy, double omegaDiff, double omegaProj, double epsilon) : 
  Solver3D (position, n_x, n_y, n_z, dim, timeStep, buoyancy), GCSSORSolver3D(omegaDiff, omegaProj, epsilon)
{
  m_time = 0.0;
}
/* Le constructeur de GSSolver3D n'a pas de paramètre, il n'est donc pas appelé explicitement */
HybridSolver3D::HybridSolver3D (double omegaDiff, double omegaProj, double epsilon) : 
  GCSSORSolver3D(omegaDiff, omegaProj, epsilon)
{
  m_time = 0.0;
}

HybridSolver3D::~HybridSolver3D ()
{  
}

/* Pas de diffusion */
void HybridSolver3D::diffuse (unsigned char b, double *const x, double *const x0, double a, double diff_visc)
{
  GS_solve(b,x,x0,a, 1/(1.0 + 6.0 * a), 2);
}

void HybridSolver3D::project (double *const p, double *const div)
{
  double h_x = 1.0 / m_nbVoxelsX, 
    h_y = 1.0 / m_nbVoxelsY,
    h_z = 1.0 / m_nbVoxelsZ;
  uint i, j, k;
  
  t = t1;
  for ( k = 1; k <= m_nbVoxelsZ; k++){
    for ( j = 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	div[t] = -0.5 * (h_x * (m_u[t+1] - m_u[t-1]) + h_y * (m_v[t+nx] - m_v[t-nx]) + h_z * (m_w[t+n2] - m_w[t-n2]));
	t++;
      }//for i
      t+=2;
    }//for j
    t+=t2nx;
    //p[IX (i, j, k)] = 0;
  }// for k
  
  //set_bnd (0, div);
  memset (p, 0, m_nbVoxels * sizeof (double));
  //set_bnd (0, p);
    
  //GS_solve(0,p,div,1, 1/6.0, 15); 
  GCSSOR(p,div,1, 6.0, m_omegaProj,4);
  
  t = t1;
  for (k = 1; k <= m_nbVoxelsZ; k++){
    for (j = 1; j <= m_nbVoxelsY; j++){
      for (i = 1; i <= m_nbVoxelsX; i++){
	m_u[t] -= 0.5 * (p[t+1] - p[t-1]) / h_x;
	m_v[t] -= 0.5 * (p[t+nx] - p[t-nx]) / h_y;
	m_w[t] -= 0.5 * (p[t+n2] - p[t-n2]) / h_z;
	t++;
      }// for i
      t+=2;
    }// for j
    t+=t2nx;
  }//for k
  
  //set_bnd (1, u);
  //set_bnd (2, v);
  //set_bnd (3, w);
}

// void HybridSolver3D::iterate ()
// { 
//   ::wxStartTimer();
//   vel_step ();
//   //  dens_step();

//   cleanSources ();
// //   set_bnd (0, m_u);
// //   set_bnd (0, m_v);
// //   set_bnd (0, m_w);
//   m_time = (m_time*m_nbIter + ::wxGetElapsedTime (false))/(double) (++m_nbIter);
  
//   cout << m_time << "      \r"; cout.flush();
// }

LODHybridSolver3D::LODHybridSolver3D (Point& position, uint n_x, uint n_y, uint n_z, double dim, double timeStep,
			    double buoyancy, double omegaDiff, double omegaProj, double epsilon) : 
  Solver3D (position, n_x, n_y, n_z, dim, timeStep, buoyancy), 
  HybridSolver3D (omegaDiff, omegaProj, epsilon)
{
  /* Attention n_x, n_y et n_z doivent être impairs */
  initialNbVoxelsX = n_x;
  initialNbVoxelsY = n_y;
  initialNbVoxelsZ = n_z;
  
  m_uTmp = new double[m_nbVoxels];
  m_vTmp = new double[m_nbVoxels];
  m_wTmp = new double[m_nbVoxels];
  
  memset (m_uTmp, 0, m_nbVoxels * sizeof (double));
  memset (m_vTmp, 0, m_nbVoxels * sizeof (double));
  memset (m_wTmp, 0, m_nbVoxels * sizeof (double));
}

LODHybridSolver3D::~LODHybridSolver3D ()
{
  delete[]m_uTmp;
  delete[]m_vTmp;
  delete[]m_wTmp;
}

void LODHybridSolver3D::divideRes ()
{
  if(m_nbVoxelsX < 4 || m_nbVoxelsY < 4 || m_nbVoxelsZ < 4 ){
    cerr << "Minimum grid resolution already reached !" << endl;
    return;
  }
  
  restreindre(m_u,m_uTmp);
  restreindre(m_v,m_vTmp);
  restreindre(m_w,m_wTmp);
  
  SWAP(m_u,m_uTmp);
  SWAP(m_v,m_vTmp);
  SWAP(m_w,m_wTmp);
  
  m_nbVoxelsX = m_nbVoxelsX/2;
  m_nbVoxelsY = m_nbVoxelsY/2;
  m_nbVoxelsZ = m_nbVoxelsZ/2;
  
  m_nbVoxels = (m_nbVoxelsX + 2) * (m_nbVoxelsY + 2) * (m_nbVoxelsZ + 2);
  
  m_aDiff = m_dt * m_diff * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
  m_aVisc = m_dt * m_visc * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
    
  /* Reconstruction des display lists */
  glDeleteLists(m_baseDisplayList,1);
  glDeleteLists(m_gridDisplayList,1);
  buildDLBase ();
  buildDLGrid ();
  
  m_dimXTimesNbVoxelsX = m_dim.x * m_nbVoxelsX;
  m_dimYTimesNbVoxelsY = m_dim.y * m_nbVoxelsY;
  m_dimZTimesNbVoxelsZ = m_dim.z * m_nbVoxelsZ;

  m_halfNbVoxelsX = m_nbVoxelsX/2;
  m_halfNbVoxelsZ = m_nbVoxelsZ/2;
  
  n2= (m_nbVoxelsX+2) * (m_nbVoxelsY+2);
  nx = m_nbVoxelsX+2;
  t1=n2 + nx +1;
  t2nx=2*nx;
}

void LODHybridSolver3D::decreaseRes ()
{
  if(m_nbVoxelsX < 4 || m_nbVoxelsY < 4 || m_nbVoxelsZ < 4 ){
    cerr << "Minimum grid resolution already reached !" << endl;
    return;
  }
  
  m_nbVoxelsX-=2;
  m_nbVoxelsY-=2;
  m_nbVoxelsZ-=2;
  
  m_nbVoxels = (m_nbVoxelsX + 2) * (m_nbVoxelsY + 2) * (m_nbVoxelsZ + 2);
  
  m_aDiff = m_dt * m_diff * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
  m_aVisc = m_dt * m_visc * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
    
  /* Reconstruction des display lists */
  glDeleteLists(m_baseDisplayList,1);
  glDeleteLists(m_gridDisplayList,1);
  buildDLBase ();
  buildDLGrid ();
  
  m_dimXTimesNbVoxelsX = m_dim.x * m_nbVoxelsX;
  m_dimYTimesNbVoxelsY = m_dim.y * m_nbVoxelsY;
  m_dimZTimesNbVoxelsZ = m_dim.z * m_nbVoxelsZ;

  m_halfNbVoxelsX = m_nbVoxelsX/2;
  m_halfNbVoxelsZ = m_nbVoxelsZ/2;
  
  n2= (m_nbVoxelsX+2) * (m_nbVoxelsY+2);
  nx = m_nbVoxelsX+2;
  t1=n2 + nx +1;
  t2nx=2*nx;
}

void LODHybridSolver3D::multiplyRes ()
{  
  if(m_nbVoxelsX == initialNbVoxelsX || m_nbVoxelsY == initialNbVoxelsY || m_nbVoxelsZ > initialNbVoxelsZ ){
    cerr << "Maximum grid resolution already reached !" << endl;
    return;
  }
  
  prolonger(m_u,m_uTmp);
  prolonger(m_v,m_vTmp);
  prolonger(m_w,m_wTmp);
  
  SWAP(m_u,m_uTmp);
  SWAP(m_v,m_vTmp);
  SWAP(m_w,m_wTmp);

  m_nbVoxelsX = m_nbVoxelsX*2+1;
  m_nbVoxelsY = m_nbVoxelsY*2+1;
  m_nbVoxelsZ = m_nbVoxelsZ*2+1;
  
  m_nbVoxels = (m_nbVoxelsX + 2) * (m_nbVoxelsY + 2) * (m_nbVoxelsZ + 2);
  
  m_aDiff = m_dt * m_diff * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
  m_aVisc = m_dt * m_visc * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
    
  /* Reconstruction des display lists */
  glDeleteLists(m_baseDisplayList,1);
  glDeleteLists(m_gridDisplayList,1);
  buildDLBase ();
  buildDLGrid ();
  
  m_dimXTimesNbVoxelsX = m_dim.x * m_nbVoxelsX;
  m_dimYTimesNbVoxelsY = m_dim.y * m_nbVoxelsY;
  m_dimZTimesNbVoxelsZ = m_dim.z * m_nbVoxelsZ;

  m_halfNbVoxelsX = m_nbVoxelsX/2;
  m_halfNbVoxelsZ = m_nbVoxelsZ/2;
  
  n2= (m_nbVoxelsX+2) * (m_nbVoxelsY+2);
  nx = m_nbVoxelsX+2;
  t1=n2 + nx +1;
  t2nx=2*nx;
}

void LODHybridSolver3D::increaseRes ()
{  
  if(m_nbVoxelsX == initialNbVoxelsX || m_nbVoxelsY == initialNbVoxelsY || m_nbVoxelsZ > initialNbVoxelsZ ){
    cerr << "Maximum grid resolution already reached !" << endl;
    return;
  }
  
  m_nbVoxelsX+=2;
  m_nbVoxelsY+=2;
  m_nbVoxelsZ+=2;
  
  m_nbVoxels = (m_nbVoxelsX + 2) * (m_nbVoxelsY + 2) * (m_nbVoxelsZ + 2);
  
  m_aDiff = m_dt * m_diff * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
  m_aVisc = m_dt * m_visc * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
    
  /* Reconstruction des display lists */
  glDeleteLists(m_baseDisplayList,1);
  glDeleteLists(m_gridDisplayList,1);
  buildDLBase ();
  buildDLGrid ();
  
  m_dimXTimesNbVoxelsX = m_dim.x * m_nbVoxelsX;
  m_dimYTimesNbVoxelsY = m_dim.y * m_nbVoxelsY;
  m_dimZTimesNbVoxelsZ = m_dim.z * m_nbVoxelsZ;

  m_halfNbVoxelsX = m_nbVoxelsX/2;
  m_halfNbVoxelsZ = m_nbVoxelsZ/2;
  
  n2= (m_nbVoxelsX+2) * (m_nbVoxelsY+2);
  nx = m_nbVoxelsX+2;
  t1=n2 + nx +1;
  t2nx=2*nx;
}

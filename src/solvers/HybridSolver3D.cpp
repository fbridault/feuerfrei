#include "HybridSolver3D.hpp"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

/* Le constructeur de GSSolver3D n'a pas de paramètre, il n'est donc pas appelé explicitement */
HybridSolver3D::HybridSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale, 
				double timeStep, double buoyancy, double omegaDiff, double omegaProj, double epsilon) : 
  Solver3D (position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy), GCSSORSolver3D(omegaDiff, omegaProj, epsilon)
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
  
  m_t = m_t1;
  for ( k = 1; k <= m_nbVoxelsZ; k++){
    for ( j = 1; j <= m_nbVoxelsY; j++){
      for ( i = 1; i <= m_nbVoxelsX; i++){
	div[m_t] = -0.5 * (h_x * (m_u[m_t+1] - m_u[m_t-1]) + h_y * (m_v[m_t+m_nx] - m_v[m_t-m_nx]) + h_z * (m_w[m_t+m_n2] - m_w[m_t-m_n2]));
	m_t++;
      }//for i
      m_t+=2;
    }//for j
    m_t+=m_t2nx;
    //p[IX (i, j, k)] = 0;
  }// for k
  
  //set_bnd (0, div);
  memset (p, 0, m_nbVoxels * sizeof (double));
  //set_bnd (0, p);
    
  //GS_solve(0,p,div,1, 1/6.0, 15); 
  GCSSOR(p,div,1, 6.0, m_omegaProj,4);
  
  m_t = m_t1;
  for (k = 1; k <= m_nbVoxelsZ; k++){
    for (j = 1; j <= m_nbVoxelsY; j++){
      for (i = 1; i <= m_nbVoxelsX; i++){
	m_u[m_t] -= 0.5 * (p[m_t+1] - p[m_t-1]) / h_x;
	m_v[m_t] -= 0.5 * (p[m_t+m_nx] - p[m_t-m_nx]) / h_y;
	m_w[m_t] -= 0.5 * (p[m_t+m_n2] - p[m_t-m_n2]) / h_z;
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

LODHybridSolver3D::LODHybridSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale, 
				      double timeStep, double buoyancy, double omegaDiff, double omegaProj, double epsilon) : 
  Solver3D (position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy), 
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

  /* Détermination de la taille du solveur de manière à ce que le plus grand côté soit de dimension dim */
  if (m_nbVoxelsX < m_nbVoxelsY){
    if (m_nbVoxelsX < m_nbVoxelsZ){
      m_nbMaxDiv = (m_nbVoxelsX-RESOLUTION_MIN)/2;
    }else{
      m_nbMaxDiv = (m_nbVoxelsZ-RESOLUTION_MIN)/2;
    }
  }else{
    if (m_nbVoxelsY < m_nbVoxelsZ){
      m_nbMaxDiv = (m_nbVoxelsY-RESOLUTION_MIN)/2;
    }else{
      m_nbMaxDiv = (m_nbVoxelsZ-RESOLUTION_MIN)/2;
    }
  }
  
}

LODHybridSolver3D::~LODHybridSolver3D ()
{
  delete[]m_uTmp;
  delete[]m_vTmp;
  delete[]m_wTmp;
}

void LODHybridSolver3D::divideRes ()
{
  if(m_nbVoxelsX < RESOLUTION_MIN || m_nbVoxelsY < RESOLUTION_MIN || m_nbVoxelsZ < RESOLUTION_MIN ){
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
  
  m_nbVoxelsXDivDimX = m_nbVoxelsX / m_dim.x;
  m_nbVoxelsYDivDimY = m_nbVoxelsY / m_dim.y;
  m_nbVoxelsZDivDimZ = m_nbVoxelsZ / m_dim.z;
  
  m_n2 = (m_nbVoxelsX+2) * (m_nbVoxelsY+2);
  m_nx = m_nbVoxelsX+2;
  m_t1 = m_n2 + m_nx +1;
  m_t2nx = 2*m_nx;
}

void LODHybridSolver3D::decreaseRes ()
{
  if(m_nbVoxelsX < RESOLUTION_MIN || m_nbVoxelsY < RESOLUTION_MIN || m_nbVoxelsZ < RESOLUTION_MIN ){
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
  
  m_nbVoxelsXDivDimX = m_nbVoxelsX / m_dim.x;
  m_nbVoxelsYDivDimY = m_nbVoxelsY / m_dim.y;
  m_nbVoxelsZDivDimZ = m_nbVoxelsZ / m_dim.z;
  
  m_n2 = (m_nbVoxelsX+2) * (m_nbVoxelsY+2);
  m_nx = m_nbVoxelsX+2;
  m_t1 = m_n2 + m_nx +1;
  m_t2nx = 2*m_nx;

  cerr << "Voxels en X : " << m_nbVoxelsX << endl;
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
  
  m_nbVoxelsXDivDimX = m_nbVoxelsX / m_dim.x;
  m_nbVoxelsYDivDimY = m_nbVoxelsY / m_dim.y;
  m_nbVoxelsZDivDimZ = m_nbVoxelsZ / m_dim.z;
  
  m_n2 = (m_nbVoxelsX+2) * (m_nbVoxelsY+2);
  m_nx = m_nbVoxelsX+2;
  m_t1 = m_n2 + m_nx +1;
  m_t2nx = 2*m_nx;
}

void LODHybridSolver3D::increaseRes ()
{  
  if( m_nbVoxelsX == initialNbVoxelsX || m_nbVoxelsY == initialNbVoxelsY || m_nbVoxelsZ > initialNbVoxelsZ ){
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
  
  m_nbVoxelsXDivDimX = m_nbVoxelsX / m_dim.x;
  m_nbVoxelsYDivDimY = m_nbVoxelsY / m_dim.y;
  m_nbVoxelsZDivDimZ = m_nbVoxelsZ / m_dim.z;
  
  m_n2 = (m_nbVoxelsX+2) * (m_nbVoxelsY+2);
  m_nx = m_nbVoxelsX+2;
  m_t1 = m_n2 + m_nx +1;
  m_t2nx = 2*m_nx;
  cerr << "Voxels en X : " << m_nbVoxelsX << endl;
}

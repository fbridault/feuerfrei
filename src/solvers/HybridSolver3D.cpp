#include "HybridSolver3D.hpp"
#include "SSE4.hpp"
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

/* Le constructeur de GSSolver _mm_free(m_uTmpr);3D n'a pas de paramètre, il n'est donc pas appelé explicitement */
HybridSolver3D::HybridSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, 
				float timeStep, float buoyancy, float omegaDiff, float omegaProj, float epsilon) : 
  Solver3D (position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy), GCSSORSolver3D(omegaDiff, omegaProj, epsilon)
{
}
/* Le constructeur de GSSolver3D n'a pas de paramètre, il n'est donc pas appelé explicitement */
HybridSolver3D::HybridSolver3D (float omegaDiff, float omegaProj, float epsilon) : 
  GCSSORSolver3D(omegaDiff, omegaProj, epsilon)
{
  cerr << m_aVisc << endl;
}

HybridSolver3D::~HybridSolver3D ()
{  
}

/* Pas de diffusion */
void HybridSolver3D::diffuse (unsigned char b, float *const x, float *const x0, float a)
{
  GCSSORSolver3D::diffuse (b, x, x0, a);
}

void HybridSolver3D::project (float *const p, float *const div)
{
  GCSSORSolver3D::project(p,div);
}

LODHybridSolver3D::LODHybridSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, 
				      float timeStep, float buoyancy, float omegaDiff, float omegaProj, float epsilon) : 
  Solver3D (position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy), 
  HybridSolver3D (omegaDiff, omegaProj, epsilon)
{
  /* Attention n_x, n_y et n_z doivent être impairs */
  initialNbVoxelsX = n_x;
  initialNbVoxelsY = n_y;
  initialNbVoxelsZ = n_z;
  
//   m_uTmp =(float*)_mm_malloc(m_nbVoxels*sizeof(float),16);
//   m_vTmp =(float*)_mm_malloc(m_nbVoxels*sizeof(float),16);
//   m_wTmp = (float*)_mm_malloc(m_nbVoxels*sizeof(float),16);
  m_uTmp = new float[m_nbVoxels];
  m_vTmp = new float[m_nbVoxels];
  m_wTmp = new float[m_nbVoxels];
  
  fill_n(m_uTmp, m_nbVoxels, 0.0f);
  fill_n(m_vTmp, m_nbVoxels, 0.0f);
  fill_n(m_wTmp, m_nbVoxels, 0.0f);

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
//   _mm_free(m_uTmp);
//   _mm_free(m_vTmp);
//   _mm_free(m_wTmp);
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
  //   glDeleteLists(m_baseDisplayList,1);
  //   glDeleteLists(m_gridDisplayList,1);
  //   buildDLBase ();
  //   buildDLGrid ();
  
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
  //   glDeleteLists(m_baseDisplayList,1);
  //   glDeleteLists(m_gridDisplayList,1);
  //   buildDLBase ();
  //   buildDLGrid ();
  
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
  //   glDeleteLists(m_baseDisplayList,1);
  //   glDeleteLists(m_gridDisplayList,1);
  //   buildDLBase ();
  //   buildDLGrid ();
  
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
  //   glDeleteLists(m_baseDisplayList,1);
  //   glDeleteLists(m_gridDisplayList,1);
  //   buildDLBase ();
  //   buildDLGrid ();
  
  m_nbVoxelsXDivDimX = m_nbVoxelsX / m_dim.x;
  m_nbVoxelsYDivDimY = m_nbVoxelsY / m_dim.y;
  m_nbVoxelsZDivDimZ = m_nbVoxelsZ / m_dim.z;
  
  m_n2 = (m_nbVoxelsX+2) * (m_nbVoxelsY+2);
  m_nx = m_nbVoxelsX+2;
  m_t1 = m_n2 + m_nx +1;
  m_t2nx = 2*m_nx;
  cerr << "Voxels en X : " << m_nbVoxelsX << endl;
}

/** Fonction de dessin de la grille */
void LODHybridSolver3D::displayGrid ()
{
  float interx = m_dim.x / (float) m_nbVoxelsX;
  float intery = m_dim.y / (float) m_nbVoxelsY;
  float interz = m_dim.z / (float) m_nbVoxelsZ;
  float i, j;
  
  glBegin (GL_LINES);
  
  glColor4f (0.5f, 0.5f, 0.5f, 0.5f);
  
  for (j = 0.0f; j <= m_dim.z; j += interz)
    {
      for (i = 0.0f; i <= m_dim.x + interx / 2; i += interx)
	{
	  glVertex3f (i, 0.0f, j);
	  glVertex3f (i, m_dim.y, j);
	}
      for (i = 0.0f; i <= m_dim.y + intery / 2; i += intery)
	{
	  glVertex3f (0.0f, i, j);
	  glVertex3f (m_dim.x, i, j);
	}
    }
  glEnd ();
}

/** Fonction de dessin du repère de base */
void LODHybridSolver3D::displayBase (){
  float interx = m_dim.x / (float) m_nbVoxelsX;
  float interz = m_dim.z / (float) m_nbVoxelsZ;
  float i;
  
  glBegin (GL_LINES);
  
  glLineWidth (1.0f);
  glColor4f (0.5f, 0.5f, 0.5f, 0.5f);
  for (i = 0.0f; i <= m_dim.x + interx / 2; i += interx)
    {
      glVertex3f (i, 0.0f, 0.0f);
      glVertex3f (i, 0.0f, m_dim.z);
    }
  for (i = 0.0f; i <= m_dim.z + interz / 2; i += interz)
    {
      glVertex3f (0.0f, 0.0f, i);
      glVertex3f (m_dim.x, 0.0f, i);
    }
  glEnd ();
};
  

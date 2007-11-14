#include "LODField3D.hpp"
#include "SSE4.hpp"

LODSolver3D::LODSolver3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, 
				      float timeStep, float buoyancy, float vorticityConfinement, float omegaDiff,
				      float omegaProj, float epsilon) :
  Solver3D (position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy, vorticityConfinement), 
  GCSSORSolver3D (omegaDiff, omegaProj, epsilon)
{
  /* Attention n_x, n_y et n_z doivent être impairs */
  m_initialNbVoxelsX = n_x;
  m_initialNbVoxelsY = n_y;
  m_initialNbVoxelsZ = n_z;
  
  m_tmp = (float*)_mm_malloc(m_nbVoxels*sizeof(float),16);
//   m_tmp = new float[m_nbVoxels];
  
  fill_n(m_tmp, m_nbVoxels, 0.0f);

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
  
  m_resCount = 0;
  m_level = 0;
}

LODSolver3D::~LODSolver3D ()
{
  _mm_free(m_tmp);
//   delete[]m_tmp;
}

void LODSolver3D::recomputeAttributes()
{
  m_nbVoxels = (m_nbVoxelsX + 2) * (m_nbVoxelsY + 2) * (m_nbVoxelsZ + 2);
  
  //m_aDiff = m_dt * m_diff * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
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
  
  m_hx= 0.5f/m_nbVoxelsX; m_invhx= 0.5f*m_nbVoxelsX;
  m_hy= 0.5f/m_nbVoxelsY; m_invhy= 0.5f*m_nbVoxelsY;
  m_hz= 0.5f/m_nbVoxelsZ; m_invhz= 0.5f*m_nbVoxelsZ;
  
  // Pour SSE
  m_debut= m_t1-m_t1%4;
  m_dernier=IX(m_nbVoxelsX,m_nbVoxelsY,m_nbVoxelsZ);
  m_nbgrps= (m_dernier-m_t1)/4+1;

  /* Il faut surtout ne pas oublier de mettre les "nouveaux" bords à 0 */
  set_bnd(0,m_u);
  set_bnd(0,m_v);
  set_bnd(0,m_w);
  set_bnd(0,m_uPrev);
  set_bnd(0,m_vPrev);
  set_bnd(0,m_wPrev);
  set_bnd(0,m_r);
  set_bnd(0,m_z);
  set_bnd(0,m_p);
  set_bnd(0,m_q);
  
//   fill_n(m_u, m_nbVoxels, 0.0f);
//   fill_n(m_v, m_nbVoxels, 0.0f);
//   fill_n(m_w, m_nbVoxels, 0.0f);
//   fill_n(m_uPrev, m_nbVoxels, 0.0f);
//   fill_n(m_vPrev, m_nbVoxels, 0.0f);
//   fill_n(m_wPrev, m_nbVoxels, 0.0f);
  
  cerr << "Voxels en X : " << m_nbVoxelsX << endl;
}

void LODSolver3D::divideRes ()
{
  if(m_nbVoxelsX < RESOLUTION_MIN || m_nbVoxelsY < RESOLUTION_MIN || m_nbVoxelsZ < RESOLUTION_MIN ){
    cerr << "Minimum grid resolution already reached !" << endl;
    return;
  }
  
  restreindre(m_u,m_tmp); SWAP(m_u,m_tmp);
  restreindre(m_v,m_tmp); SWAP(m_v,m_tmp);
  restreindre(m_w,m_tmp); SWAP(m_w,m_tmp);
  
  restreindre(m_uPrev,m_tmp); SWAP(m_uPrev,m_tmp);
  restreindre(m_vPrev,m_tmp); SWAP(m_vPrev,m_tmp);
  restreindre(m_wPrev,m_tmp); SWAP(m_wPrev,m_tmp);
    
  m_nbVoxelsX = m_nbVoxelsX/2;
  m_nbVoxelsY = m_nbVoxelsY/2;
  m_nbVoxelsZ = m_nbVoxelsZ/2;
  
  m_level++;

  recomputeAttributes();
}

void LODSolver3D::multiplyRes ()
{  
  if(m_nbVoxelsX == m_initialNbVoxelsX || m_nbVoxelsY == m_initialNbVoxelsY || m_nbVoxelsZ > m_initialNbVoxelsZ ){
    cerr << "Maximum grid resolution already reached !" << endl;
    return;
  }
  
  prolonger(m_u,m_tmp); SWAP(m_u,m_tmp);
  prolonger(m_v,m_tmp); SWAP(m_v,m_tmp);
  prolonger(m_w,m_tmp); SWAP(m_w,m_tmp);
  
  prolonger(m_uPrev,m_tmp); SWAP(m_uPrev,m_tmp);
  prolonger(m_vPrev,m_tmp); SWAP(m_vPrev,m_tmp);
  prolonger(m_wPrev,m_tmp); SWAP(m_wPrev,m_tmp);
  
  m_level--;

  if(m_level)
    {
      m_nbVoxelsX = m_initialNbVoxelsX / (2*m_level);
      m_nbVoxelsY = m_initialNbVoxelsY / (2*m_level);
      m_nbVoxelsZ = m_initialNbVoxelsZ / (2*m_level);
    }
  else
    {
      m_nbVoxelsX = m_initialNbVoxelsX;
      m_nbVoxelsY = m_initialNbVoxelsY;
      m_nbVoxelsZ = m_initialNbVoxelsZ;
    }
  
  recomputeAttributes();
}

void LODSolver3D::decrementRes ()
{
  if(m_nbVoxelsX < RESOLUTION_MIN || m_nbVoxelsY < RESOLUTION_MIN || m_nbVoxelsZ < RESOLUTION_MIN ){
    cerr << "Minimum grid resolution already reached !" << endl;
    return;
  }
  
  m_nbVoxelsX-=2;
  m_nbVoxelsY-=2;
  m_nbVoxelsZ-=2;

  recomputeAttributes();  
}
void LODSolver3D::incrementRes ()
{  
  if( m_nbVoxelsX == m_initialNbVoxelsX || m_nbVoxelsY == m_initialNbVoxelsY || m_nbVoxelsZ > m_initialNbVoxelsZ ){
    cerr << "Maximum grid resolution already reached !" << endl;
    return;
  }
  
  m_nbVoxelsX+=2;
  m_nbVoxelsY+=2;
  m_nbVoxelsZ+=2;

  recomputeAttributes();
}

/** Fonction de dessin de la grille */
void LODSolver3D::displayGrid ()
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
void LODSolver3D::displayBase (){
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
}

LODField3D::LODField3D (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, float timeStep,
			float buoyancy, float vorticityConfinement, float omegaDiff, float omegaProj, float epsilon) : 
  Field3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy),
  m_fakeField(position, dim, scale, timeStep, buoyancy),
  m_solver(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy, vorticityConfinement, omegaDiff, omegaProj, epsilon)
{
  m_currentField = &m_solver;
  m_fieldToSwitch = &m_solver;
}

LODSmoothField::LODSmoothField (const Point& position, uint n_x, uint n_y, uint n_z, float dim, const Point& scale, float timeStep,
				float buoyancy, float vorticityConfinement, float omegaDiff, float omegaProj, float epsilon) : 
  LODField3D(position, n_x, n_y, n_z, dim, scale, timeStep, buoyancy, vorticityConfinement, omegaDiff, omegaProj, epsilon)
{
  m_switch = 0;
}

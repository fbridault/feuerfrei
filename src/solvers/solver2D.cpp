#include "solver2D.hpp"

#include <math.h>
#include "../scene/graphicsFn.hpp"

Solver2D::Solver2D ()
{
}

Solver2D::Solver2D (CTransform& a_rTransform, uint n_x, uint n_y,float dim, float timeStep, float buoyancy,
					float vorticityConfinement) :
  Field(a_rTransform, timeStep, buoyancy)
{
  m_nbVoxelsX = n_x;
  m_nbVoxelsY = n_y;

  /* Détermination de la taille du solveur de manière à ce que le plus grand côté soit de dimension dim */
  if (m_nbVoxelsX > m_nbVoxelsY){
      m_dimX = dim;
      m_dimY = m_dimX * m_nbVoxelsY / m_nbVoxelsX;
  }else{
      m_dimY = dim;
      m_dimX = m_dimY * m_nbVoxelsX / m_nbVoxelsY;
  }

  m_nbVoxels = (m_nbVoxelsX + 2) * (m_nbVoxelsY + 2);

  m_u = new float[m_nbVoxels];
  m_v = new float[m_nbVoxels];
  m_dens = new float[m_nbVoxels];
  m_uPrev = new float[m_nbVoxels];
  m_vPrev = new float[m_nbVoxels];
  m_densPrev = new float[m_nbVoxels];
  m_uSrc = new float[m_nbVoxels];
  m_vSrc = new float[m_nbVoxels];
  m_densSrc = new float[m_nbVoxels];
  m_rot      = new float[m_nbVoxels];

  fill_n(m_u, m_nbVoxels, 0.0f);
  fill_n(m_v, m_nbVoxels, 0.0f);
  fill_n(m_uPrev, m_nbVoxels, 0.0f);
  fill_n(m_vPrev, m_nbVoxels, 0.0f);
  fill_n(m_uSrc, m_nbVoxels, 0.0f);
  fill_n(m_vSrc, m_nbVoxels, 0.0f);
  fill_n(m_dens, m_nbVoxels, 0.0f);
  fill_n(m_densPrev, m_nbVoxels, 0.0f);
  fill_n(m_densSrc, m_nbVoxels, 0.0f);

  m_visc = 0.000022f;
  m_diff = 0.0001f;

  m_aDiff = m_dt * m_diff * m_nbVoxelsX * m_nbVoxelsY;
  m_aVisc = m_dt * m_visc * m_nbVoxelsX * m_nbVoxelsY;

  /* Construction des display lists */
  buildDLBase ();
  buildDLGrid ();

  m_vorticityConfinement = vorticityConfinement;

  m_hx= 0.5f/n_x;
  m_hy= 0.5f/n_y;

  m_invhx= 0.5f*n_x;
  m_invhy= 0.5f*n_y;

  m_nbVoxelsXDivDimX = m_dimX * m_nbVoxelsX;
  m_nbVoxelsYDivDimY = m_dimY * m_nbVoxelsY;

  m_halfNbVoxelsX = m_nbVoxelsX/2;

  m_nx = m_nbVoxelsX+2;
  m_t1= m_nx +1;
  m_t2nx = 2*m_nx;

  m_forceCoef = 0.01f;
  m_forceRatio = 1/m_forceCoef;
}

Solver2D::~Solver2D ()
{
  delete[]m_u;
  delete[]m_v;
  delete[]m_dens;

  delete[]m_uPrev;
  delete[]m_vPrev;
  delete[]m_densPrev;

  delete[]m_uSrc;
  delete[]m_vSrc;
  delete[]m_densSrc;

  delete[]m_rot;
}

void Solver2D::set_bnd (unsigned char b, float *const x)
{
  uint i;

  /* Attention cela ne prend pas en compte les coins et les arêtes entre les coins */
  for (i = 1; i <= m_nbVoxelsY; i++)
    {
      x[IX (0, i)] = x[IX(1, i)];
      x[IX (m_nbVoxelsX + 1, i)] = x[IX(m_nbVoxelsX, i)];
    }

  for (i = 1; i <= m_nbVoxelsX; i++)
    {
      x[IX (i, 0)] = x[IX(i, 1)];
      x[IX (i, m_nbVoxelsY + 1)] = x[IX (i, m_nbVoxelsY)];
    }
}

void Solver2D::advect (unsigned char b, float *const d, const float *const d0,
		     const float *const u, const float *const v)
{
  uint i, j, i0, j0, i1, j1;
  float x, y, s0, t0, s1, t1, dt0_x, dt0_y;

  dt0_x = m_dt * m_nbVoxelsX;
  dt0_y = m_dt * m_nbVoxelsY;

  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      {
	x = i - dt0_x * u[IX (i, j)];
	y = j - dt0_y * v[IX (i, j)];

	if (x < 0.5f) x = 0.5f;
	if (x > m_nbVoxelsX + 0.5f) x = m_nbVoxelsX + 0.5f;
	i0 = (uint) x; i1 = i0 + 1;

	if (y < 0.5f) y = 0.5f;
	if (y > m_nbVoxelsY + 0.5f) y = m_nbVoxelsY + 0.5f;
	j0 = (uint) y; j1 = j0 + 1;

	s1 = x-i0; s0= 1-s1; t1 = y-j0; t0 = 1-t1;
	d[IX(i,j)] = s0*(t0*d0[IX(i0,j0)]+t1*d0[IX(i0,j1)]) + s1*(t0*d0[IX(i1,j0)]+t1*d0[IX(i1,j1)]);
      }

  //  set_bnd (b, d);
}

void Solver2D::addVorticityConfinement(float * const u, float *const  v)
{
  uint i,j;
  float eps =m_dt*m_forceCoef*m_vorticityConfinement*100.0f;//epsilon
  float x;
  float Nx,Ny;
  float invNormeN;

  /** Calcul de m_rot la norme du rotationnel du champ de vélocité (m_u, m_v)
   */
  m_t = m_t1;
  for (j=1; j<=m_nbVoxelsY; j++) {
    for (i=1; i<=m_nbVoxelsX; i++) {
      // En 2D le rotationnel est un scalaire = dm_y/dx - dm_x/dy
      x = m_rot[m_t] = (v[m_t+1] - v[m_t-1]) * m_invhx - (u[m_t+m_nx] - u[m_t-m_nx]) * m_invhy;
      // m_rot = |m_rot|
      // La normalisation n'a pas de sens ici
//    m_rot[m_t] = sqrtf(x*x+y*y);

      m_t++;
    }//for i
    m_t+=2;
  }//for j

  /* Calcul du gradient normalisé du rotationnel m_rot
   * Calcul du produit vectoriel N^m_rot
   * Le vecteur est multiplié par epsilon*h
   * et est ajouté au champ de vélocité
   */
  m_t=m_t1;
  for (j=1; j<=m_nbVoxelsY; j++) {
    for (i=1; i<=m_nbVoxelsX; i++) {

      Nx = (m_rot[m_t+1] - m_rot[m_t-1]) * m_invhx;
      Ny = (m_rot[m_t+m_nx] - m_rot[m_t-m_nx]) * m_invhy;

      invNormeN = 1.0/(sqrtf(Nx*Nx+Ny*Ny)+0.000001f);

      Nx *= invNormeN;
      Ny *= invNormeN;

      // Le rotationnel d'un champ scalaire N^m_rot est un vecteur 2D = (dmrot/dy,-dmrot/dx)
      u[m_t] +=(Ny*m_rot[m_t]) * eps * m_hx * 2.0f;
      v[m_t] +=(-Nx*m_rot[m_t]) * eps * m_hy * 2.0f;
      m_t++;
      }//for i
    m_t+=2;
  }//for j

}//AddVorticityConfinement

void Solver2D::dens_step()
{
  add_source ( m_dens, m_densSrc);
  SWAP (m_densPrev, m_dens); diffuse ( 0, m_dens, m_densPrev, m_aDiff);
  SWAP (m_densPrev, m_dens); advect ( 0, m_dens, m_densPrev, m_u, m_v);
}

void Solver2D::vel_step ()
{
  add_source (m_u, m_uSrc);
  add_source (m_v, m_vSrc);
  addVorticityConfinement(m_u,m_v);
  SWAP (m_uPrev, m_u);
  diffuse (1, m_u, m_uPrev, m_aVisc);
  SWAP (m_vPrev, m_v);
  diffuse (2, m_v, m_vPrev, m_aVisc);
  project (m_uPrev, m_vPrev);
  SWAP (m_uPrev, m_u);
  SWAP (m_vPrev, m_v);
  advect (1, m_u, m_uPrev, m_uPrev, m_vPrev);
  advect (2, m_v, m_vPrev, m_uPrev, m_vPrev);
  project (m_uPrev, m_vPrev);
}

void Solver2D::iterate ()
{
  /* Cellule(s) génératrice(s) */
//   for (uint i = 1; i < m_nbVoxelsX + 1; i++)
//     for (uint j = 1; j < m_nbVoxelsY + 1; j++)
// 	m_vSrc[IX(i, j)] += m_buoyancy / (float) (m_nbVoxelsY-j+1);

  if(m_permanentExternalForces.x || m_permanentExternalForces.y)
    addExternalForces(m_permanentExternalForces,false);

  vel_step ();
  dens_step();

  m_nbIter++;
  cleanSources ();
}

void Solver2D::cleanSources ()
{
  fill_n(m_uSrc, m_nbVoxels, 0.0f);
  fill_n(m_vSrc, m_nbVoxels, 0.0f);
  fill_n(m_densSrc, m_nbVoxels, 0.0f);
}

void Solver2D::buildDLGrid ()
{
  float interx = m_dimX / (float) m_nbVoxelsX;
  float intery = m_dimY / (float) m_nbVoxelsY;
  float i;

  m_gridDisplayList=glGenLists(1);
  glNewList (m_gridDisplayList, GL_COMPILE);

  glBegin(GL_LINES);
  glColor3f(0.5f, 0.5f, 0.5f);
  for(i = 0.0 ; i <= m_dimX+interx ; i+= interx){
    glVertex3f( i, 0.0f, 0.0f);
    glVertex3f( i, m_dimY, 0.0f);
  }
  for(i = 0.0 ; i <= m_dimY+intery ; i+= intery){
    glVertex3f(0.0f, i,  0.0f);
    glVertex3f(m_dimX, i,  0.0f);
  }
  glEnd();
  glEndList();
}

void Solver2D::buildDLBase ()
{
  m_baseDisplayList=glGenLists(1);
  glNewList (m_baseDisplayList, GL_COMPILE);

  glBegin(GL_LINE_LOOP);
  glColor3f(0.5f, 0.5f, 0.5f);
  glVertex3f( 0.0f, 0.0f, 0.0f);
  glVertex3f( 0.0f, m_dimY, 0.0f);
  glVertex3f( m_dimX, m_dimY,  0.0f);
  glVertex3f( m_dimX, 0.0f,  0.0f);
  glEnd();
  glEndList();
}

void Solver2D::displayVelocityField (void)
{
  float inc_x = m_dimX / (float) m_nbVoxelsX;
  float inc_y = m_dimY / (float) m_nbVoxelsY;
  CVector vect;

  for (uint i = 1; i <= m_nbVoxelsX; i++)
    {
      for (uint j = 1; j <= m_nbVoxelsY; j++)
	{
	  /* Affichage du champ de vélocité */
	  glPushMatrix();
	  glTranslatef(inc_x*i-inc_x/2.0f,inc_y*j-inc_y/2.0f,0.0f);
	  //      printf("%vélocité %d %d %f %f\n",i,j,u[IX(i,j)],v[IX(i,j)]);

	  vect.x = getU (i, j);  vect.y = getV (i, j);
	  if (vect.x != 0 && vect.y != 0)
	    displayArrow (vect);
	  glPopMatrix();

	  /* Affichage de la densité */
	  //      printf("densité %d %d %f \n",i,j,dens[IX(i,j)]*10000);
	}
    }
}

void Solver2D::displayDensityField (void)
{
  float inc_x = m_dimX / (float) m_nbVoxelsX;
  float inc_y = m_dimY / (float) m_nbVoxelsY;

  for (uint i = 1; i <= m_nbVoxelsX; i++)
    for (uint j = 1; j <= m_nbVoxelsY; j++)
      if(getDens(i,j) > 0.0f){
	glColor4f(1.0f,1.0f,1.0f,getDens(i,j)*10000);
	glBegin(GL_QUADS);
	glVertex3f(inc_x*(i-1),inc_y*(j-1),-0.01f);
	glVertex3f(inc_x*i,inc_y*(j-1),-0.01f);
	glVertex3f(inc_x*i,inc_y*j,-0.01f);
	glVertex3f(inc_x*(i-1),inc_y*j,-0.01f);
	glEnd();
      }
}

void Solver2D::displayArrow (const CVector& direction)
{
  float norme_vel = sqrt (direction.x * direction.x + direction.y * direction.z);
  float taille = m_dimX * m_dimY * norme_vel * m_forceRatio * .01f;
  float angle;
  CVector ref, dir(direction);

  dir.normalize ();

  ref.x = 1.0f; ref.y=0.0f; ref.z =0.0f;

  angle = acosf(dir.x*ref.x + dir.y*ref.y);
  if(dir.y<0.0f) angle = -angle;

  glRotatef(angle*RAD_TO_DEG,0.0f,0.0f,1.0f);

  glBegin(GL_LINES);

  glColor4f(1.0, 0.0, 0.0,1.0);

  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(taille, 0.0f, 0.0f);
  glVertex3f(taille-taille/4, taille/4, 0.0f);
  glVertex3f(taille, 0.0f,  0.0f);
  glVertex3f(taille-taille/4, -taille/4, 0.0f);
  glVertex3f(taille, 0.0f,  0.0f);

  glEnd();
}

void Solver2D::addExternalForces(const CPoint& position, bool move)
{
  uint i;
  CPoint strength;
  CPoint force;

  if(move){
    force = position - m_position;
    strength.x = strength.y = .05f*m_nbVoxelsX;
    m_position=position;
  }else{
    force = position;
    strength = position * .01f * m_nbVoxelsX;
    strength.x = fabs(strength.x);
    strength.y = fabs(strength.y);
  }

  uint widthx = m_nbVoxelsX - m_nbVoxelsX/2;
  uint widthy = m_nbVoxelsY - m_nbVoxelsY/2;

  uint ceilx = m_nbVoxelsX/2;
  uint ceily = m_nbVoxelsY/2;

  /* Ajouter des forces externes */
  if(force.x)
    if( force.x > 0.0f)
      for (i = ceily; i <= widthy; i++)
	addUsrc (m_nbVoxelsX - 1, i, -strength.x);
    else
      for (i = ceily; i <= widthy; i++)
	addUsrc (1, i, strength.x);
  if(force.y)
    if( force.y > 0.0f)
      for (i = ceilx; i <= widthx; i++)
	addVsrc (i, m_nbVoxelsY - 1, -strength.y);
    else
      for (i = ceilx; i <= widthx; i++)
	addVsrc (i, 1, strength.y);
}

void Solver2D::addDensity(int id)
{
  switch(id){
  case 1 : addDensSrc(1,m_nbVoxelsY/2, 0.01f); break;
  case 2 : addDensSrc(m_nbVoxelsX,m_nbVoxelsY/2, 0.01f); break;
  case 3 : addDensSrc(m_nbVoxelsX/2,m_nbVoxelsY, 0.01f); break;
  case 4 : addDensSrc(m_nbVoxelsX/2,1, 0.01f); break;
  }
}

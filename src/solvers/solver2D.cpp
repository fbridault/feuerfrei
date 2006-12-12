#include "solver2D.hpp"

#include <math.h>
#include "../scene/graphicsFn.hpp"

Solver2D::Solver2D ()
{
}

Solver2D::Solver2D (Point& position, uint n_x, uint n_y,double dim, double timeStep, double buoyancy) : 
  Field(position, timeStep, buoyancy)
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
  
  m_u = new double[m_nbVoxels];
  m_v = new double[m_nbVoxels];
  m_dens = new double[m_nbVoxels];
  m_uPrev = new double[m_nbVoxels];
  m_vPrev = new double[m_nbVoxels];
  m_densPrev = new double[m_nbVoxels];
  m_uSrc = new double[m_nbVoxels];
  m_vSrc = new double[m_nbVoxels];
  m_densSrc = new double[m_nbVoxels];
  
  memset (m_u, 0, m_nbVoxels * sizeof (double));
  memset (m_v, 0, m_nbVoxels * sizeof (double));
  memset (m_dens, 0, m_nbVoxels * sizeof (double));
  memset (m_uPrev, 0, m_nbVoxels * sizeof (double));
  memset (m_vPrev, 0, m_nbVoxels * sizeof (double));
  memset (m_densPrev, 0, m_nbVoxels * sizeof (double));
  memset (m_uSrc, 0, m_nbVoxels * sizeof (double));
  memset (m_vSrc, 0, m_nbVoxels * sizeof (double));
  memset (m_densSrc, 0, m_nbVoxels * sizeof (double));
    
  m_aDiff = m_dt * m_diff * m_nbVoxelsX * m_nbVoxelsY;
  m_aVisc = m_dt * m_visc * m_nbVoxelsX * m_nbVoxelsY;
    
  /* Construction des display lists */
  buildDLBase ();
  buildDLGrid ();

  m_visc = 0.00000015;
  m_diff = 0.01;
  
  m_dimXTimesNbVoxelsX = m_dimX * m_nbVoxelsX;
  m_dimYTimesNbVoxelsY = m_dimY * m_nbVoxelsY;

  m_halfNbVoxelsX = m_nbVoxelsX/2;
  
  nx = m_nbVoxelsX+2;
  t1= nx +1;
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
}

void Solver2D::set_bnd (unsigned char b, double *const x)
{
  uint i, j;

  /* Attention cela ne prend pas en compte les coins et les arêtes entre les coins */
  for (i = 1; i <= m_nbVoxelsY; i++)
    {
      x[IX (0, i)] = 0;	//x[IX(i,j,1)];
      x[IX (m_nbVoxelsX + 1, i)] = 0;	//x[IX(i,j,N)];
    }
  
  for (i = 1; i <= m_nbVoxelsX; i++)
    {
      x[IX (i, 0)] = 0;	//x[IX(i, 1, j)];
      //x[IX(i, N+1, j)] = 0;//x[IX(i,N,j)];
      x[IX (i, m_nbVoxelsY + 1)] = 0;//-- x[IX (i, m_nbVoxelsY, j)];
    } 
}

void Solver2D::advect (unsigned char b, double *const d, const double *const d0,
		     const double *const u, const double *const v)
{
  uint i, j, i0, j0, i1, j1;
  double x, y, z, s0, t0, s1, t1, dt0_x, dt0_y;

  dt0_x = m_dt * m_nbVoxelsX;
  dt0_y = m_dt * m_nbVoxelsY;
  
  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      {
	x = i - dt0_x * u[IX (i, j)];
	y = j - dt0_y * v[IX (i, j)];
	
	if (x < 0.5) x = 0.5;
	if (x > m_nbVoxelsX + 0.5) x = m_nbVoxelsX + 0.5;
	i0 = (uint) x; i1 = i0 + 1;
	
	if (y < 0.5) y = 0.5;
	if (y > m_nbVoxelsY + 0.5) y = m_nbVoxelsY + 0.5;
	j0 = (uint) y; j1 = j0 + 1;
	
	s1 = x-i0; s0= 1-s1; t1 = y-j0; t0 = 1-t1;
	d[IX(i,j)] = s0*(t0*d0[IX(i0,j0)]+t1*d0[IX(i0,j1)]) + s1*(t0*d0[IX(i1,j0)]+t1*d0[IX(i1,j1)]);
      }
  
  //set_bnd (b, d);
}

void Solver2D::dens_step()
{
  add_source ( m_dens, m_densSrc);
  SWAP (m_densPrev, m_dens); diffuse ( 0, m_dens, m_densPrev, m_aDiff, m_diff);
  SWAP (m_densPrev, m_dens); advect ( 0, m_dens, m_densPrev, m_u, m_v);
}

void Solver2D::vel_step ()
{
  add_source (m_u, m_uSrc);
  add_source (m_v, m_vSrc);
  SWAP (m_uPrev, m_u);
  diffuse (1, m_u, m_uPrev, m_aVisc, m_visc);
  SWAP (m_vPrev, m_v);
  diffuse (2, m_v, m_vPrev, m_aVisc, m_visc);
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
  for (uint i = 1; i < m_nbVoxelsX + 1; i++)
    for (uint j = 1; j < m_nbVoxelsY + 1; j++)
	m_vSrc[IX(i, j)] += m_buoyancy / (double) (m_nbVoxelsY-j+1);
  
  if(arePermanentExternalForces)
    addExternalForces(permanentExternalForces,false);
  
  vel_step ();
  dens_step();
  
  m_nbIter++;
  
  cleanSources ();
//   set_bnd (0, m_u);
//   set_bnd (0, m_v);
//   set_bnd (0, m_w);
  
}

void Solver2D::cleanSources ()
{
  m_uSrc = (double *) memset (m_uSrc, 0, m_nbVoxels * sizeof (double));
  m_vSrc = (double *) memset (m_vSrc, 0, m_nbVoxels * sizeof (double));
  m_densSrc = (double *) memset (m_densSrc, 0, m_nbVoxels * sizeof (double));
}

void Solver2D::buildDLGrid ()
{
  double interx = m_dimX / (double) m_nbVoxelsX;
  double intery = m_dimY / (double) m_nbVoxelsY;
  float i;
  
  m_gridDisplayList=glGenLists(1);
  glNewList (m_gridDisplayList, GL_COMPILE);

  glBegin(GL_LINES); 
  glColor3f(0.5, 0.5, 0.5);
  for(i = 0.0 ; i <= m_dimX+interx ; i+= interx){
    glVertex3f( i, 0.0, 0.0);
    glVertex3f( i, m_dimY, 0.0);
  }
  for(i = 0.0 ; i <= m_dimY+intery ; i+= intery){
    glVertex3f(0.0, i,  0.0);
    glVertex3f(m_dimX, i,  0.0);
  }
  glEnd();
  glEndList();
}

void Solver2D::buildDLBase ()
{
  double interx = m_dimX / (double) m_nbVoxelsX;
  double intery = m_dimY / (double) m_nbVoxelsY;
  
  m_baseDisplayList=glGenLists(1);
  glNewList (m_baseDisplayList, GL_COMPILE);

  glBegin(GL_LINE_LOOP); 
  glColor3f(0.5, 0.5, 0.5);
  glVertex3f( 0.0, 0.0, 0.0);
  glVertex3f( 0.0, m_dimY, 0.0);
  glVertex3f( m_dimX, m_dimY,  0.0);
  glVertex3f( m_dimX, 0.0,  0.0);
  glEnd();
  glEndList();
}

void Solver2D::displayVelocityField (void)
{
  double inc_x = m_dimX / (double) m_nbVoxelsX;
  double inc_y = m_dimY / (double) m_nbVoxelsY;
  Vector vect;
  
  for (uint i = 1; i <= m_nbVoxelsX; i++)
    {
      for (uint j = 1; j <= m_nbVoxelsY; j++)
	{
	  /* Affichage du champ de vélocité */
	  glPushMatrix();
	  glTranslatef(inc_x*i-inc_x/2.0,inc_y*j-inc_y/2.0,0.0);
	  //      printf("%vélocité %d %d %f %f\n",i,j,u[IX(i,j)],v[IX(i,j)]);
	  
	  vect.x = getU (i, j);  vect.y = getV (i, j);
	  displayArrow (vect);
	  glPopMatrix();
	  
	  /* Affichage de la densité */
	  //      printf("densité %d %d %f \n",i,j,dens[IX(i,j)]*10000);
	}
    }
}

void Solver2D::displayDensityField (void)
{
  double inc_x = m_dimX / (double) m_nbVoxelsX;
  double inc_y = m_dimY / (double) m_nbVoxelsY;
  
  for (uint i = 1; i <= m_nbVoxelsX; i++)
    for (uint j = 1; j <= m_nbVoxelsY; j++)
      if(getDens(i,j) > 0.0){	
	glColor4f(1.0,1.0,1.0,getDens(i,j)*10000);
	glBegin(GL_QUADS);
	glVertex3f(inc_x*(i-1),inc_y*(j-1),-0.01);
	glVertex3f(inc_x*i,inc_y*(j-1),-0.01);
	glVertex3f(inc_x*i,inc_y*j,-0.01);
	glVertex3f(inc_x*(i-1),inc_y*j,-0.01);
	glEnd();
      }
}

void Solver2D::displayArrow (Vector& direction)
{
  double norme_vel = sqrt (direction.x * direction.x + direction.y * direction.z);
  double taille = m_dimX * m_dimY * norme_vel * 10;
  float angle;
  Vector ref;
  
  ref.x = 1.0; ref.y=0.0; ref.z =0.0;
  
  direction.normalize();
  angle = acosf(direction.x*ref.x + direction.y*ref.y);
  if(direction.y<0.0) angle = -angle;

  glRotatef(angle*RAD_TO_DEG,0.0,0.0,1.0);

  glBegin(GL_LINES);
 
  glColor4f(1.0, 0.0, 0.0,1.0);

  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(taille, 0.0, 0.0);
  glVertex3f(taille-taille/4, taille/4, 0.0);
  glVertex3f(taille, 0.0,  0.0);
  glVertex3f(taille-taille/4, -taille/4, 0.0);
  glVertex3f(taille, 0.0,  0.0);

  glEnd();
}

void Solver2D::addExternalForces(Point& position, bool move)
{
  uint i;
  Point strength;
  Point force;
  
  if(move){
    force = position - m_position;
    strength.x = strength.y = .005*m_nbVoxelsX;
    m_position=position;
  }else{
    force = position;
    strength = position * .001 * m_nbVoxelsX;
    strength.x = fabs(strength.x);
    strength.y = fabs(strength.y);
  }
  
  uint widthx = m_nbVoxelsX - m_nbVoxelsX/2;
  uint widthy = m_nbVoxelsY - m_nbVoxelsY/2;
  
  uint ceilx = m_nbVoxelsX/2;
  uint ceily = m_nbVoxelsY/2;
  
  /* Ajouter des forces externes */
  if(force.x)
    if( force.x > 0)
      for (i = ceily; i <= widthy; i++)
	addUsrc (m_nbVoxelsX - 1, i, -strength.x);
    else
      for (i = ceily; i <= widthy; i++)
	addUsrc (1, i, strength.x); 
  if(force.y)
    if( force.y > 0)
      for (i = ceilx; i <= widthx; i++)
	addVsrc (i, m_nbVoxelsY - 1, -strength.y);
    else
      for (i = ceilx; i <= widthx; i++)
	addVsrc (i, 1, strength.y);
}

void Solver2D::addDensity(int id)
{
  switch(id){
  case 1 : addDensSrc(1,m_nbVoxelsY/2, 0.0001); break;
  case 2 : addDensSrc(m_nbVoxelsX,m_nbVoxelsY/2, 0.0001); break;
  case 3 : addDensSrc(m_nbVoxelsX/2,m_nbVoxelsY, 0.0001); break;
  case 4 : addDensSrc(m_nbVoxelsX/2,1, 0.0001); break;
  }
}

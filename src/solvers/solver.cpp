#include "solver.hpp"

#include <math.h>
#include "../scene/graphicsFn.hpp"

Solver::Solver ()
{
}

Solver::Solver (Point& position, int n_x, int n_y, int n_z, double dim, double timeStep, double buoyancy) : m_position(position)
{
  m_nbVoxelsX = n_x;
  m_nbVoxelsY = n_y;
  m_nbVoxelsZ = n_z;
  
  /* Détermination de la taille du solveur de manière à ce que */
  /*  le plus grand côté soit de dimension dim */
  if (m_nbVoxelsX > m_nbVoxelsY){
    if (m_nbVoxelsX > m_nbVoxelsZ){
      m_dimX = dim;
      m_dimY = m_dimX * m_nbVoxelsY / m_nbVoxelsX;
      m_dimZ = m_dimX * m_nbVoxelsZ / m_nbVoxelsX;
    }else{
      m_dimZ = dim;
      m_dimX = m_dimZ * m_nbVoxelsX / m_nbVoxelsZ;
      m_dimY = m_dimZ * m_nbVoxelsY / m_nbVoxelsZ;
    }
  }else{
    if (m_nbVoxelsY > m_nbVoxelsZ){
      m_dimY = dim;
      m_dimX = m_dimY * m_nbVoxelsX / m_nbVoxelsY;
      m_dimZ = m_dimY * m_nbVoxelsZ / m_nbVoxelsY;
    }else{
      m_dimZ = dim;
      m_dimX = m_dimZ * m_nbVoxelsX / m_nbVoxelsZ;
      m_dimY = m_dimZ * m_nbVoxelsY / m_nbVoxelsZ;
    }
  }
  
  m_nbVoxels = (m_nbVoxelsX + 2) * (m_nbVoxelsY + 2) * (m_nbVoxelsZ + 2);
  m_dt = timeStep;
  
  m_u = new double[m_nbVoxels];
  m_v = new double[m_nbVoxels];
  m_w = new double[m_nbVoxels];
  m_uPrev = new double[m_nbVoxels];
  m_vPrev = new double[m_nbVoxels];
  m_wPrev = new double[m_nbVoxels];
  m_dens = new double[m_nbVoxels];
  m_densPrev = new double[m_nbVoxels];
  m_densSrc = new double[m_nbVoxels];
  m_uSrc = new double[m_nbVoxels];
  m_vSrc = new double[m_nbVoxels];
  m_wSrc = new double[m_nbVoxels];
  
  memset (m_u, 0, m_nbVoxels * sizeof (double));
  memset (m_v, 0, m_nbVoxels * sizeof (double));
  memset (m_w, 0, m_nbVoxels * sizeof (double));
  memset (m_uPrev, 0, m_nbVoxels * sizeof (double));
  memset (m_vPrev, 0, m_nbVoxels * sizeof (double));
  memset (m_wPrev, 0, m_nbVoxels * sizeof (double));
  memset (m_dens, 0, m_nbVoxels * sizeof (double));
  memset (m_densPrev, 0, m_nbVoxels * sizeof (double));
  memset (m_densSrc, 0, m_nbVoxels * sizeof (double));
  memset (m_uSrc, 0, m_nbVoxels * sizeof (double));
  memset (m_vSrc, 0, m_nbVoxels * sizeof (double));
  memset (m_wSrc, 0, m_nbVoxels * sizeof (double));
  
  m_visc = 0.00000015;
  m_diff = 0.001;
  m_nbSteps = 20;
  m_nbIter = 0;
  
  m_aDiff = m_dt * m_diff * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
  m_aVisc = m_dt * m_visc * m_nbVoxelsX * m_nbVoxelsY * m_nbVoxelsZ;
    
  /* Construction des display lists */
  buildDLBase ();
  buildDLGrid ();

  m_buoyancy=buoyancy;

  m_dimXTimesNbVoxelsX = m_dimX * m_nbVoxelsX;
  m_dimXTimesNbVoxelsY = m_dimY * m_nbVoxelsY;
  m_dimXTimesNbVoxelsZ = m_dimZ * m_nbVoxelsZ;

  m_halfNbVoxelsX = m_nbVoxelsX/2;
  m_halfNbVoxelsZ = m_nbVoxelsZ/2;
  
  n2= (m_nbVoxelsX+2) * (m_nbVoxelsY+2);
  nx = m_nbVoxelsX+2;
  t1=n2 + nx +1;
  t2nx=2*nx;
}

Solver::~Solver ()
{
  delete[]m_u;
  delete[]m_v;
  delete[]m_w;

  delete[]m_uPrev;
  delete[]m_vPrev;
  delete[]m_wPrev;
  delete[]m_dens;

  delete[]m_densPrev;
  delete[]m_densSrc;

  delete[]m_uSrc;
  delete[]m_vSrc;
  delete[]m_wSrc;
  
  glDeleteLists(m_baseDisplayList,1);
  glDeleteLists(m_gridDisplayList,1);
}

void Solver::set_bnd (int b, double *const x)
{
  int i, j;

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

/* Ajout des forces externes */
void Solver::add_source (double *const x, double *const src)
{
  int i;

  for (i = 0; i < m_nbVoxels; i++)
    x[i] += m_dt * src[i];
}

/* Pas d'advection => dÃƒÂ©placement du fluide sur lui-même */
void Solver::advect (int b, double *const d, const double *const d0,
		     const double *const u, const double *const v,
		     const double *const w)
{
  int i, j, k, i0, j0, k0, i1, j1, k1;
  double x, y, z, r0, s0, t0, r1, s1, t1, dt0_x, dt0_y, dt0_z;

  dt0_x = m_dt * m_nbVoxelsX;
  dt0_y = m_dt * m_nbVoxelsY;
  dt0_z = m_dt * m_nbVoxelsZ;
  
  for (i = 1; i <= m_nbVoxelsX; i++)
    for (j = 1; j <= m_nbVoxelsY; j++)
      for (k = 1; k <= m_nbVoxelsZ; k++)
	{
	  x = i - dt0_x * u[IX (i, j, k)];
	  y = j - dt0_y * v[IX (i, j, k)];
	  z = k - dt0_z * w[IX (i, j, k)];
	  
	  if (x < 0.5) x = 0.5;
	  if (x > m_nbVoxelsX + 0.5) x = m_nbVoxelsX + 0.5;
	  i0 = (int) x; i1 = i0 + 1;
	  
	  if (y < 0.5) y = 0.5;
	  if (y > m_nbVoxelsY + 0.5) y = m_nbVoxelsY + 0.5;
	  j0 = (int) y; j1 = j0 + 1;
	  
	  if (z < 0.5) z = 0.5;
	  if (z > m_nbVoxelsZ + 0.5) z = m_nbVoxelsZ + 0.5;
	  k0 = (int) z; k1 = k0 + 1;

	  r1 = x - i0;
	  r0 = 1 - r1;
	  s1 = y - j0;
	  s0 = 1 - s1;
	  t1 = z - k0;
	  t0 = 1 - t1;
	  
	  d[IX (i, j, k)] = r0 * (s0 * (t0 * d0[IX (i0, j0, k0)] + t1 * d0[IX (i0, j0, k1)]) +
				  s1 * (t0 * d0[IX (i0, j1, k0)] + t1 * d0[IX (i0, j1, k1)])) +
	    r1 * (s0 * (t0 * d0[IX (i1, j0, k0)] + t1 * d0[IX (i1, j0, k1)]) +
		  s1 * (t0 * d0[IX (i1, j1, k0)] + t1 * d0[IX (i1, j1, k1)]));
	}
  
  //set_bnd (b, d);
}

// void Solver::dens_step()
// {
//   add_source ( m_dens, m_densSrc);
//   SWAP (m_densPrev, m_dens); diffuse ( 0, m_dens, m_densPrev, a_diff, diff);
//   SWAP (m_densPrev, m_dens); advect ( 0, m_dens, m_densPrev, m_u, v, w);
// }

void Solver::vel_step ()
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

void Solver::iterate ()
{ 
  /* Cellule(s) génératrice(s) */
  for (int i = 1; i < m_nbVoxelsX + 1; i++)
    for (int j = 1; j < m_nbVoxelsY + 1; j++)
      for (int k = 1; k < m_nbVoxelsZ + 1; k++)
	m_vSrc[IX(i, j, k)] += m_buoyancy / (double) (m_nbVoxelsY-j+1);
  
  vel_step ();
  //  dens_step();

  m_nbIter++;

  cleanSources ();
//   set_bnd (0, m_u);
//   set_bnd (0, m_v);
//   set_bnd (0, m_w);
  
}

void Solver::cleanSources ()
{
  m_uSrc = (double *) memset (m_uSrc, 0, m_nbVoxels * sizeof (double));
  m_vSrc = (double *) memset (m_vSrc, 0, m_nbVoxels * sizeof (double));
  m_wSrc = (double *) memset (m_wSrc, 0, m_nbVoxels * sizeof (double));
}

void Solver::buildDLGrid ()
{
  double interx = m_dimX / (double) m_nbVoxelsX;
  double intery = m_dimY / (double) m_nbVoxelsY;
  double interz = m_dimZ / (double) m_nbVoxelsZ;
  double i, j;
  
  m_gridDisplayList=glGenLists(1);
  glNewList (m_gridDisplayList, GL_COMPILE);
  glPushMatrix ();
  glTranslatef (-m_dimX / 2.0, 0, m_dimZ / 2.0);
  glBegin (GL_LINES);

  glColor4f (0.5, 0.5, 0.5, 0.5);

  for (j = 0.0; j <= m_dimZ; j += interz)
    {
      for (i = 0.0; i <= m_dimX + interx / 2; i += interx)
	{
	  glVertex3d (i, 0.0, -j);
	  glVertex3d (i, m_dimY, -j);
	}
      for (i = 0.0; i <= m_dimY + intery / 2; i += intery)
	{
	  glVertex3d (0.0, i, -j);
	  glVertex3d (m_dimX, i, -j);
	}
    }
  glEnd ();
  glPopMatrix ();
  glEndList ();
}

void Solver::buildDLBase ()
{
  double interx = m_dimX / (double) m_nbVoxelsX;
  double interz = m_dimZ / (double) m_nbVoxelsZ;
  double i;

  m_baseDisplayList=glGenLists(1);
  glNewList (m_baseDisplayList, GL_COMPILE);
  glPushMatrix ();
  glTranslatef (-m_dimX / 2.0, 0.0, m_dimZ / 2.0);
  glBegin (GL_LINES);

  glLineWidth (1.0);
  glColor4f (0.5, 0.5, 0.5, 0.5);
  for (i = 0.0; i <= m_dimX + interx / 2; i += interx)
    {
      glVertex3d (i, 0.0, -m_dimZ);
      glVertex3d (i, 0.0, 0.0);
    }
  for (i = 0.0; i <= m_dimZ + interz / 2; i += interz)
    {
      glVertex3d (0.0, 0.0, i - m_dimZ);
      glVertex3d (m_dimX, 0.0, i - m_dimZ);
    }
  glEnd ();
  glPopMatrix ();
  glEndList ();
}

void Solver::displayVelocityField (void)
{
  double inc_x = m_dimX / (double) m_nbVoxelsX;
  double inc_y = m_dimY / (double) m_nbVoxelsY;
  double inc_z = m_dimZ / (double) m_nbVoxelsZ;
  
  for (int i = 1; i <= m_nbVoxelsX; i++)
    {
      for (int j = 1; j <= m_nbVoxelsY; j++)
	{
	  for (int k = 1; k <= m_nbVoxelsZ; k++)
	    {
	      Vector vect;
	      /* Affichage du champ de vélocité */
	      glPushMatrix ();
	      glTranslatef (inc_x * i - inc_x / 2.0 - m_dimX / 2.0,
			    inc_y * j - inc_y / 2.0, 
			    inc_z * k - inc_z / 2.0 - m_dimZ / 2.0);
	      //    printf("vélocité %d %d %d %f %f %f\n",i,j,k,getU(i,j,k)],getV(i,j,k),getW(i,j,k));
	      //SDL_mutexP (lock);
	      vect.x = getU (i, j, k);
	      vect.y = getV (i, j, k);
	      vect.z = getW (i, j, k);
	      //SDL_mutexV (lock);
	      displayArrow (&vect);
	      glPopMatrix ();
	    }
	}
    }
}

void Solver::displayArrow (Vector * const direction)
{
  double norme_vel = sqrt (direction->x * direction->x +
			   direction->y * direction->z +
			   direction->z * direction->z);
  double taille = m_dimX * m_dimY * m_dimZ * norme_vel / 2.5;
  double angle;
  Vector axeRot, axeCone (0.0, 0.0, 1.0);

  direction->normalize ();

  /* On obtient un vecteur perpendiculaire au plan dÃ©fini par l'axe du cÃ´ne et la direction souhaitÃ©e */
  axeRot = axeCone ^ *direction;

  /* On rÃ©cupÃ¨re l'angle de rotation entre les deux vecteurs */
  angle = acos (axeCone * *direction);

  glRotatef (angle * RAD_TO_DEG, axeRot.x, axeRot.y, axeRot.z);
  /***********************************************************************************/

  /* DÃ©gradÃ© de couleur bleu vers rouge */
  /* ProblÃ¨me : on ne connaÃ®t pas l'Ã©chelle des valeurs */
  /* On va donc tenter de prendre une valeur max suffisamment grande */
  /* pour pouvoir discerner au mieux les variations de la vÃ©locitÃ© */

//  printf("%f\n",norme_vel);
  glColor4f (norme_vel / VELOCITE_MAX, 0.0, (VELOCITE_MAX - norme_vel) / VELOCITE_MAX, 0.75);

  GraphicsFn::SolidCone (taille / 4, taille, 3, 3);
}


void Solver::moveTo(Point& position)
{
  int i,j;
  Point move = position - m_position;
  double strength=1.5;
  
  m_position=position;

  /* Ajouter des forces externes */
  if(move.x)
    if( move.x > 0)
      for (i = -m_nbVoxelsZ / 4 - 1; i <= m_nbVoxelsZ / 4 + 1; i++)
	for (j = -m_nbVoxelsY / 4 - 1; j <= m_nbVoxelsY / 4 + 1; j++)
	  addUsrc (m_nbVoxelsX - 1,
		   ((int) (ceil (m_nbVoxelsY / 2.0))) + j,
		   ((int) (ceil (m_nbVoxelsZ / 2.0))) + i, -strength);
    else
      for (i = -m_nbVoxelsZ / 4 - 1; i <= m_nbVoxelsZ / 4 + 1; i++)
	for (j = -m_nbVoxelsY / 4 - 1; j <= m_nbVoxelsY / 4 + 1; j++)
	  addUsrc (2,
		   ((int) (ceil (m_nbVoxelsY / 2.0))) + j,
		   ((int) (ceil (m_nbVoxelsZ / 2.0))) + i, strength);  
  if(move.y)
    if( move.y > 0)
      for (i = -m_nbVoxelsX / 4 - 1; i <= m_nbVoxelsX / 4 + 1; i++)
	for (j = -m_nbVoxelsZ / 4 - 1; j < m_nbVoxelsZ / 4 + 1; j++)
	  addVsrc (((int) (ceil (m_nbVoxelsX / 2.0))) + i,
		   m_nbVoxelsY - 1,
		   ((int) (ceil (m_nbVoxelsZ / 2.0))) + j, -strength);
    else
      for (i = -m_nbVoxelsX / 4 - 1; i <= m_nbVoxelsX / 4 + 1; i++)
	for (j = -m_nbVoxelsZ / 4 - 1; j <= m_nbVoxelsZ / 4 + 1; j++)
	  addVsrc (((int) (ceil (m_nbVoxelsX / 2.0))) + i, 
		   2,
		   ((int) (ceil (m_nbVoxelsZ / 2.0))) + j, strength/10.0);
  if(move.z)
    if( move.z > 0)
      for (i = -m_nbVoxelsX / 4 - 1; i <= m_nbVoxelsX / 4 + 1; i++)
	for (j = -m_nbVoxelsY / 4 - 1; j <= m_nbVoxelsY / 4 - 1; j++)
	  addWsrc (((int) (ceil (m_nbVoxelsX / 2.0))) + i,
		   ((int) (ceil (m_nbVoxelsY / 2.0))) + j,
		   m_nbVoxelsZ - 1, -strength);
    else
      for (i = -m_nbVoxelsX / 4 - 1; i <= m_nbVoxelsX / 4 + 1; i++)
	for (j = -m_nbVoxelsY / 4 - 1; j <= m_nbVoxelsY / 4 - 1; j++)
	  addWsrc (((int) (ceil (m_nbVoxelsX / 2.0))) + i,
		   ((int) (ceil (m_nbVoxelsY / 2.0))) + j,
		   2, strength);
}

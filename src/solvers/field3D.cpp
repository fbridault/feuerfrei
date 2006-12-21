#include "field3D.hpp"

#include <math.h>
#include "../scene/graphicsFn.hpp"

Field3D::Field3D ()
{
}

Field3D::Field3D (Point& position, uint n_x, uint n_y, uint n_z, double dim, double timeStep, double buoyancy) : 
  Field(position, timeStep, buoyancy)
{
  m_nbVoxelsX = n_x;
  m_nbVoxelsY = n_y;
  m_nbVoxelsZ = n_z;
  
  /* Détermination de la taille du solveur de manière à ce que le plus grand côté soit de dimension dim */
  if (m_nbVoxelsX > m_nbVoxelsY){
    if (m_nbVoxelsX > m_nbVoxelsZ){
      m_dim.x = dim;
      m_dim.y = m_dim.x * m_nbVoxelsY / m_nbVoxelsX;
      m_dim.z = m_dim.x * m_nbVoxelsZ / m_nbVoxelsX;
    }else{
      m_dim.z = dim;
      m_dim.x = m_dim.z * m_nbVoxelsX / m_nbVoxelsZ;
      m_dim.y = m_dim.z * m_nbVoxelsY / m_nbVoxelsZ;
    }
  }else{
    if (m_nbVoxelsY > m_nbVoxelsZ){
      m_dim.y = dim;
      m_dim.x = m_dim.y * m_nbVoxelsX / m_nbVoxelsY;
      m_dim.z = m_dim.y * m_nbVoxelsZ / m_nbVoxelsY;
    }else{
      m_dim.z = dim;
      m_dim.x = m_dim.z * m_nbVoxelsX / m_nbVoxelsZ;
      m_dim.y = m_dim.z * m_nbVoxelsY / m_nbVoxelsZ;
    }
  }
  
  m_nbVoxels = (m_nbVoxelsX + 2) * (m_nbVoxelsY + 2) * (m_nbVoxelsZ + 2);

  m_halfNbVoxelsX = m_nbVoxelsX/2;
  m_halfNbVoxelsZ = m_nbVoxelsZ/2;
  
  m_u = new double[m_nbVoxels];
  m_v = new double[m_nbVoxels];
  m_w = new double[m_nbVoxels];
  m_uSrc = new double[m_nbVoxels];
  m_vSrc = new double[m_nbVoxels];
  m_wSrc = new double[m_nbVoxels];
  
  memset (m_u, 0, m_nbVoxels * sizeof (double));
  memset (m_v, 0, m_nbVoxels * sizeof (double));
  memset (m_w, 0, m_nbVoxels * sizeof (double));
  memset (m_uSrc, 0, m_nbVoxels * sizeof (double));
  memset (m_vSrc, 0, m_nbVoxels * sizeof (double));
  memset (m_wSrc, 0, m_nbVoxels * sizeof (double));
    
  /* Construction des display lists */
  buildDLBase ();
  buildDLGrid ();
}

Field3D::~Field3D ()
{
  delete[]m_u;
  delete[]m_v;
  delete[]m_w;

  delete[]m_uSrc;
  delete[]m_vSrc;
  delete[]m_wSrc;
}

void Field3D::vel_step ()
{
  add_source (m_u, m_uSrc);
  add_source (m_v, m_vSrc);
  add_source (m_w, m_wSrc);
}

void Field3D::iterate ()
{ 
  double tmp;
  m_u = (double *) memset (m_u, 0, m_nbVoxels * sizeof (double));
  m_v = (double *) memset (m_v, 0, m_nbVoxels * sizeof (double));
  m_w = (double *) memset (m_w, 0, m_nbVoxels * sizeof (double));
  /* Cellule(s) génératrice(s) */
  for (uint i = 1; i < m_nbVoxelsX + 1; i++)
    for (uint j = 1; j < m_nbVoxelsY + 1; j++)
      for (uint k = 1; k < m_nbVoxelsZ + 1; k++){
	tmp = m_buoyancy * j/(double)m_nbVoxelsY;
	addVsrc( i, j, k, tmp );
      }
  if(arePermanentExternalForces)
    addExternalForces(permanentExternalForces,false);
  
  vel_step ();

  m_nbIter++;

  cleanSources ();
}

void Field3D::cleanSources ()
{
  m_uSrc = (double *) memset (m_uSrc, 0, m_nbVoxels * sizeof (double));
  m_vSrc = (double *) memset (m_vSrc, 0, m_nbVoxels * sizeof (double));
  m_wSrc = (double *) memset (m_wSrc, 0, m_nbVoxels * sizeof (double));
}

void Field3D::buildDLGrid ()
{
  double interx = m_dim.x / (double) m_nbVoxelsX;
  double intery = m_dim.y / (double) m_nbVoxelsY;
  double interz = m_dim.z / (double) m_nbVoxelsZ;
  double i, j;
  
  m_gridDisplayList=glGenLists(1);
  glNewList (m_gridDisplayList, GL_COMPILE);
  glPushMatrix ();
  glTranslatef (-m_dim.x / 2.0, 0, m_dim.z / 2.0);
  glBegin (GL_LINES);
  
  glColor4f (0.5, 0.5, 0.5, 0.5);
  
  for (j = 0.0; j <= m_dim.z; j += interz)
    {
      for (i = 0.0; i <= m_dim.x + interx / 2; i += interx)
	{
	  glVertex3d (i, 0.0, -j);
	  glVertex3d (i, m_dim.y, -j);
	}
      for (i = 0.0; i <= m_dim.y + intery / 2; i += intery)
	{
	  glVertex3d (0.0, i, -j);
	  glVertex3d (m_dim.x, i, -j);
	}
    }
  glEnd ();
  glPopMatrix ();
  glEndList ();
}

void Field3D::buildDLBase ()
{
  double interx = m_dim.x / (double) m_nbVoxelsX;
  double interz = m_dim.z / (double) m_nbVoxelsZ;
  double i;
  
  m_baseDisplayList=glGenLists(1);
  glNewList (m_baseDisplayList, GL_COMPILE);
  glPushMatrix ();
  glTranslatef (-m_dim.x / 2.0, 0.0, m_dim.z / 2.0);
  glBegin (GL_LINES);
  
  glLineWidth (1.0);
  glColor4f (0.5, 0.5, 0.5, 0.5);
  for (i = 0.0; i <= m_dim.x + interx / 2; i += interx)
    {
      glVertex3d (i, 0.0, -m_dim.z);
      glVertex3d (i, 0.0, 0.0);
    }
  for (i = 0.0; i <= m_dim.z + interz / 2; i += interz)
    {
      glVertex3d (0.0, 0.0, i - m_dim.z);
      glVertex3d (m_dim.x, 0.0, i - m_dim.z);
    }
  glEnd ();
  glPopMatrix ();
  glEndList ();
}

void Field3D::displayVelocityField (void)
{
  double inc_x = m_dim.x / (double) m_nbVoxelsX;
  double inc_y = m_dim.y / (double) m_nbVoxelsY;
  double inc_z = m_dim.z / (double) m_nbVoxelsZ;
  
  for (uint i = 1; i <= m_nbVoxelsX; i++)
    {
      for (uint j = 1; j <= m_nbVoxelsY; j++)
	{
	  for (uint k = 1; k <= m_nbVoxelsZ; k++)
	    {
	      Vector vect;
	      /* Affichage du champ de vélocité */
	      glPushMatrix ();
	      glTranslatef (inc_x * i - inc_x / 2.0 - m_dim.x / 2.0,
			    inc_y * j - inc_y / 2.0, 
			    inc_z * k - inc_z / 2.0 - m_dim.z / 2.0);
	      //SDL_mutexP (lock);
	      vect = getUVW (i, j, k);
	      //SDL_mutexV (lock);
	      displayArrow (vect);
	      glPopMatrix ();
	    }
	}
    }
}

void Field3D::displayArrow (Vector& direction)
{
  double norme_vel = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;
  double taille = m_dim.x * m_dim.y * m_dim.z * norme_vel * m_forceRatio;
  double angle;
  Vector axeRot, axeCone (0.0, 0.0, 1.0);
  
  direction.normalize ();
  
  /* On obtient un vecteur perpendiculaire au plan défini par l'axe du cône et la direction souhaitée */
  axeRot = axeCone ^ direction;
  
  /* On récupère l'angle de rotation entre les deux vecteurs */
  angle = acos (axeCone * direction);
  
  glRotatef (angle * RAD_TO_DEG, axeRot.x, axeRot.y, axeRot.z);
  /***********************************************************************************/
  
  /* Dégradé de couleur bleu vers rouge */
  /* Problème : on ne connaît pas l'échelle des valeurs */
  /* On va donc tenter de prendre une valeur max suffisamment grande */
  /* pour pouvoir discerner au mieux les variations de la vélocité */
  
//  printf("%f\n",norme_vel);
  glColor4f (norme_vel / VELOCITE_MAX, 0.0, (VELOCITE_MAX - norme_vel) / VELOCITE_MAX, 0.75);
  
  GraphicsFn::SolidCone (taille / 4, taille, 3, 3);
}

void Field3D::addExternalForces(Point& position, bool move)
{
  uint i,j;
  Point strength;
  Point force;
  
  if(move){
    force = position - m_position;
    strength.x = strength.y = strength.z = .001;
    m_position=position;
  }else{
    force = position;
    strength = position * .1;
  }
  
  /* Ajouter des forces externes */
  if(force.x)
      for (uint i = 1; i < m_nbVoxelsX + 1; i++)
	for (uint j = 1; j < m_nbVoxelsY + 1; j++)
	  for (uint k = 1; k < m_nbVoxelsZ + 1; k++)
	    addUsrc (i, j, k, strength.x*j/(double)m_nbVoxelsY);
  if(force.y)
      for (uint i = 1; i < m_nbVoxelsX + 1; i++)
	for (uint j = 1; j < m_nbVoxelsY + 1; j++)
	  for (uint k = 1; k < m_nbVoxelsZ + 1; k++)
	    addVsrc (i, j, k, strength.y);
  if(force.z)
      for (uint i = 1; i < m_nbVoxelsX + 1; i++)
	for (uint j = 1; j < m_nbVoxelsY + 1; j++)
	  for (uint k = 1; k < m_nbVoxelsZ + 1; k++)
	    addWsrc (i, j, k, strength.z*j/(double)m_nbVoxelsY);

//   /* Ajouter des forces externes */
//   if(force.x)
//     if( force.x > 0)
//       for (i = ceilz; i <= widthz; i++)
// 	for (j = ceily; j <= widthy; j++)
// 	  addUsrc (m_nbVoxelsX, j, i, -strength.x);
//     else
//       for (i = ceilz; i <= widthz; i++)
// 	for (j = ceily; j <= widthy; j++)
// 	  addUsrc (1, j, i, strength.x); 
//   if(force.y)
//     if( force.y > 0)
//       for (i = ceilx; i <= widthx; i++)
// 	for (j = ceilz; j < widthz; j++)
// 	  addVsrc (i, m_nbVoxelsY, j, -strength.y/10.0);
//     else
//       for (i = ceilx; i <= widthx; i++)
// 	for (j = ceilz; j <= widthz; j++)
// 	  addVsrc (i, 1, j, strength.y/10.0);
//   if(force.z)
//     if( force.z > 0)
//       for (i = ceilx; i <= widthx; i++)
// 	for (j = ceily; j <= widthy; j++)
// 	  addWsrc (i, j, m_nbVoxelsZ, -strength.z);
//     else
//       for (i = ceilx; i <= widthx; i++)
// 	for (j = ceily; j <= widthy; j++)
// 	  addWsrc (i, j, 1, strength.z);
}

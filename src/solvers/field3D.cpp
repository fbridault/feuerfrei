#include "field3D.hpp"

#include <math.h>
#include "../scene/graphicsFn.hpp"

Field3D::Field3D ()
{
}

Field3D::Field3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale, double timeStep, double buoyancy) : 
  Field(position, timeStep, buoyancy)
{
  m_nbVoxelsX = n_x;
  m_nbVoxelsY = n_y;
  m_nbVoxelsZ = n_z;
  
  /* D�termination de la taille du solveur de mani�re � ce que le plus grand c�t� soit de dimension dim */
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
  
  m_nbVoxelsXDivDimX = m_nbVoxelsX / m_dim.x;
  m_nbVoxelsYDivDimY = m_nbVoxelsY / m_dim.y;
  m_nbVoxelsZDivDimZ = m_nbVoxelsZ / m_dim.z;
  
  /* Construction des display lists */
  buildDLBase ();
  buildDLGrid ();
  
  m_scale = scale;
}

Field3D::~Field3D ()
{
}

void Field3D::buildDLGrid ()
{
  double interx = m_dim.x / (double) m_nbVoxelsX;
  double intery = m_dim.y / (double) m_nbVoxelsY;
  double interz = m_dim.z / (double) m_nbVoxelsZ;
  double i, j;
  
  m_gridDisplayList=glGenLists(1);
  glNewList (m_gridDisplayList, GL_COMPILE);
  glBegin (GL_LINES);
  
  glColor4f (0.5, 0.5, 0.5, 0.5);
  
  for (j = 0.0; j <= m_dim.z; j += interz)
    {
      for (i = 0.0; i <= m_dim.x + interx / 2; i += interx)
	{
	  glVertex3d (i, 0.0, j);
	  glVertex3d (i, m_dim.y, j);
	}
      for (i = 0.0; i <= m_dim.y + intery / 2; i += intery)
	{
	  glVertex3d (0.0, i, j);
	  glVertex3d (m_dim.x, i, j);
	}
    }
  glEnd ();
  glEndList ();
}

void Field3D::buildDLBase ()
{
  double interx = m_dim.x / (double) m_nbVoxelsX;
  double interz = m_dim.z / (double) m_nbVoxelsZ;
  double i;
  
  m_baseDisplayList=glGenLists(1);
  glNewList (m_baseDisplayList, GL_COMPILE);
  glBegin (GL_LINES);
  
  glLineWidth (1.0);
  glColor4f (0.5, 0.5, 0.5, 0.5);
  for (i = 0.0; i <= m_dim.x + interx / 2; i += interx)
    {
      glVertex3d (i, 0.0, 0.0);
      glVertex3d (i, 0.0, m_dim.z);
    }
  for (i = 0.0; i <= m_dim.z + interz / 2; i += interz)
    {
      glVertex3d (0.0, 0.0, i);
      glVertex3d (m_dim.x, 0.0, i);
    }
  glEnd ();
  glEndList ();
}

void Field3D::displayArrow (const Vector& direction)
{
  double norme_vel = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;
  double taille = m_dim.x * m_dim.y * m_dim.z * norme_vel * m_forceRatio;
  double angle;
  Vector axeRot, axeCone (0.0, 0.0, 1.0), dir(direction);
  
  dir.normalize ();
  
  /* On obtient un vecteur perpendiculaire au plan d�fini par l'axe du c�ne et la direction souhait�e */
  axeRot = axeCone ^ dir;
  
  /* On r�cup�re l'angle de rotation entre les deux vecteurs */
  angle = acos (axeCone * dir);
  
  glRotatef (angle * RAD_TO_DEG, axeRot.x, axeRot.y, axeRot.z);
  /***********************************************************************************/
  
  /* D�grad� de couleur bleu vers rouge */
  /* Probl�me : on ne conna�t pas l'�chelle des valeurs */
  /* On va donc tenter de prendre une valeur max suffisamment grande */
  /* pour pouvoir discerner au mieux les variations de la v�locit� */
  
//  printf("%f\n",norme_vel);
  glColor4f (norme_vel / VELOCITE_MAX, 0.0, (VELOCITE_MAX - norme_vel) / VELOCITE_MAX, 0.75);
  
  GraphicsFn::SolidCone (taille / 4, taille, 3, 3);
}
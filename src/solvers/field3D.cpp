#include "field3D.hpp"

#include <math.h>
#include <engine/Utility/GraphicsFn.hpp>
#include <engine/Common.hpp>

Field3D::Field3D ()
{
}

Field3D::Field3D (CTransform& a_rTransform, uint n_x, uint n_y, uint n_z, float dim, float timeStep, float buoyancy) :
  Field(a_rTransform, timeStep, buoyancy)
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

  m_nbVoxelsXDivDimX = m_nbVoxelsX / m_dim.x;
  m_nbVoxelsYDivDimY = m_nbVoxelsY / m_dim.y;
  m_nbVoxelsZDivDimZ = m_nbVoxelsZ / m_dim.z;

  /* Construction des display lists */
  buildDLBase ();

  m_nbMaxDiv = 1;
}

Field3D::~Field3D ()
{
#ifdef RTFLAMES_BUILD
  m_fireSources.clear();
#endif
}

void Field3D::buildDLGrid ()
{
  float interx = m_dim.x / (float) m_nbVoxelsX;
  float intery = m_dim.y / (float) m_nbVoxelsY;
  float interz = m_dim.z / (float) m_nbVoxelsZ;
  float i, j;

  m_gridDisplayList=glGenLists(1);
  glNewList (m_gridDisplayList, GL_COMPILE);
  glBegin (GL_LINES);

  glColor4f (0.5f, 0.5f, 0.5f, 0.5f);

  for (j = 0.0f; j <= m_dim.z; j += interz)
    {
      for (i = 0.0f; i <= m_dim.x + interx / 2; i += interx)
	{
	  glVertex3f (i, 0.0f, j);
	  glVertex3f (i, m_dim.y, j);
	}
      for (i = 0.0; i <= m_dim.y + intery / 2; i += intery)
	{
	  glVertex3f (0.0f, i, j);
	  glVertex3f (m_dim.x, i, j);
	}
    }
  glEnd ();
  glEndList ();
}

void Field3D::buildDLBase ()
{
  float interx = m_dim.x / (float) m_nbVoxelsX;
  float interz = m_dim.z / (float) m_nbVoxelsZ;
  float i;

  m_baseDisplayList=glGenLists(1);
  glNewList (m_baseDisplayList, GL_COMPILE);
  glBegin (GL_LINES);

  glLineWidth (1.0);
  glColor4f (0.5, 0.5, 0.5, 0.5);
  for (i = 0.0; i <= m_dim.x + interx / 2; i += interx)
    {
      glVertex3f (i, 0.0, 0.0);
      glVertex3f (i, 0.0, m_dim.z);
    }
  for (i = 0.0; i <= m_dim.z + interz / 2; i += interz)
    {
      glVertex3f (0.0, 0.0, i);
      glVertex3f (m_dim.x, 0.0, i);
    }
  glEnd ();
  glEndList ();
}

void Field3D::displayArrow (const CVector& direction)
{
  float norme_vel = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;
  float taille = norme_vel * m_forceRatio;
  float angle;
  CVector axeRot, axeCone (0.0, 0.0, 1.0), dir(direction);

  dir.normalize ();

  /* On obtient un vecteur perpendiculaire au plan défini par l'axe du cône et la direction souhaitée */
  axeRot = axeCone ^ dir;

  /* On récupère l'angle de rotation entre les deux vecteurs */
  angle = acos (axeCone * dir);

  glRotatef (angle * RAD_TO_DEG, axeRot.x, axeRot.y, axeRot.z);
  /***********************************************************************************/

  /* Dégradé de couleur bleu vers rouge */
  /* Problème : on ne connaît pas l'échelle des valeurs */
  /* On va donc tenter de prendre une valeur max suffisamment grande */
  /* pour pouvoir discerner au mieux les variations de la vélocité */

  //  printf("%f\n",norme_vel);
  glColor4f (norme_vel / VELOCITE_MAX, 0.0, (VELOCITE_MAX - norme_vel) / VELOCITE_MAX, 0.75);

  UGraphicsFn::SolidCone (taille/2.0f, taille, 3, 3);
}

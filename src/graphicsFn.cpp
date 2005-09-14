#include "graphicsFn.hpp"

Texture *tex1, *tex2, *tex3;

static GLUquadricObj *quadObj;

#define QUAD_OBJ_INIT() { if(!quadObj) initQuadObj(); }


static GLubyte fps[][13] = {
  {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0,
   0xff},
  {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7,
   0xfe},
  {0x00, 0x00, 0x7e, 0xe7, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0xe0, 0xc0, 0xe7,
   0x7e},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00,
   0x00}
};
static GLubyte digits[][13] = {
  {0x00, 0x00, 0x3c, 0x66, 0xc3, 0x81, 0x81, 0x81, 0x81, 0x81, 0xc3, 0x66,
   0x3c},
  {0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x63, 0x33, 0x1a, 0x0f, 0x07,
   0x03},
  {0x00, 0x00, 0x3f, 0xff, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x86, 0xc3, 0x63,
   0x3e},
  {0x00, 0x00, 0x3c, 0x66, 0xc3, 0x83, 0x06, 0x3c, 0x06, 0x83, 0xc3, 0x66,
   0x3c},
  {0x00, 0x00, 0x06, 0x06, 0x06, 0x06, 0x7f, 0xff, 0x66, 0x36, 0x1e, 0x0e,
   0x06},
  {0x00, 0x00, 0xfc, 0xfe, 0x03, 0x06, 0x7c, 0xf8, 0xc0, 0xc0, 0xc0, 0xff,
   0x7e},
  {0x00, 0x00, 0x7e, 0xc3, 0x81, 0x83, 0xfe, 0x60, 0x20, 0x30, 0x18, 0x0c,
   0x06},
  {0x00, 0x00, 0x40, 0x40, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x02, 0x83, 0xff,
   0xff},
  {0x00, 0x00, 0x7e, 0xc3, 0x81, 0x81, 0xc3, 0x7e, 0xc3, 0x81, 0x81, 0xc3,
   0x7e},
  {0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x7e, 0xc3, 0x81, 0x81, 0xc3,
   0x7e}
};
static GLuint fontOffset;

static void
initQuadObj (void)
{
  quadObj = gluNewQuadric ();
  if (!quadObj)
    cerr << ("out of memory.");
}

void
GraphicsFn::SolidSphere (GLdouble radius, GLint slices, GLint stacks)
{
  QUAD_OBJ_INIT ();
  gluQuadricDrawStyle (quadObj, GLU_FILL);
  gluQuadricNormals (quadObj, GLU_SMOOTH);
  /* If we ever changed/used the texture or orientation state
     of quadObj, we'd need to change it to the defaults here
     with gluQuadricTexture and/or gluQuadricOrientation. */
  gluSphere (quadObj, radius, slices, stacks);
}

void
GraphicsFn::SolidCone (GLdouble base, GLdouble height, GLint slices,
		       GLint stacks)
{
  QUAD_OBJ_INIT ();
  gluQuadricDrawStyle (quadObj, GLU_FILL);
  gluQuadricNormals (quadObj, GLU_SMOOTH);
  /* If we ever changed/used the texture or orientation state
     of quadObj, we'd need to change it to the defaults here
     with gluQuadricTexture and/or gluQuadricOrientation. */
  gluCylinder (quadObj, base, 0.0, height, slices, stacks);
}

void
GraphicsFn::SolidCylinder (GLdouble base, GLdouble height, GLint slices,
			   GLint stacks)
{
  QUAD_OBJ_INIT ();
  gluQuadricDrawStyle (quadObj, GLU_FILL);
  gluQuadricNormals (quadObj, GLU_SMOOTH);
  /* If we ever changed/used the texture or orientation state
     of quadObj, we'd need to change it to the defaults here
     with gluQuadricTexture and/or gluQuadricOrientation. */
  gluCylinder (quadObj, base, base, height, slices, stacks);
}

void
GraphicsFn::grille (int x, int y, int z)
{
  double interx = dim_x / (double) x;
  double intery = dim_y / (double) y;
  double interz = dim_z / (double) z;
  double i, j;

  glNewList (GRILLE, GL_COMPILE);
  glPushMatrix ();
  glTranslatef (-dim_x / 2.0, 0, dim_z / 2.0);
  glBegin (GL_LINES);

  glColor4f (0.5, 0.5, 0.5, 0.5);

  for (j = 0.0; j <= dim_z; j += interz)
    {
      for (i = 0.0; i <= dim_x + interx / 2; i += interx)
	{
	  glVertex3f (i, 0.0, -j);
	  glVertex3f (i, dim_y, -j);
	}
      for (i = 0.0; i <= dim_y + intery / 2; i += intery)
	{
	  glVertex3f (0.0, i, -j);
	  glVertex3f (dim_x, i, -j);
	}
    }
  glEnd ();
  glPopMatrix ();
  glEndList ();
}

void
GraphicsFn::repere (int x, int y, int z)
{
  double interx = dim_x / (double) x;
  double interz = dim_z / (double) z;
  double i;

  glNewList (REPERE, GL_COMPILE);
  glPushMatrix ();
  glTranslatef (-dim_x / 2.0, 0.0, dim_z / 2.0);
  glBegin (GL_LINES);

  glLineWidth (1.0);
  glColor4f (0.5, 0.5, 0.5, 0.5);
  for (i = 0.0; i <= dim_x + interx / 2; i += interx)
    {
      glVertex3f (i, 0.0, -dim_z);
      glVertex3f (i, 0.0, 0.0);
    }
  for (i = 0.0; i <= dim_z + interz / 2; i += interz)
    {
      glVertex3f (0.0, 0.0, i - dim_z);
      glVertex3f (dim_x, 0.0, i - dim_z);
    }
  glEnd ();
  glPopMatrix ();
  glEndList ();
}

void
GraphicsFn::fleche (CVector * const direction, int x, int y, int z)
{
  double norme_vel =
    sqrt (direction->getX () * direction->getX () +
	  direction->getY () * direction->getY () +
	  direction->getZ () * direction->getZ ());
  double taille = dim_x * dim_y * dim_z * norme_vel / 2.5;
  double angle;
  CVector axeRot, axeCone (0.0, 0.0, 1.0);

  direction->normalize ();

  /* On obtient un vecteur perpendiculaire au plan défini par l'axe du cône et la direction souhaitée */
  axeRot = axeCone ^ *direction;

  /* On récupère l'angle de rotation entre les deux vecteurs */
  angle = acos (axeCone * *direction);

  glRotatef (angle * RAD_TO_DEG, axeRot.getX (), axeRot.getY (),
	     axeRot.getZ ());
  /***********************************************************************************/

  /* Dégradé de couleur bleu vers rouge */
  /* Problème : on ne connaît pas l'échelle des valeurs */
  /* On va donc tenter de prendre une valeur max suffisamment grande */
  /* pour pouvoir discerner au mieux les variations de la vélocité */

//  printf("%f\n",norme_vel);
  glColor4f (norme_vel / VELOCITE_MAX, 0.0,
	     (VELOCITE_MAX - norme_vel) / VELOCITE_MAX, 0.75);

  GraphicsFn::SolidCone (taille / 4, taille, 3, 3);
}

void GraphicsFn::cylindre(float hauteur, float rayon, float slices )
{
  double angle = 2*PI/slices;
  double k;
  CPoint pts[4];
  CVector v1, v2, v3;
    
  GLfloat mat_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };

  glMaterialfv (GL_FRONT, GL_DIFFUSE, mat_diffuse);

//  glColor3f (0.9, 0.9, 0.9);
  glNewList(CYLINDRE,GL_COMPILE);
  for(k=0.0;k<(2*PI);k+=angle){
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(rayon*cos(k+angle),-2,rayon*sin(k+angle));
	glVertex3f(rayon*cos(k),-2,rayon*sin(k));
	glVertex3f(0,-2,0);
	glEnd();
	
	pts[0].setX(rayon*cos(k));
	pts[0].setY(-2);
	pts[0].setZ(rayon*sin(k));
	pts[1].setX(rayon*cos(k+angle));
	pts[1].setY(-2);
	pts[1].setZ(rayon*sin(k+angle));
	pts[2].setX(rayon*cos(k+angle));
	pts[2].setY(hauteur-2);
	pts[2].setZ(rayon*sin(k+angle));
	pts[3].setX(rayon*cos(k));
	pts[3].setY(hauteur-2);
	pts[3].setZ(rayon*sin(k));
	/* calcul de la normale */
	v1.setX(pts[1].getX() - pts[0].getX());
	v1.setY(pts[1].getY() - pts[0].getY());
	v1.setZ(pts[1].getZ() - pts[0].getZ());
	v2.setX(pts[2].getX() - pts[1].getX());
	v2.setY(pts[2].getY() - pts[1].getY());
	v2.setZ(pts[2].getZ() - pts[1].getZ());
	v3 = v2 ^ v1;
	//MathFn::normalisation(&v3); // Fait automatiquement
	
	glBegin(GL_QUADS);
	glNormal3f(v3.getX(), v3.getY(), v3.getZ());
	glVertex3f(rayon*cos(k),-2,rayon*sin(k));
	glNormal3f(v3.getX(), v3.getY(), v3.getZ());
	glVertex3f(rayon*cos(k),hauteur-2,rayon*sin(k));
	glNormal3f(v3.getX(), v3.getY(), v3.getZ());
	glVertex3f(rayon*cos(k+angle),hauteur-2,rayon*sin(k+angle));
	glNormal3f(v3.getX(), v3.getY(), v3.getZ());
	glVertex3f(rayon*cos(k+angle),-2,rayon*sin(k+angle));
	glEnd();
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(0,hauteur-2,0);
	glVertex3f(rayon*cos(k+angle),hauteur-2,rayon*sin(k+angle));
	glVertex3f(rayon*cos(k),hauteur-2,rayon*sin(k));
    glEnd();
  }
  glEndList();
}

void
GraphicsFn::makeRasterFont (void)
{
  GLuint i, j;
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

  fontOffset = glGenLists (10);
  for (i = 0, j = '0'; i < 10; i++, j++)
    {
      glNewList (fontOffset + j, GL_COMPILE);
      glBitmap (8, 13, 0.0, 2.0, 10.0, 0.0, digits[i]);
      glEndList ();
    }
  for (i = 0; i < 4; i++)
    {
      glNewList (fontOffset + 10 + i, GL_COMPILE);
      glBitmap (8, 13, 0.0, 2.0, 10.0, 0.0, fps[i]);
      glEndList ();
    }
}

// static void GraphicsFn::init_font(void)
// {
//   glShadeModel(GL_FLAT);
//   makeRasterFont();
// }

void
GraphicsFn::printString (char *s, int x, int y)
{
  int i;

  glDisable (GL_LIGHTING);
  glColor3f (1.0, 1.0, 1.0);
  glRasterPos2i (x, y);

  for (i = 0; i < 4; i++)
    glCallList (fontOffset + 10 + i);

  glPushAttrib (GL_LIST_BIT);
  glListBase (fontOffset);
  glCallLists (strlen (s), GL_UNSIGNED_BYTE, (GLubyte *) s);
  glPopAttrib ();
  glEnable (GL_LIGHTING);
}

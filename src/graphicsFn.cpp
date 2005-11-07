#include "graphicsFn.hpp"

static GLUquadricObj *quadObj;

#define QUAD_OBJ_INIT() { if(!quadObj) initQuadObj(); }

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
GraphicsFn::SolidDisk (GLdouble rayon, GLint slices, GLint loops)
{
  QUAD_OBJ_INIT ();
  gluQuadricDrawStyle (quadObj, GLU_FILL);
  gluQuadricNormals (quadObj, GLU_SMOOTH);
  /* If we ever changed/used the texture or orientation state
     of quadObj, we'd need to change it to the defaults here
     with gluQuadricTexture and/or gluQuadricOrientation. */
  gluDisk (quadObj, 0, rayon, slices, loops);
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

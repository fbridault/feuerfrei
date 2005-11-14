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

void GraphicsFn::cylindre(double hauteur, double rayon, double slices )
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
	glVertex3d(rayon*cos(k+angle),-2,rayon*sin(k+angle));
	glVertex3d(rayon*cos(k),-2,rayon*sin(k));
	glVertex3d(0,-2,0);
	glEnd();
	
	pts[0].x = rayon*cos(k);
	pts[0].y = -2;
	pts[0].z = rayon*sin(k);
	pts[1].x = rayon*cos(k+angle);
	pts[1].y = -2;
	pts[1].z = rayon*sin(k+angle);
	pts[2].x = rayon*cos(k+angle);
	pts[2].y = hauteur-2;
	pts[2].z = rayon*sin(k+angle);
	pts[3].x = rayon*cos(k);
	pts[3].y = hauteur-2;
	pts[3].z = rayon*sin(k);
	/* calcul de la normale */
	v1.x = pts[1].x - pts[0].x;
	v1.y = pts[1].y - pts[0].y;
	v1.z = pts[1].z - pts[0].z;
	v2.x = pts[2].x - pts[1].x;
	v2.y = pts[2].y - pts[1].y;
	v2.z = pts[2].z - pts[1].z;
	v3 = v2 ^ v1;
	//MathFn::normalisation(&v3); // Fait automatiquement
	
	glBegin(GL_QUADS);
	glNormal3f(v3.x, v3.y, v3.z);
	glVertex3d(rayon*cos(k),-2,rayon*sin(k));
	glNormal3f(v3.x, v3.y, v3.z);
	glVertex3d(rayon*cos(k),hauteur-2,rayon*sin(k));
	glNormal3f(v3.x, v3.y, v3.z);
	glVertex3d(rayon*cos(k+angle),hauteur-2,rayon*sin(k+angle));
	glNormal3f(v3.x, v3.y, v3.z);
	glVertex3d(rayon*cos(k+angle),-2,rayon*sin(k+angle));
	glEnd();
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3d(0,hauteur-2,0);
	glVertex3d(rayon*cos(k+angle),hauteur-2,rayon*sin(k+angle));
	glVertex3d(rayon*cos(k),hauteur-2,rayon*sin(k));
    glEnd();
  }
  glEndList();
}

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

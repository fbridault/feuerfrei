#if !defined(GRAPHICSFN_H)
#define GRAPHICSFN_H

#include <GL/gl.h>
#include <GL/glu.h>

#include "texture.hpp"

/** Ensemble de fonctions statiques utilis�es pour le dessin de la sc�ne.
 *
 * @author	Flavien Bridault
 */

class GraphicsFn
{
public:
  static void SolidSphere(GLdouble radius, GLint slices, GLint stacks);
  static void SolidCylinder(GLdouble base, GLdouble height, GLint slices, GLint stacks);
  static void SolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
  static void SolidDisk (GLdouble rayon, GLint slices, GLint loops);
};

#endif

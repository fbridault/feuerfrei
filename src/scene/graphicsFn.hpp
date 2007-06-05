#if !defined(GRAPHICSFN_H)
#define GRAPHICSFN_H

#include <GL/gl.h>
#include <GL/glu.h>

#include "../vector.hpp"

/** Ensemble de fonctions statiques utilisées pour le dessin de la scène.
 *
 * @author	Flavien Bridault
 */

class GraphicsFn
{
public:
  static void SolidSphere(GLfloat radius, GLint slices, GLint stacks);
  static void SolidCylinder(GLfloat base, GLfloat height, GLint slices, GLint stacks);
  static void SolidCone(GLfloat base, GLfloat height, GLint slices, GLint stacks);
  static void SolidDisk (GLfloat rayon, GLint slices, GLint loops);
  static void SolidBox (const Point& ptMin, const Point& ptMax);
};

#endif

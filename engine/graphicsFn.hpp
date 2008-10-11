#if !defined(GRAPHICSFN_H)
#define GRAPHICSFN_H

#include <GL/gl.h>
#include <GL/glu.h>

#include "pointVector.hpp"

/** Ensemble de fonctions statiques utilisées pour le dessin.
 *
 * @author	Flavien Bridault
 */

class CGraphicsFn
{
public:
	static void SolidSphere(GLfloat radius, GLint slices, GLint stacks);
	static void SolidCylinder(GLfloat base, GLfloat height, GLint slices, GLint stacks);
	static void SolidCone(GLfloat base, GLfloat height, GLint slices, GLint stacks);
	static void SolidDisk (GLfloat rayon, GLint slices, GLint loops);
	static void SolidBox (const CPoint& ptMin, const CPoint& ptMax);
};

#endif

#if !defined(GRAPHICSFN_H)
#define GRAPHICSFN_H

#include <GL/gl.h>
#include <GL/glu.h>

class CPoint;

/** Ensemble de fonctions statiques utilisées pour le dessin.
 *
 * @author	Flavien Bridault
 */

class UGraphicsFn
{
private:
	UGraphicsFn() {};

public:
	static void SolidSphere(GLfloat radius, GLint slices, GLint stacks);
	static void SolidCylinder(GLfloat base, GLfloat height, GLint slices, GLint stacks);
	static void SolidCone(GLfloat base, GLfloat height, GLint slices, GLint stacks);
	static void SolidDisk (GLfloat rayon, GLint slices, GLint loops);
	static void SolidBox (const CPoint& ptMin, const CPoint& ptMax);
//	static void renderBitmapString(float x, float y, float height, void *font, const char *string);
};

#endif

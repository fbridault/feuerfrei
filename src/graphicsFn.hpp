#if !defined(GRAPHICSFN_H)
#define GRAPHICSFN_H

#include "header.h"

#include "texture.hpp"

/** Ensemble de fonctions statiques utilisées pour le dessin de la scène.
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
  
  /** Construit une display list pour dessiner un cylindre.
  * @param hauteur hauteur du cylindre
  * @param rayon rayon du cylindre
  * @param slices nombre de segments du cylindre
  */
  static void cylindre(double hauteur, double rayon, double slices );
};

#endif

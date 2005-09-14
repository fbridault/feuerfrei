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
  
  /** Construit une display list pour dessiner un cylindre.
  * @param hauteur hauteur du cylindre
  * @param rayon rayon du cylindre
  * @param slices nombre de segments du cylindre
  */
  static void cylindre(float hauteur, float rayon, float slices );
  
  /** Construit une display list pour dessiner le repère du solveur. */
  static void repere(int x, int y, int z);
  
  /** Construit une display list pour dessiner le repère de la grille. */
  static void grille(int x, int y, int z);
  
  /** Dessine une flèche de vélocité dans le solveur de fluide.
  * @param direction vecteur contenant la vélocité à afficher
  */
  static void fleche( CVector* const direction,int x, int y, int z);

  static void makeRasterFont(void);
  static void printString(char *s,int x,int y);
};

#endif

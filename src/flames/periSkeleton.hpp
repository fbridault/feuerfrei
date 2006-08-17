#if !defined(PERISKELETON_H)
#define PERISKELETON_H

class PeriSkeleton;
class LeadSkeleton;

#include "skeleton.hpp"

class Skeleton;

/** Classe représentant les squelettes périphériques, voir la classe Skeleton pour plus de
 * détails. Un squelette périphérique se réfère à un squelette guide relatif pour la
 * construction des surfaces NURBS.
 *
 * @author	Flavien Bridault
 */
class PeriSkeleton : public Skeleton
{
public:
  /** Constructeur de squelette périphérique
   * @param s pointeur sur le solveur de fluides
   * @param position position de la flamme dans l'espace
   * @param pt position de l'origine du squelette
   * @param pls durée de vie initiale d'une particule
   * @param guide pointeur sur le squelette guide associé (généralement le plus proche,
   * dans le cas de la bougie simple, le problème est simple puisqu'il n'y en a qu'un seul)
   */
  PeriSkeleton(Solver* const s, const Point& position, const Point& rootMoveFactor, 
	       LeadSkeleton *guide, uint *pls);
  virtual ~PeriSkeleton();
  
  /** Retourne un pointeur sur le squelette guide relatif */
  LeadSkeleton* getLeadSkeleton(){return guide;};

private:  
  /** Pointeur sur le squelette guide associé (généralement le plus proche,
   * dans le cas de la bougie simple, le problème est simple puisqu'il n'y en a qu'un seul)
   */
  LeadSkeleton *guide;
};

#endif

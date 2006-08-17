#if !defined(LEADSKELETON_H)
#define LEADSKELETON_H

class LeadSkeleton;

#include "skeleton.hpp"

class Skeleton;

/** Classe repr�sentant les squelettes guides, voir la classe Skeleton pour plus de
 * d�tails.
 *
 * @author	Flavien Bridault
 */
class LeadSkeleton : public Skeleton
{
public:
  /** Constructeur de squelette guide
   * @param s pointeur sur le solveur de fluides
   * @param position position de la flamme dans l'espace
   * @param pt position de l'origine du squelette
   * @param pls dur�e de vie initiale d'une particule
   */
  LeadSkeleton(Solver* const s, const Point& position, const Point& rootMoveFactor, uint *pls);

  virtual ~LeadSkeleton();
  
private:

  /** Dessine une particule d'un squelette guide
   * @param particle particule � dessiner
   */
  void drawParticle (Particle * const particle);
};

#endif

#if !defined(LEADSKELETON_H)
#define LEADSKELETON_H

class LeadSkeleton;
class FreeLeadSkeleton;
class FreePeriSkeleton;

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
  
  /** M�thode de s�paration d'un squelette
   * @param splitHeight hauteur de la d�coupe
   */
  FreeLeadSkeleton* split (uint splitHeight);
  
private:
  /** Dessine une particule d'un squelette guide
   * @param particle particule � dessiner
   */
  void drawParticle (Particle * const particle);
};

/** Classe repr�sentant les squelettes guides libres.
 *
 * @author	Flavien Bridault
 */
class FreeLeadSkeleton : public FreeSkeleton
{
public:
  /** Constructeur de squelette p�riph�rique
   * @param src pointeur sur le squelette p�riph�rique initial
   * @param splitHeight hauteur de d�coupe
   *
   */
  FreeLeadSkeleton(const LeadSkeleton* const src, uint splitHeight);
  virtual ~FreeLeadSkeleton();

  /** Duplique un squelette
   * @param offset valeur du d�calage dans l'espace du squelette
   */
  virtual FreePeriSkeleton* dup(Point& offset);
private:  
  /** Dessine une particule d'un squelette guide
   * @param particle particule � dessiner
   */
  void drawParticle (Particle * const particle);
};

#endif

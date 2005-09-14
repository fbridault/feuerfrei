#if !defined(LEADSKELETON_H)
#define LEADSKELETON_H

class LeadSkeleton;

#include "skeleton.hpp"

class Skeleton;

/** Classe représentant les squelettes guides, voir la classe Skeleton pour plus de
 * détails.
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
   */
  LeadSkeleton(Solver* const s, const CPoint position, const CPoint pt);

  /** Constructeur de squelette guide
   * @param s pointeur sur le solveur de fluides
   * @param position position de la flamme dans l'espace
   * @param pt position de l'origine du squelette
   * @param pls durée de vie initiale d'une particule
   */
  LeadSkeleton(Solver* const s, const CPoint position, const CPoint pt, int pls);

  virtual ~LeadSkeleton();
  
  /** Déplacement d'une particule
   * @param displayParticle booléen indiquant si les particules doivent être affichées à l'écran
  */
  void move(bool displayParticle);

private:
  /** Déplace une particule dans le champ de vélocité.
   * @param pos position de la particule
   * @param displayParticle booléen indiquant si les particules doivent être affichées à l'écran
   */
  int move_particle(Particle* const pos, bool displayParticle);

  /** Déplace l'origine du squelette dans le champ de vélocité.
   * @param displayParticle booléen indiquant si les particules doivent être affichées à l'écran
   */
  int move_origine(bool displayParticle);
};

#endif

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
  LeadSkeleton(Solver* const s, const Point& position, const Point& rootMoveFactor, int pls);

  virtual ~LeadSkeleton();
  
  /** D�placement d'une particule
   * @param displayParticle bool�en indiquant si les particules doivent �tre affich�es � l'�cran
  */
  void move();

private:
  /** D�place une particule dans le champ de v�locit�.
   * @param pos position de la particule
   * @param displayParticle bool�en indiquant si les particules doivent �tre affich�es � l'�cran
   */
  int moveParticle(Particle* const pos);

  /** D�place l'origine du squelette dans le champ de v�locit�.
   * @param displayParticle bool�en indiquant si les particules doivent �tre affich�es � l'�cran
   */
  int moveRoot();

  void drawParticle (Particle * const particle);
};

#endif
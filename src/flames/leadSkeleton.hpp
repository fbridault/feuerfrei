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
  /** Constructeur de squelette guide.
   * @param s Pointeur sur le solveur de fluides.
   * @param position Position de la flamme dans l'espace.
   * @param rootMoveFactor Amplitude du d�placement autoris� pour l'origine du squelette. Varie
   * en fonction du type de flamme.
   * @param pls Dur�e de vie initiale d'une particule.
   */
  LeadSkeleton(Solver3D* const s, const Point& position, const Point& rootMoveFactor, uint *pls);

  /** Destructeur. */
  virtual ~LeadSkeleton();
  
  /** M�thode de s�paration d'un squelette
   * @param splitHeight hauteur de la d�coupe
   */
  FreeLeadSkeleton* split (uint splitHeight);
  
  virtual void drawRoot ();
private:
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

  /** Duplique un squelette.
   * @param offset Valeur du d�calage dans l'espace du squelette par rapport au squelette courant.
   */
  virtual FreePeriSkeleton* dup(Point& offset);
  
private:  
  /** Dessine une particule d'un squelette guide.
   * @param particle Particule � dessiner.
   */
  void drawParticle (Particle * const particle);
};

#endif

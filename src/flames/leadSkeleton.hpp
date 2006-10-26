#if !defined(LEADSKELETON_H)
#define LEADSKELETON_H

class LeadSkeleton;
class FreeLeadSkeleton;
class FreePeriSkeleton;

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
  /** Constructeur de squelette guide.
   * @param s Pointeur sur le solveur de fluides.
   * @param position Position de la flamme dans l'espace.
   * @param rootMoveFactor Amplitude du déplacement autorisé pour l'origine du squelette. Varie
   * en fonction du type de flamme.
   * @param pls Durée de vie initiale d'une particule.
   */
  LeadSkeleton(Solver3D* const s, const Point& position, const Point& rootMoveFactor, uint *pls);

  /** Destructeur. */
  virtual ~LeadSkeleton();
  
  /** Méthode de séparation d'un squelette
   * @param splitHeight hauteur de la découpe
   */
  FreeLeadSkeleton* split (uint splitHeight);
  
  virtual void drawRoot ();
private:
  void drawParticle (Particle * const particle);
};

/** Classe représentant les squelettes guides libres.
 *
 * @author	Flavien Bridault
 */
class FreeLeadSkeleton : public FreeSkeleton
{
public:
  /** Constructeur de squelette périphérique
   * @param src pointeur sur le squelette périphérique initial
   * @param splitHeight hauteur de découpe
   *
   */
  FreeLeadSkeleton(const LeadSkeleton* const src, uint splitHeight);
  virtual ~FreeLeadSkeleton();

  /** Duplique un squelette.
   * @param offset Valeur du décalage dans l'espace du squelette par rapport au squelette courant.
   */
  virtual FreePeriSkeleton* dup(Point& offset);
  
private:  
  /** Dessine une particule d'un squelette guide.
   * @param particle Particule à dessiner.
   */
  void drawParticle (Particle * const particle);
};

#endif

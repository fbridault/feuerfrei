#if !defined(PERISKELETON_H)
#define PERISKELETON_H

class PeriSkeleton;
class LeadSkeleton;
class FreePeriSkeleton;
class FreeLeadSkeleton;

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
  /** Constructeur de squelette périphérique.
   * @param s Pointeur sur le solveur de fluides.
   * @param position Position de la flamme dans l'espace.
   * @param rootMoveFactor Amplitude du déplacement autorisé pour l'origine du squelette. Varie
   * en fonction du type de flamme.
   * @param leadSkeleton pointeur sur le squelette guide associé (généralement le plus proche,
   * dans le cas de la bougie simple, le problème est simple puisqu'il n'y en a qu'un seul)
   * @param pls Durée de vie initiale d'une particule.
   */
  PeriSkeleton(Solver3D* const s, const Point& position, const Point& rootMoveFactor, 
	       LeadSkeleton *leadSkeleton, uint *pls);
  /** Destructeur. */
  virtual ~PeriSkeleton();
  
  /** Méthode de séparation d'un squelette.
   * @param splitHeight hauteur de la découpe.
   * @param leadSkeleton squelette guide relatif.
   */
  FreePeriSkeleton* split (uint splitHeight, FreeLeadSkeleton* leadSkeleton);
  
  /** Retourne un pointeur sur le squelette guide relatif. */
  LeadSkeleton* getLeadSkeleton() const {return m_lead;};

private:  
  /** Pointeur sur le squelette guide associé (généralement le plus proche,
   * dans le cas de la bougie simple, le problème est simple puisqu'il n'y en a qu'un seul)
   */
  LeadSkeleton *m_lead;
};

/** Classe représentant les squelettes périphériques libres.
 *
 * @author	Flavien Bridault
 */
class FreePeriSkeleton : public FreeSkeleton
{
public:
  /** Constructeur de squelette périphérique libre. Permet de construire un squelette à partir
   * d'un autre, en découpant celui en deux à la hauteur passée en paramètre. Le squelette
   * crée sera constitué de la partie supérieure, donc des particules comprises dans
   * l'intervalle [0;splitHeight].
   * @param src Pointeur sur le squelette source.
   * @param src Pointeur sur le squelette périphérique initial.
   * @param leadSkeleton Pointeur sur le squelette guide relatif.
   * @param splitHeight Hauteur de découpe.
   */
  FreePeriSkeleton(const PeriSkeleton* const src, FreeLeadSkeleton* const leadSkeleton, uint splitHeight);
  
  /** Constructeur de squelette périphérique libre.
   * @param size Nombre de particules maximum du squelette.
   * @param s Pointeur sur le solveur de fluides.
   * @param leadSkeleton Pointeur sur le squelette guide relatif.
   */
  FreePeriSkeleton(uint size, Solver3D* s, FreeLeadSkeleton* const leadSkeleton);
  /** Destructeur */
  virtual ~FreePeriSkeleton();
  
  /** Retourne un pointeur sur le squelette guide relatif */
  FreeLeadSkeleton* getLeadSkeleton() const {return m_lead;};
  
  friend class FreeLeadSkeleton;
private:  
  /** Pointeur sur le squelette guide associé (généralement le plus proche,
   * dans le cas de la bougie simple, le problème est simple puisqu'il n'y en a qu'un seul)
   */
  FreeLeadSkeleton *m_lead;
};

#endif

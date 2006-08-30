#if !defined(PERISKELETON_H)
#define PERISKELETON_H

class PeriSkeleton;
class LeadSkeleton;
class FreePeriSkeleton;
class FreeLeadSkeleton;

#include "skeleton.hpp"

class Skeleton;

/** Classe repr�sentant les squelettes p�riph�riques, voir la classe Skeleton pour plus de
 * d�tails. Un squelette p�riph�rique se r�f�re � un squelette guide relatif pour la
 * construction des surfaces NURBS.
 *
 * @author	Flavien Bridault
 */
class PeriSkeleton : public Skeleton
{
public:
  /** Constructeur de squelette p�riph�rique
   * @param s pointeur sur le solveur de fluides
   * @param position position de la flamme dans l'espace
   * @param pt position de l'origine du squelette
   * @param pls dur�e de vie initiale d'une particule
   * @param guide pointeur sur le squelette guide associ� (g�n�ralement le plus proche,
   * dans le cas de la bougie simple, le probl�me est simple puisqu'il n'y en a qu'un seul)
   */
  PeriSkeleton(Solver* const s, const Point& position, const Point& rootMoveFactor, 
	       LeadSkeleton *leadSkeleton, uint *pls);
  virtual ~PeriSkeleton();
  
  /** M�thode de s�paration d'un squelette
   * @param splitHeight hauteur de la d�coupe
   * @param leadSkeleton squelette guide relatif
   */
  FreePeriSkeleton* split (uint splitHeight, FreeLeadSkeleton* leadSkeleton);
  
  /** Retourne un pointeur sur le squelette guide relatif */
  LeadSkeleton* getLeadSkeleton() const {return m_lead;};

private:  
  /** Pointeur sur le squelette guide associ� (g�n�ralement le plus proche,
   * dans le cas de la bougie simple, le probl�me est simple puisqu'il n'y en a qu'un seul)
   */
  LeadSkeleton *m_lead;
};

/** Classe repr�sentant les squelettes p�riph�riques libres.
 *
 * @author	Flavien Bridault
 */
class FreePeriSkeleton : public FreeSkeleton
{
public:
  /** Constructeur de squelette p�riph�rique
   * @param src pointeur sur le squelette p�riph�rique initial
   * @param leadSkeleton pointeur sur le squelette guide relatif
   * @param splitHeight hauteur de d�coupe
   *
   */
  FreePeriSkeleton(const PeriSkeleton* const src, FreeLeadSkeleton* const leadSkeleton, uint splitHeight);
  FreePeriSkeleton(uint size, Solver* s, FreeLeadSkeleton* const leadSkeleton);
  virtual ~FreePeriSkeleton();
  
  /** Retourne un pointeur sur le squelette guide relatif */
  FreeLeadSkeleton* getLeadSkeleton() const {return m_lead;};
  
  friend class FreeLeadSkeleton;
private:  
  /** Pointeur sur le squelette guide associ� (g�n�ralement le plus proche,
   * dans le cas de la bougie simple, le probl�me est simple puisqu'il n'y en a qu'un seul)
   */
  FreeLeadSkeleton *m_lead;
};

#endif

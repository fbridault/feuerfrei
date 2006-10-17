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
  /** Constructeur de squelette p�riph�rique.
   * @param s Pointeur sur le solveur de fluides.
   * @param position Position de la flamme dans l'espace.
   * @param rootMoveFactor Amplitude du d�placement autoris� pour l'origine du squelette. Varie
   * en fonction du type de flamme.
   * @param leadSkeleton pointeur sur le squelette guide associ� (g�n�ralement le plus proche,
   * dans le cas de la bougie simple, le probl�me est simple puisqu'il n'y en a qu'un seul)
   * @param pls Dur�e de vie initiale d'une particule.
   */
  PeriSkeleton(Solver3D* const s, const Point& position, const Point& rootMoveFactor, 
	       LeadSkeleton *leadSkeleton, uint *pls);
  /** Destructeur. */
  virtual ~PeriSkeleton();
  
  /** M�thode de s�paration d'un squelette.
   * @param splitHeight hauteur de la d�coupe.
   * @param leadSkeleton squelette guide relatif.
   */
  FreePeriSkeleton* split (uint splitHeight, FreeLeadSkeleton* leadSkeleton);
  
  /** Retourne un pointeur sur le squelette guide relatif. */
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
  /** Constructeur de squelette p�riph�rique libre. Permet de construire un squelette � partir
   * d'un autre, en d�coupant celui en deux � la hauteur pass�e en param�tre. Le squelette
   * cr�e sera constitu� de la partie sup�rieure, donc des particules comprises dans
   * l'intervalle [0;splitHeight].
   * @param src Pointeur sur le squelette source.
   * @param src Pointeur sur le squelette p�riph�rique initial.
   * @param leadSkeleton Pointeur sur le squelette guide relatif.
   * @param splitHeight Hauteur de d�coupe.
   */
  FreePeriSkeleton(const PeriSkeleton* const src, FreeLeadSkeleton* const leadSkeleton, uint splitHeight);
  
  /** Constructeur de squelette p�riph�rique libre.
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
  /** Pointeur sur le squelette guide associ� (g�n�ralement le plus proche,
   * dans le cas de la bougie simple, le probl�me est simple puisqu'il n'y en a qu'un seul)
   */
  FreeLeadSkeleton *m_lead;
};

#endif

#if !defined(PERISKELETON_H)
#define PERISKELETON_H

class CPeriSkeleton;
class CLeadSkeleton;
class CFreePeriSkeleton;
class CFreeLeadSkeleton;

#include "skeleton.hpp"

class ISkeleton;

/** Classe représentant les squelettes périphériques, voir la classe ISkeleton pour plus de
 * détails. Un squelette périphérique se réfère à un squelette guide relatif pour la
 * construction des surfaces NURBS.
 *
 * @author	Flavien Bridault
 */
class CPeriSkeleton : public ISkeleton
{
public:
  /** Constructeur de squelette périphérique.
   * @param s CPointeur sur le solveur de fluides.
   * @param position Position de la flamme dans l'espace.
   * @param rootMoveFactor Amplitude du déplacement autorisé pour l'origine du squelette. Varie
   * en fonction du type de flamme.
   * @param leadSkeleton pointeur sur le squelette guide associé (généralement le plus proche,
   * dans le cas de la bougie simple, le problème est simple puisqu'il n'y en a qu'un seul)
   * @param pls Durée de vie initiale d'une particule.
   */
  CPeriSkeleton(	Field3D& a_rField, const CPoint& a_rPosition, const CPoint& a_rRootMoveFactor,
							CLeadSkeleton& a_rLeadSkeleton, uint a_uiPls);
  /** Destructeur. */
  virtual ~CPeriSkeleton();

  /** Méthode de séparation d'un squelette.
   * @param splitHeight hauteur de la découpe.
   * @param leadSkeleton squelette guide relatif.
   */
  CFreePeriSkeleton* split (uint a_uiSplitHeight, CFreeLeadSkeleton& a_rLeadSkeleton);

  void addParticle(CPoint const& a_rParticle);

  virtual bool moveParticle(CParticle& a_rParticle);

  /** Retourne un pointeur sur le squelette guide relatif. */
  CLeadSkeleton& getLeadSkeleton() const {return m_rLead;};

  /** Méthode d'ajout de forces en utilisant celle du squelette relatif. Ceci
   * est utilisé uniquement dans le cas des bougies.
   */
  void addForces ();

private:
  /** CPointeur sur le squelette guide associé (généralement le plus proche,
   * dans le cas de la bougie simple, le problème est simple puisqu'il n'y en a qu'un seul)
   */
  CLeadSkeleton& m_rLead;
};

/** Classe représentant les squelettes périphériques libres.
 *
 * @author	Flavien Bridault
 */
class CFreePeriSkeleton : public IFreeSkeleton
{
public:
  /** Constructeur de squelette périphérique libre. Permet de construire un squelette à partir
   * d'un autre, en découpant celui en deux à la hauteur passée en paramètre. Le squelette
   * crée sera constitué de la partie supérieure, donc des particules comprises dans
   * l'intervalle [0;splitHeight].
   * @param src CPointeur sur le squelette source.
   * @param src CPointeur sur le squelette périphérique initial.
   * @param leadSkeleton CPointeur sur le squelette guide relatif.
   * @param splitHeight Hauteur de découpe.
   */
  CFreePeriSkeleton(IFreeSkeleton const& a_rSrc, CFreeLeadSkeleton& a_rLeadSkeleton, uint a_uiSplitHeight);

  /** Constructeur de squelette périphérique libre.
   * @param size Nombre de particules maximum du squelette.
   * @param s CPointeur sur le solveur de fluides.
   * @param leadSkeleton CPointeur sur le squelette guide relatif.
   */
  CFreePeriSkeleton(uint a_uiSize, Field3D& a_rField, CFreeLeadSkeleton& a_rLeadSkeleton);
  /** Destructeur */
  virtual ~CFreePeriSkeleton();

  /** Retourne un pointeur sur le squelette guide relatif */
  CFreeLeadSkeleton& getLeadSkeleton() const {return m_rLead;};

  friend class CFreeLeadSkeleton;
private:
  /** CPointeur sur le squelette guide associé (généralement le plus proche,
   * dans le cas de la bougie simple, le problème est simple puisqu'il n'y en a qu'un seul)
   */
  CFreeLeadSkeleton& m_rLead;
};

#endif

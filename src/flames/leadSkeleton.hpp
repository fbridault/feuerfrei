#if !defined(LEADSKELETON_H)
#define LEADSKELETON_H

class CLeadSkeleton;
class CFreeLeadSkeleton;
class CFreePeriSkeleton;

#include "skeleton.hpp"

#include "noise.hpp"

class ISkeleton;

/** Classe représentant les squelettes centraux, voir la classe ISkeleton pour plus de
 * détails.
 *
 * @author	Flavien Bridault
 */
class CLeadSkeleton : public ISkeleton
{
public:
  /** Constructeur de squelette central.
   * @param s CPointeur sur le solveur de fluides.
   * @param position Position de la flamme dans l'espace.
   * @param rootMoveFactor Amplitude du déplacement autorisé pour l'origine du
   * squelette. Varie en fonction du type de flamme.  @param pls Durée de vie
   * initiale d'une particule.
   */
  CLeadSkeleton(	Field3D& a_rField, const CPoint& position, const CPoint& rootMoveFactor, uint a_uiPls, float u,
								float noiseIncrement, float noiseMin, float noiseMax);

  /** Destructeur. */
  virtual ~CLeadSkeleton();

  /** Méthode de séparation d'un squelette.
   * @param splitHeight hauteur de la découpe.
   */
  CFreeLeadSkeleton* split (uint a_uiSplitHeight);

  void addParticle(CPoint const& a_rParticle);

  virtual void drawRoot () const;

  /** Méthode d'ajout de force propre au squelette, autrement dit application locale de la FDF.
   *
   * @param fdf Code correspondant au type de FDF parmi {FDF_LINEAR
   * ,FDF_BILINEAR ,FDF_EXPONENTIAL,FDF_GAUSS,FDF_RANDOM}.
   * @param innerForce Force appliquée.
   * @param perturbate Perturbation appliquée à la FDF parmi
   * {FLICKERING_VERTICAL, FLICKERING_RANDOM1, FLICKERING_RANDOM2,
   * FLICKERING_NOISE, FLICKERING_NONE}
   */
  void addForces (int fdf, float innerForce, char perturbate);

  /** Récupération de la dernière force appliquée au squelette. Utilisée par les
   * squelettes périphériques relatifs dans le cas des bougies pour appliquer
   * une force à leur base.
   *
   * @return Valeur de la force.
   */
  float getLastAppliedForce () const { return m_lastAppliedForce; };

private:
  void drawParticle (CParticle const& a_rParticle) const;

  /** Valeur d'entrée de la fonction de distribution de carburant F(u). */
  float m_u;

  /** Dernière force appliquée par le squelette central dans la grille. On est
   * obligé de stocker cette valeur à cause de l'application de forces propres
   * aux squelettes périphériques avec les RealFields dans le cas des bougies
   * (voir CPeriSkeleton::addForces() )
   */
  float m_lastAppliedForce;

  /** Générateur de bruit de Perlin. */
  PerlinNoise1D m_noiseGenerator;

  /** Compteur utilisé pour la génération de forces périodiques. */
  float m_perturbateCount;
};

/** Classe représentant les squelettes centraux libres.
 *
 * @author	Flavien Bridault
 */
class CFreeLeadSkeleton : public IFreeSkeleton
{
public:
  /** Constructeur de squelette périphérique
   *
   * @param src pointeur sur le squelette périphérique initial
   * @param splitHeight hauteur de découpe
   *
   */
  CFreeLeadSkeleton(IFreeSkeleton const& a_rSrc, uint a_uiSplitHeight);
  virtual ~CFreeLeadSkeleton();

  /** Duplique un squelette.
   *
   * @param offset Valeur du décalage dans l'espace du squelette par rapport au
   * squelette courant.
   */
  virtual CFreePeriSkeleton* dup(const CPoint& offset);

private:
  /** Dessine une particule d'un squelette central.
   *
   * @param particle Particule à dessiner.
   */
  void drawParticle (CParticle const& a_rParticle) const;
};

#endif

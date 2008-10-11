#if !defined(LEADSKELETON_H)
#define LEADSKELETON_H

class LeadSkeleton;
class FreeLeadSkeleton;
class FreePeriSkeleton;

#include "skeleton.hpp"

#include "noise.hpp"

class Skeleton;

/** Classe représentant les squelettes centraux, voir la classe Skeleton pour plus de
 * détails.
 *
 * @author	Flavien Bridault
 */
class LeadSkeleton : public Skeleton
{
public:
  /** Constructeur de squelette central.
   * @param s CPointeur sur le solveur de fluides.
   * @param position Position de la flamme dans l'espace.
   * @param rootMoveFactor Amplitude du déplacement autorisé pour l'origine du
   * squelette. Varie en fonction du type de flamme.  @param pls Durée de vie
   * initiale d'une particule.
   */
  LeadSkeleton(Field3D* const s, const CPoint& position, const CPoint& rootMoveFactor, uint pls, float u,
	       float noiseIncrement, float noiseMin, float noiseMax);

  /** Destructeur. */
  virtual ~LeadSkeleton();

  /** Méthode de séparation d'un squelette.
   * @param splitHeight hauteur de la découpe.
   */
  FreeLeadSkeleton* split (uint splitHeight);

  void addParticle(const CPoint* const pt);

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
  void drawParticle (Particle * const particle) const;

  /** Valeur d'entrée de la fonction de distribution de carburant F(u). */
  float m_u;

  /** Dernière force appliquée par le squelette central dans la grille. On est
   * obligé de stocker cette valeur à cause de l'application de forces propres
   * aux squelettes périphériques avec les RealFields dans le cas des bougies
   * (voir PeriSkeleton::addForces() )
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
class FreeLeadSkeleton : public FreeSkeleton
{
public:
  /** Constructeur de squelette périphérique
   *
   * @param src pointeur sur le squelette périphérique initial
   * @param splitHeight hauteur de découpe
   *
   */
  FreeLeadSkeleton(const LeadSkeleton* const src, uint splitHeight);
  virtual ~FreeLeadSkeleton();

  /** Duplique un squelette.
   *
   * @param offset Valeur du décalage dans l'espace du squelette par rapport au
   * squelette courant.
   */
  virtual FreePeriSkeleton* dup(const CPoint& offset);

private:
  /** Dessine une particule d'un squelette central.
   *
   * @param particle Particule à dessiner.
   */
  void drawParticle (Particle * const particle) const;
};

#endif

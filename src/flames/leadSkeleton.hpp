#if !defined(LEADSKELETON_H)
#define LEADSKELETON_H

class LeadSkeleton;
class FreeLeadSkeleton;
class FreePeriSkeleton;

#include "skeleton.hpp"

#include "noise.hpp"

class Skeleton;

/** Classe repr�sentant les squelettes centraux, voir la classe Skeleton pour plus de
 * d�tails.
 *
 * @author	Flavien Bridault
 */
class LeadSkeleton : public Skeleton
{
public:
  /** Constructeur de squelette central.
   * @param s CPointeur sur le solveur de fluides.
   * @param position Position de la flamme dans l'espace.
   * @param rootMoveFactor Amplitude du d�placement autoris� pour l'origine du
   * squelette. Varie en fonction du type de flamme.  @param pls Dur�e de vie
   * initiale d'une particule.
   */
  LeadSkeleton(Field3D* const s, const CPoint& position, const CPoint& rootMoveFactor, uint pls, float u,
	       float noiseIncrement, float noiseMin, float noiseMax);

  /** Destructeur. */
  virtual ~LeadSkeleton();

  /** M�thode de s�paration d'un squelette.
   * @param splitHeight hauteur de la d�coupe.
   */
  FreeLeadSkeleton* split (uint splitHeight);

  void addParticle(const CPoint* const pt);

  virtual void drawRoot () const;

  /** M�thode d'ajout de force propre au squelette, autrement dit application locale de la FDF.
   *
   * @param fdf Code correspondant au type de FDF parmi {FDF_LINEAR
   * ,FDF_BILINEAR ,FDF_EXPONENTIAL,FDF_GAUSS,FDF_RANDOM}.
   * @param innerForce Force appliqu�e.
   * @param perturbate Perturbation appliqu�e � la FDF parmi
   * {FLICKERING_VERTICAL, FLICKERING_RANDOM1, FLICKERING_RANDOM2,
   * FLICKERING_NOISE, FLICKERING_NONE}
   */
  void addForces (int fdf, float innerForce, char perturbate);

  /** R�cup�ration de la derni�re force appliqu�e au squelette. Utilis�e par les
   * squelettes p�riph�riques relatifs dans le cas des bougies pour appliquer
   * une force � leur base.
   *
   * @return Valeur de la force.
   */
  float getLastAppliedForce () const { return m_lastAppliedForce; };

private:
  void drawParticle (Particle * const particle) const;

  /** Valeur d'entr�e de la fonction de distribution de carburant F(u). */
  float m_u;

  /** Derni�re force appliqu�e par le squelette central dans la grille. On est
   * oblig� de stocker cette valeur � cause de l'application de forces propres
   * aux squelettes p�riph�riques avec les RealFields dans le cas des bougies
   * (voir PeriSkeleton::addForces() )
   */
  float m_lastAppliedForce;

  /** G�n�rateur de bruit de Perlin. */
  PerlinNoise1D m_noiseGenerator;

  /** Compteur utilis� pour la g�n�ration de forces p�riodiques. */
  float m_perturbateCount;
};

/** Classe repr�sentant les squelettes centraux libres.
 *
 * @author	Flavien Bridault
 */
class FreeLeadSkeleton : public FreeSkeleton
{
public:
  /** Constructeur de squelette p�riph�rique
   *
   * @param src pointeur sur le squelette p�riph�rique initial
   * @param splitHeight hauteur de d�coupe
   *
   */
  FreeLeadSkeleton(const LeadSkeleton* const src, uint splitHeight);
  virtual ~FreeLeadSkeleton();

  /** Duplique un squelette.
   *
   * @param offset Valeur du d�calage dans l'espace du squelette par rapport au
   * squelette courant.
   */
  virtual FreePeriSkeleton* dup(const CPoint& offset);

private:
  /** Dessine une particule d'un squelette central.
   *
   * @param particle Particule � dessiner.
   */
  void drawParticle (Particle * const particle) const;
};

#endif

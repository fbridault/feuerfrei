#if !defined(SKELETON_H)
#define SKELETON_H

class ISkeleton;
class CParticle;

#include "flames.hpp"
#include "particle.hpp"

#define FULL_SKELETON 0
#define HALF_SKELETON 1

class Field3D;

/** La classe IFreeSkeleton est utilisé pour les squelettes de flammes
 * détachées. Il s'agit d'une file de particules gérée avec un vecteur, mais
 * elle ne permet que la sortie d'éléments. Seule la sous-classe ISkeleton permet
 * d'ajouter des particules. Le nombre, la durée de vie et la position des
 * particules sont fixés dans le constructeur. Un IFreeSkeleton est donc
 * obligatoirement éphémère.<br>
 *
 * La sortie d'éléments dans la file se gère à l'aide de la fonctions
 * removeParticle().  Il est également utile de préciser que la queue de la file
 * est mobile. Son indice prend donc des valeurs comprises dans [0;size[. La
 * tête de la file est donc immobile et fixée à 0.  L'élément en queue de file
 * est généralement le plus près de l'origine du squelette, et l'élément en tête
 * est donc le plus éloigné de l'origine.
 *
 * @author	Flavien Bridault
 */
class IFreeSkeleton
{
public:
	/** Constructeur de squelette libre.
	 *
	 * @param size Nombre de particules maximum du squelette.
	 * @param s CPointeur sur le solveur de fluides.
	 */
	IFreeSkeleton(uint a_uiSize, Field3D& a_rField);

	/** Constructeur de squelette libre. Permet de construire un squelette à
	 * partir d'un autre, en découpant celui en deux à la hauteur passée en
	 * paramètre. Le squelette crée sera constitué de la partie supérieure, donc
	 * des particules comprises dans l'intervalle [0;splitHeight].
	 *
	 * @param src CPointeur sur le squelette source.
	 * @param splitHeight Hauteur de la découpe.
	 */
	IFreeSkeleton(IFreeSkeleton const& a_rSrc, uint splitHeight);

	/** Destructeur */
	virtual ~IFreeSkeleton();

	/** Donne l'élément en tête de file.
	 *
	 * @return particule en tête de file
	 */
	CParticle const& getLastParticle() const
	{
		return m_queue[m_iHeadIndex];
	};

	/** Donne l'élément à l'indice passé en paramètre.
	 *
	 * @param i Indice de la particule.
	 * @return Particule à l'indice i.
	 */
	virtual CParticle const& getParticle(uint i) const
	{
		return m_queue[i];
	};

	/** Donne l'élément situé au milieu.
	 *
	 * @return CPointeur sur la particule du milieu.
	 */
	CParticle const& getMiddleParticle() const
	{
		return m_queue[m_iHeadIndex/2];
	};

	/** Donne la taille du squelette, sans tenir compte éventuellement de
	 * l'origine.
	 *
	 * @return nombre de particules contenues dans la file du squelette
	 */
	virtual uint getInternalSize() const
	{
		return m_iHeadIndex+1;
	};

	/** Donne la taille du squelette, sans tenir compte éventuellement de
	 * l'origine.
	 *
	 * @return nombre de particules contenues dans la file du squelette
	 */
	virtual uint getSize() const
	{
		return m_iHeadIndex+1;
	};

	/** Donne l'élément en tête de file.
	 *
	 * @return particule en tête de file
	 */
	float getSelfVelocity() const
	{
		return m_selfVelocity;
	};

	/** Retourne true si le squelette ne contient plus de particules.
	 *
	 * @return true sur le squelette est mort
	 */
	virtual bool isDead() const
	{
		return (m_iHeadIndex < 0);
	};

	/** Dessine le squelette à l'écran. */
	virtual void draw () const;

	/** Déplacement des particules du squelette et suppression des particules
	 * mortes. Aucune particule n'est générée dans cette classe.
	 */
	virtual void move();

	/** Déplace une particule dans le champ de vélocité.
	 *
	 * @param pos position de la particule
	 * @return false si la particule est morte et doit être détruite.
	 */
	virtual bool moveParticle(CParticle& a_rParticle);

protected:

	/** Donne l'élément à l'indice passé en paramètre.
	 *
	 * @param i Indice de la particule.
	 * @return Particule à l'indice i.
	 */
	CParticle& grabParticle(uint i)
	{
		return m_queue[i];
	};

	/** Echange deux particules dans la file.
	 *
	 * @param i indice de la première particule
	 * @param j indice de la seconde particule
	 */
	void swap(uint i, uint j);

	/** Supprime la particule à une position donnée.
	 *
	 * @param n indice
	 */
	virtual void removeParticle(uint n);

	/** Affectation de la position d'une particule dans l'espace.
	 *
	 * @param i indice de la particule dans la file
	 * @param pt nouvelle position de la particule
	 */
	void updateParticle(uint i, CPoint const& pt)
	{
		m_queue[i] = pt;
		m_queue[i].decreaseLife();
	}

	/** Dessine les particules du squelettes
	 *
	 * @param particle Particule à dessiner.
	 */
	virtual void drawParticle (CParticle const& a_rParticle) const;

	/** CPointeur sur le solveur de fluides. */
	Field3D& m_rField;

	/** Indice de la tête de la file. */
	int m_iHeadIndex;

	/** File de particules. */
	CParticle *m_queue;

	/** Velocité propre des particules du squelette, utilisée par les FakeFields. */
	float m_selfVelocity;
};

/** La classe ISkeleton est une file de particules gérée avec un vecteur, mais
 * elle transgresse néanmoins la règle en permettant notamment un accès direct à
 * tous les membres de la file sans les supprimer.
 *
 * L'entrée et la sortie d'éléments dans la file se gèrent à l'aide des
 * fonctions addParticle() et removeParticle().  Si l'entrée est
 * systématiquement en queue de file, la sortie peut s'effectuer au-delà de la
 * tête de file.  Il est également utile de préciser que la queue de la file est
 * mobile. Son indice prend donc des valeurs comprises dans
 * [0;NB_PARTICLES_MAX[. La tête de la file est donc immobile et fixée à 0.
 * L'élément en queue de file est généralement le plus près de l'origine du
 * squelette, et l'élément en tête est donc le plus éloigné de l'origine. Chaque
 * particule possède une durée de vie qui est décrementée dès qu'un nouvel
 * élément est inséré.
 *
 * @author	Flavien Bridault
 */
class ISkeleton : public IFreeSkeleton
{
public:
	/** Constructeur de squelette périphérique libre
	 *
	 * @param s CPointeur sur le solveur de fluides.
	 * @param position Position de la flamme dans l'espace.
	 * @param rootMoveFactor Amplitude du déplacement autorisé pour l'origine du
	 * squelette. Varie en fonction du type de flamme.
	 * @param pls Durée de vie initiale d'une particule.
	 */
	ISkeleton(Field3D& a_rField, const CPoint& position, const CPoint& rootMoveFactor, uint pls);
	/** Destructeur. */
	virtual ~ISkeleton(){};

	void draw () const;

	/** Déplacement des particules du squelette, génération d'une nouvelle
	 * particule et suppression des particules mortes.
	 */
	void move();
	virtual bool moveParticle(CParticle& a_rParticle);

	virtual CParticle const& getParticle(uint i) const
	{
		return m_queue[(m_lod) ? i*2 : i];
	};

	virtual uint getSize() const
	{
		return ( (m_lod) ? (m_iHeadIndex / 2)+1 : m_iHeadIndex+1);
	};

	/** Déplacement de l'origine du squelette. */
	void moveRoot ();

	/** Retourne un pointeur sur l'origine du squelette.
	 * @return CPointeur sur l'origine du squelette.
	 */
	CPoint const& getRoot()
	{
		return m_root;
	};

	/* Change la valeur du niveau de détail du squelette
	 *
	 * @param value valeur parmi {FULL_SKELETON détaillé, HALF_SKELETON grossier
	 * (on considère une particule sur 2)}
	 */
	void setLOD(u_char value)
	{
		m_lod = value;
	};

	/** Affectation de la durée de vie des nouvelles particules du squelette. */
	void setLifeSpan(uint lifeSpan)
	{
		m_lifeSpan = lifeSpan;
	};

protected:
	/** Insère une particule en queue de file.
	 *
	 * @param pt position de la particule
	 */
	virtual void addParticle(CPoint const& a_rParticle) = 0;

	/** Dessine l'origine du squelette. */
	virtual void drawRoot () const;

	/** Origine actuelle du squelette. */
	CPoint m_root;

	/** Origine initiale du squelette. */
	CPoint m_rootSave;

	/** Durée de vie des particules du squelette. */
	uint m_lifeSpan;

private:
	/** Contient trois facteurs correctifs pour le déplacement de l'origine
	 * des squelettes. Selon le type de flamme, il est en effet nécessaire
	 * que les origines se déplacent différemment.
	 */
	CPoint m_rootMoveFactor;

	/** Variable correspondant au niveau de détail : FULL_SKELETON détaillé,
	 *  HALF_SKELETON grossier (on considère une particule sur 2) */
	u_char m_lod;
};

#endif

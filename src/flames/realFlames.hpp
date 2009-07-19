#ifndef REALFLAMES_HPP
#define REALFLAMES_HPP

class CLineFlame;
class CPointFlame;
class CDetachedFlame;
class IDetachableFireSource;
class CBitmapTexture;
#include "abstractFlames.hpp"

#include <list>

/**********************************************************************************************************************/
/**************************************** DEFINITION DE LA CLASSE LINEFLAME **************************************/
/**********************************************************************************************************************/

/** La classe CLineFlame implémente une flamme qui provient d'une mèche "linéaire".<br>
 * L'objet CWick appartient à la classe CLineFlame, il est donc précisé dans le constructeur
 * de CWick que l'objet doit être importé dans la scène dans l'état "detached", de sorte que
 * le constructeur de la scène ne cherche pas à le référencer ni à le détruire.
 *
 * @author	Flavien Bridault
 */
class CLineFlame : public IRealFlame
{
public:
	/** Constructeur.
	 * @param flameConfig CPointeur vers la configuration de la flamme.
	 * @param tex CPointeur vers la texture à utiliser.
	 * @param s CPointeur vers le solveur.
	 * @param wickFileName Chaîne de caractère contenant le nom du fichier contenant la mèche.
	 * @param parentFire CPointeur sur le feu auquel appartient la flamme.
	 * @param detachedFlamesWidth Largeur des flammes détachées.
	 * @param wickName Chaîne de caractère contenant le nom de la mèche dans le fichier OBJ.
	 */
	CLineFlame (	const FlameConfig& flameConfig, ITexture const& a_rTex, Field3D& a_rField,
	             CWick *wickObject, float width, float detachedFlamesWidth, IDetachableFireSource *parentFire=NULL);
	virtual ~CLineFlame();

	virtual void drawFlame(bool display, bool displayParticle) const
	{
		if (displayParticle) drawParticles();
		if (display) drawLineFlame();
	};

	virtual CVector getMainDirection() const
	{
		CVector direction;
		for (uint i = 0; i < m_nbLeadSkeletons; i++)
		{
			direction += m_leadSkeletons[i]->getParticle(0);
		}
		direction = direction / m_nbLeadSkeletons;

		return direction;
	}

	virtual bool build()
	{
		if (m_flat)
		{
			m_nbFixedPoints = 1;
			m_shadingType = m_shadingType | 2;
			return buildFlat();
		}
		else
		{
			m_nbFixedPoints = 3;
			m_shadingType = m_shadingType & 1;
			return buildNormal();
		}
	}

	virtual CPoint const& getCenter () const
	{
		return m_center;
	};

	CPoint getTop() const
	{
		return m_top;
	};
	CPoint getBottom() const
	{
		return m_bottom;
	};

	virtual void setSamplingTolerance(u_char value)
	{
		INurbsFlame::setSamplingTolerance(value);
		m_samplingMethod = value;
	};

	void computeVTexCoords();
	void breakCheck();

	/** Méthode permettant de générer des étincelles dans le feu.
	 * @todo Cette méthode n'est pas encore terminée.
	 */
//   virtual void generateAndDrawSparks();
private:
	virtual bool buildNormal()
	{
		if (IRealFlame::build())
		{
			computeCenterAndExtremities();
			return true;
		}
		return false;
	}

	virtual bool buildFlat ();

	virtual void computeCenterAndExtremities()
	{
		CPoint averagePos;

		/* Calcul des extrémités haute et basse */
		m_top.resetToNull();
		m_bottom.resetToNull();
		for (vector < CLeadSkeleton * >::iterator skeletonsIterator = m_leadSkeletons.begin ();
		        skeletonsIterator != m_leadSkeletons.end (); skeletonsIterator++)
		{
			m_top += (*skeletonsIterator)->getParticle(0);
			m_bottom += (*skeletonsIterator)->getRoot();
		}
		m_top = m_top / (float)m_leadSkeletons.size();
		m_bottom = m_bottom / (float)m_leadSkeletons.size();

		/* Calcul du centre */
		for (uint i = 1; i < m_nbLeadSkeletons-1 ; i++)
			averagePos += m_leadSkeletons[i]->getMiddleParticle ();

		m_center = averagePos / (m_nbLeadSkeletons-2);
	}

	/** Liste des particules utilisées pour afficher des étincelles */
	list<CParticle *> m_sparksList;

	/** CPointeur sur le feu auquel appartient la flamme */
	IDetachableFireSource *m_parentFire;

	/** Largeur des flammes détachées */
	float m_detachedFlamesWidth;

	CPoint m_top, m_bottom, m_center;

	u_char m_samplingMethod;

	GLfloat m_vTexInit;
};

/**********************************************************************************************************************/
/*************************************** DEFINITION DE LA CLASSE POINTFLAME **************************************/
/**********************************************************************************************************************/

/** La classe CPointFlame implémente une flamme qui provient d'une mèche verticale droite.<br>
 * Elle génère ses squelettes à partir du maillage de la mèche dont le nom est fourni au
 * constructeur.
 *
 * @author	Flavien Bridault
 */
class CPointFlame : public IRealFlame
{
public:
	/** Constructeur.
	 * @param flameConfig CPointeur sur le configuration de la flamme.
	 * @param tex CPointeur vers la texture à utiliser.
	 * @param s CPointeur vers le solveur.
	 * @param rayon Valeur du rayon du cercle formé par les racines des squelettes.
	 * @param wick Optionnel, objet représentant la mèche. Si NULL, un cylindre simple est utilisé.
	 */
	CPointFlame ( const FlameConfig& flameConfig, ITexture const& a_rTex, Field3D& a_rField,
	              float a_fRayon, CWick *a_pWick);

	/** Destructeur*/
	virtual ~CPointFlame();

	virtual void drawFlame(bool display, bool displayParticle) const
	{
		if (displayParticle) drawParticles();
		if (display) drawPointFlame();
	};

	virtual CVector getMainDirection() const
	{
		return m_leadSkeletons[0]->getParticle(0);
	};
	virtual CPoint const& getCenter () const
	{
		return m_leadSkeletons[0]->getMiddleParticle ();
	};

	CPoint getTop() const
	{
		return m_leadSkeletons[0]->getParticle(0);
	};
	CPoint getBottom() const
	{
		return m_leadSkeletons[0]->getRoot();
	};

	/** Fonction testant si les squelettes doivent se briser. Si c'est le cas, elle effectue la division.
	 * Dans le cas d'une CPointFlame, cette méthode ne fait rien du tout pour l'instant.
	 */
	void breakCheck() {};

	virtual void addForces (int fdf, float innerForce, char perturbate)
	{
		for (vector < CLeadSkeleton * >::iterator skeletonsIterator = m_leadSkeletons.begin ();
		        skeletonsIterator != m_leadSkeletons.end (); skeletonsIterator++)
			(*skeletonsIterator)->addForces (fdf, innerForce, perturbate);
		for (uint i = 0; i < m_nbSkeletons; i++)
			m_periSkeletons[i]->addForces ();
	}
};


/**********************************************************************************************************************/
/********************************************* DEFINITION DE LA CLASSE DETACHEDFLAME **********************************/
/**********************************************************************************************************************/

/** La classe CDetachedFlame défini une flamme détachée, autrement dit une flamme éphémère qui n'est pas attachée à un
 * quelconque support. Pour l'instant seule une classe héritée de IRealFlame peut produire une flamme détachée.
 *
 * @author	Flavien Bridault
 */
class CDetachedFlame : public INurbsFlame
{
public:
	/** Constructeur de flamme détachée. Il est appelée lors de la division d'une flamme dans la méthode breackCheck().
	 * @param source Flamme qui a générée la flamme détachée.
	 * @param nbLeadSkeletons Nombre de squelettes guides.
	 * @param leadSkeletons Tableaux contenant les squelettes guides.
	 * @param nbSkeletons Nombre de squelettes périphériques.
	 * @param periSkeletons Tableaux contenant les squelettes périphériques.
	 * @param tex CPointeur vers la texture à utiliser.
	 */
	CDetachedFlame(const IRealFlame* const source, uint nbLeadSkeletons, CFreeLeadSkeleton **leadSkeletons,
	               uint nbSkeletons, CFreePeriSkeleton **periSkeletons, ITexture const& a_rTex,
	               bool smoothShading, u_char samplingMethod);

	/** Destructeur*/
	virtual ~CDetachedFlame ();

	virtual bool build();
	void computeVTexCoords();

	virtual void drawFlame(bool display, bool displayParticle) const
	{
		if (displayParticle) drawParticles();
		if (display) drawLineFlame();
	};

	void drawParticles() const
	{
		uint i;
		/* Déplacement et détermination du maximum */
		for (i = 0; i < m_nbSkeletons; i++)
			m_periSkeletons[i]->draw();
		for (i = 0; i < m_nbLeadSkeletons; i++)
			m_leadSkeletons[i]->draw();
	};

	virtual CPoint getTop() const
	{
		CPoint averagePos;
		for (uint i = 0; i < m_nbLeadSkeletons; i++)
			averagePos += m_leadSkeletons[i]->getParticle(0);
		averagePos = averagePos / m_nbLeadSkeletons;
		return averagePos;
	}
	virtual CPoint getBottom() const
	{
		CPoint averagePos;
		for (uint i = 0; i < m_nbLeadSkeletons; i++)
			averagePos += m_leadSkeletons[i]->getLastParticle();
		averagePos = averagePos / m_nbLeadSkeletons;
		return averagePos;
	}

protected:
	/** CPointeur vers les squelettes guide. */
	CFreeLeadSkeleton **m_leadSkeletons;
	/** Nombres de squelettes guides. */
	uint m_nbLeadSkeletons;
	/** Tableau contenant les pointeurs vers les squelettes périphériques. */
	CFreePeriSkeleton **m_periSkeletons;

	/** Tableau temporaire contenant les distances entre les particules d'un squelette. */
	float *m_distances;

	/** Tableau temporaire utilisé pour classer les indices des distances entre points de contrôle
	 * lors de l'ajout de points de contrôle supplémentaires dans la NURBS.  Alloué une seule fois
	 * en début de programme à la taille maximale pour des raisons évidentes d'optimisation du temps
	 * d'exécution.
	 */
	int *m_maxDistancesIndexes;
};

#endif

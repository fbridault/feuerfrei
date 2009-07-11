#ifndef BASICFLAMES_H
#define BASICFLAMES_H

class INurbsFlame;
class IFixedFlame;
class IRealFlame;
class ITexture;


#include "periSkeleton.hpp"
#include "leadSkeleton.hpp"
#include "../solvers/solver3D.hpp"
#include "wick.hpp"

#include <list>

#ifndef CALLBACK
#define CALLBACK
#endif

class CPeriSkeleton;
class CLeadSkeleton;
class Field3D;

#ifdef COUNT_NURBS_POLYGONS
extern uint g_count;
#endif

/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE NURBSFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe INurbsFlame est l'objet de plus bas niveau représentant une flamme. Elle définit les propriétés nécessaires
 * pour la construction d'une NURBS, ainsi que la localisation de la flamme dans l'espace. En revanche, la notion de
 * squelette n'apparaît absolument pas ici.
 *
 * @author	Flavien Bridault
 */
class INurbsFlame
{
public:
	/** Constructeur de flamme par défaut n'effectuant pas l'allocation des tableaux utilisés pour la génération
	 * des points de contrôle. L'allocation doit donc être effectuée par la classe fille, ce que fait par exemple
	 * la classe IFixedFlame.
	 * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage.
	 * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
	 * @param tex CPointeur sur la texture de la flamme.
	 */
	INurbsFlame(uint nbSkeletons, ushort nbFixedPoints, ITexture const& a_rTexture);

	/** Constructeur de flamme.
	 * @param source CPointeur sur la flamme qui a généré la flamme courante.
	 * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage.
	 * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
	 * @param tex CPointeur sur la texture de la flamme.
	 */
	INurbsFlame(const INurbsFlame* const source, uint nbSkeletons, ushort nbFixedPoints, ITexture const& a_rTexture);
	virtual ~INurbsFlame ();

	void initNurbs(GLUnurbsObj** nurbs);
	/** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules
	 * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
	 * des vecteurs de noeuds.
	 * @return false si un problème dans la contruction est survenu (pas assez de particules par exemple)
	 */
	virtual bool build() = 0;

	virtual void drawLineFlame() const;

	/** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
	 * build() avec le placage de texture.
	 * @param display Affiche ou non la flamme.
	 * @param displayParticle Affiche ou non les particules.
	 */
	virtual void drawFlame(bool display, bool displayParticle) const = 0;

	/** Ajuste la valeur d'échantillonnage de la NURBS.
	 * @param value Valeur de sampling, compris dans un intervalle [1;4].
	 */
	virtual void setSamplingTolerance(u_char value)
	{
		switch (value)
		{
			case 4:
				gluNurbsProperty(m_nurbs, GLU_U_STEP, 4);
				gluNurbsProperty(m_nurbs, GLU_V_STEP, 4);
				break;
			case 3:
				gluNurbsProperty(m_nurbs, GLU_U_STEP, 3);
				gluNurbsProperty(m_nurbs, GLU_V_STEP, 3);
				break;
			case 2:
				gluNurbsProperty(m_nurbs, GLU_U_STEP, 2);
				gluNurbsProperty(m_nurbs, GLU_V_STEP, 2);
				break;
			case 1:
				gluNurbsProperty(m_nurbs, GLU_U_STEP, 1);
				gluNurbsProperty(m_nurbs, GLU_V_STEP, 1);
				break;
			default:
				cerr << "Bad NURBS step parameter" << endl;
		}
	};

	/** Active ou désactive l'affichage texturé sur la flamme. */
	virtual void setSmoothShading (bool state)
	{
		m_shadingType = (state) ? m_shadingType | 1 : m_shadingType & 2;
	};

	/** Retourne le nombre de squelettes */
	uint getNbSkeletons() const
	{
		return m_nbSkeletons;
	};

	/** Retourne le nombre de points fixes, autrement dit le nombre de racines dans la flamme */
	unsigned short getNbFixedCPoints() const
	{
		return m_nbFixedPoints;
	};

	/** Méthode permettant de cloner la flamme passée en paramètre
	 * @param source Flamme à cloner
	 */
	void cloneNURBSPropertiesFrom(const INurbsFlame& source)
	{
		m_maxParticles = source.m_maxParticles;

		copy(source.m_ctrlPoints, &source.m_ctrlPoints[(m_maxParticles + m_nbFixedPoints) * (m_nbSkeletons + m_uorder) * 3], m_ctrlPoints);
		m_uknotsCount = source.m_uknotsCount;
		m_vknotsCount = source.m_vknotsCount;
		copy(source.m_uknots, &source.m_uknots[m_uknotsCount], m_uknots);
		copy(source.m_vknots, &source.m_vknots[m_vknotsCount], m_vknots);
		copy(source.m_texCPoints, &source.m_texCPoints[(m_maxParticles + m_nbFixedPoints) * (m_nbSkeletons + m_uorder) * 2], m_texCPoints);
		copy(source.m_texTmp, &source.m_texTmp[m_maxParticles + m_nbFixedPoints], m_texTmp);
		m_vsize = source.m_vsize;
	}

	ITexture const& getTexture() const
	{
		return m_rTexture;
	};

protected:
	/** Affiche la flamme sous forme de NURBS, à partir du tableau de points de contrôles et du tableau
	 * de coordonnées de texture construits au préalable.
	 */
	void drawNurbs () const;

	/** Fonction simplifiant l'affectation d'un point de contrôle. L'algorithme de construction
	 * de la NURBS parcours de façon séquentielle les squelettes, aucun indice dans le tableau
	 * de points de contrôles n'est donc passé. En lieu et place, un pointeur est utilisé et
	 * incrémenté après chaque affectation dans cette fonction. Le même système est employé
	 * pour le tableau des coordonnées (u,v) de texture. La coordonnée u est précalculée dans
	 * texTmp, tandis que la coordonnée v est passée en paramètre.
	 *
	 * @param pt point à affecter dans le tableau
	 * @param v valeur de la coordonnée de texture t
	 */
	void setCtrlCPoint (CPoint const& a_rPoint, GLfloat u)
	{
		*m_ctrlPoints++ =a_rPoint.x;
		*m_ctrlPoints++ = a_rPoint.y;
		*m_ctrlPoints++ = a_rPoint.z;
		*m_texCPoints++ = u;
		*m_texCPoints++ = *m_texTmp++;
		m_count++;
	}

	static void CALLBACK nurbsError(GLenum errorCode)
	{
		const GLubyte *estring;

		estring = gluErrorString(errorCode);
		cerr << "Nurbs error : " << estring << endl;
		exit(0);
	}

	static void CALLBACK NurbsBegin(GLenum type, GLvoid *shadingType)
	{
#ifdef COUNT_NURBS_POLYGONS
		g_count++;
#endif
		/* Si on est en mode NURBS plate */
		if ( *(u_char *)shadingType & 2)
		{
			glDisable(GL_CULL_FACE);
			if ( (*(u_char *)shadingType & 1))
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			else
				glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		}
		else
			if ( ! (*(u_char *)shadingType & 1))
				glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		glBegin(type);
	}

	static void CALLBACK NurbsEnd(GLvoid *shadingType)
	{
		glEnd();
		if ( *(char *)shadingType & 2)
			/* Si on est en mode NURBS plate */
		{
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT,GL_FILL);
		}
		else
			if ( ! (*(char *)shadingType & 1))
				glPolygonMode(GL_FRONT,GL_FILL);
	}

	static void CALLBACK NurbsVertex ( GLfloat *vertex )
	{
		glVertex3fv(vertex);
	}

	static void CALLBACK NurbsNormal ( GLfloat *normal )
	{
		glNormal3fv(normal);
	}

	static void CALLBACK NurbsTexCoord ( GLfloat *texCoord )
	{
		glTexCoord2fv(texCoord);
	}

	/** Ordre de la NURBS en u (égal au degré en u + 1). */
	u_char m_uorder;
	/** Ordre de la NURBS en v (égal au degré en v + 1). */
	u_char m_vorder;
	/** Nombre de squelettes de la flamme. */
	uint m_nbSkeletons;
	/** Matrice de points de contrôle */
	GLfloat *m_ctrlPoints;
	/** Copie du pointeur vers le tableau de points de contrôle */
	GLfloat  *m_ctrlPointsSave;

	GLfloat *m_texCPoints, *m_texCPointsSave;
	GLfloat *m_texTmp, *m_texTmpSave;

	/** Vecteur de noeuds en u */
	GLfloat *m_uknots;
	/** Vecteur de noeuds en v */
	GLfloat *m_vknots;
	/** Tableau temporaire utilisé pour stocker les distances entre chaque point de contrôle d'un
	 * squelette. Alloué une seule fois en début de programme à la taille maximale pour des raisons
	 * évidentes d'optimisation du temps d'exécution.
	 */
	uint m_uknotsCount, m_vknotsCount;
	uint m_maxParticles;
	uint m_count;
	/** Nombre de points fixes pour chaque direction v, par exemple origine des squelettes, sommet du guide */
	unsigned short m_nbFixedPoints;

	/** Nombre de points total dans la direction v de la NURBS, soit m_nbFixedPoints+m_maxParticles */
	uint m_vsize;
	/** Codage du type de shading : 1er bit à 1 si fil de fer; 2e bit à 1 si front and back */
	u_char m_shadingType;

	/** ITexture de la flamme */
	ITexture const& m_rTexture;
	/* Incrément en u pour la coordonnée de texture */
	float m_utexInc;

private:
	/** Objet OpenGL permettant de définir la NURBS */
	GLUnurbsObj *m_nurbs;
};


/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE FIXEDFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe IFixedFlame est l'objet de plus bas niveau représentant une flamme dont la base est fixe. Par rapport à la
 * classe de base INurbsFlame, elle rajoute la prise en compte des origines des squelettes, notamment pour l'allocation
 * des tableaux de points de contrôles. Elle défini également quelques méthodes abstraites pour obtenir des informations
 * sur la localisation de la flamme nécessaire pour l'éclairage. Tout ceci n'est en effet
 * pas nécessaire pour la définition d'une flamme détachée.
 *
 * @author	Flavien Bridault
 */
class IFixedFlame : public INurbsFlame
{
public:
	/** Constructeur de flamme par défaut.
	 * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage.
	 * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
	 * @param tex CPointeur sur la texture de la flamme.
	 */
	IFixedFlame(uint nbSkeletons, ushort nbFixedPoints, ITexture const& a_rTexture);

	virtual ~IFixedFlame ();

	/** Dessine la mèche de la flamme.
	 * @param displayBoxes Affiche ou non le partitionnement de la mèche.
	 */
	virtual void drawWickBoxes() const = 0;

	/** Dessine une flamme ponctuelle. La différence avec drawLineFlame() est que la texture est translatée
	 * pour rester en face de l'observateur.
	 */
	virtual void drawPointFlame() const;

	/** Retourne la direction de la base de la flamme vers la derniere particule
	 * pour orienter le solide photométrique.
	 * @return Direction.
	 */
	virtual CVector getMainDirection() const = 0;

	/** Retourne le centre de la flamme.
	 * @return Centre de la flamme.
	 */
	virtual CPoint const& getCenter () const = 0;

	/** Renvoie un pointeur vers le sommet de la flamme.
	 * @return CPointeur vers le sommet.
	 */
	virtual CPoint getTop() const = 0;

	/** Renvoie un pointeur vers le bas de la flamme.
	 * @return CPointeur vers le bas.
	 */
	virtual CPoint getBottom() const = 0;

};

class CDetachedFlame;

#include <vector>
/**********************************************************************************************************************/
/********************************************* DEFINITION DE LA CLASSE REALFLAME **************************************/
/**********************************************************************************************************************/

/** La classe IRealFlame, par rapport à la classe IFixedFlame, ajoute la notion de squelettes de flamme ainsi que les
 * interactions avec un solveur de fluides. Elle est qualifiée de "Real" en comparaison avec les CloneFlame.
 * Cette classe reste abstraite et est héritée par les classes CLineFlame et CPointFlame.
 * Elle permet de définir une primitive géométrique pour une flamme, mais ne permet pas de construire une flamme
 * en tant que source de lumière d'une scène. C'est la classe IFireSource qui permet de définir ceci, en utilisant
 * les classes héritées de IRealFlame comme élément de base.<br>
 * Une IRealFlame est construite à sa position relative définie dans m_position. Lors du dessin, la translation dans
 * le repère du feu est donc déjà effectuée. C'est la classe IFireSource qui s'occupe de placer correctement dans
 * le repère du monde. En revanche, la mèche en définie en (0,0,0) et il faut donc la translater dans le repère du
 * feu à chaque opération de dessin.
 *
 * @author	Flavien Bridault
 */
class IRealFlame : public IFixedFlame
{
public:
	/** Constructeur de flamme.
	 * @param nbSkeletons Nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage.
	 * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
	 * @param tex CPointeur sur la texture de la flamme.
	 * @param s CPointeur vers le solveur.
	 */
	IRealFlame(uint nbSkeletons, ushort nbFixedPoints, ITexture const& a_rTexture);
	virtual ~IRealFlame ();

	/** Fonction appelée par le solveur de fluides pour ajouter l'élévation thermique de la flamme.
	 */
	virtual void addForces (int fdf, float innerForce, char perturbate)
	{
		for (vector < CLeadSkeleton * >::iterator skeletonsIterator = m_leadSkeletons.begin ();
		     skeletonsIterator != m_leadSkeletons.end (); skeletonsIterator++)
			(*skeletonsIterator)->addForces (fdf, innerForce, perturbate);
	}

	/** Affectation de la durée de vie des squelettes guides.
	 * @param value Durée de vie en itérations.
	 */
	virtual void setLeadLifeSpan(uint value)
	{
		for (vector < CLeadSkeleton * >::iterator skeletonsIterator = m_leadSkeletons.begin ();
		     skeletonsIterator != m_leadSkeletons.end (); skeletonsIterator++)
			(*skeletonsIterator)->setLifeSpan(value);
	};

	/** Affectation de la durée de vie des squelettes périphériques.
	 * @param value Durée de vie en itérations.
	 */
	virtual void setPeriLifeSpan(uint value)
	{
		for (uint i = 0; i < m_nbSkeletons; i++)
		{
			assert(m_periSkeletons[i] != NULL);
			m_periSkeletons[i]->setLifeSpan(value);
		}
	};

	/** Display wick boxes
	 */
	virtual void drawWickBoxes() const
	{
		m_wick->drawWickBoxes();
	};

	/** Affiche les particules de tous les squelettes composants la flamme. */
	void drawParticles() const
	{
		uint i;
		/* Déplacement et détermination du maximum */
		if (!m_flat)
			for (i = 0; i < m_nbSkeletons; i++)
			{
				assert(m_periSkeletons[i] != NULL);
				m_periSkeletons[i]->draw();
			}
		for (i = 0; i < m_nbLeadSkeletons; i++)
		{
			assert(m_leadSkeletons[i] != NULL);
			m_leadSkeletons[i]->draw();
		}
	};

	/** Place un marqueur pour ordonner un changement de niveau de
	 *  détail dans les squelettes à la prochaine construction.
	 * @param value FULL_SKELETON ou HALF_SKELETON
	 */
	void setSkeletonsLOD(u_char value)
	{
		m_lodSkel = value;
		m_lodSkelChanged = true;
	};

	/** Effectue le changement de niveau de détail dans les squelettes. */
	virtual void changeSkeletonsLOD()
	{
		uint i;
		for (i = 0; i < m_nbSkeletons; i++)
			m_periSkeletons[i]->setLOD(m_lodSkel);
		for (i = 0; i < m_nbLeadSkeletons; i++)
			m_leadSkeletons[i]->setLOD(m_lodSkel);
		m_lodSkelChanged = false;
	};

	void setFlatFlame(bool value)
	{
		m_flat = value;
	};

	/** Fonction chargée de remplir les coordonées de texture dans la direction v. Elles sont
	 * en effet dépendantes du nombre de particules dans les squelettes et il est donc nécessaire
	 * de les recalculer à chaque construction de la NURBS.
	 */
	virtual void computeVTexCoords();
	virtual bool build();

	virtual CVector getMainDirection() const = 0;
	virtual CPoint const& getCenter () const = 0;

	virtual CPoint getTop() const = 0;
	virtual CPoint getBottom() const = 0;

	/** Fonction testant si les squelettes doivent se briser. Si c'est le cas, elle effectue la division. */
	virtual void breakCheck() = 0;

protected:
	/** Vecteur contenant les squelettes guide. */
	vector < CLeadSkeleton * > m_leadSkeletons;

	/** Nombres de squelettes guides */
	uint m_nbLeadSkeletons;
	/** Tableau contenant les pointeurs vers les squelettes périphériques. */
	CPeriSkeleton **m_periSkeletons;

	float *m_distances;
	/** Tableau temporaire utilisé pour classer les indices des distances entre points de contrôle
	 * lors de l'ajout de points de contrôle supplémentaires dans la NURBS.  Alloué une seule fois
	 * en début de programme à la taille maximale pour des raisons évidentes d'optimisation du temps
	 * d'exécution.
	 */
	int *m_maxDistancesIndexes;

	u_char m_lodSkel, m_flat;
	/** Indique qu'un changement de niveau de détail sur les squelettes a été demandé. */
	bool m_lodSkelChanged;
	/** Mèche de la flamme */
	CWick *m_wick;
};

#endif

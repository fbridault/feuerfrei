#ifndef GLOBALFIELD_HPP
#define GLOBALFIELD_HPP

class GlobalField;

#include "field3D.hpp"
#include <vector>
#include <list>

class FieldThread;
class Field3D;
class CSpatialGraph;

/** La classe GlobalField est un champ de vecteur qui englobe toute la scène. Il sert de conteneur pour tous
 * les solveurs et c'est lui qui transmet les forces globales aux solveurs locaux.
 *
 * @author	Flavien Bridault.
 */
class GlobalField
{
public:
	/** Constructeur du solveur.
	 * @param localFields Tableau contenant les champs de vecteurs locaux.
	 * @param nbLocalFields Nombre de champs locaux.
	 * @param scene CPointeur sur la scène
	 * @param n Résolution de la grille sur la dimension maximale.
	 * @param timeStep Pas de temps utilisé pour la simulation.
	 * @param buoyancy Intensité de la force de flottabilité dans le solveur.
	 * @param omegaDiff Paramètre omega pour la diffusion
	 * @param omegaProj Paramètre omega pour la projection.
	 * @param epsilon Tolérance d'erreur pour GCSSOR.
	 */
#ifdef MULTITHREADS
	GlobalField(list <FieldThread *> const& threads, CSpatialGraph& a_rGraph, char type, uint n,
#else
	GlobalField(vector <Field3D *> const& fields, CSpatialGraph& a_rGraph, char type, uint n,
#endif
	            float timeStep, float vorticityConfinement, float omegaDiff, float omegaProj, float epsilon);
	/** Destructeur. */
	virtual ~GlobalField ()
	{
		delete m_pField;
	};

	/** Lance une itération du solveur. */
	void iterate (void)
	{
		m_pField->iterate();
		shareForces();
	};
	void cleanSources (void)
	{
		m_pField->cleanSources();
	};

	/** Ajoute de façon ponctuelle des forces externes sur une des faces du champ. Il faut également considérer
	 * l'ajout des forces dans les solveurs locaux.
	 * @param position Nouvelle position du solveur. Détermine l'intensité de la force.
	 * @param move Si true, alors le solveur est en plus déplacé à la position passée en paramètre.
	 */
	void shareForces();

	/** Ajoute de façon permanente des forces externes sur une des faces du solveur. Cette méthode
	 * est utilisée principalement lorsque que du vent est appliqué sur une flamme.
	 * @param forces Intensité de la force en (x,y,z).
	 */
	virtual void addPermanentExternalForces(CPoint& forces)
	{
		m_pField->addPermanentExternalForces(forces);
	}

	/** Ajoute de façon temporaire (une itération) des forces externes sur une des faces du solveur. Cette méthode
	 * est utilisée principalement lorsque que du vent est appliqué sur une flamme.
	 * @param forces Intensité de la force en (x,y,z).
	 */
	virtual void addTemporaryExternalForces(CPoint& forces)
	{
		m_pField->addTemporaryExternalForces(forces);
	}

	/** Modifie la force de flottabilité dans le solveur
	 * @param value Nouvelle valeur.
	 */
	virtual void setBuoyancy(float value)
	{
		m_pField->setBuoyancy(value);
	};
	/** Modifie la force de flottabilité dans le solveur
	 * @param value Nouvelle valeur.
	 */
	virtual void setVorticity(float value)
	{
		m_pField->setVorticity(value);
	};

	/** Fonction de dessin du champ de vélocité */
	void displayVelocityField (void)
	{
		m_pField->displayVelocityField();
	};

	/** Fonction de dessin de la grille */
	void displayGrid (void)
	{
		m_pField->displayGrid();
	};

	/** Fonction de dessin du repère de base */
	void displayBase (void)
	{
		m_pField->displayBase();
	};

	/** Retourne la position du solveur dans le repère du monde
	 * @return Position dans l'espace.
	 */
	CPoint getPosition (void)
	{
		CTransform const& rTransform = m_pField->GetTransform();
		return rTransform.GetWorldPosition();
	};

protected:
	/** CPointeur sur les solveurs contenus */
	vector <Field3D*> m_localFields;
	Field3D *m_pField;
};

#endif

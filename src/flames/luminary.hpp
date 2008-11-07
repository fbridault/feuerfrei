#ifndef LUMINARY_HPP
#define LUMINARY_HPP

#include "abstractFires.hpp"
#include "../solvers/field3D.hpp"

/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE LUMINARY ****************************************/
/**********************************************************************************************************************/

/** La classe Luminary
 *
 * @author	Flavien Bridault
 */
class Luminary
{
public:
	/** Constructeur d'une source de flammes. La position de la source est donnée dans le repère du solveur.
	 * @param config Configuration du luminaire.
	 * @param fields Vecteur contenant les solveurs de la scène.
	 * @param fireSources Vecteur contenant les feux de la scène.
	 * @param scene CPointeur sur la scène.
	 * @param filename Nom du fichier contenant le luminaire.
	 */
	Luminary (const LuminaryConfig& a_rConfig,
						vector <Field3D *> &a_vpFields,
						vector <IFireSource *> &a_vpFireSources,
						CScene& a_rScene,
						CharCPtrC a_szFilename,
						const CShader& a_rGenShadowCubeMapShader,
						const CRenderTarget& a_rShadowRenderTarget);

	/** Destructeur */
	virtual ~Luminary ();

	Field3D* initField(const SolverConfig& fieldConfig, const CPoint& position);
	IFireSource* initFire(	const FlameConfig& a_rFlameConfig,
											CharCPtrC a_szFilename,
											Field3D* a_pField,
											CScene& a_rScene,
											const CShader& a_rShadowMapShader,
											const CRenderTarget& a_rShadowRenderTarget);

	/** Ajuste le niveau de détail de la NURBS.
	 * @param value valeur comprise entre 1 et LOD_VALUES.
	 */
	virtual void setLOD(u_char value)
	{
		for (list < IFireSource* >::iterator fireIterator = m_fireSources.begin ();
		     fireIterator != m_fireSources.end (); fireIterator++)
			(*fireIterator)->setLOD(value);
	};

	/** Affectation du coefficient multiplicateur de la FDF.
	 * @param value Coefficient.
	 */
	virtual void setInnerForce(float value)
	{
		for (list < IFireSource* >::iterator fireIterator = m_fireSources.begin ();
		     fireIterator != m_fireSources.end (); fireIterator++)
			(*fireIterator)->setInnerForce(value);
	};

	/** Affectation de la FDF.
	 * @param value FDF.
	 */
	virtual void setFDF(int value)
	{
		for (list < IFireSource* >::iterator fireIterator = m_fireSources.begin ();
		     fireIterator != m_fireSources.end (); fireIterator++)
			(*fireIterator)->setFDF(value);
	};

	/** Affectation de la méthode de perturbation.
	 * @param value Perturbation.
	 */
	virtual void setPerturbateMode(char value)
	{
		for (list < IFireSource* >::iterator fireIterator = m_fireSources.begin ();
		     fireIterator != m_fireSources.end (); fireIterator++)
			(*fireIterator)->setPerturbateMode(value);
	};

	/** Affectation de la durée de vie des squelettes guides.
	 * @param value Durée de vie en itérations.
	 */
	virtual void setLeadLifeSpan(uint value)
	{
		for (list < IFireSource* >::iterator fireIterator = m_fireSources.begin ();
		     fireIterator != m_fireSources.end (); fireIterator++)
			(*fireIterator)->setLeadLifeSpan(value);
	};

	/** Affectation de la durée de vie des squelettes périphériques.
	 * @param value Durée de vie en itérations.
	 */
	virtual void setPeriLifeSpan(uint value)
	{
		for (list < IFireSource* >::iterator fireIterator = m_fireSources.begin ();
		     fireIterator != m_fireSources.end (); fireIterator++)
			(*fireIterator)->setPeriLifeSpan(value);
	};

	virtual void setBuoyancy(float value)
	{
		for (list < Field3D* >::iterator fieldIterator = m_fields.begin ();
		     fieldIterator != m_fields.end (); fieldIterator++)
			(*fieldIterator)->setBuoyancy(value);
	}

	virtual void setVorticity(float value)
	{
		for (list < Field3D* >::iterator fieldIterator = m_fields.begin ();
		     fieldIterator != m_fields.end (); fieldIterator++)
			(*fieldIterator)->setVorticity(value);
	}

	/** Déplace le luminaire.
	 * @param forces Déplacement en (x,y,z).
	 */
	virtual void Move(const CPoint& a_rPosition)
	{
		CPoint diff = a_rPosition - m_position;
		for (list < Field3D* >::iterator fieldIterator = m_fields.begin ();
			fieldIterator != m_fields.end (); fieldIterator++)
			(*fieldIterator)->move(diff);

		for (list < IFireSource* >::iterator fireIterator = m_fireSources.begin ();
			fireIterator != m_fireSources.end (); fireIterator++)
				(*fireIterator)->Move(diff.x, diff.y, diff.z);

		for (vector < CObject* >::const_iterator luminaryIterator = m_luminary.begin ();
			luminaryIterator  != m_luminary.end (); luminaryIterator++)
			(*luminaryIterator)->SetPosition(a_rPosition);
		m_position = a_rPosition;
	}

protected:
	/** Luminaire */
	vector <CObject *> m_luminary;
	/** Luminaire */
	list <IFireSource *> m_fireSources;
	/** Luminaire */
	list <Field3D *> m_fields;

	/** Il se peut que le luminaire ne soit pas un objet physique, dans ce cas ce booléen est à false */
	bool m_hasLuminary;

	/** Position du luminaire. */
	CPoint m_position;
};


#endif

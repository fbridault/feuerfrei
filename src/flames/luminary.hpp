#ifndef LUMINARY_HPP
#define LUMINARY_HPP

#include "abstractFires.hpp"
#include "../solvers/field3D.hpp"

/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE LUMINARY ****************************************/
/**********************************************************************************************************************/

/** The CLuminary class keeps track of association between fires and fields.
 * It is not a renderable object and thus it doesn't belongs to a spatial graph.
 * However it knows the main transform of a fire graph and is thus used to move it.
 * @author	Flavien Bridault
 */
class CLuminary
{
public:
	/** Constructeur d'une source de flammes. La position de la source est donnée dans le repère du solveur.
	 * @param a_rConfig Configuration du luminaire.
	 * @param a_vpFields Vecteur contenant les solveurs de la scène.
	 * @param a_vpFireSources Vecteur contenant les feux de la scène.
	 * @param a_rScene CPointeur sur la scène.
	 * @param a_szFilename Nom du fichier contenant le luminaire.
	 */
	CLuminary (	const LuminaryConfig& a_rConfig,
				vector <Field3D *> &a_vpFields,
				vector <IFireSource *> &a_vpFireSources,
				CSpatialGraph &a_rGraph,
				CScene& a_rScene,
				CharCPtrC a_szFilename,
				const CShader& a_rGenShadowCubeMapShader,
				const CRenderTarget& a_rShadowRenderTarget);

	/** Destructeur */
	virtual ~CLuminary ();

	/** Déplace le luminaire.
	 * @param forces Déplacement en (x,y,z).
	 */
	virtual void Move(const CPoint& a_rPosition)
	{
		assert(m_pTransform != NULL);
		CVector diff = a_rPosition - m_pTransform->GetLocalPosition();

		/* Notify fields of movement so that it can add forces */
		ForEachIter(itField, CFieldList, m_vpFields)
		{
			(*itField)->move(diff);
		}

		m_pTransform->SetPosition(a_rPosition);
	}

//---------------------------------------------------------------------------------------------------------------------
//  Specific methods
//---------------------------------------------------------------------------------------------------------------------

	/** Ajuste le niveau de détail de la NURBS.
	 * @param value valeur comprise entre 1 et LOD_VALUES.
	 */
	void setLOD(u_char value)
	{
		ForEachIter(itFire, CFireSourceList, m_vpFireSources)
		{
			(*itFire)->setLOD(value);
		}
	}

	/** Affectation du coefficient multiplicateur de la FDF.
	 * @param value Coefficient.
	 */
	void setInnerForce(float value)
	{
		ForEachIter(itFire, CFireSourceList, m_vpFireSources)
		{
			(*itFire)->setInnerForce(value);
		}
	}

	/** Affectation de la FDF.
	 * @param value FDF.
	 */
	void setFDF(int value)
	{
		ForEachIter(itFire, CFireSourceList, m_vpFireSources)
		{
			(*itFire)->setFDF(value);
		}
	}

	/** Affectation de la méthode de perturbation.
	 * @param value Perturbation.
	 */
	void setPerturbateMode(char value)
	{
		ForEachIter(itFire, CFireSourceList, m_vpFireSources)
		{
			(*itFire)->setPerturbateMode(value);
		}
	}

	/** Affectation de la durée de vie des squelettes guides.
	 * @param value Durée de vie en itérations.
	 */
	void setLeadLifeSpan(uint value)
	{
		ForEachIter(itFire, CFireSourceList, m_vpFireSources)
		{
			(*itFire)->setLeadLifeSpan(value);
		}
	}

	/** Affectation de la durée de vie des squelettes périphériques.
	 * @param value Durée de vie en itérations.
	 */
	void setPeriLifeSpan(uint value)
	{
		ForEachIter(itFire, CFireSourceList, m_vpFireSources)
		{
			(*itFire)->setPeriLifeSpan(value);
		}
	}

	void setBuoyancy(float value)
	{
		ForEachIter(itField, CFieldList, m_vpFields)
		{
			(*itField)->setBuoyancy(value);
		}
	}

	void setVorticity(float value)
	{
		ForEachIter(itField, CFieldList, m_vpFields)
		{
			(*itField)->setVorticity(value);
		}
	}

private :

	static Field3D* CreateField(const SolverConfig& fieldConfig, CTransform& a_rTransform);
	static IFireSource* CreateFire(	const FlameConfig& a_rFlameConfig,
									CharCPtrC a_szFilename,
									Field3D& a_rField,
									CTransform &a_rTransform,
									CScene& a_rScene,
									const CShader& a_rShadowMapShader,
									const CRenderTarget& a_rShadowRenderTarget);

//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

	/** Fires */
	typedef list <IFireSource *> CFireSourceList;
	CFireSourceList m_vpFireSources;

	/** Fields */
	typedef list <Field3D *> CFieldList;
	CFieldList m_vpFields;

	/** Il se peut que le luminaire ne soit pas un objet physique, dans ce cas ce booléen est à false */
	bool m_hasLuminary;

	/** Référence vers le noeud de transformation */
	CTransform *m_pTransform;
};


#endif

/* ILight.h: interface for the ILight class. */

#ifndef SOURCE_H
#define SOURCE_H

class ILight;
class CCamera;
class CRenderTarget;
class CSpatialGraph;
class CRenderList;

#include "../Maths/CVector.hpp"
#include "../Physics/CEnergy.hpp"

#include "CTransform.hpp"
#include "../Shading/Glsl.hpp"

#include <vector>
#include <string>

using namespace std;

/**
 * Classe de base repr&eacute;sentant une source lumineuse.
 * Une source lumineuse peut &ecirc;tre de cinq types diff&eacute;rents (pointLight, areaLight, etc.) et
 * chaque type poss&egrave;de ses attributs propres pour d&eacute;finir compl&egrave;tement une source.
 *
 * @author	Flavien Bridault
 * @version	%I%, %G%
 * @since	1.0
 * @see Material, CEnergy
 */
class ILight : public ISceneItem
{
protected:
	/**
	 * Constructeur param&eacute;trique pour une source de type pointLight.
	 * @param P position de la source.
	 * @param I intensit&eacute; lumineuse de la source.
	 */
	ILight (CTransform& a_rTransform, const CEnergy & I, GLuint depthMapSize, const CRenderTarget& shadowRenderTarget);

public:

	/**
	 * Destructeur par d&eacute;faut.
	 */
	virtual ~ILight ()
	{
		delete [] m_lightProjectionMatrix;
		delete [] m_lightModelViewMatrix;
		glDeleteLists(m_volumeDL, 1);
	}

//---------------------------------------------------------------------------------------------------------------------
//  Inherited methods
//---------------------------------------------------------------------------------------------------------------------

	/** Donne l'englobant de l'objet.
	 * @param max Retourne le coin supérieur de l'englobant.
	 * @param min Retourne le coin inférieur de l'englobant.
	 */
	virtual void GetBoundingBox(CPoint& a_rMax, CPoint& a_rMin) const {}

	virtual void Render() const;

	virtual void Move(CVector const& a_rDir){};

	/** Calcule la visibilité de l'objet
	 * @param view Référence sur la caméra
	 */
	virtual void ComputeVisibility(const CCamera &a_rView) {};

//---------------------------------------------------------------------------------------------------------------------
//  Virtual methods
//---------------------------------------------------------------------------------------------------------------------

	virtual void chooseForwardShader( const CShader* spotShader, const CShader* omniShader) = 0;
	virtual void chooseDeferredShader(const CShader* spotShader, const CShader* omniShader) = 0;

	/** Rendu à partir du point de vue de la source. */
	virtual void CastShadows(CCamera &camera, CRenderList const& a_rRenderList, GLfloat *invModelViewMatrix) = 0;

	virtual void renderLightVolume(const CCamera& camera) const = 0;

	virtual void preRendering(bool shadows) const;
	virtual void postRendering(bool shadows) const;

	virtual void DrawForSelection() const;

	virtual CVector generateRandomRay() const = 0;

//---------------------------------------------------------------------------------------------------------------------
//  Specific methods
//---------------------------------------------------------------------------------------------------------------------

	/**
	 * Lecture de l'intensit&eacute; lumineuse de la source.
	 */
	const CEnergy& GetEnergy () const
	{
		return m_lightEnergy;
	};
	/**
	 * Lecture de l'intensit&eacute; lumineuse de la source.
	 */
	CEnergy& GrabEnergy ()
	{
		return m_lightEnergy;
	};
	/**
	 * Lecture de l'intensit&eacute; lumineuse de la source.
	 */
	void setEnergy (const CEnergy& a_rEnergy)
	{
		m_lightEnergy = a_rEnergy;
	};

	void GetMatrices(GLfloat** projection, GLfloat** modelView)
	{
		*projection = m_lightProjectionMatrix;
		*modelView = m_lightModelViewMatrix;
	}
	void toggle()
	{
		m_bEnabled = !m_bEnabled;
	};

	bool IsEnabled() const
	{
		return m_bEnabled;
	};

	void ToggleShader()
	{
		if (m_shader == m_forwardShader)
			m_shader = m_deferredShader;
		else
			m_shader = m_forwardShader;
	}

	CTransform const& GetTransform() const
	{
		return m_rTransform;
	}
	CTransform& GrabTransform()
	{
		return m_rTransform;
	}

protected:

//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

	CEnergy m_lightEnergy;
	CTransform& m_rTransform;

	bool m_bEnabled;
	GLfloat *m_lightProjectionMatrix, *m_lightModelViewMatrix;
	GLuint m_depthMapW, m_depthMapH;

	const CShader* m_shader, *m_forwardShader, *m_deferredShader;
	const CRenderTarget &m_shadowRenderTarget;
	GLuint m_volumeDL;
	GLUquadricObj *m_quadObj;
};

/*********************************************************************************************************************/
/**		Class COmniLight          			  	 														   			 */
/*********************************************************************************************************************/
class COmniLight : public ILight
{
public:
	COmniLight (CTransform& a_rTransform, const CEnergy& I, GLuint depthMapSize, const CShader& genShadowCubeMapShader,
				const CRenderTarget& shadowRenderTarget);
	/** Destructeur par d&eacute;faut. */
	virtual ~COmniLight ();


//---------------------------------------------------------------------------------------------------------------------
//  Inherited methods
//---------------------------------------------------------------------------------------------------------------------

	virtual void chooseForwardShader( const CShader* spotShader, const CShader* omniShader);
	virtual void chooseDeferredShader(const CShader* spotShader, const CShader* omniShader);

	/** Rendu à partir du point de vue de la source. */
	virtual void CastShadows(CCamera &camera, CRenderList const& a_rRenderList, GLfloat *invModelViewMatrix);

	virtual void renderLightVolume(const CCamera& camera) const;

	virtual void preRendering(bool shadows) const;

	virtual CVector generateRandomRay() const;

private:

//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

	const CShader& m_genShadowCubeMapShader;
	float m_radius;
};

class CSpotLight : public ILight
{
public:
	/**
	 * Constructeur param&eacute;trique pour une source de type spotLight.
	 * @param P position de la source.
	 * @param I intensit&eacute; lumineuse de la source.
	 */
	CSpotLight (CTransform& a_rTransform, const CVector& direction, const CEnergy & I, float cutoff,
				GLuint depthMapSize, const CRenderTarget& shadowRenderTarget);

	/**
	 * Destructeur par d&eacute;faut.
	 */
	virtual ~CSpotLight ()
	{
	};

//---------------------------------------------------------------------------------------------------------------------
//  Inherited methods
//---------------------------------------------------------------------------------------------------------------------

	virtual void chooseForwardShader( const CShader* spotShader, const CShader* omniShader);
	virtual void chooseDeferredShader(const CShader* spotShader, const CShader* omniShader);

	/** Rendu à partir du point de vue de la source. */
	virtual void CastShadows(CCamera& camera, CRenderList const& a_rRenderList, GLfloat *invModelViewMatrix);

	virtual void renderLightVolume(const CCamera& camera) const;

	virtual void preRendering(bool shadows) const;

	virtual CVector generateRandomRay() const;

//---------------------------------------------------------------------------------------------------------------------
//  Specific methods
//---------------------------------------------------------------------------------------------------------------------

	const CPoint& GetDirection () const
	{
		return m_direction;
	}

private:

//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

	CVector m_direction;
	CVector m_iAxis,m_jAxis;
	float m_cutoff, m_angle;
	float m_height, m_base;
};
#endif

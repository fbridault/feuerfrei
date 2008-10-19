/* ILight.h: interface for the ILight class. */

#ifndef SOURCE_H
#define SOURCE_H

class ILight;
class CCamera;
class CRenderTarget;

#include "../Maths/CVector.hpp"
#include "../Physics/CEnergy.hpp"

#include "CObject.hpp"
#include "../Shading/Glsl.hpp"

#include <vector>
#include <string>

using namespace std;

class LightFactory
{
private:
	LightFactory();
	~LightFactory();
public:
	static ILight* getInstance(const char *type, GLuint depthMapSize, const CShader* genShadowCubeMapShader,
	                           const CRenderTarget *shadowRenderTarget);
};

/**
 * Classe de base repr&eacute;sentant une source lumineuse.
 * Une source lumineuse peut &ecirc;tre de cinq types diff&eacute;rents (pointLight, areaLight, etc.) et
 * chaque type poss&egrave;de ses attributs propres pour d&eacute;finir compl&egrave;tement une source.
 *
 * @author	Christophe Cassagnab&egrave;re modifi&eacute; par Flavien Bridault
 * @version	%I%, %G%
 * @since	1.0
 * @see Material, Energy
 */
class ILight : public CSceneItem
{
protected:
	/**
	 * Constructeur param&eacute;trique pour une source de type pointLight.
	 * @param P position de la source.
	 * @param I intensit&eacute; lumineuse de la source.
	 */
	ILight (const CPoint & P, const Energy & I, GLuint depthMapSize, const CRenderTarget *shadowRenderTarget);

public:
	virtual void chooseForwardShader( const CShader* spotShader, const CShader* omniShader) = 0;
	virtual void chooseDeferredShader(const CShader* spotShader, const CShader* omniShader) = 0;

	/**
	 * Destructeur par d&eacute;faut.
	 */
	virtual ~ILight ()
	{
		delete [] m_lightProjectionMatrix;
		delete [] m_lightModelViewMatrix;
		glDeleteLists(m_volumeDL, 1);
	};

	virtual void draw() const;
	virtual void drawForSelection() const;

	/**
	 * Lecture de l'intensit&eacute; lumineuse de la source.
	 */
	const Energy& getEnergy () const
	{
		return m_lightEnergy;
	};

	void getMatrices(GLfloat** projection, GLfloat** modelView)
	{
		*projection = m_lightProjectionMatrix;
		*modelView = m_lightModelViewMatrix;
	}

	virtual void updateModelViewPosition(GLfloat m[16]);
	void move (float x, float y, float z);

	/** Rendu à partir du point de vue de la source. */
	virtual void castShadows(CCamera &camera, const CScene& scene, GLfloat *invModelViewMatrix) = 0;

	virtual void renderLightVolume(const CCamera* const camera) const = 0;

	virtual void preRendering(bool shadows) const;
	virtual void postRendering(bool shadows) const;

	void toggle() {
		m_enabled = !m_enabled;
	};

	bool isEnabled() {
		return m_enabled;
	};

	void toggleShader()
	{
		if (m_shader == m_forwardShader)
			m_shader = m_deferredShader;
		else
			m_shader = m_forwardShader;
	}

	virtual CVector generateRandomRay() const = 0;

protected:
	Energy m_lightEnergy;

	CPoint m_modelViewPosition;
	bool m_enabled;
	GLfloat *m_lightProjectionMatrix, *m_lightModelViewMatrix;
	GLuint m_depthMapW, m_depthMapH;

	const CShader* m_shader, *m_forwardShader, *m_deferredShader;
	const CRenderTarget *m_shadowRenderTarget;
	GLuint m_volumeDL;
	GLUquadricObj *m_quadObj;
};

class COmniLight : public ILight
{
public:
	COmniLight (const CPoint& P, const Energy& I, GLuint depthMapSize, const CShader* genShadowCubeMapShader,
	           const CRenderTarget *shadowRenderTarget);
	/** Destructeur par d&eacute;faut. */
	virtual ~COmniLight ();

	void chooseForwardShader( const CShader* spotShader, const CShader* omniShader);
	void chooseDeferredShader(const CShader* spotShader, const CShader* omniShader);

	/** Rendu à partir du point de vue de la source. */
	void castShadows(CCamera &camera, const CScene& scene, GLfloat *invModelViewMatrix);

	void renderLightVolume(const CCamera* const camera) const;

	void preRendering(bool shadows) const;

	CVector generateRandomRay() const;

private:
	const CShader *m_genShadowCubeMapShader;
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
	CSpotLight (const CPoint & P, const CVector& direction, const Energy & I, float cutoff,
	           GLuint depthMapSize, const CRenderTarget *shadowRenderTarget);

	/**
	 * Destructeur par d&eacute;faut.
	 */
	virtual ~CSpotLight ()
	{
	};

	void chooseForwardShader( const CShader* spotShader, const CShader* omniShader);
	void chooseDeferredShader(const CShader* spotShader, const CShader* omniShader);

	void updateModelViewPosition(GLfloat m[16]);

	const CPoint& getDirection () const
	{
		return m_direction;
	};

	/** Rendu à partir du point de vue de la source. */
	void castShadows(CCamera &camera, const CScene& scene, GLfloat *invModelViewMatrix);

	void renderLightVolume(const CCamera* const camera) const;

	void preRendering(bool shadows) const;

	CVector generateRandomRay() const;

private:
	CVector m_direction, m_modelViewDirection;
	CVector m_iAxis,m_jAxis;
	float m_cutoff, m_angle;
	float m_height, m_base;
};
#endif

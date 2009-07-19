/* ILight.cpp: implementation of the ILight class. */
#include "Light.hpp"

#include <string.h>

#include "../Utility/GraphicsFn.hpp"
#include "CCamera.hpp"
#include "CScene.hpp"
#include "CRenderList.hpp"
#include "../Shading/CRenderTarget.hpp"

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
ILight::ILight (CTransform& a_rTransform, const CEnergy & I, GLuint depthMapSize, const CRenderTarget& shadowRenderTarget) :
	ISceneItem(NRenderType::eImmediate),
	m_rTransform(a_rTransform),
	m_shadowRenderTarget(shadowRenderTarget)
{
	m_lightEnergy = I;
	m_depthMapW = m_depthMapH = depthMapSize;
	m_lightProjectionMatrix = new GLfloat[16];
	m_lightModelViewMatrix = new GLfloat[16];

	m_bEnabled = true;

	m_rTransform.AddChild(this);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void ILight::Render() const
{
	CPoint const& rPosition = m_rTransform.GetWorldPosition();
	CPoint const& rScale = m_rTransform.GetScale();
	glPushMatrix();
	glTranslatef(rPosition.x,rPosition.y, rPosition.z);
	glScalef(rScale.x, rScale.y, rScale.z);
	if (IsSelected())
		glColor3f(0.95,0.0,.2);
	else
		glColor3f(0.95,1.0,.2);
	UGraphicsFn::SolidSphere(0.003, 10, 10);
	glPopMatrix();
}


//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void ILight::DrawForSelection() const
{
	CPoint const& rPosition = m_rTransform.GetWorldPosition();
	CPoint const& rScale = m_rTransform.GetScale();
	glPushMatrix();
	glTranslatef(rPosition.x,rPosition.y, rPosition.z);
	glScalef(rScale.x, rScale.y, rScale.z);
	glPushName(GetItemName());
	UGraphicsFn::SolidSphere(0.003, 10, 10);
	glPopName();
	glPopMatrix();
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void ILight::preRendering(bool shadows) const
{
	m_shadowRenderTarget.bindTexture();
	assert(m_shader != NULL);

	CPoint position = m_rTransform.GetWorldPosition() * g_modelViewMatrix;
	CShaderState& rShaderState = CShaderState::GetInstance();

	rShaderState.Enable(*m_shader);
	rShaderState.SetUniform3f("u_lightCentre", position.x,position.y,position.z);
	rShaderState.SetUniform3f("u_intensity",   m_lightEnergy[0], m_lightEnergy[1] ,m_lightEnergy[2]);
	rShaderState.SetUniform1i("u_shadowMap", SHADOW_MAP_TEX_UNIT);
	if (shadows)
		rShaderState.SetUniform1i("u_shadowsEnabled",1);
	else
		rShaderState.SetUniform1i("u_shadowsEnabled",0);

	if (m_shader == m_deferredShader)
	{
		rShaderState.SetUniform1i("u_normalsTex", 1);
		rShaderState.SetUniform1i("u_positionsTex", 2);
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void ILight::postRendering(bool shadows) const
{
	CShaderState& rShaderState = CShaderState::GetInstance();
	rShaderState.Disable();

	if (shadows)
	{
		glActiveTexture(GL_TEXTURE0+SHADOW_MAP_TEX_UNIT);               // Reset shadow map texture coordinates
		glMatrixMode(GL_TEXTURE);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
}


/*********************************************************************************************************************/
/**		Class COmniLight	          			  	 													   			 */
/*********************************************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
COmniLight::COmniLight(CTransform& a_rTransform, const CEnergy &I, GLuint depthMapSize,
                       const CShader& genShadowCubeMapShader, const CRenderTarget& shadowRenderTarget) :
		ILight(a_rTransform,I,depthMapSize,shadowRenderTarget),
		m_genShadowCubeMapShader(genShadowCubeMapShader)
{
	/** Création du light volume : sphère */
	m_radius = sqrt(4.0/GetEnergy().max());
	m_volumeDL = glGenLists(1);
	glNewList(m_volumeDL, GL_COMPILE);
	UGraphicsFn::SolidSphere(m_radius,10,10);
	glEndList();
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
COmniLight::~COmniLight ()
{
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void COmniLight::chooseForwardShader (const CShader* spotShader, const CShader* omniShader)
{
	assert(omniShader != NULL);

	m_shader = m_forwardShader = omniShader;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void COmniLight::chooseDeferredShader(const CShader* spotShader, const CShader* omniShader)
{
	assert(omniShader != NULL);

	m_deferredShader = omniShader;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void COmniLight::castShadows(CCamera &camera, CRenderList const& a_rRenderList, GLfloat *invModelViewMatrix)
{
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport (0, 0, m_depthMapW, m_depthMapW);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective (90.0f, 1.0f , 0.01f, 1.5f);

	glMatrixMode(GL_MODELVIEW);

	m_shadowRenderTarget.BindTarget();
	m_shadowRenderTarget.bindChannel(0);

	CShaderState& rShaderState = CShaderState::GetInstance();
	rShaderState.Enable(m_genShadowCubeMapShader);

	CPoint const& rPosition = m_rTransform.GetWorldPosition();
	rShaderState.SetUniform3f("u_lightCentre", rPosition.x,rPosition.y,rPosition.z);

	CDrawState &rDrawState = CDrawState::GetInstance();
	rDrawState.SetShadingFilter(NShadingFilter::eAll);
	rDrawState.SetShadingType(NShadingType::eAmbient);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	camera.setFromViewPoint(rPosition,CVector( 1.0, 0.0, 0.0), CVector( 0.0, -1.0, 0.0)); // Il faut "inverser" les textures en y à cause du repère
	a_rRenderList.Render();

	m_shadowRenderTarget.bindChannel(1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	camera.setFromViewPoint(rPosition,CVector(-1.0, 0.0, 0.0), CVector( 0.0, -1.0, 0.0));
	a_rRenderList.Render();

	m_shadowRenderTarget.bindChannel(2);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	camera.setFromViewPoint(rPosition,CVector( 0.0, 1.0, 0.0), CVector( 0.0, 0.0, 1.0));
	a_rRenderList.Render();

	m_shadowRenderTarget.bindChannel(3);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	camera.setFromViewPoint(rPosition,CVector( 0.0,-1.0, 0.0), CVector( 0.0, 0.0, -1.0));
	a_rRenderList.Render();

	m_shadowRenderTarget.bindChannel(4);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	camera.setFromViewPoint(rPosition,CVector( 0.0, 0.0, 1.0), CVector( 0.0, -1.0, 0.0));
	a_rRenderList.Render();

	m_shadowRenderTarget.bindChannel(5);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	camera.setFromViewPoint(rPosition,CVector( 0.0, 0.0,-1.0), CVector( 0.0, -1.0, 0.0));
	a_rRenderList.Render();

	rShaderState.Disable();
	CRenderTarget::BindDefaultTarget();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glPopAttrib();

	glMatrixMode(GL_MODELVIEW);
	camera.setView();

	glActiveTexture(GL_TEXTURE0+SHADOW_MAP_TEX_UNIT);  	// On utilise la matrice de texture comme matrice de transformation des coordonnées
	glMatrixMode(GL_TEXTURE);               			// des points dans le repère de la lumière
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(invModelViewMatrix);     				// Et enfin on applique la matrice inverse de transformation de la caméra

	glMatrixMode(GL_MODELVIEW);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void COmniLight::preRendering(bool shadows) const
{
	GLfloat offset=2.f;
	GLfloat filter[6][3] = { {offset, 0.f, 0.f}, {-offset, 0.f, 0.f}, {0.f, offset, 0.f}, {0.f, -offset, 0.f},
		{0.f, 0.f, offset}, {0.f, 0.f, -offset}
	};
	CPoint const& rPosition = m_rTransform.GetWorldPosition();

	ILight::preRendering(shadows);

	CShaderState const& rShaderState = CShaderState::GetInstance();
	rShaderState.SetUniform1f( "u_mapSize", 1.f/(float)m_depthMapW);
	rShaderState.SetUniform3fv("u_vFilter", &filter[0][0], 6);
	rShaderState.SetUniform3f( "u_worldLightCentre", rPosition.x,rPosition.y,rPosition.z);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void COmniLight::renderLightVolume(const CCamera& camera) const
{
	/** Détermine si l'on est à l'intérieur ou à l'extérieur du volume */
	bool bIn = false;

	CPoint const& rPosition = m_rTransform.GetWorldPosition();
	CVector vecToLight = CPoint(0,0,0) - camera.GetPosition() - rPosition;
	if ( vecToLight.norm() < m_radius )
	{
		glCullFace(GL_FRONT);
		bIn = true;
	}

	glPushMatrix();
	glTranslatef(rPosition.x, rPosition.y, rPosition.z);
	glCallList(m_volumeDL);
	glPopMatrix();

	if (bIn) glCullFace(GL_BACK);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CVector COmniLight::generateRandomRay() const
{
	CVector rayDir;
	float r1,r2, gamma,kappa;

	r1 = rand()/(float)RAND_MAX;
	r2 = rand()/(float)RAND_MAX;

	gamma = sqrt(r2 * ( 1 - r2 ));
	kappa = 2*M_PI*r1;
	rayDir.x = 2*cos ( kappa ) * gamma;
	rayDir.y = 2*sin ( kappa ) * gamma;
	rayDir.z = ( 1 - 2*r2 );

	return rayDir;
}


/*********************************************************************************************************************/
/**		Class CSpotLight	          			  	 													   			 */
/*********************************************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CSpotLight::CSpotLight (CTransform& a_rTransform, const CVector& direction, const CEnergy & I, float angle,
                        GLuint depthMapSize, const CRenderTarget& shadowRenderTarget) :
	ILight(a_rTransform, I,depthMapSize,shadowRenderTarget)
{
	GLuint slices=10;
	m_direction = direction;
	m_direction.normalize();

	m_angle  = angle*DEG_TO_RAD/2.0;
	m_cutoff = cos(m_angle);

	m_jAxis=(m_direction^m_direction.GetMinCoord()).normalize();
	m_iAxis=(m_jAxis^m_direction).normalize();

	/** Création du light volume : cône */
	m_height = sqrt(10.0/GetEnergy().max());

	m_base = tanf(m_angle) * m_height;
	m_volumeDL = glGenLists(1);
	glNewList(m_volumeDL, GL_COMPILE);
	UGraphicsFn::SolidCone(m_base, m_height, slices, 1);
	/** Fermeture du cône */
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	//glRotatef(180.0f/slices, 0.0f, 0.0f, 1.0f);
	UGraphicsFn::SolidDisk(m_base, slices, 1);
	glEndList();
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CSpotLight::chooseForwardShader( const CShader* spotShader, const CShader* omniShader)
{
	assert(spotShader != NULL);

	m_shader = m_forwardShader = spotShader;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CSpotLight::chooseDeferredShader(const CShader* spotShader, const CShader* omniShader)
{
	assert(spotShader != NULL);

	m_deferredShader = spotShader;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CSpotLight::castShadows(CCamera &camera, CRenderList const& a_rRenderList, GLfloat *invModelViewMatrix)
{
	CVector up(0,0,1);
	CPoint const& rPosition = m_rTransform.GetWorldPosition();

	// Check if dir and up vectors are colinear for glutLookAt openGL call
	// We compute the trapezoid area = norm of the cross product */
	CVector area = m_direction^up;
	if (area.norm() < EPSILON)
		up=CVector(0,1,0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective (90.0f, m_depthMapW/(float)(m_depthMapH), 0.01f, 2.0f);

	glMatrixMode(GL_MODELVIEW);
	camera.setFromViewPoint(rPosition,m_direction,up);

	glGetFloatv (GL_PROJECTION_MATRIX, m_lightProjectionMatrix); // On récupère la matrice de projection à partir de la source
	glGetFloatv (GL_MODELVIEW_MATRIX, m_lightModelViewMatrix);   // On récupère la matrice de transformation à partir de la source

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport (0, 0, m_depthMapW, m_depthMapH);

	CDrawState &rDrawState = CDrawState::GetInstance();
	rDrawState.SetShadingFilter(NShadingFilter::eAll);
	rDrawState.SetShadingType(NShadingType::eAmbient);

	m_shadowRenderTarget.BindTarget();
	glClear(GL_DEPTH_BUFFER_BIT);
	a_rRenderList.Render();
	CRenderTarget::BindDefaultTarget();

	glPopAttrib();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	camera.setView();

	glActiveTexture(GL_TEXTURE0+SHADOW_MAP_TEX_UNIT);  // On utilise la matrice de texture comme matrice de transformation des coordonnées
	glMatrixMode(GL_TEXTURE);               // des points dans le repère de la lumière
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.5, 0.5, 0.5-0.001);            // Les coordonnées normalisées sont dans [-1.0, 1.0], or on les veut dans [0.0, 1.0]
	glScalef(0.5, 0.5, 0.5);                // pour indexer la shadow map. (x,y) comme UVs et (z) comme profondeur pour comparaison
	glMultMatrixf(m_lightProjectionMatrix); // On replace les coordonnées dans le repère de la lumière
	glMultMatrixf(m_lightModelViewMatrix);
	glMultMatrixf(invModelViewMatrix);      // Et enfin on applique la matrice inverse de transformation de la caméra

	glMatrixMode(GL_MODELVIEW);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CSpotLight::preRendering(bool shadows) const
{
	ILight::preRendering(shadows);
	assert(m_shader != NULL);

	CVector direction = m_direction * g_modelViewMatrix;

	CShaderState const& rShaderState = CShaderState::GetInstance();
	rShaderState.SetUniform3f("u_spotDirection", direction.x, direction.y, direction.z);
	rShaderState.SetUniform1f("u_spotCutoff", m_cutoff);
	rShaderState.SetUniform2f("u_texmapscale",1.0/(float)m_depthMapW,1.0/(float)m_depthMapH);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CSpotLight::renderLightVolume(const CCamera& camera) const
{
	/** Chercher rotation de la direction initiale du cône en Z, vers m_direction. */
	CVector dir(0.0, 0.0, -1.0);
	float angle = acos(dir * m_direction);  // m_direction est déjà normalisé
	bool in=false;
	CVector axeRot = dir ^ m_direction;
	CPoint const& rPosition = m_rTransform.GetWorldPosition();

	/** Détermine si l'on est à l'intérieur ou à l'extérieur du volume */
	CVector vecToLight = CPoint(0,0,0)-camera.GetPosition() - rPosition;
	vecToLight.normalize();

	if ( (vecToLight * m_direction) > m_cutoff)
	{
		glCullFace(GL_FRONT);
		in = true;
	}

	glPushMatrix();
	glTranslatef(rPosition.x, rPosition.y, rPosition.z);
	glRotatef( angle * RAD_TO_DEG, axeRot.x, axeRot.y, axeRot.z);
	glTranslatef(0.0, 0.0, -m_height);
	glCallList(m_volumeDL);
	glPopMatrix();

	if (in) glCullFace(GL_BACK);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CVector CSpotLight::generateRandomRay() const
{
	CVector dir,rayDir;
	float r1,r2;
	float gamma, eta, kappa, nu;


	r1 = rand()/(float)RAND_MAX;
	r2 = rand()/(float)RAND_MAX;

	gamma=1-cos(m_angle);
	eta=1-r2*gamma;
	kappa=sqrt(1-eta*eta);
	nu=2*M_PI*r1;

	dir.x = cos ( nu ) * kappa;
	dir.y = sin ( nu ) * kappa;
	dir.z = 1-r2*gamma;

	rayDir.x = m_iAxis.x*dir.x + m_jAxis.x*dir.y + m_direction.x*dir.z;
	rayDir.y = m_iAxis.y*dir.x + m_jAxis.y*dir.y + m_direction.y*dir.z;
	rayDir.z = m_iAxis.z*dir.x + m_jAxis.z*dir.y + m_direction.z*dir.z;

	return rayDir;
}

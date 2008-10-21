/* ILight.cpp: implementation of the ILight class. */
#include "Light.hpp"

#include <string.h>

#include "../Utility/GraphicsFn.hpp"
#include "CCamera.hpp"
#include "CScene.hpp"
#include "../Shading/CRenderTarget.hpp"


ILight* LightFactory::getInstance(const char *type, GLuint depthMapSize, const CShader* genShadowCubeMapShader,
                                  const CRenderTarget *shadowRenderTarget)
{
	CPoint p, i;
	CVector d;

	p.randomize(-0.4, 0.4);
	p.y = (p.y)/2.0;

// TODO: Replace strcmp
	if (!strcmp(type,"spot"))
	{
		i.randomize( 0.01, 0.3);
		d.randomize(-1.0, 1.0);
		return new CSpotLight(p,d,Energy(i.x,i.y,i.z),0.9, depthMapSize, shadowRenderTarget);
	}
	else
		if (!strcmp(type,"omni"))
		{
			i.randomize( 0.01, 0.2);
			return new COmniLight(p,Energy(i.x,i.y,i.z),depthMapSize, genShadowCubeMapShader, shadowRenderTarget);
		}
		else
		{
			cerr << "(EE) Cannot create light, bad type" << endl;
			return NULL;
		}
}


ILight::ILight (const CPoint & P, const Energy & I, GLuint depthMapSize, const CRenderTarget *shadowRenderTarget) :
		CSceneItem(P)
{
	assert(shadowRenderTarget != NULL);

	m_lightEnergy = I;
	m_glName = CScene::glNameCounter++;
	m_depthMapW = m_depthMapH = depthMapSize;
	m_lightProjectionMatrix = new GLfloat[16];
	m_lightModelViewMatrix = new GLfloat[16];

	m_shadowRenderTarget = shadowRenderTarget;

	m_enabled = true;
	m_selected = false;
};


void ILight::draw() const
{
	glPushMatrix();
	glTranslatef(m_position.x, m_position.y, m_position.z);
	if (m_selected)
		glColor3f(0.95,0.0,.2);
	else
		glColor3f(0.95,1.0,.2);
	UGraphicsFn::SolidSphere(0.003, 10, 10);
	glPopMatrix();
}


void ILight::drawForSelection() const
{
	glPushMatrix();
	glTranslatef(m_position.x, m_position.y, m_position.z);
	glPushName(m_glName);
	UGraphicsFn::SolidSphere(0.003, 10, 10);
	glPopName();
	glPopMatrix();
}


void ILight::move (float x, float y, float z)
{
	CSceneItem::move(x,y,z);
	updateModelViewPosition(g_modelViewMatrix);
}


void ILight::updateModelViewPosition(GLfloat m[16])
{
	m_modelViewPosition = m_position * m;
}


void ILight::preRendering(bool shadows) const
{
	m_shadowRenderTarget->bindTexture();
	m_shader->Enable();
	m_shader->SetUniform3f("u_lightCentre", m_modelViewPosition.x,m_modelViewPosition.y,m_modelViewPosition.z);
	m_shader->SetUniform3f("u_intensity",   m_lightEnergy[0], m_lightEnergy[1] ,m_lightEnergy[2]);
	m_shader->SetUniform1i("u_shadowMap",   SHADOW_MAP_TEX_UNIT);
	if (shadows)
		m_shader->SetUniform1i("u_shadowsEnabled",1);
	else
		m_shader->SetUniform1i("u_shadowsEnabled",0);


	if (m_shader == m_deferredShader)
	{
		m_deferredShader->SetUniform1i("u_normalsTex", 1);
		m_deferredShader->SetUniform1i("u_positionsTex", 2);
	}
}


void ILight::postRendering(bool shadows) const
{
	m_shader->Disable();
	if (shadows)
	{
		glActiveTexture(GL_TEXTURE0+SHADOW_MAP_TEX_UNIT);               // Reset shadow map texture coordinates
		glMatrixMode(GL_TEXTURE);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
}

/***************************************************************************************************/
/******************************************* OMNILIGHT *********************************************/
/***************************************************************************************************/
COmniLight::COmniLight(const CPoint &P, const Energy &I, GLuint depthMapSize,
                       const CShader* genShadowCubeMapShader, const CRenderTarget* shadowRenderTarget) :
		ILight(P,I,depthMapSize,shadowRenderTarget)
{
	assert(genShadowCubeMapShader != NULL);

	m_genShadowCubeMapShader = genShadowCubeMapShader;

	/** Création du light volume : sphère */
	m_radius = sqrt(4.0/getEnergy().max());
	m_volumeDL = glGenLists(1);
	glNewList(m_volumeDL, GL_COMPILE);
	UGraphicsFn::SolidSphere(m_radius,10,10);
	glEndList();
}


COmniLight::~COmniLight ()
{
}


void COmniLight::chooseForwardShader (const CShader* spotShader, const CShader* omniShader)
{
	assert(omniShader != NULL);

	m_shader = m_forwardShader = omniShader;
}


void COmniLight::chooseDeferredShader(const CShader* spotShader, const CShader* omniShader)
{
	assert(omniShader != NULL);

	m_deferredShader = omniShader;
}


void COmniLight::castShadows(CCamera &camera, const CScene& scene, GLfloat *invModelViewMatrix)
{
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport (0, 0, m_depthMapW, m_depthMapW);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective (90.0f, 1.0f , 0.01f, 1.5f);

	glMatrixMode(GL_MODELVIEW);

	m_shadowRenderTarget->bindTarget();
	m_shadowRenderTarget->bindChannel(0);

	m_genShadowCubeMapShader->Enable();
	m_genShadowCubeMapShader->SetUniform3f("u_lightCentre", m_position.x,m_position.y,m_position.z);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	camera.setFromViewPoint(m_position,CVector( 1.0, 0.0, 0.0), CVector( 0.0, -1.0, 0.0)); // Il faut "inverser" les textures en y à cause du repère
	scene.drawSceneWT();

	m_shadowRenderTarget->bindChannel(1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	camera.setFromViewPoint(m_position,CVector(-1.0, 0.0, 0.0), CVector( 0.0, -1.0, 0.0));
	scene.drawSceneWT();

	m_shadowRenderTarget->bindChannel(2);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	camera.setFromViewPoint(m_position,CVector( 0.0, 1.0, 0.0), CVector( 0.0, 0.0, 1.0));
	scene.drawSceneWT();

	m_shadowRenderTarget->bindChannel(3);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	camera.setFromViewPoint(m_position,CVector( 0.0,-1.0, 0.0), CVector( 0.0, 0.0, -1.0));
	scene.drawSceneWT();

	m_shadowRenderTarget->bindChannel(4);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	camera.setFromViewPoint(m_position,CVector( 0.0, 0.0, 1.0), CVector( 0.0, -1.0, 0.0));
	scene.drawSceneWT();

	m_shadowRenderTarget->bindChannel(5);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	camera.setFromViewPoint(m_position,CVector( 0.0, 0.0,-1.0), CVector( 0.0, -1.0, 0.0));
	scene.drawSceneWT();

	m_genShadowCubeMapShader->Disable();
	m_shadowRenderTarget->bindDefaultTarget();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glPopAttrib();

	glMatrixMode(GL_MODELVIEW);
	camera.setView();

	glActiveTexture(GL_TEXTURE0+SHADOW_MAP_TEX_UNIT);  // On utilise la matrice de texture comme matrice de transformation des coordonnées
	glMatrixMode(GL_TEXTURE);               // des points dans le repère de la lumière
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(invModelViewMatrix);      // Et enfin on applique la matrice inverse de transformation de la caméra

	glMatrixMode(GL_MODELVIEW);
}


void COmniLight::preRendering(bool shadows) const
{
	GLfloat offset=2.0f;
	GLfloat filter[6][3] = { {offset, 0.0, 0.0}, {-offset, 0.0, 0.0}, {0.0, offset, 0.0}, {0.0, -offset, 0.0},
		{0.0, 0.0, offset}, {0.0, 0.0, -offset}
	};
	ILight::preRendering(shadows);
	m_shader->SetUniform1f( "u_mapSize",1.0/(float)m_depthMapW);
	m_shader->SetUniform3fv("u_vFilter", &filter[0][0], 6);
	m_shader->SetUniform3f( "u_worldLightCentre", m_position.x,m_position.y,m_position.z);
}


void COmniLight::renderLightVolume(const CCamera* const camera) const
{
	/** Détermine si l'on est à l'intérieur ou à l'extérieur du volume */
	bool in=false;
	CVector vecToLight = camera->getPosition() - m_position;
	if ( vecToLight.norm() < m_radius )
	{
		glCullFace(GL_FRONT);
		in = true;
	}

	glPushMatrix();
	glTranslatef(m_position.x, m_position.y, m_position.z);
	glCallList(m_volumeDL);
	glPopMatrix();

	if (in) glCullFace(GL_BACK);
}


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


/***************************************************************************************************/
/******************************************* SPOTLIGHT *********************************************/
/***************************************************************************************************/
CSpotLight::CSpotLight (const CPoint & P, const CVector& direction, const Energy & I, float angle,
                        GLuint depthMapSize, const CRenderTarget* shadowRenderTarget) :
		ILight(P,I,depthMapSize,shadowRenderTarget)
{
	GLuint slices=10;
	m_direction = direction;
	m_direction.normalize();

	m_angle  = angle*DEG_TO_RAD/2.0;
	m_cutoff = cos(m_angle);

	m_jAxis=(m_direction^m_direction.getMinCoord()).normalize();
	m_iAxis=(m_jAxis^m_direction).normalize();

	/** Création du light volume : cône */
	m_height = sqrt(10.0/getEnergy().max());

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


void CSpotLight::chooseForwardShader( const CShader* spotShader, const CShader* omniShader)
{
	assert(spotShader != NULL);

	m_shader = m_forwardShader = spotShader;
}

void CSpotLight::chooseDeferredShader(const CShader* spotShader, const CShader* omniShader)
{
	assert(spotShader != NULL);

	m_deferredShader = spotShader;
}


void CSpotLight::updateModelViewPosition(GLfloat m[16])
{
	ILight::updateModelViewPosition(m);
	m_modelViewDirection = m_direction * m;
}


void CSpotLight::castShadows(CCamera &camera, const CScene& scene, GLfloat *invModelViewMatrix)
{
	CVector up(0,0,1);

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
	camera.setFromViewPoint(m_position,m_direction,up);

	glGetFloatv (GL_PROJECTION_MATRIX, m_lightProjectionMatrix); // On récupère la matrice de projection à partir de la source
	glGetFloatv (GL_MODELVIEW_MATRIX, m_lightModelViewMatrix);   // On récupère la matrice de transformation à partir de la source

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport (0, 0, m_depthMapW, m_depthMapH);

	m_shadowRenderTarget->bindTarget();
	glClear(GL_DEPTH_BUFFER_BIT);
	scene.drawSceneWT();
	m_shadowRenderTarget->bindDefaultTarget();

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


void CSpotLight::preRendering(bool shadows) const
{
	ILight::preRendering(shadows);

	m_shader->SetUniform3f("u_spotDirection", m_modelViewDirection.x, m_modelViewDirection.y, m_modelViewDirection.z);
	m_shader->SetUniform1f("u_spotCutoff", m_cutoff);
	m_shader->SetUniform2f("u_texmapscale",1.0/(float)m_depthMapW,1.0/(float)m_depthMapH);
}


void CSpotLight::renderLightVolume(const CCamera* const camera) const
{
	/** Chercher rotation de la direction initiale du cône en Z, vers m_direction. */
	CVector dir(0.0, 0.0, -1.0);
	float angle = acos(dir * m_direction);  // m_direction est déjà normalisé
	bool in=false;
	CVector axeRot = dir ^ m_direction;

	/** Détermine si l'on est à l'intérieur ou à l'extérieur du volume */
	CVector vecToLight = camera->getPosition() - m_position;
	vecToLight.normalize();

	if ( (vecToLight * m_direction) > m_cutoff)
	{
		glCullFace(GL_FRONT);
		in = true;
	}

	glPushMatrix();
	glTranslatef(m_position.x, m_position.y, m_position.z);
	glRotatef( angle * RAD_TO_DEG, axeRot.x, axeRot.y, axeRot.z);
	glTranslatef(0.0, 0.0, -m_height);
	glCallList(m_volumeDL);
	glPopMatrix();

	if (in) glCullFace(GL_BACK);
}


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

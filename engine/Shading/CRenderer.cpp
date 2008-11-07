#include "CRenderer.hpp"

#include "Glsl.hpp"
#include "CRenderTarget.hpp"
#include "../Scene/CScene.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// -------------------------------------------------------
// Forward renderer constructor.
//
CForwardRenderer::CForwardRenderer(CScene& a_rScene) : IRenderer(a_rScene)
{
	m_brdfShader        = new CShader("lightingFP.glsl",    "AMBIENT");
	m_directSpotShader  = new CShader("lightingVP.glsl",    "lightingFP.glsl", "SPOT");
	m_directOmniShader  = new CShader("lightingVP.glsl",    "lightingFP.glsl", "OMNI");

	for ( uint i=0 ; i < m_rScene.getSourcesCount(); i++)
		m_rScene.getSource(i)->chooseForwardShader(m_directSpotShader,m_directOmniShader);
}


// -------------------------------------------------------
// Drawing method for direct lighting.
//
void CForwardRenderer::drawDirect(const ILight& a_rSrc) const
{
	a_rSrc.preRendering(m_bShadows);

	m_rScene.drawSceneWT();

	a_rSrc.postRendering(m_bShadows);
}

// -------------------------------------------------------
// Drawing method for brdf factor.
//
void CForwardRenderer::drawBrdf() const
{
	m_brdfShader->SetUniform1i("u_textureObjet",OBJECT_TEX_UNIT);
	m_brdfShader->SetUniform1i("u_textureEnabled",1);
	m_rScene.drawSceneTEX();
	m_brdfShader->SetUniform1i("u_textureEnabled",0);
	m_rScene.drawSceneWTEX();
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// -------------------------------------------------------
// Forward renderer constructor.
//
CDeferredRenderer::CDeferredRenderer(CScene& a_rScene, uint width, uint height, const CCamera& camera) :
		IRenderer(a_rScene), m_camera(camera)
{
	m_brdfShader       = new CShader("deferredShadingVP.glsl","lightingFP.glsl,deferredShadingFP.glsl","AMBIENT,DEFERRED_SHADING");
	m_directSpotShader = new CShader("deferredShadingVP.glsl","lightingFP.glsl,deferredShadingFP.glsl","SPOT,DEFERRED_SHADING");
	m_directOmniShader = new CShader("deferredShadingVP.glsl","lightingFP.glsl,deferredShadingFP.glsl","OMNI,DEFERRED_SHADING");

	for ( uint i=0 ; i < m_rScene.getSourcesCount(); i++)
		m_rScene.getSource(i)->chooseDeferredShader(m_directSpotShader,m_directOmniShader);

	m_gBufferShader    = new CShader("deferredShadingVP.glsl","deferredShadingFP.glsl", "GBUFFER_GEN");
	initGBuffer(width, height);
}


// -------------------------------------------------------
// Destructor
//
CDeferredRenderer::~CDeferredRenderer()
{
	destroyGBuffer();
	delete m_gBufferShader;
}


// -------------------------------------------------------
// Initialize GBuffer
//
void CDeferredRenderer::initGBuffer(uint width, uint height)
{
	m_gBufferTargets = new CRenderTarget(width,height);
	m_gBufferTargets->addTarget("color rect rgba16f nearest",0);
	m_gBufferTargets->addTarget("color rect rgba16f nearest",1);
	m_gBufferTargets->addTarget("color rect rgba16f nearest",2);
	m_gBufferTargets->addDepthRenderBuffer();
}


// -------------------------------------------------------
// Destroy GBuffer
//
void CDeferredRenderer::destroyGBuffer()
{
	delete m_gBufferTargets;
}


// -------------------------------------------------------
// Drawing method for indirect lighting.
//
void CDeferredRenderer::drawDirect(const ILight& a_rSrc) const
{
	a_rSrc.preRendering(m_bShadows);
//  glMatrixMode(GL_PROJECTION);
//  glPushMatrix();
//  glLoadIdentity();
//  gluOrtho2D(-1, 1, -1, 1);
//  glMatrixMode(GL_MODELVIEW);
//  glPushMatrix();
//  glLoadIdentity();
//
//  glBegin(GL_QUADS);
//  glVertex3f(-1.0f,-1.0f, 0.0f);
//  glVertex3f( 1.0f,-1.0f, 0.0f);
//  glVertex3f( 1.0f, 1.0f, 0.0f);
//  glVertex3f(-1.0f, 1.0f, 0.0f);
//  glEnd();
//
//  glMatrixMode(GL_PROJECTION);
//  glPopMatrix();
//  glMatrixMode(GL_MODELVIEW);
//  glPopMatrix();

	a_rSrc.renderLightVolume(m_camera);
	a_rSrc.postRendering(m_bShadows);
}

// -------------------------------------------------------
// Drawing method for brdf factor.
//
void CDeferredRenderer::drawBrdf() const
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	m_brdfShader->Enable();
	m_brdfShader->SetUniform1i("u_colorsTex", 0);

	glBegin(GL_QUADS);
	glVertex3f(-1.0f,-1.0f,-1.0f);
	glVertex3f( 1.0f,-1.0f,-1.0f);
	glVertex3f( 1.0f, 1.0f,-1.0f);
	glVertex3f(-1.0f, 1.0f,-1.0f);
	glEnd();

	m_brdfShader->Disable();

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


// -------------------------------------------------------
// Generate GBuffer.
//
void CDeferredRenderer::genGBuffer() const
{
	m_gBufferTargets->bindTarget();

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	m_gBufferShader->Enable();

	m_gBufferShader->SetUniform1i("u_textureObjet",OBJECT_TEX_UNIT);
	m_gBufferShader->SetUniform1i("u_textureEnabled",1);
	m_rScene.drawSceneTEX();
	m_gBufferShader->SetUniform1i("u_textureEnabled",0);
	m_rScene.drawSceneWTEX();

	m_gBufferShader->Disable();

	m_gBufferTargets->bindDefaultTarget();

	m_gBufferTargets->bindTexture();
}

void CDeferredRenderer::drawTex(uint width, uint height, uint i) const
{
	glDisable (GL_DEPTH_TEST);

	glBlendFunc (GL_ONE, GL_ONE);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	m_gBufferTargets->drawTextureOnScreen(width,height,i);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable (GL_DEPTH_TEST);
}

void CDeferredRenderer::drawColorsTex(uint width, uint height) const
{
	CDeferredRenderer::drawTex(width,height,0);
}

void CDeferredRenderer::drawNormalsTex(uint width, uint height) const
{
	CDeferredRenderer::drawTex(width,height,1);
}

void CDeferredRenderer::drawPositionsTex(uint width, uint height) const
{
	CDeferredRenderer::drawTex(width,height,2);
}

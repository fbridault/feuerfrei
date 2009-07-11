#include "CRenderer.hpp"

#include "Glsl.hpp"
#include "CRenderTarget.hpp"
#include "../Scene/CScene.hpp"
#include "../Scene/CRenderList.hpp"
#include "../Scene/CSpatialGraph.hpp"


//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CForwardRenderer::CForwardRenderer(CScene& a_rScene)
{
	m_pBrdfShader        = new CShader("lightingFP.glsl",    "AMBIENT");
	m_pDirectSpotShader  = new CShader("lightingVP.glsl",    "lightingFP.glsl", "SPOT");
	m_pDirectOmniShader  = new CShader("lightingVP.glsl",    "lightingFP.glsl", "OMNI");

	for ( uint i=0 ; i < a_rScene.GetSourcesCount(); i++)
		a_rScene.GetSource(i).chooseForwardShader(m_pDirectSpotShader,m_pDirectOmniShader);
}


//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CForwardRenderer::drawDirect(const ILight& a_rSrc, CRenderList const& a_rRenderList) const
{
	a_rSrc.preRendering(m_bShadows);

	CDrawState &rDrawState = CDrawState::GetInstance();
	rDrawState.SetShadingFilter(NShadingFilter::eAll);
	rDrawState.SetShadingType(NShadingType::eAmbient);

	a_rRenderList.Render();

	a_rSrc.postRendering(m_bShadows);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CForwardRenderer::drawBrdf(CRenderList const& a_rRenderList) const
{
	CDrawState &rDrawState = CDrawState::GetInstance();
	rDrawState.SetShadingType(NShadingType::eNormal);

	CShaderState& rShaderState = CShaderState::GetInstance();
	rShaderState.Enable(*m_pBrdfShader);

	rShaderState.SetUniform1i("u_textureObjet",OBJECT_TEX_UNIT);
	rShaderState.SetUniform1i("u_textureEnabled",1);
	rDrawState.SetShadingFilter(NShadingFilter::eTextured);
	a_rRenderList.Render();
	rShaderState.SetUniform1i("u_textureEnabled",0);
	rDrawState.SetShadingFilter(NShadingFilter::eFlat);
	a_rRenderList.Render();
	rShaderState.Disable();
}



//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CDeferredRenderer::CDeferredRenderer(CScene& a_rScene, uint width, uint height, const CCamera& camera) :
	m_rCamera(camera)
{
	m_pBrdfShader       = new CShader("deferredShadingVP.glsl","lightingFP.glsl,deferredShadingFP.glsl","AMBIENT,DEFERRED_SHADING");
	m_pDirectSpotShader = new CShader("deferredShadingVP.glsl","lightingFP.glsl,deferredShadingFP.glsl","SPOT,DEFERRED_SHADING");
	m_pDirectOmniShader = new CShader("deferredShadingVP.glsl","lightingFP.glsl,deferredShadingFP.glsl","OMNI,DEFERRED_SHADING");

	for ( uint i=0 ; i < a_rScene.GetSourcesCount(); i++)
		a_rScene.GetSource(i).chooseDeferredShader(m_pDirectSpotShader,m_pDirectOmniShader);

	m_pGBufferShader    = new CShader("deferredShadingVP.glsl","deferredShadingFP.glsl", "GBUFFER_GEN");
	initGBuffer(width, height);
}



//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CDeferredRenderer::~CDeferredRenderer()
{
	destroyGBuffer();
	delete m_pGBufferShader;
}


//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CDeferredRenderer::initGBuffer(uint width, uint height)
{
	m_pGBufferTargets = new CRenderTarget(width,height);
	m_pGBufferTargets->addTarget("color rect rgba16f nearest",0);
	m_pGBufferTargets->addTarget("color rect rgba16f nearest",1);
	m_pGBufferTargets->addTarget("color rect rgba16f nearest",2);
	m_pGBufferTargets->addDepthRenderBuffer();
}


//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CDeferredRenderer::destroyGBuffer()
{
	delete m_pGBufferTargets;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CDeferredRenderer::drawDirect(const ILight& a_rSrc, CRenderList const& a_rRenderList) const
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

	a_rSrc.renderLightVolume(m_rCamera);
	a_rSrc.postRendering(m_bShadows);
}


//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CDeferredRenderer::drawBrdf(CRenderList const& a_rRenderList) const
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	CShaderState& rShaderState = CShaderState::GetInstance();
	rShaderState.Enable(*m_pBrdfShader);

	rShaderState.SetUniform1i("u_colorsTex", 0);

	glBegin(GL_QUADS);
	glVertex3f(-1.0f,-1.0f,-1.0f);
	glVertex3f( 1.0f,-1.0f,-1.0f);
	glVertex3f( 1.0f, 1.0f,-1.0f);
	glVertex3f(-1.0f, 1.0f,-1.0f);
	glEnd();

	rShaderState.Disable();

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CDeferredRenderer::genGBuffer(CRenderList const& a_rRenderList) const
{
	CDrawState &rDrawState = CDrawState::GetInstance();
	rDrawState.SetShadingType(NShadingType::eNormal);

	m_pGBufferTargets->BindTarget();

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


	CShaderState& rShaderState = CShaderState::GetInstance();
	rShaderState.Enable(*m_pGBufferShader);

	rShaderState.SetUniform1i("u_textureObjet",OBJECT_TEX_UNIT);
	rShaderState.SetUniform1i("u_textureEnabled",1);
	rDrawState.SetShadingFilter(NShadingFilter::eTextured);
	a_rRenderList.Render();
	rShaderState.SetUniform1i("u_textureEnabled",0);
	rDrawState.SetShadingFilter(NShadingFilter::eFlat);
	a_rRenderList.Render();

	rShaderState.Disable();

	CRenderTarget::BindDefaultTarget();

	m_pGBufferTargets->bindTexture();
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
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
	m_pGBufferTargets->drawTextureOnScreen(width,height,i);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable (GL_DEPTH_TEST);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CDeferredRenderer::drawColorsTex(uint width, uint height) const
{
	CDeferredRenderer::drawTex(width,height,0);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CDeferredRenderer::drawNormalsTex(uint width, uint height) const
{
	CDeferredRenderer::drawTex(width,height,1);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CDeferredRenderer::drawPositionsTex(uint width, uint height) const
{
	CDeferredRenderer::drawTex(width,height,2);
}

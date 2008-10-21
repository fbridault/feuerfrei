#include "DPengine.hpp"
#include "../interface/GLFlameCanvas.hpp"
#include <engine/Shading/CRenderTarget.hpp>

DepthPeelingEngine::DepthPeelingEngine(uint width, uint height, uint nbLayers) :
	m_oDpShader("depthPeeling.fp", ""), m_oDpRendererShader("viewportSizedTex.fp", "")
{
	m_width = width;
	m_height = height;
	m_nbLayers = m_nbLayersMax = nbLayers;
	assert(nbLayers <= DEPTH_PEELING_LAYERS_MAX);

	generateTex();
	m_flamesDisplayList = glGenLists(1);
}

DepthPeelingEngine::~DepthPeelingEngine()
{
	deleteTex();
}

void DepthPeelingEngine::deleteTex()
{
	delete m_renderTarget[0];
	delete m_renderTarget[1];
	delete m_sceneDepthRenderTarget;
	delete m_alwaysTrueDepthTex;
}

void DepthPeelingEngine::generateTex()
{
	m_renderTarget[0] = new CRenderTarget(m_width, m_height);
	m_renderTarget[1] = new CRenderTarget(m_width, m_height);

	for (uint i=0; i < m_nbLayers/2; i++)
	{
		m_renderTarget[0]->addTarget("color rect rgba nearest",0,i);
		m_renderTarget[1]->addTarget("color rect rgba nearest",0,i);
	}

	m_renderTarget[0]->addTarget("color rect rgba nearest",0,m_nbLayers/2);

	m_renderTarget[0]->addTarget("depth rect shadow nearest greater",1);
	m_renderTarget[1]->addTarget("depth rect shadow nearest greater",1);

	m_sceneDepthRenderTarget = new CRenderTarget("depth shadow rect nearest less",m_width, m_height, 2);

	m_alwaysTrueDepthTex = new CDepthTexture(GL_TEXTURE_RECTANGLE_ARB, m_width,m_height, GL_NEAREST, GLenum(GL_ALWAYS));
}

void DepthPeelingEngine::makePeels(GLFlameCanvas* const glBuffer, const CScene* const scene)
{
	uint l;

	/* On stocke la profondeur de la scène dans une texture qui servira */
	/* comme deuxième test de profondeur pour le depth peeling */
	/* Il y a donc en tout trois tests de profondeur */
	m_sceneDepthRenderTarget->bindTarget();

	glClear(GL_DEPTH_BUFFER_BIT);

	scene->drawSceneWT();

	m_curDepthTex = 0;

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ZERO);
	for (l=0; l <= m_nbLayers; l++){
		// On effectue le rendu dans le FBO
		m_renderTarget[m_curDepthTex]->bindTarget();
		m_renderTarget[m_curDepthTex]->bindChannel(l/2);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		m_sceneDepthRenderTarget->bindTexture();
		m_oDpShader.Enable();
		m_oDpShader.SetUniform1i("s_textureObjet",0);
		m_oDpShader.SetUniform1i("s_prevDepth",1);
		m_oDpShader.SetUniform1i("s_sceneDepth",2);

		/* Pour le premier layer, on construit la display list */
		/* et le premier test de profondeur est toujours vrai */
		if ( !l ){
			m_alwaysTrueDepthTex->bind(1);
			/* Dessin de la flamme */
			glNewList(m_flamesDisplayList,GL_COMPILE_AND_EXECUTE);
			glBuffer->drawFlames();
			glEndList();
		}else{
			/* Pour les layers > 0, le premier test de profondeur est effectué avec */
			/* la profondeur de la passe précédente */
			m_renderTarget[1-m_curDepthTex]->bindTexture();
			glCallList(m_flamesDisplayList);
		}
		m_oDpShader.Disable();

		m_curDepthTex = 1 - m_curDepthTex;
	}
	glDisable (GL_BLEND);
	m_renderTarget[0]->bindDefaultTarget();

	glDeleteLists(m_flamesDisplayList,1);
}

void DepthPeelingEngine::render(GLFlameCanvas* const glBuffer)
{
	glDepthFunc (GL_LEQUAL);
	glEnable (GL_BLEND);
	glBlendFunc (GL_ONE, GL_ONE);

	glEnable(GL_TEXTURE_RECTANGLE_ARB);

	m_oDpRendererShader.Enable();
	m_oDpRendererShader.SetUniform1i("text",0);

	for (int l=m_nbLayers/2-1; l >= 0 ; l--)
	{
		m_renderTarget[0]->bindTexture(l);
		glBuffer->drawFlamesBoundingBoxes();
		m_renderTarget[1]->bindTexture(l);
		glBuffer->drawFlamesBoundingBoxes();
	}
	m_renderTarget[0]->bindTexture(m_nbLayers/2);
	glBuffer->drawFlamesBoundingBoxes();

	m_oDpRendererShader.Disable();

	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glDisable (GL_BLEND);
	glDepthFunc (GL_LESS);
}

//void DepthPeelingEngine::renderFS()
//{
//  glShadeModel (GL_FLAT);
//  glDisable (GL_DEPTH_TEST);
//
//  glBlendFunc (GL_ONE, GL_ONE);
//  glMatrixMode(GL_PROJECTION);
//  glPushMatrix();
//  glLoadIdentity();
//  gluOrtho2D(-1, 1, -1, 1);
//  glMatrixMode(GL_MODELVIEW);
//  glPushMatrix();
//  glLoadIdentity();
//
//  glEnable(GL_TEXTURE_RECTANGLE_ARB);
//
//  for(int l=m_nbLayers/2-1; l >= 0 ; l--){
//    m_renderTarget[0]->bindTexture(l);
//    glBuffer->drawFlamesBoundingBoxes();
//    m_renderTarget[1]->bindTexture(l);
//    glBuffer->drawFlamesBoundingBoxes();
//  }
//  m_renderTarget[0]->bindTexture(m_nbLayers/2);
//  glBuffer->drawFlamesBoundingBoxes();
//
//  glDisable(GL_TEXTURE_RECTANGLE_ARB);
//
//  glMatrixMode(GL_PROJECTION);
//  glPopMatrix();
//  glMatrixMode(GL_MODELVIEW);
//  glPopMatrix();
//
//  glEnable (GL_DEPTH_TEST);
//  glShadeModel (GL_SMOOTH);
//}

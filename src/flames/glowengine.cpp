#include "glowengine.hpp"
#include <engine/Scene/CRenderList.hpp>

GlowEngine::GlowEngine(uint w, uint h, uint scaleFactor[GLOW_LEVELS]) :
	m_oShaderX("glowShaderX.fp",""), m_oShaderY("glowShaderY.fp", ""), m_oBlurRendererShader("viewportSizedScaledTex.fp", "")
{
  m_uiInitialWidth = w;
  m_uiInitialHeight = h;

  for(int i=0; i < GLOW_LEVELS; i++){
    m_auiScaleFactor[i] = scaleFactor[i];
    m_auiWidth[i] = w/m_auiScaleFactor[i];
    m_auiHeight[i] = h/m_auiScaleFactor[i];
  }

  generateTex();

  /* Offsets centrés pour taille texture en entrée = taille texture en sortie */
  for(int j=0; j < FILTER_SIZE; j++)
    m_afOffsets[0][j] = j-FILTER_SIZE/2;
  /* Offsets centrés pour taille texture en entrée > taille texture en sortie */
  for(int j=0; j < FILTER_SIZE; j++){
    m_afOffsets[1][j] = (j-FILTER_SIZE/2)*(int)(m_auiScaleFactor[1]);
  }
}

GlowEngine::~GlowEngine()
{
  deleteTex();
}

void GlowEngine::generateTex()
{
  for(int i=0; i < GLOW_LEVELS; i++){
    m_auiWidth[i] = m_uiInitialWidth/m_auiScaleFactor[i];
    m_auiHeight[i] = m_uiInitialHeight/m_auiScaleFactor[i];

    m_apFirstPassRT[i] = new CRenderTarget(m_auiWidth[i], m_auiHeight[i]);
    m_apFirstPassRT[i]->addTarget("color rect rgba depthbuffer linear",0);

    m_apSecondPassRT[i] = new CRenderTarget(m_auiWidth[i], m_auiHeight[i]);
    m_apSecondPassRT[i]->addTarget("color rect rgba depthbuffer linear",0);
  }
}

void GlowEngine::deleteTex()
{
  for(int i=0; i < GLOW_LEVELS; i++)
    {
      delete m_apFirstPassRT[i];
      delete m_apSecondPassRT[i];
    }
}

void GlowEngine::Blur(CRenderList const& a_rRenderList)
{
	glDepthFunc (GL_LEQUAL);

	CGlowState& rGlowState = CGlowState::GetInstance();
	rGlowState.Enable();
	rGlowState.SetPassNumber(0);

	CShaderState& rShaderState = CShaderState::GetInstance();

	/** 1 - Blur à la résolution de l'écran */

	/* Partie X du filtre */
	rShaderState.Enable(m_oShaderX);
	rShaderState.SetUniform1f("scale",m_auiScaleFactor[0]);
	rShaderState.SetUniform1fv("offsets",m_afOffsets[0],FILTER_SIZE);

	m_apSecondPassRT[0]->BindTarget();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	m_apFirstPassRT[0]->bindTexture();

	/** On dessine seulement les englobants des flammes pour indiquer à quel endroit effectuer le blur */
	a_rRenderList.Render();

	/* Partie Y du filtre */
	rShaderState.Enable(m_oShaderY);
	rShaderState.SetUniform1f("scale",m_auiScaleFactor[0]);
	rShaderState.SetUniform1fv("offsets",m_afOffsets[0],FILTER_SIZE);

	m_apFirstPassRT[0]->BindTarget();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	m_apSecondPassRT[0]->bindTexture();
	a_rRenderList.Render();

	/** 2 - Blur à une résolution inférieure */
	rGlowState.SetPassNumber(1);

	m_apSecondPassRT[1]->BindTarget();
	rShaderState.Enable(m_oShaderX);
	glEnable (GL_BLEND);
	glBlendFunc (GL_ONE, GL_ONE);

	glViewport (0, 0, m_auiWidth[1], m_auiHeight[1]);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	/* Partie X du filtre */
	/* Attention, il faut prendre les offsets correspondants à la texture à la résolution normale */
	rShaderState.SetUniform1fv("offsets",m_afOffsets[1],FILTER_SIZE);
	rShaderState.SetUniform1f("scale",m_auiScaleFactor[1]);
	m_apFirstPassRT[0]->bindTexture();
	a_rRenderList.Render();

	/* Partie Y du filtre */
	rShaderState.Enable(m_oShaderY);
	rShaderState.SetUniform1fv("offsets",m_afOffsets[0],FILTER_SIZE);
	rShaderState.SetUniform1f("scale",m_auiScaleFactor[0]);

	m_apFirstPassRT[1]->BindTarget();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	m_apSecondPassRT[1]->bindTexture();
	a_rRenderList.Render();

	rShaderState.Disable();
	glDisable(GL_BLEND);

	rGlowState.Disable();
	CRenderTarget::BindDefaultTarget();

	glDepthFunc (GL_LESS);
}

void GlowEngine::DrawBlur(CRenderList const& a_rRenderList, bool glowOnly)
{
	CShaderState& rShaderState = CShaderState::GetInstance();

	glDepthFunc (GL_LEQUAL);
	glEnable (GL_BLEND);
	glBlendFunc (GL_ONE, GL_ONE);

	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	rShaderState.Enable(m_oBlurRendererShader);
	rShaderState.SetUniform1i("text", 0);

	for(int i=glowOnly ? 1 : 0; i < GLOW_LEVELS; i++)
	{
		rShaderState.SetUniform1f("scale", 1/(float)m_auiScaleFactor[i]);
		m_apFirstPassRT[i]->bindTexture();
		a_rRenderList.Render();
	}

	rShaderState.Disable();
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glDepthFunc (GL_LESS);
	glDisable (GL_BLEND);
}

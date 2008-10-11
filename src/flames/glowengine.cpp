#include "glowengine.hpp"
#include "../interface/GLFlameCanvas.hpp"

GlowEngine::GlowEngine(uint w, uint h, uint scaleFactor[GLOW_LEVELS])
{
  m_initialWidth = w;
  m_initialHeight = h;

  m_blurFragmentShader8X.load("glowShaderX.fp");
  m_blurFragmentShader8Y.load("glowShaderY.fp");
  m_programX.attachShader(m_blurFragmentShader8X);
  m_programY.attachShader(m_blurFragmentShader8Y);
  m_programX.link();
  m_programY.link();

  m_blurRendererShader.load("viewportSizedScaledTex.fp");
  m_blurRendererProgram.attachShader(m_blurRendererShader);
  m_blurRendererProgram.link();

  for(int i=0; i < GLOW_LEVELS; i++){
    m_scaleFactor[i] = scaleFactor[i];
    m_width[i] = w/m_scaleFactor[i];
    m_height[i] = h/m_scaleFactor[i];
  }

  generateTex();

  /* Offsets centrés pour taille texture en entrée = taille texture en sortie */
  for(int j=0; j < FILTER_SIZE; j++)
    m_offsets[0][j] = j-FILTER_SIZE/2;
  /* Offsets centrés pour taille texture en entrée > taille texture en sortie */
  for(int j=0; j < FILTER_SIZE; j++){
    m_offsets[1][j] = (j-FILTER_SIZE/2)*(int)(m_scaleFactor[1]);
  }
}

GlowEngine::~GlowEngine()
{
  deleteTex();
}

void GlowEngine::generateTex()
{
  for(int i=0; i < GLOW_LEVELS; i++){
    m_width[i] = m_initialWidth/m_scaleFactor[i];
    m_height[i] = m_initialHeight/m_scaleFactor[i];

    m_firstPassRT[i] = new RenderTarget(m_width[i], m_height[i]);
    m_firstPassRT[i]->addTarget("color rect rgba depthbuffer linear",0);

    m_secondPassRT[i] = new RenderTarget(m_width[i], m_height[i]);
    m_secondPassRT[i]->addTarget("color rect rgba depthbuffer linear",0);
  }
}

void GlowEngine::deleteTex()
{
  for(int i=0; i < GLOW_LEVELS; i++)
    {
      delete m_firstPassRT[i];
      delete m_secondPassRT[i];
    }
}

void GlowEngine::blur(GLFlameCanvas* const glBuffer)
{
  glDepthFunc (GL_LEQUAL);

  /* Blur à la résolution de l'écran */
  m_programX.enable();
  m_programX.setUniform1f("scale",m_scaleFactor[0]);
  m_programX.setUniform1fv("offsets",m_offsets[0],FILTER_SIZE);

  m_secondPassRT[0]->bindTarget();
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  m_firstPassRT[0]->bindTexture();

  /** On dessine seulement les englobants des flammes pour indiquer à quel endroit effectuer le blur */
  glBuffer->drawFlamesBoundingBoxes(m_programX,0);

  m_programY.enable();
  m_programY.setUniform1f("scale",m_scaleFactor[0]);
  m_programY.setUniform1fv("offsets",m_offsets[0],FILTER_SIZE);

  m_firstPassRT[0]->bindTarget();
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  m_secondPassRT[0]->bindTexture();
  glBuffer->drawFlamesBoundingBoxes(m_programY,0);

  /* Blur à une résolution inférieure */
  m_secondPassRT[1]->bindTarget();
  m_programX.enable();
  glEnable (GL_BLEND);
  glBlendFunc (GL_ONE, GL_ONE);

  glViewport (0, 0, m_width[1], m_height[1]);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  /* Partie X du filtre */
  /* Attention, il faut prendre les offsets correspondants à la texture à la résolution normale */
  m_programX.setUniform1fv("offsets",m_offsets[1],FILTER_SIZE);
  m_programX.setUniform1f("scale",m_scaleFactor[1]);
  m_firstPassRT[0]->bindTexture();
  glBuffer->drawFlamesBoundingBoxes(m_programX,1);

  /* Partie Y du filtre */
  m_programY.enable();
  m_programY.setUniform1fv("offsets",m_offsets[0],FILTER_SIZE);
  m_programY.setUniform1f("scale",m_scaleFactor[0]);

  m_firstPassRT[1]->bindTarget();
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  m_secondPassRT[1]->bindTexture();
  glBuffer->drawFlamesBoundingBoxes(m_programY,1);

  m_programY.disable();
  glDisable(GL_BLEND);

  m_firstPassRT[1]->bindDefaultTarget();

  glDepthFunc (GL_LESS);
}

void GlowEngine::drawBlur(GLFlameCanvas* const glBuffer, bool glowOnly)
{
  glDepthFunc (GL_LEQUAL);
  glEnable (GL_BLEND);
  glBlendFunc (GL_ONE, GL_ONE);

  glEnable(GL_TEXTURE_RECTANGLE_ARB);
  m_blurRendererProgram.enable();
  m_blurRendererProgram.setUniform1i("text",0);

  for(int i=glowOnly ? 1 : 0; i < GLOW_LEVELS; i++){
    m_blurRendererProgram.setUniform1f("scale",1/(float)m_scaleFactor[i]);
    m_firstPassRT[i]->bindTexture();
    glBuffer->drawFlamesBoundingBoxes();
  }

  m_blurRendererProgram.disable();
  glDisable(GL_TEXTURE_RECTANGLE_ARB);
  glDepthFunc (GL_LESS);
  glDisable (GL_BLEND);
}

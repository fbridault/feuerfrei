#include "gammaEngine.hpp"

GammaEngine::GammaEngine(uint width, uint height, bool recompile)
{
  m_fp.load ( "gamma.fs", recompile);
  attachShader(m_fp);
  link();
  m_gamma = 1;
  
  m_width = width;
  m_height = height;
  
  m_fbo.Initialize(width, height);
  m_renderTex = new Texture(GL_TEXTURE_RECTANGLE_ARB, GL_NEAREST, width, height);
  m_fbo.Activate();
  m_fbo.ColorAttach(m_renderTex->getTexture(), 0);
  m_fbo.RenderBufferAttach();
  m_fbo.Deactivate();
}

GammaEngine::~GammaEngine()
{
  delete m_renderTex;
}

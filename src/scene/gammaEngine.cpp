#include "gammaEngine.hpp"

GammaEngine::GammaEngine(uint width, uint height, bool recompile)
{
  m_fp.load ( "gamma.fp", recompile);
  attachShader(m_fp);
  link();
  m_gamma = 1;

  m_width = width;
  m_height = height;

  m_renderTarget = new RenderTarget("color rect rgba depthbuffer nearest",width, height,0);
}

GammaEngine::~GammaEngine()
{
  delete m_renderTarget;
}

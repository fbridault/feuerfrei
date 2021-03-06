#include "CGammaFX.hpp"

CGammaFX::CGammaFX(uint width, uint height, bool recompile)
	: m_oShader("gamma.fp","")
{
  m_gamma = 1;

  m_width = width;
  m_height = height;

  m_renderTarget = new CRenderTarget("color rect rgba depthbuffer nearest",width, height,0);
}

CGammaFX::~CGammaFX()
{
  delete m_renderTarget;
}

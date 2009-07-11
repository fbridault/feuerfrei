#include "IRenderer.hpp"

#include "../Common.hpp"
#include "Glsl.hpp"

// -------------------------------------------------------
// Default Constructor
//
IRenderer::IRenderer()
{
  m_bShadows=false;
}


// -------------------------------------------------------
// Destructor
//
IRenderer::~IRenderer()
{
  delete m_pBrdfShader;
  delete m_pDirectSpotShader;
  delete m_pDirectOmniShader;
}

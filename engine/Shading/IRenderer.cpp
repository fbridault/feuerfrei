#include "IRenderer.hpp"

#include "../Common.hpp"
#include "Glsl.hpp"

// -------------------------------------------------------
// Default Constructor
//
IRenderer::IRenderer(CScene& a_rScene) : m_rScene(a_rScene)
{
  m_bShadows=false;
}


// -------------------------------------------------------
// Destructor
//
IRenderer::~IRenderer()
{
  delete m_brdfShader;
  delete m_directSpotShader;
  delete m_directOmniShader;
}

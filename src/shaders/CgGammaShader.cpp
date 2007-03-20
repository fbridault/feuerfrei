#include "CgGammaShader.hpp"

CgGammaShader::CgGammaShader(const wxString& sourceName, const wxString& shaderName, uint width, uint height,
			     const CGcontext* const context, bool recompile) :
  CgShader (sourceName, shaderName, context, CG_GL_FRAGMENT, recompile)
{
  m_paramGamma = cgGetNamedParameter(program, "gamma");
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

CgGammaShader::~CgGammaShader()
{
  delete m_renderTex;
}

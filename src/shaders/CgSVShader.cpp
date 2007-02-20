#include "CgSVShader.hpp"

CgSVShader::CgSVShader (const wxString& sourceName, const wxString& shaderName, const CGcontext* const context,
			const double* const fatnessVec, const double* const extrudeDistVec, bool recompile ):
  CgBasicVertexShader (sourceName, shaderName, context, recompile)
{  
  // Récupération des paramètres
  m_lightPos = cgGetNamedParameter (program, "LightPos");
  m_modelViewMatrix = cgGetNamedParameter (program, "ModelViewMatrix");
  m_fatness = cgGetNamedParameter (program, "Fatness");
  m_shadowExtrudeDist = cgGetNamedParameter (program, "ShadowExtrudeDist");

  setFatness (fatnessVec);
  setShadowExtrudeDist (extrudeDistVec);
}

CgSVShader::~CgSVShader ()
{
}

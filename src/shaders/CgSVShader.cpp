#include "CgSVShader.hpp"

CgSVShader::CgSVShader (const wxString& sourceName, const wxString& shaderName, CGcontext *context, double *fatnessVec, double *extrudeDistVec, bool recompile ):
  CgBasicVertexShader (sourceName, shaderName, context, recompile)
{  
  // R�cup�ration des param�tres
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
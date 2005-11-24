#include "CgSVShader.hpp"

CgSVShader::CgSVShader (const wxString& sourceName, const wxString& shaderName, CGcontext *context, bool recompile):
  CgBasicVertexShader (sourceName, shaderName, context, recompile)
{
  GLdouble fatnessVec[] = { -0.001, -0.001, -0.001, 0.0 };
  GLdouble shadowExtrudeDistVec[] = { 5.0, 5.0, 5.0, 0.0 };
  
  // Récupération des paramètres
  lightPos = cgGetNamedParameter (program, "LightPos");
  modelViewMatrix = cgGetNamedParameter (program, "ModelViewMatrix");
  fatness = cgGetNamedParameter (program, "Fatness");
  shadowExtrudeDist = cgGetNamedParameter (program, "ShadowExtrudeDist");

  setFatness (fatnessVec);
  setshadowExtrudeDist (shadowExtrudeDistVec);
}

CgSVShader::~CgSVShader ()
{
}

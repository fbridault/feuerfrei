#include "CgSVShader.hpp"

CgSVShader::CgSVShader (char *sourceName, char *shaderName, CGcontext *context):
  CgShader (sourceName, shaderName, context, CG_GL_VERTEX)
{
  // R�cup�ration des param�tres
  lightPos = cgGetNamedParameter (program, "LightPos");
  worldViewMatrix = cgGetNamedParameter (program, "WorldViewMatrix");
  modelViewMatrix = cgGetNamedParameter (program, "ModelViewMatrix");
  fatness = cgGetNamedParameter (program, "Fatness");
  shadowExtrudeDist = cgGetNamedParameter (program, "ShadowExtrudeDist");
}

CgSVShader::~CgSVShader ()
{
}

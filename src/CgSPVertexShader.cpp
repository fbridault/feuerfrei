#include "CgSPVertexShader.hpp"

CgSPVertexShader::CgSPVertexShader(char *sourceName, char *shaderName, CGcontext *context) : 
  CgShader (sourceName, shaderName, context, CG_GL_VERTEX)
{
  // R�cup�ration des param�tres
  modelViewProjectionMatrix = cgGetNamedParameter(program, "ModelViewProj");
}

CgSPVertexShader::~CgSPVertexShader()
{
}

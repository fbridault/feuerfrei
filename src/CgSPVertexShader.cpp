#include "CgSPVertexShader.hpp"

CgSPVertexShader::CgSPVertexShader(char *sourceName, char *shaderName, CGcontext *context) : 
  CgShader (sourceName, shaderName, context, CG_GL_VERTEX)
{
  // Récupération des paramètres
  modelViewProjectionMatrix = cgGetNamedParameter(program, "ModelViewProj");
}

CgSPVertexShader::~CgSPVertexShader()
{
}

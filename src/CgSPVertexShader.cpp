#include "CgSPVertexShader.hpp"

CgSPVertexShader::CgSPVertexShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context) : 
  CgShader (sourceName, shaderName, context, CG_GL_VERTEX)
{
  // Récupération des paramètres
  modelViewProjectionMatrix = cgGetNamedParameter(program, "ModelViewProj");
}

CgSPVertexShader::~CgSPVertexShader()
{
}

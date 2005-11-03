#include "CgSPVertexShader.hpp"

CgSPVertexShader::CgSPVertexShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context) : 
  CgShader (sourceName, shaderName, context, CG_GL_VERTEX)
{
  // R�cup�ration des param�tres
  modelViewProjectionMatrix = cgGetNamedParameter(program, "ModelViewProj");
}

CgSPVertexShader::~CgSPVertexShader()
{
}

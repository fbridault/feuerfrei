#include "CgBougieShaders.hpp"

CgBougieVertexShader::CgBougieVertexShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context) : 
  CgSPVertexShader (sourceName, shaderName, context)
{
  // R�cup�ration des param�tres
  paramTexTranslation = cgGetNamedParameter(program, "texTranslation");
  paramModelViewInv = cgGetNamedParameter(program, "ModelViewInv");
}

CgBougieVertexShader::~CgBougieVertexShader()
{
  cerr << "delete bogie shader" << endl;
}

/***********************************************************************************************************/

CgBougieFragmentShader::CgBougieFragmentShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context) : 
  CgShader (sourceName, shaderName, context, CG_GL_FRAGMENT)
{
  // R�cup�ration des param�tres
  paramTexture = cgGetNamedParameter(program, "texture");
}

CgBougieFragmentShader::~CgBougieFragmentShader()
{
}

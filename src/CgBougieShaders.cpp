#include "CgBougieShaders.hpp"

CgBougieVertexShader::CgBougieVertexShader(const wxString& sourceName, const wxString& shaderName, 
					   CGcontext *context, bool recompile) : 
  CgBasicVertexShader (sourceName, shaderName, context, recompile)
{
  // Récupération des paramètres
  paramTexTranslation = cgGetNamedParameter(program, "texTranslation");
  paramModelViewInv = cgGetNamedParameter(program, "ModelViewInv");
}

CgBougieVertexShader::~CgBougieVertexShader()
{
  cerr << "delete bogie shader" << endl;
}

/***********************************************************************************************************/

CgBougieFragmentShader::CgBougieFragmentShader(const wxString& sourceName, const wxString& shaderName,
					       CGcontext *context, bool recompile) : 
  CgShader (sourceName, shaderName, context, CG_GL_FRAGMENT, recompile)
{
  // Récupération des paramètres
  paramTexture = cgGetNamedParameter(program, "texture");
}

CgBougieFragmentShader::~CgBougieFragmentShader()
{
}

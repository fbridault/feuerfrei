#include "CgBougieShaders.hpp"

CgBougieVertexShader::CgBougieVertexShader(char *sourceName, char *shaderName, CGcontext *context) : 
  CgSPVertexShader (sourceName, shaderName, context)
{
  // Récupération des paramètres
  paramTexTranslation = cgGetNamedParameter(program, "texTranslation");
}

CgBougieVertexShader::~CgBougieVertexShader()
{
}

/***********************************************************************************************************/

CgBougieFragmentShader::CgBougieFragmentShader(char *sourceName, char *shaderName, CGcontext *context) : 
  CgShader (sourceName, shaderName, context, CG_GL_FRAGMENT)
{
  // Récupération des paramètres
  paramTexture = cgGetNamedParameter(program, "texture");
  paramModelViewInv = cgGetNamedParameter(program, "ModelViewInv");
}

CgBougieFragmentShader::~CgBougieFragmentShader()
{
}

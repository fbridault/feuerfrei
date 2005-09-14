#include "CgBougieShaders.hpp"

CgBougieVertexShader::CgBougieVertexShader(char *sourceName, char *shaderName, CGcontext *context) : 
  CgSPVertexShader (sourceName, shaderName, context)
{
  // R�cup�ration des param�tres
  paramTexTranslation = cgGetNamedParameter(program, "texTranslation");
}

CgBougieVertexShader::~CgBougieVertexShader()
{
}

/***********************************************************************************************************/

CgBougieFragmentShader::CgBougieFragmentShader(char *sourceName, char *shaderName, CGcontext *context) : 
  CgShader (sourceName, shaderName, context, CG_GL_FRAGMENT)
{
  // R�cup�ration des param�tres
  paramTexture = cgGetNamedParameter(program, "texture");
  paramModelViewInv = cgGetNamedParameter(program, "ModelViewInv");
}

CgBougieFragmentShader::~CgBougieFragmentShader()
{
}

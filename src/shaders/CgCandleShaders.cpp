#include "CgCandleShaders.hpp"

CgCandleVertexShader::CgCandleVertexShader(const wxString& sourceName, const wxString& shaderName, 
					   CGcontext *context, bool recompile) : 
  CgBasicVertexShader (sourceName, shaderName, context, recompile)
{
  // R�cup�ration des param�tres
  paramTexTranslation = cgGetNamedParameter(program, "texTranslation");
  paramModelViewInv = cgGetNamedParameter(program, "ModelViewInv");
}

CgCandleVertexShader::~CgCandleVertexShader()
{
}

/***********************************************************************************************************/

CgCandleFragmentShader::CgCandleFragmentShader(const wxString& sourceName, const wxString& shaderName,
					       CGcontext *context, bool recompile) : 
  CgShader (sourceName, shaderName, context, CG_GL_FRAGMENT, recompile)
{
  // R�cup�ration des param�tres
  paramTexture = cgGetNamedParameter(program, "texture");
}

CgCandleFragmentShader::~CgCandleFragmentShader()
{
}

#include "CgBlurShaders.hpp"

#include <math.h>

CgBlurVertexShader::CgBlurVertexShader(const wxString& sourceName, const wxString& shaderName, 
				       const CGcontext* const context, bool recompile) : 
  CgBasicVertexShader (sourceName, shaderName, context, recompile)
{
  // Récupération des paramètres
  paramOffsets = cgGetNamedParameter(program, "offsets");
}

CgBlurVertexShader::~CgBlurVertexShader()
{
}

/***********************************************************************************************************/

CgBlurFragmentShader::CgBlurFragmentShader(const wxString& sourceName, const wxString& shaderName, 
					   const CGcontext* const context, bool recompile) : 
  CgShader (sourceName, shaderName, context, CG_GL_FRAGMENT, recompile)
{
  // Récupération des paramètres
  paramWeights = cgGetNamedParameter(program, "weights");
  paramTexture = cgGetNamedParameter(program, "text");
  paramDivide = cgGetNamedParameter(program, "divide");
}

CgBlurFragmentShader::~CgBlurFragmentShader()
{
}

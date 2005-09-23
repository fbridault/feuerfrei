#include "CgBlurShaders.hpp"

CgBlurVertexShader::CgBlurVertexShader(char *sourceName, char *shaderName, CGcontext *context) : 
  CgSPVertexShader (sourceName, shaderName, context)
{
  // Récupération des paramètres
  paramOffsets = cgGetNamedParameter(program, "offsets");
  
  offsets[0] = -3;
  offsets[1] = -2;
  offsets[2] = -1;
  offsets[3] = 0;
  offsets[4] = 1;
  offsets[5] = 2;
  offsets[6] = 3;
  offsets[7] = 0;
}

CgBlurVertexShader::~CgBlurVertexShader()
{
}

/***********************************************************************************************************/

CgBlurFragmentShader::CgBlurFragmentShader(char *sourceName, char *shaderName, CGcontext *context, Texture *tex) : 
  CgShader (sourceName, shaderName, context, CG_GL_FRAGMENT)
{
  // Récupération des paramètres
  paramWeights = cgGetNamedParameter(program, "weights");
  paramTexture = cgGetNamedParameter(program, "texture");
  weights[0] = 2;
  weights[1] = 5;
  weights[2] = 10;
  weights[3] = 20;
  weights[4] = 10;
  weights[5] = 5;
  weights[6] = 2;
  weights[7] = 0;
}

CgBlurFragmentShader::~CgBlurFragmentShader()
{
}

#include "CgBlurShaders.hpp"

CgBlurVertexShader::CgBlurVertexShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context) : 
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

CgBlurFragmentShader::CgBlurFragmentShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context) : 
  CgShader (sourceName, shaderName, context, CG_GL_FRAGMENT)
{
  // Récupération des paramètres
  paramWeights = cgGetNamedParameter(program, "weights");
  paramTexture = cgGetNamedParameter(program, "text");
  paramDivide = cgGetNamedParameter(program, "divide");

  computeWeights(4.5);
}

void CgBlurFragmentShader::computeWeights(float sigma)
{
  //coef = 1/sqrt(2*PI*sigma);
  divide = 0.0;

  /* Calcul des poids */
  for(int x=-3 ; x<=3 ; x++){
    weights[x+3] = expf(-(x*x)/(sigma*sigma));
    divide += weights[x+3];
  }

  weights[7] = 0;

//   weights[0] = 5;
//   weights[1] = 10;
//   weights[2] = 20;
//   weights[3] = 30;
//   weights[4] = 20;
//   weights[5] = 10;
//   weights[6] = 5;
//   weights[7] = 0;

//   divide = 100;

}

CgBlurFragmentShader::~CgBlurFragmentShader()
{
}

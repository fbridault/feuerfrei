#ifndef CGBLURSHADERS_H
#define CGBLURSHADERS_H

class CgBlurVertexShader;
class CgBlurFragmentShader;

#include "CgShader.hpp"
#include "texture.hpp"
	
class CgShader;

class CgBlurVertexShader : public CgBasicVertexShader
{
public:
  CgBlurVertexShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context);
  virtual ~CgBlurVertexShader();
  
  void setOffsetsArray(){
    cgGLSetParameterArray1d(paramOffsets, 0, 8, offsets);
  };
  
private:
  CGparameter paramOffsets;
  double offsets[8];
};

class CgBlurFragmentShader : public CgShader
{
public:
  CgBlurFragmentShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context);
  virtual ~CgBlurFragmentShader();
  
  void computeWeights(double sigma);

  void setWeightsArray(){
    cgGLSetParameterArray1d(paramWeights, 0, 8, weights);
    cgGLSetParameter1d(paramDivide, divide);
  };

  void setTexture(Texture* tex){
    cgGLSetTextureParameter(paramTexture, tex->getTexture());
    cgGLEnableTextureParameter(paramTexture);
  };
  
  void setTexture(GLuint tex){
    cgGLSetTextureParameter(paramTexture, tex);
    cgGLEnableTextureParameter(paramTexture);
  };

private:
  CGparameter paramWeights;
  CGparameter paramDivide;
  CGparameter paramTexture;
  double weights[8];
  double divide;
};

#endif

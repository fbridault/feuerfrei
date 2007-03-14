#ifndef CGBLURSHADERS_H
#define CGBLURSHADERS_H

class CgBlurVertexShader;
class CgBlurFragmentShader;

#include "CgShader.hpp"
#include "../scene/texture.hpp"

#define FILTER_SIZE 8

class CgShader;

class CgBlurVertexShader : public CgBasicVertexShader
{
public:
  CgBlurVertexShader(const wxString& sourceName, const wxString& shaderName, const CGcontext* const context, bool recompile=false);
  virtual ~CgBlurVertexShader();
  
  void setOffsetsArray(double offsets[FILTER_SIZE]){
    cgGLSetParameterArray1d(paramOffsets, 0, FILTER_SIZE, offsets);
  };
  
private:
  CGparameter paramOffsets;
};

class CgBlurFragmentShader : public CgShader
{
public:
  CgBlurFragmentShader(const wxString& sourceName, const wxString& shaderName, const CGcontext* const context, bool recompile=false);
  virtual ~CgBlurFragmentShader();
  
  void setWeightsArray(double weights[FILTER_SIZE], double divide){
    cgGLSetParameterArray1d(paramWeights, 0, FILTER_SIZE, weights);
    cgGLSetParameter1d(paramDivide, divide);
  };
  
  void setTexture(const Texture* const tex){
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
  CGparameter paramVisibilityTex;
};

#endif

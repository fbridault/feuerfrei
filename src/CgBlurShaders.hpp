#ifndef CGBOUGIESHADERS_H
#define CGBOUGIESHADERS_H

class CgBlurVertexShader;
class CgBlurFragmentShader;

#include "CgShader.hpp"
#include "CgSPVertexShader.hpp"
#include "texture.hpp"
	
class CgShader;
class CgSPVertexShader;

class CgBlurVertexShader : public CgSPVertexShader
{
public:
  CgBlurVertexShader(char *sourceName, char *shaderName, CGcontext *context);
  virtual ~CgBlurVertexShader();
  
  void setOffsetsArray(){
    cgGLSetParameterArray1f(paramOffsets, 0, 8, offsets);
  };
  
private:
  CGparameter paramOffsets;
  float offsets[8];
};

class CgBlurFragmentShader : public CgShader
{
public:
  CgBlurFragmentShader(char *sourceName, char *shaderName, CGcontext *context, Texture *tex);
  virtual ~CgBlurFragmentShader();
  
  void setWeightsArray(){
    cgGLSetParameterArray1f(paramWeights, 0, 8, weights);
  };

  void setTexture(Texture* tex){
    cgGLSetTextureParameter(paramTexture, tex->getTexture());
    cgGLEnableTextureParameter(paramTexture);
  };
  
private:
  CGparameter paramWeights;
  CGparameter paramTexture;
  float weights[8];
};

#endif

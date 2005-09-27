#ifndef CGBOUGIESHADERS_H
#define CGBOUGIESHADERS_H

class CgBougieFragmentShader;

#include "CgShader.hpp"
#include "CgSPVertexShader.hpp"
#include "texture.hpp"
	
class CgShader;
class CgSPVertexShader;

class CgBougieVertexShader : public CgSPVertexShader
{
public:
  CgBougieVertexShader(char *sourceName, char *shaderName, CGcontext *context);
  virtual ~CgBougieVertexShader();
  
  void setTexTranslation(GLfloat value){
    cgGLSetParameter1f(paramTexTranslation, value);
  };
  
private:
  CGparameter paramTexTranslation;
};

class CgBougieFragmentShader : public CgShader
{
public:
  CgBougieFragmentShader(char *sourceName, char *shaderName, CGcontext *context);
  virtual ~CgBougieFragmentShader();
  
  void setTexture(Texture* tex){
    cgGLSetTextureParameter(paramTexture, tex->getTexture());
    cgGLEnableTextureParameter(paramTexture);
  };
  
  void setInverseModelViewMatrix(){
    cgGLSetStateMatrixParameter(paramModelViewInv, CG_GL_MODELVIEW_MATRIX,CG_GL_MATRIX_INVERSE);
  };
  
private:
  CGparameter paramTexture;
  CGparameter paramModelViewInv;
};

#endif

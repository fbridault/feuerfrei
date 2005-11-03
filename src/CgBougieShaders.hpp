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
  CgBougieVertexShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context);
  virtual ~CgBougieVertexShader();
  
  void setTexTranslation(GLfloat value){
    cgGLSetParameter1f(paramTexTranslation, value);
  };
  
  void setInverseModelViewMatrix(){
    cgGLSetStateMatrixParameter(paramModelViewInv, CG_GL_MODELVIEW_MATRIX,CG_GL_MATRIX_INVERSE);
  };

private:
  CGparameter paramTexTranslation;
  CGparameter paramModelViewInv;
};

class CgBougieFragmentShader : public CgShader
{
public:
  CgBougieFragmentShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context);
  virtual ~CgBougieFragmentShader();
  
  void setTexture(Texture* tex){
    cgGLSetTextureParameter(paramTexture, tex->getTexture());
    cgGLEnableTextureParameter(paramTexture);
  };
    
private:
  CGparameter paramTexture;
};

#endif

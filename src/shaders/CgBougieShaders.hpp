#ifndef CGBOUGIESHADERS_H
#define CGBOUGIESHADERS_H

class CgBougieFragmentShader;

#include "CgShader.hpp"
#include "texture.hpp"

class CgShader;
class CgSPVertexShader;

class CgBougieVertexShader : public CgBasicVertexShader
{
public:
  CgBougieVertexShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context, bool recompile=false);
  virtual ~CgBougieVertexShader();
  
  void setTexTranslation(GLdouble value){
    cgGLSetParameter1d(paramTexTranslation, value);
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
  CgBougieFragmentShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context, bool recompile=false);
  virtual ~CgBougieFragmentShader();
  
  void setTexture(Texture* tex){
    cgGLSetTextureParameter(paramTexture, tex->getTexture());
    cgGLEnableTextureParameter(paramTexture);
  };
    
private:
  CGparameter paramTexture;
};

#endif

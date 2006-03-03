#ifndef CGCANDLESHADERS_H
#define CGCANDLESHADERS_H

class CgCandleFragmentShader;

#include "CgShader.hpp"
#include "../scene/texture.hpp"

class CgShader;
class CgSPVertexShader;

class CgCandleVertexShader : public CgBasicVertexShader
{
public:
  CgCandleVertexShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context, bool recompile=false);
  virtual ~CgCandleVertexShader();
  
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

class CgCandleFragmentShader : public CgShader
{
public:
  CgCandleFragmentShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context, bool recompile=false);
  virtual ~CgCandleFragmentShader();
  
  void setTexture(Texture* tex){
    cgGLSetTextureParameter(paramTexture, tex->getTexture());
    cgGLEnableTextureParameter(paramTexture);
  };
    
private:
  CGparameter paramTexture;
};

#endif

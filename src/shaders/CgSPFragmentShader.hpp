#ifndef CGSPFRAGMENTSHADER_H
#define CGSPFRAGMENTSHADER_H

class CgSPFragmentShader;

#include <math.h>

#include "../flames/ies.hpp"
#include "../scene/texture.hpp"
#include "CgShader.hpp"

class CgShader;

/** Classe servant d'interface avec les fragment programs 
 * du solide photométrique.
 *
 * @author	Flavien Bridault
 */
class CgSPFragmentShader : public CgShader
{
public:
    /** Constructeur par défaut
   * @param sourceName Nom du fichier source
   * @param shaderName Nom du programme Cg
   * @param context Pointeur vers le contexte Cg (il doit être déjà créé)
   * @param type : 1 ou 0 selon qu'il y ait interpolation ou non
   */
  CgSPFragmentShader(const wxString& sourceName, const wxString& shaderName, uint nbFlames, CGcontext *context, 
		     uint type, const wxString& extraParameters, bool recompile=true);
  virtual ~CgSPFragmentShader();
  
  void setIsTextured(int value){
    cgSetParameter1i(paramIsTextured, value);
  }
  
  void SetTexture(GLuint texture, uint tex2DSize[2]){
    m_tex = texture;
    m_tex2DSize[0] = tex2DSize[0];
    m_tex2DSize[1] = tex2DSize[1];
  };
  
  void enableShader(GLdouble *centreSP, GLdouble *fluctuationIntensite, GLdouble *AZValues)
  {
//     cgGLSetStateMatrixParameter(TextureSPMatrix, CG_GL_TEXTURE_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetTextureParameter(paramTextureSP, m_tex);
    cgGLEnableTextureParameter(paramTextureSP);
    
    cgGLSetParameterArray3d(paramCentreSP, 0, m_nbFlames, centreSP);
    cgGLSetParameterArray1d(paramFluctuationIntensite, 0, m_nbFlames, fluctuationIntensite);
    
    cgGLSetParameter2d(paramTex2DSize, m_tex2DSize[0], m_tex2DSize[1]);
    if(interp)
      cgGLSetParameterArray3d(lazimut_lzenith_denom, 0, m_nbFlames, AZValues);
    else
      cgGLSetParameterArray2d(lazimut_lzenith, 0, m_nbFlames, AZValues);
    enableProfile();
    bindProgram();
  }
  
private:
  bool interp;
  
  CGparameter paramTextureSP;
  CGparameter lazimut_lzenith_denom;
  CGparameter lazimut_lzenith;
  CGparameter TextureSPMatrix;
  CGparameter paramCentreSP;
  CGparameter paramFluctuationIntensite;
  CGparameter paramIsTextured;
  CGparameter paramTex2DSize;
  
  GLuint m_tex, m_nbFlames, m_tex2DSize[2];
};

#endif

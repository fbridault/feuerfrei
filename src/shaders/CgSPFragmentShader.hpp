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
  
  void SetTexture(GLuint texture){
    m_tex = texture;
  };
  
  void SetInitialParameters(GLdouble *AZValues){
    
    cgGLSetTextureParameter(paramTextureSP, m_tex);
    cgGLEnableTextureParameter(paramTextureSP);
    
    cgSetParameter1i(paramIncR, m_nbFlames > 1 ? 1/(m_nbFlames-1) : 0);
    if(interp)
      cgGLSetParameterArray3d(lazimut_lzenith_denom, 0, m_nbFlames, AZValues);
    else
      cgGLSetParameterArray2d(lazimut_lzenith, 0, m_nbFlames, AZValues);
  }
  
  void enableShader(GLdouble *centreSP, GLdouble *fluctuationIntensite)
  {
//     cgGLSetStateMatrixParameter(TextureSPMatrix, CG_GL_TEXTURE_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetParameterArray3d(paramCentreSP, 0, m_nbFlames, centreSP);
    cgGLSetParameterArray1d(paramFluctuationIntensite, 0, m_nbFlames, fluctuationIntensite);
    
    enableProfile();
    bindProgram();
  }
  
private:
  bool interp;
  
  CGparameter paramTextureSP;
  CGparameter lazimut_lzenith_denom;
  CGparameter lazimut_lzenith;
  CGparameter paramIncR;
  CGparameter TextureSPMatrix;
  CGparameter paramCentreSP;
  CGparameter paramFluctuationIntensite;
  CGparameter paramIsTextured;
  
  GLuint m_tex, m_nbFlames;
};

#endif

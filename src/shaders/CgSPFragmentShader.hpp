#ifndef CGSPFRAGMENTSHADER_H
#define CGSPFRAGMENTSHADER_H

class CgSPFragmentShader;

#include <math.h>

#include "../flames/ies.hpp"
#include "../scene/texture.hpp"
#include "CgShader.hpp"

class CgShader;

/** Classe servant d'interface avec le fragment program des solides photométriques.
 *
 * @author	Flavien Bridault
 */
class CgSPFragmentShader : public CgShader
{
public:
    /** Constructeur par défaut.
   * @param sourceName Nom du fichier source.
   * @param shaderName Nom du programme Cg.
   * @param nbFlames Nombre de flammes, ce qui correspond également au nombre de solides photométrique.
   * @param context Pointeur vers le contexte Cg (il doit être déjà créé).
   * @param type : 1 ou 0 selon qu'il y ait interpolation ou non.
   * @param recompile Indique s'il faut recompiler le shader à partir du fichier .cg ou si le .o est déjà compilé.
   */
  CgSPFragmentShader(const wxString& sourceName, const wxString& shaderName, uint nbFlames, CGcontext *context, 
		     uint type, bool recompile=true);
  /** Destructeur. */
  virtual ~CgSPFragmentShader();
  
  /** Indique au fragment program que l'objet courant est texturé. 
   * @param value O si l'objet n'est pas texturé, 1 si l'objet est texturé.
   */
  void setIsTextured(int value){
    cgSetParameter1i(paramIsTextured, value);
  }
  
  /** Affecte l'identifiant de la texture.
   * @param texture Identifiant OpenGL de la texture.
   */
  void SetTexture(GLuint texture){
    m_tex = texture;
  };
  
  /** Initialise les paramètres uniformes du fragment program.
   * @param AZValues tableau contenant les valeurs azimuthales et zénithales.
   */
  void SetInitialParameters(GLdouble *AZValues){
    
    cgGLSetTextureParameter(paramTextureSP, m_tex);
    cgGLEnableTextureParameter(paramTextureSP);
    
    cgSetParameter1i(paramIncR, m_nbFlames > 1 ? 1/(m_nbFlames-1) : 0);
    if(interp)
      cgGLSetParameterArray3d(lazimut_lzenith_denom, 0, m_nbFlames, AZValues);
    else
      cgGLSetParameterArray2d(lazimut_lzenith, 0, m_nbFlames, AZValues);
  }
  
  /** Active le shader.
   * @param centreSP Centre des solides photométriques. Le type de la variable est un tableau avec trois doubles
   * pour chaque centre.
   * @param fluctuationIntensite Coefficient pondérateur utilisé pour faire varier l'intensité de la source. Le
   * paramètre est un tableau contenant un coefficient pour chaque flamme.
   */
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

#ifndef CGSPFRAGMENTSHADER_H
#define CGSPFRAGMENTSHADER_H

class CgSPFragmentShader;

#include <math.h>

#include "ies.hpp"
#include "texture.hpp"
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
   * @param ieslist Liste des fichiers IES
   * @param type : 1 ou 0 selon qu'il y ait interpolation ou non
   */
  CgSPFragmentShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context, IESList *ieslist, bool type);
  virtual ~CgSPFragmentShader();
  
  Texture* getTexture(void){
    return texture_solide_photometrique;
  };
  
  void setTexture(Texture *tex){
    texture_solide_photometrique = tex;
  };
  
  void setTextureSP(){
    cgGLSetTextureParameter(paramTextureSP, texture_solide_photometrique->getTexture());
    cgGLEnableTextureParameter(paramTextureSP);
  };
  
  void setAZD(){
    cgGLSetParameter3f(lazimut_lzenith_denom,iesList->getCurrentIESfile()->getLazimut(),iesList->getCurrentIESfile()->getLzenith(),iesList->getCurrentIESfile()->getDenom());
  };
  
  void setLAzimutLZenith(){
    cgGLSetParameter2f(lazimut_lzenith,iesList->getCurrentIESfile()->getLazimutTEX(),iesList->getCurrentIESfile()->getLzenithTEX());
  };
  
  void setTextureSPMatrix(){
    cgGLSetStateMatrixParameter(TextureSPMatrix, CG_GL_TEXTURE_MATRIX,CG_GL_MATRIX_IDENTITY);
  };
  
  void setparamCentreSP(CPoint *centreSP){
    cgGLSetParameter3f(paramCentreSP,centreSP->getX(),centreSP->getY(),centreSP->getZ());
  };
  
  void setparamFluctuationIntensite(GLfloat fluctuationIntensite){
    cgGLSetParameter1f(paramFluctuationIntensite,fluctuationIntensite);
  };
  
  void enableShader(CPoint *centreSP, GLfloat fluctuationIntensite){
    setTextureSPMatrix();
    setTextureSP();
    setparamCentreSP(centreSP);
    setparamFluctuationIntensite(fluctuationIntensite);
    if(interp)
      setAZD();
    else
      setLAzimutLZenith();
    enableProfile();
    bindProgram();
  }
  
private:
  Texture *texture_solide_photometrique;
  IESList *iesList;
  bool interp;
  
  CGparameter paramTextureSP;
  CGparameter lazimut_lzenith_denom;
  CGparameter lazimut_lzenith;
  CGparameter TextureSPMatrix;
  CGparameter paramCentreSP;
  CGparameter paramFluctuationIntensite;
};

#endif

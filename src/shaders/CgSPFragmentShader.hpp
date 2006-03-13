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
   * @param ieslist Liste des fichiers IES
   * @param type : 1 ou 0 selon qu'il y ait interpolation ou non
   */
  CgSPFragmentShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context, 
		     IESList *ieslist, bool type, bool recompile=false);
  virtual ~CgSPFragmentShader();
    
  void setTexture(){
    cgGLSetTextureParameter(paramTextureSP, iesList->getCurrentIESfile()->getTexture()->getTexture());
    cgGLEnableTextureParameter(paramTextureSP);
  };
  
  void setAZD(){
    cgGLSetParameter3d(lazimut_lzenith_denom,iesList->getCurrentIESfile()->getLazimut(),iesList->getCurrentIESfile()->getLzenith(),iesList->getCurrentIESfile()->getDenom());
  };
  
  void setLAzimutLZenith(){
    cgGLSetParameter2d(lazimut_lzenith,iesList->getCurrentIESfile()->getLazimutTEX(),iesList->getCurrentIESfile()->getLzenithTEX());
  };
  
  void setTextureSPMatrix(){
    cgGLSetStateMatrixParameter(TextureSPMatrix, CG_GL_TEXTURE_MATRIX,CG_GL_MATRIX_IDENTITY);
  };
  
  void setparamCentreSP(Point *centreSP){
    cgGLSetParameter3d(paramCentreSP,centreSP->x,centreSP->y,centreSP->z);
  };
  
  void setparamFluctuationIntensite(GLdouble fluctuationIntensite){
    cgGLSetParameter1d(paramFluctuationIntensite,fluctuationIntensite);
  };
  
  void enableShader(Point *centreSP, GLdouble fluctuationIntensite){
    setTextureSPMatrix();
    setTexture();
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

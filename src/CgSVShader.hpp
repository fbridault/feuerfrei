#ifndef CGSVSHADER_H
#define CGSVSHADER_H

class CgSVShader;

#include "CgShader.hpp"
	
class CgShader;

/** Classe spécifique au shader utilisé pour la génération des shadow volumes. 
 * Le shader ici utilisé est à peu de choses près celui trouvé dans le
 * tutorial Cg de NVIDIA, il faudrait donc implémenter une meilleure
 * génération des volumes que celle-ci. 
 *
 * @author	Flavien Bridault
 */
class CgSVShader : public CgShader
{
public:
    /** Constructeur par défaut
   * @param sourceName Nom du fichier source
   * @param shaderName Nom du programme Cg
   * @param context Pointeur vers le contexte Cg (il doit être déjà créé)
   */
  CgSVShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context);
  virtual ~CgSVShader();
  
  /** Réglage de l'épaisseur des shadow volumes */
  void setFatness(GLdouble *fatnessVec){
    cgGLSetParameter4dv(fatness, fatnessVec);
  };
  
  /** Réglage de la profondeur des shadow volumes */ 
  void setshadowExtrudeDist(GLdouble *shadowExtrudeDistVec){
    cgGLSetParameter4dv(shadowExtrudeDist, shadowExtrudeDistVec);
  };
  
  /** Réglage de la position de la lumière */
  void setLightPos(GLdouble lightPosition[4]){
    cgGLSetParameter4dv(lightPos, lightPosition);
  };
  
  /** Réglage de la matrice du modèle Ã  la matrice du modèle courante inverse */
  void setModelViewMatrixToInverse(){
    cgGLSetStateMatrixParameter(modelViewMatrix, CG_GL_MODELVIEW_MATRIX,CG_GL_MATRIX_INVERSE);
  };

  /** Réglage de la matrice du monde Ã  la matrice modèle+projection courante */
  void setWorldViewMatrixToIdentity(){
    cgGLSetStateMatrixParameter(worldViewMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX,CG_GL_MATRIX_IDENTITY);
  };
    
private:
  /** Position de la lumière */
  CGparameter lightPos;
  CGparameter modelViewMatrix;
  CGparameter worldViewMatrix;
  /** Epaisseur des shadow volumes */
  CGparameter fatness;
  /** Profondeur des shadow volumes */
  CGparameter shadowExtrudeDist;
};

#endif

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
class CgSVShader : public CgBasicVertexShader
{
public:
    /** Constructeur par défaut
   * @param sourceName Nom du fichier source
   * @param shaderName Nom du programme Cg
   * @param context Pointeur vers le contexte Cg (il doit être déjà créé)
   */
  CgSVShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context, double *fatness, double *extrudeDist, bool recompile=false );
  virtual ~CgSVShader();
  
  /** Réglage de l'épaisseur des shadow volumes */
  void setFatness(GLdouble *fatnessVec){
    cgGLSetParameter4dv(m_fatness, fatnessVec);
  };
  
  /** Réglage de la profondeur des shadow volumes */ 
  void setShadowExtrudeDist(GLdouble *shadowExtrudeDistVec){
    cgGLSetParameter4dv(m_shadowExtrudeDist, shadowExtrudeDistVec);
  };
  
  /** Réglage de la position de la lumière */
  void setLightPos(GLfloat lightPosition[4]){
    cgGLSetParameter4fv(m_lightPos, lightPosition);
  };
  
  /** Réglage de la matrice du modèle Ã  la matrice du modèle courante inverse */
  void setModelViewMatrixToInverse(){
    cgGLSetStateMatrixParameter(m_modelViewMatrix, CG_GL_MODELVIEW_MATRIX,CG_GL_MATRIX_INVERSE);
  };
  
private:
  /** Position de la lumière */
  CGparameter m_lightPos;
  CGparameter m_modelViewMatrix;
  /** Epaisseur des shadow volumes */
  CGparameter m_fatness;
  /** Profondeur des shadow volumes */
  CGparameter m_shadowExtrudeDist;
};

#endif

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
   * @param sourceName Nom du fichier source.
   * @param shaderName Nom du programme Cg.
   * @param context Pointeur vers le contexte Cg (il doit être déjà créé).
   * @param fatness Largeur du shadow volume.
   * @param extrudeDist Profondeur du shadow volume.
   * @param recompile Indique s'il faut recompiler le shader à partir du fichier .cg ou si le .o est déjà compilé.
   */
  CgSVShader(const wxString& sourceName, const wxString& shaderName, const CGcontext* const context, const double* const fatness, 
	     const double* const extrudeDist, bool recompile=false );
  /** Desctructeur. */
  virtual ~CgSVShader();
  
  /** Réglage de l'épaisseur des shadow volumes */
  void setFatness(const GLdouble* const fatnessVec) const{
    cgGLSetParameter4dv(m_fatness, fatnessVec);
  };
  
  /** Réglage de la profondeur des shadow volumes */ 
  void setShadowExtrudeDist(const GLdouble* const shadowExtrudeDistVec) const{
    cgGLSetParameter4dv(m_shadowExtrudeDist, shadowExtrudeDistVec);
  };
  
  /** Réglage de la position de la lumière */
  void setLightPos(const GLfloat lightPosition[4]) const{
    cgGLSetParameter4fv(m_lightPos, lightPosition);
  };
  
  /** Réglage de la matrice du modèle Ã  la matrice du modèle courante inverse */
  void setModelViewMatrixToInverse() const{
    cgGLSetStateMatrixParameter(m_modelViewMatrix, CG_GL_MODELVIEW_MATRIX,CG_GL_MATRIX_INVERSE);
  };
  
private:
  /** Position de la lumière */
  CGparameter m_lightPos;
  /** Matrice de la vue du modèle. */
  CGparameter m_modelViewMatrix;
  /** Epaisseur des shadow volumes */
  CGparameter m_fatness;
  /** Profondeur des shadow volumes */
  CGparameter m_shadowExtrudeDist;
};

#endif

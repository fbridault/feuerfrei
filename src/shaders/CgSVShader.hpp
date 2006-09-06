#ifndef CGSVSHADER_H
#define CGSVSHADER_H

class CgSVShader;

#include "CgShader.hpp"
	
class CgShader;

/** Classe sp�cifique au shader utilis� pour la g�n�ration des shadow volumes. 
 * Le shader ici utilis� est � peu de choses pr�s celui trouv� dans le
 * tutorial Cg de NVIDIA, il faudrait donc impl�menter une meilleure
 * g�n�ration des volumes que celle-ci. 
 *
 * @author	Flavien Bridault
 */
class CgSVShader : public CgBasicVertexShader
{
public:
    /** Constructeur par d�faut
   * @param sourceName Nom du fichier source.
   * @param shaderName Nom du programme Cg.
   * @param context Pointeur vers le contexte Cg (il doit �tre d�j� cr��).
   * @param fatness Largeur du shadow volume.
   * @param extrudeDist Profondeur du shadow volume.
   * @param recompile Indique s'il faut recompiler le shader � partir du fichier .cg ou si le .o est d�j� compil�.
   */
  CgSVShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context, double *fatness, double *extrudeDist, bool recompile=false );
  /** Desctructeur. */
  virtual ~CgSVShader();
  
  /** R�glage de l'�paisseur des shadow volumes */
  void setFatness(GLdouble *fatnessVec){
    cgGLSetParameter4dv(m_fatness, fatnessVec);
  };
  
  /** R�glage de la profondeur des shadow volumes */ 
  void setShadowExtrudeDist(GLdouble *shadowExtrudeDistVec){
    cgGLSetParameter4dv(m_shadowExtrudeDist, shadowExtrudeDistVec);
  };
  
  /** R�glage de la position de la lumi�re */
  void setLightPos(GLfloat lightPosition[4]){
    cgGLSetParameter4fv(m_lightPos, lightPosition);
  };
  
  /** R�glage de la matrice du mod�le à la matrice du mod�le courante inverse */
  void setModelViewMatrixToInverse(){
    cgGLSetStateMatrixParameter(m_modelViewMatrix, CG_GL_MODELVIEW_MATRIX,CG_GL_MATRIX_INVERSE);
  };
  
private:
  /** Position de la lumi�re */
  CGparameter m_lightPos;
  /** Matrice de la vue du mod�le. */
  CGparameter m_modelViewMatrix;
  /** Epaisseur des shadow volumes */
  CGparameter m_fatness;
  /** Profondeur des shadow volumes */
  CGparameter m_shadowExtrudeDist;
};

#endif

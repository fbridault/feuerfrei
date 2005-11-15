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
   * @param sourceName Nom du fichier source
   * @param shaderName Nom du programme Cg
   * @param context Pointeur vers le contexte Cg (il doit �tre d�j� cr��)
   */
  CgSVShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context);
  virtual ~CgSVShader();
  
  /** R�glage de l'�paisseur des shadow volumes */
  void setFatness(GLdouble *fatnessVec){
    cgGLSetParameter4dv(fatness, fatnessVec);
  };
  
  /** R�glage de la profondeur des shadow volumes */ 
  void setshadowExtrudeDist(GLdouble *shadowExtrudeDistVec){
    cgGLSetParameter4dv(shadowExtrudeDist, shadowExtrudeDistVec);
  };
  
  /** R�glage de la position de la lumi�re */
  void setLightPos(GLdouble lightPosition[4]){
    cgGLSetParameter4dv(lightPos, lightPosition);
  };
  
  /** R�glage de la matrice du mod�le à la matrice du mod�le courante inverse */
  void setModelViewMatrixToInverse(){
    cgGLSetStateMatrixParameter(modelViewMatrix, CG_GL_MODELVIEW_MATRIX,CG_GL_MATRIX_INVERSE);
  };
    
private:
  /** Position de la lumi�re */
  CGparameter lightPos;
  CGparameter modelViewMatrix;
  /** Epaisseur des shadow volumes */
  CGparameter fatness;
  /** Profondeur des shadow volumes */
  CGparameter shadowExtrudeDist;
};

#endif

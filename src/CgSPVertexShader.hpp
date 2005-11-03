#ifndef CGSPVERTEXSHADER_H
#define CGSPVERTEXSHADER_H

class CgSPShader;

#include "CgShader.hpp"
	
class CgShader;


/** Classe servant d'interface avec les vertex programs 
 * du solide photom�trique.
 *
 * @author	Flavien Bridault
 */
class CgSPVertexShader : public CgShader
{
public:
  /** Constructeur par d�faut
   * @param sourceName Nom du fichier source
   * @param shaderName Nom du programme Cg
   * @param context Pointeur vers le contexte Cg (il doit �tre d�j� cr��)
   */
  CgSPVertexShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context);
  virtual ~CgSPVertexShader();
  
  void setModelViewProjectionMatrix(){
    cgGLSetStateMatrixParameter(modelViewProjectionMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX,CG_GL_MATRIX_IDENTITY);
  };
      
private:
  CGparameter modelViewProjectionMatrix; /* parametres des vertex program */
};

#endif

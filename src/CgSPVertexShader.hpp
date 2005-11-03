#ifndef CGSPVERTEXSHADER_H
#define CGSPVERTEXSHADER_H

class CgSPShader;

#include "CgShader.hpp"
	
class CgShader;


/** Classe servant d'interface avec les vertex programs 
 * du solide photométrique.
 *
 * @author	Flavien Bridault
 */
class CgSPVertexShader : public CgShader
{
public:
  /** Constructeur par défaut
   * @param sourceName Nom du fichier source
   * @param shaderName Nom du programme Cg
   * @param context Pointeur vers le contexte Cg (il doit être déjà créé)
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

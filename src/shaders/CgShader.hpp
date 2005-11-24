#ifndef CGSHADER_H
#define CGSHADER_H

class CgShader;

#include "header.h"
  
/** Abstraction d'un shader Cg. Il est recommandé d'utiliser cette classe
 * pour implémenter de nouveaux types de shaders
 *
 * @author	Flavien Bridault
 */
class CgShader
{
public:
  /** Constructeur par défaut
   * @param sourceName Nom du fichier source
   * @param shaderName Nom du programme Cg
   * @param context Pointeur vers le contexte Cg (il doit être déjà créé)
   * @param type Type du shader : CG_GL_VERTEX ou CG_GL_FRAGMENT
   */
  CgShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context, CGGLenum type, bool recompile=false);
  virtual ~CgShader();
  
  /** Activation du profil */
  void enableProfile(){
    cgGLEnableProfile(profile);
  };
  
  /** Bind du program */
  void bindProgram(){
    cgGLBindProgram(program);
  };
  
    /** Activation du shader */
  virtual void enableShader(){
    cgGLEnableProfile(profile);
    cgGLBindProgram(program);
  }

  /** Désactivation du profil */
  void disableProfile(){
    cgGLDisableProfile(profile);
  };
  
protected:
  /** Programme Cg pour le shader */
  CGprogram program;
  /** Profil Cg pour le shader */
  CGprofile profile;
};

/** Classe pour un Vertex Shader "de base" avec comme seul paramètre
 *  la matrice résultante de la concaténation de la projection et de la
 *  modelview.
 */
class CgBasicVertexShader : public CgShader
{
public:
  /** Constructeur par défaut
   * @param sourceName Nom du fichier source
   * @param shaderName Nom du programme Cg
   * @param context Pointeur vers le contexte Cg (il doit être déjà créé)
   */
  CgBasicVertexShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context, bool recompile=false);
  virtual ~CgBasicVertexShader(){};
  
  /** Met à jour la matrice résultante de la concaténation de la projection et de la
   * modelview. Cette méthode doit être appelée après chaque série de transformations OpenGL
   * pour que celles-ci soit prises en compte par le vertex shader
   */
  void setModelViewProjectionMatrix(){
    cgGLSetStateMatrixParameter(modelViewProjectionMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX,CG_GL_MATRIX_IDENTITY);
  };
private:  
  CGparameter modelViewProjectionMatrix;
};
#endif

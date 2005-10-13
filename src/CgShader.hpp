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
  CgShader(char *sourceName, char *shaderName, CGcontext *context, CGGLenum type);
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

#endif

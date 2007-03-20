#ifndef CGSHADER_H
#define CGSHADER_H

class CgShader;

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <wx/string.h>
#include <wx/intl.h>

/** Abstraction d'un shader Cg. Il est recommandé d'utiliser cette classe
 * pour implémenter de nouveaux types de shaders
 *
 * @author	Flavien Bridault
 */
class CgShader
{
public:
  /** Constructeur par défaut.
   * @param sourceName Nom du fichier source.
   * @param shaderName Nom du programme Cg.
   * @param context Pointeur vers le contexte Cg (il doit être déjà créé).
   * @param type Type du shader : CG_GL_VERTEX ou CG_GL_FRAGMENT.
   * @param recompile Indique s'il faut recompiler le shader à partir du fichier .cg ou si le .o est déjà compilé.
   * @param extraParameters Paramètres supplémentaires optionels sur la ligne de compilation. Ceci est utilisé par
   * exemple lors de la définition des solides photométriques multiples afin de définir une constante correspondant
   * au nombre de solides.
   */
  CgShader(const wxString& sourceName, const wxString& shaderName, const CGcontext* const context, CGGLenum type,
	   bool recompile=false, const wxString& extraParameters=_(""));
  virtual ~CgShader();
  
  /** Activation du profil. */
  virtual void enableProfile() const{
    cgGLEnableProfile(profile);
  };
  
  /** Bind du programme. */
  virtual void bindProgram() const{
    cgGLBindProgram(program);
  };
  
    /** Activation du shader. */
  virtual void enableShader() const{
    cgGLEnableProfile(profile);
    cgGLBindProgram(program);
  }

  /** Désactivation du profil. */
  virtual void disableProfile() const{
    cgGLDisableProfile(profile);
  };
  
  /** Désactivation du shader. */
  virtual void disableShader(){
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
  /** Constructeur par défaut.
   * @param sourceName Nom du fichier source.
   * @param shaderName Nom du programme Cg.
   * @param context Pointeur vers le contexte Cg (il doit être déjà créé).
   * @param recompile Indique s'il faut recompiler le shader à partir du fichier .cg ou si le .o est déjà compilé.
   */
  CgBasicVertexShader(const wxString& sourceName, const wxString& shaderName, const CGcontext* const context, bool recompile=false);
  virtual ~CgBasicVertexShader(){};
  
  /** Met à jour la matrice résultante de la concaténation de la projection et de la
   * modelview. Cette méthode doit être appelée après chaque série de transformations OpenGL
   * pour que celles-ci soit prises en compte par le vertex shader
   */
  void setModelViewProjectionMatrix() const{
    cgGLSetStateMatrixParameter(modelViewProjectionMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX,CG_GL_MATRIX_IDENTITY);
  };
private:
  /** Concaténation de la matrice de la vue du modèle et de la matrice de projection. */
  CGparameter modelViewProjectionMatrix;
};
#endif

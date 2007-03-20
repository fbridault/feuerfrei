#ifndef CGSHADER_H
#define CGSHADER_H

class CgShader;

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <wx/string.h>
#include <wx/intl.h>

/** Abstraction d'un shader Cg. Il est recommand� d'utiliser cette classe
 * pour impl�menter de nouveaux types de shaders
 *
 * @author	Flavien Bridault
 */
class CgShader
{
public:
  /** Constructeur par d�faut.
   * @param sourceName Nom du fichier source.
   * @param shaderName Nom du programme Cg.
   * @param context Pointeur vers le contexte Cg (il doit �tre d�j� cr��).
   * @param type Type du shader : CG_GL_VERTEX ou CG_GL_FRAGMENT.
   * @param recompile Indique s'il faut recompiler le shader � partir du fichier .cg ou si le .o est d�j� compil�.
   * @param extraParameters Param�tres suppl�mentaires optionels sur la ligne de compilation. Ceci est utilis� par
   * exemple lors de la d�finition des solides photom�triques multiples afin de d�finir une constante correspondant
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

  /** D�sactivation du profil. */
  virtual void disableProfile() const{
    cgGLDisableProfile(profile);
  };
  
  /** D�sactivation du shader. */
  virtual void disableShader(){
    cgGLDisableProfile(profile);
  };
  
protected:
  /** Programme Cg pour le shader */
  CGprogram program;
  /** Profil Cg pour le shader */
  CGprofile profile;
};

/** Classe pour un Vertex Shader "de base" avec comme seul param�tre
 *  la matrice r�sultante de la concat�nation de la projection et de la
 *  modelview.
 */
class CgBasicVertexShader : public CgShader
{
public:
  /** Constructeur par d�faut.
   * @param sourceName Nom du fichier source.
   * @param shaderName Nom du programme Cg.
   * @param context Pointeur vers le contexte Cg (il doit �tre d�j� cr��).
   * @param recompile Indique s'il faut recompiler le shader � partir du fichier .cg ou si le .o est d�j� compil�.
   */
  CgBasicVertexShader(const wxString& sourceName, const wxString& shaderName, const CGcontext* const context, bool recompile=false);
  virtual ~CgBasicVertexShader(){};
  
  /** Met � jour la matrice r�sultante de la concat�nation de la projection et de la
   * modelview. Cette m�thode doit �tre appel�e apr�s chaque s�rie de transformations OpenGL
   * pour que celles-ci soit prises en compte par le vertex shader
   */
  void setModelViewProjectionMatrix() const{
    cgGLSetStateMatrixParameter(modelViewProjectionMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX,CG_GL_MATRIX_IDENTITY);
  };
private:
  /** Concat�nation de la matrice de la vue du mod�le et de la matrice de projection. */
  CGparameter modelViewProjectionMatrix;
};
#endif

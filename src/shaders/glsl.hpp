#ifndef GLSL_H
#define GLSL_H

class GLSLProgram;

#include <GL/gl.h>
#include <iostream>

using namespace std;

/** Wrapper pour un shader GLSL.
 */
class GLSLShader
{
public:
  /** Constructeur par défaut.
   * @param sourceName Nom du fichier source.
   * @param recompile Indique s'il faut recompiler le shader à partir du fichier .cg ou si le .o est déjà compilé.
   */
  GLSLShader() {};
  virtual ~GLSLShader() { glDeleteShader(m_shader); };
  
  void load(const char* fileName, bool recompile=false, const char* macro=NULL) const;
  
  GLuint getID() const { return m_shader; };
  char* getFileContents(const char* fileName, const char* macro=NULL) const;

protected:
  GLuint m_shader;
};

class GLSLVertexShader : public GLSLShader
{
public:
  GLSLVertexShader() { m_shader=glCreateShader(GL_VERTEX_SHADER); };
  virtual ~GLSLVertexShader() {};
};

class GLSLFragmentShader : public GLSLShader
{
public:
  GLSLFragmentShader() { m_shader=glCreateShader(GL_FRAGMENT_SHADER); };
  virtual ~GLSLFragmentShader() {};
};

/** Abstraction d'un programme GLSL. 
 *
 * @author	Flavien Bridault
 */
class GLSLProgram
{
public:
  /** Constructeur par défaut.
   * @param sourceName Nom du fichier source.
   * @param shaderName Nom du programme Cg.
   * @param type Type du shader : CG_GL_VERTEX ou CG_GL_FRAGMENT.
   * @param recompile Indique s'il faut recompiler le shader à partir du fichier .cg ou si le .o est déjà compilé.
   * @param extraParameters Paramètres supplémentaires optionels sur la ligne de compilation. Ceci est utilisé par
   * exemple lors de la définition des solides photométriques multiples afin de définir une constante correspondant
   * au nombre de solides.
   */
  GLSLProgram() { m_program = glCreateProgram(); };
  virtual ~GLSLProgram() { glDeleteProgram(m_program); };
    
  virtual void link() const{
    GLint infologLength;
    glLinkProgram(m_program);

    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 0)
      {
	char *infoLog = new char[infologLength];	    
	int charsWritten  = 0;
	glGetProgramInfoLog(m_program, infologLength, &charsWritten, infoLog);
	//cerr << infoLog << endl;
	delete [] infoLog;
      }
  }

  /** Activation du programme. */
  virtual void enable() const{
    glUseProgram(m_program);
  }
  
  /** Désactivation du programme. */
  virtual void disable() const{
    glUseProgram(0);
  };
  
  /** Attachement d'un shader au programme. */
  virtual void attachShader(const GLSLShader& shader) const{
    glAttachShader(m_program,shader.getID());
  }
  
  /** Détachement d'un shader au programme. */
  virtual void detachShader(const GLSLShader& shader) const{
    glDetachShader(m_program,shader.getID());
  }
  
  void setUniform1f(const char* var, GLfloat value) const
  {
    glUniform1f(glGetUniformLocation(m_program,var), value);
  }
  
  void setUniform3f(const char* var, GLfloat val1,  GLfloat val2,  GLfloat val3 ) const
  {
    glUniform3f(glGetUniformLocation(m_program,var), val1, val2, val3 );
  }
  
  void setUniform4f(const char* var, GLfloat value[4]) const
  {
    glUniform4f(glGetUniformLocation(m_program,var), value[0], value[1], value[2], value[3]);
  }
  
  void setUniform1i(const char* var, GLint value) const
  {
    glUniform1i(glGetUniformLocation(m_program,var), value);
  }

  void setUniform1fv(const char* var, GLfloat *value, GLsizei count) const
  {
    glUniform1fv(glGetUniformLocation(m_program,var), count, value);
  }

  void setUniform2fv(const char* var, GLfloat *value, GLsizei count) const
  {
    glUniform2fv(glGetUniformLocation(m_program,var), count, value);
  }
  
  void setUniform3fv(const char* var, GLfloat *value, GLsizei count) const
  {
    glUniform3fv(glGetUniformLocation(m_program,var), count, value);
  }
  
  void setUniform1f(GLint var, GLfloat value) const
  {
    glUniform1f(var, value);
  }
  
  void setUniform1i(GLint var, GLint value) const
  {
    glUniform1i(var, value);
  }

  void setUniform1fv(GLint var, GLfloat *value, GLsizei count) const
  {
    glUniform1fv(var, count, value);
  }
  
  GLint getParameter(const char* var) const { return glGetUniformLocation(m_program,var); };
  
private:
  /** ID du programme */
  GLuint m_program;
};

#endif

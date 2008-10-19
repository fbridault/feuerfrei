/*
 *  glsl.h
 *
 * Created by Flavien Bridault
 *
 *
 */

#ifndef GLSL_H
#define GLSL_H

class CShader;

#ifdef __APPLE__
#include <openGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <iostream>
#include <vector>

using namespace std;

/** Wrapper pour un shader GLSL.
 *
 * @author	Flavien Bridault
 *
 */
class IShaderProgram
{
protected:
	GLuint m_shader;

	/** Default Constructor
	 * @param sourceName ILight file name.
	 */
	IShaderProgram() {};
	virtual ~IShaderProgram()
	{
		glDeleteShader(m_shader);
	};

	void load(const string& fileNames, const string& macros ) const;
	GLuint getID() const
	{
		return m_shader;
	};

private:
	void addMacros(const string& macros, string& source) const;
	void getFileContents(const string& fileName, string& source) const;
	void splitStringInStringsArray(const string& names, vector<string>& splitNames,const string& prefix, const string& suffix) const;

	friend class CShader;
};

class CVertexProgram : public IShaderProgram
{
public:
	CVertexProgram()
	{
		m_shader=glCreateShader(GL_VERTEX_SHADER);
	};
	virtual ~CVertexProgram() {};
};

class CFragmentProgram : public IShaderProgram
{
public:
	CFragmentProgram()
	{
		m_shader=glCreateShader(GL_FRAGMENT_SHADER);
	};
	virtual ~CFragmentProgram() {};
};

/** Abstraction d'un programme GLSL, contenant un vertex et un fragment shader
 *
 * @author	Flavien Bridault
 */
class CShader
{
private:
	/** ID du programme */
	GLuint m_program;
	/** Vertex program. */
	CVertexProgram m_vertexShader;
	/** Fragment program. */
	CFragmentProgram m_fragmentShader;

public:
	/** Constructeur par défaut.
	 * @param vpname Nom du fichier source.
	 * @param fpname Nom du fichier source.
	 */
	CShader(const string& vpname, const string& fpname, const string& macros);

	/** Constructeur ne prenant qu'un seul fragment program. Les fonctions fixes du pipeline
	 *  sont implicitement utilisées pour les vertex.
	 *
	 * @param fpname Nom du fichier source.
	 */
	CShader(const string& fpname, const string& macros);

	virtual ~CShader()
	{
		glDeleteProgram(m_program);
	};

	virtual void Link() const
	{
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
	virtual void Enable() const
	{
		glUseProgram(m_program);
	}

	/** Désactivation du programme. */
	virtual void Disable() const
	{
		glUseProgram(0);
	};

	/** Attachement d'un shader au programme. */
	virtual void AttachShader(const IShaderProgram& shader) const
	{
		glAttachShader(m_program,shader.getID());
	}

	/** Détachement d'un shader au programme. */
	virtual void DetachShader(const IShaderProgram& shader) const
	{
		glDetachShader(m_program,shader.getID());
	}

	void SetUniform1f(const char* const var, GLfloat value) const
	{
		glUniform1f(glGetUniformLocation(m_program,var), value);
	}

	void SetUniform2f(const char* const var, GLfloat val1,  GLfloat val2 ) const
	{
		glUniform2f(glGetUniformLocation(m_program,var), val1, val2 );
	}

	void SetUniform3f(const char* const var, GLfloat val1,  GLfloat val2,  GLfloat val3 ) const
	{
		glUniform3f(glGetUniformLocation(m_program,var), val1, val2, val3 );
	}

	void SetUniform4f(const char* const var, GLfloat val1,  GLfloat val2,  GLfloat val3, GLfloat val4 ) const
	{
		glUniform4f(glGetUniformLocation(m_program,var), val1, val2, val3, val4 );
	}

	void SetUniform4f(const char* const var, GLfloat value[4]) const
	{
		glUniform4f(glGetUniformLocation(m_program,var), value[0], value[1], value[2], value[3]);
	}

	void SetUniform1i(const char* const var, GLint value) const
	{
		glUniform1i(glGetUniformLocation(m_program,var), value);
	}

	void SetUniform1fv(const char* const var, const GLfloat* const value, GLsizei count) const
	{
		glUniform1fv(glGetUniformLocation(m_program,var), count, value);
	}

	void SetUniform2fv(const char* const var, const GLfloat* const value, GLsizei count) const
	{
		glUniform2fv(glGetUniformLocation(m_program,var), count, value);
	}

	void SetUniform3fv(const char* const var, const GLfloat* const value, GLsizei count) const
	{
		glUniform3fv(glGetUniformLocation(m_program,var), count, value);
	}

	void SetUniformMatrix4fv(const char* const var, const GLfloat* const value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_program,var), 1, GL_FALSE, value);
	}

	void SetUniform1f(GLint var, GLfloat value) const
	{
		glUniform1f(var, value);
	}

	void SetUniform1i(GLint var, GLint value) const
	{
		glUniform1i(var, value);
	}

	void SetUniform1fv(GLint var, const GLfloat * const value, GLsizei count) const
	{
		glUniform1fv(var, count, value);
	}

	GLint GetParameter(const char* const var) const
	{
		return glGetUniformLocation(m_program,var);
	};

};

#endif

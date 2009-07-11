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
#include "../Common.hpp"
#include "../Utility/ISingleton.hpp"

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
	GLuint GetID() const
	{
		return m_shader;
	};

private:
	void AddMacros(const string& macros, string& source) const;
	void GetFileContents(const string& fileName, string& source) const;
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
	GLuint m_uiProgram;
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
	~CShader()
	{
		glDeleteProgram(m_uiProgram);
	}

	// TODO: Bouger dans le state
	static void SetShadersDirectory(CharCPtrC a_szDirectory);

	void Link() const
	{
		GLint infologLength;
		glLinkProgram(m_uiProgram);

		glGetProgramiv(m_uiProgram, GL_INFO_LOG_LENGTH, &infologLength);
		if (infologLength > 0)
		{
			char *infoLog = new char[infologLength];
			int charsWritten  = 0;
			glGetProgramInfoLog(m_uiProgram, infologLength, &charsWritten, infoLog);
			//cerr << infoLog << endl;
			delete [] infoLog;
		}
	}

	/** Attachement d'un shader au programme. */
	void AttachShader(const IShaderProgram& shader) const
	{
		glAttachShader(m_uiProgram,shader.GetID());
	}

	/** Détachement d'un shader au programme. */
	void DetachShader(const IShaderProgram& shader) const
	{
		glDetachShader(m_uiProgram,shader.GetID());
	}

	GLuint GetProgramId() const { return m_uiProgram; }
};

/** Simple singleton allowing to get the current shader and set uniforms. */
class CShaderState : public ITSingleton<CShaderState>
{
	friend class ITSingleton<CShaderState>;
	friend class CShader;

public:

	CShader const& GetShader() const { assert(m_pCurrentShader != NULL); return *m_pCurrentShader; }

	void Enable(CShader const& a_rShader)
	{
		m_pCurrentShader = &a_rShader;
		GLuint uiProgram = a_rShader.GetProgramId();
		glUseProgram(uiProgram);
	}
	void Disable()
	{
		m_pCurrentShader = NULL;
		glUseProgram(0);
	}

	void SetUniform1f(const char* const var, GLfloat value) const
	{
		CShader const& rShader = GetShader();
		GLuint uiProgram = rShader.GetProgramId();
		glUniform1f(glGetUniformLocation(uiProgram,var), value);
	}

	void SetUniform2f(const char* const var, GLfloat val1,  GLfloat val2 ) const
	{
		CShader const& rShader = GetShader();
		GLuint uiProgram = rShader.GetProgramId();
		glUniform2f(glGetUniformLocation(uiProgram,var), val1, val2 );
	}

	void SetUniform3f(const char* const var, GLfloat val1,  GLfloat val2,  GLfloat val3 ) const
	{
		CShader const& rShader = GetShader();
		GLuint uiProgram = rShader.GetProgramId();
		glUniform3f(glGetUniformLocation(uiProgram,var), val1, val2, val3 );
	}

	void SetUniform4f(const char* const var, GLfloat val1,  GLfloat val2,  GLfloat val3, GLfloat val4 ) const
	{
		CShader const& rShader = GetShader();
		GLuint uiProgram = rShader.GetProgramId();
		glUniform4f(glGetUniformLocation(uiProgram,var), val1, val2, val3, val4 );
	}

	void SetUniform4f(const char* const var, GLfloat value[4]) const
	{
		CShader const& rShader = GetShader();
		GLuint uiProgram = rShader.GetProgramId();
		glUniform4f(glGetUniformLocation(uiProgram,var), value[0], value[1], value[2], value[3]);
	}

	void SetUniform1i(const char* const var, GLint value) const
	{
		CShader const& rShader = GetShader();
		GLuint uiProgram = rShader.GetProgramId();
		glUniform1i(glGetUniformLocation(uiProgram,var), value);
	}

	void SetUniform1fv(const char* const var, const GLfloat* const value, GLsizei count) const
	{
		CShader const& rShader = GetShader();
		GLuint uiProgram = rShader.GetProgramId();
		glUniform1fv(glGetUniformLocation(uiProgram,var), count, value);
	}

	void SetUniform2fv(const char* const var, const GLfloat* const value, GLsizei count) const
	{
		CShader const& rShader = GetShader();
		GLuint uiProgram = rShader.GetProgramId();
		glUniform2fv(glGetUniformLocation(uiProgram,var), count, value);
	}

	void SetUniform3fv(const char* const var, const GLfloat* const value, GLsizei count) const
	{
		CShader const& rShader = GetShader();
		GLuint uiProgram = rShader.GetProgramId();
		glUniform3fv(glGetUniformLocation(uiProgram,var), count, value);
	}

	void SetUniformMatrix4fv(const char* const var, const GLfloat* const value) const
	{
		CShader const& rShader = GetShader();
		GLuint uiProgram = rShader.GetProgramId();
		glUniformMatrix4fv(glGetUniformLocation(uiProgram,var), 1, GL_FALSE, value);
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
		CShader const& rShader = GetShader();
		GLuint uiProgram = rShader.GetProgramId();
		return glGetUniformLocation(uiProgram,var);
	};

//---------------------------------------------------------------------------------------------------------------------
//  Attributes
//---------------------------------------------------------------------------------------------------------------------

	CShader const* m_pCurrentShader;
};

#endif

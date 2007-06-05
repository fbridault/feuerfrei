#include "glsl.hpp"

#include <fstream>

#include <assert.h>

#define SHADERS_DIRECTORY "src/shaders/"


void GLSLShader::load(const char* fileName, bool recompile, const char* macro ) const
{
  GLint infologLength;
  GLchar* source;
  char *path,dir[]=SHADERS_DIRECTORY;
  int compiled;
  
  path = new char[strlen(dir)+strlen(fileName)+1];
  strcpy(path,dir);
  path=strcat(path,fileName);
  source = getFileContents(path,macro);
  delete [] path;
  
  glShaderSource(m_shader, 1, (const char **)&source, NULL);
  
  delete[] source;
  
  glCompileShader(m_shader);
  glGetShaderiv(m_shader, GL_COMPILE_STATUS, &compiled);
  
  //assert(compiled && "The shader could not be compiled");
  if(compiled)
    cerr << "Shader " << fileName << " successfully compiled" << endl;
  else{
    glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 0)
      {
	char *infoLog = new char[infologLength];	    
	int charsWritten  = 0;
	glGetShaderInfoLog(m_shader, infologLength, &charsWritten, infoLog);
	cerr << infoLog << endl;
	delete [] infoLog;
      }
  }
}

char* GLSLShader::getFileContents(const char* fileName, const char* macro) const
{
  int length;
  char* buffer, *buffer2;
  
  ifstream input(fileName);
  
  if (!input.is_open()){
    cerr << "Could not open shader named " << fileName << endl;
    return(NULL);
  }
  
  input.seekg(0, ios::end);
  length = input.tellg();
  input.seekg(0, ios::beg);
  
  buffer = new char[length];
  input.getline(buffer, length, '\0');
  if(macro){
    buffer2 = new char[length+strlen(macro)];
    strcpy(buffer2,macro);
    strcat(buffer2,buffer);
    delete [] buffer;
    buffer = buffer2;
  }
  
  input.close();
  
  return(buffer);
}

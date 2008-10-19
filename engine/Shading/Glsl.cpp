#include "Glsl.hpp"

#include <fstream>

#define SEPARATOR ","
#define SHADERS_DIRECTORY "src/shaders/"

#include <iostream>

#include "../Common.hpp"


using namespace std;

CShader::CShader(const string& vpname, const string& fpname, const string& macros)
{
  assert(!vpname.empty());
  assert(!fpname.empty());

  m_program = glCreateProgram();

  m_vertexShader.load(vpname,macros);
  m_fragmentShader.load(fpname,macros);

  AttachShader(m_vertexShader);
  AttachShader(m_fragmentShader);

  Link();
};

CShader::CShader(const string& fpname, const string& macros)
{
  assert(!fpname.empty());

  m_program = glCreateProgram();

  m_fragmentShader.load(fpname,macros);

  AttachShader(m_fragmentShader);

  Link();
};


void IShaderProgram::load(const string& fileNames, const string& macros ) const
{
  GLint infologLength;
  int compiled;
  vector<string> vFileNames;

  assert(!fileNames.empty());

  /** On splitte d'abord les noms de fichiers en un tableau de chaînes */
  /** En ajoutant le nom du répertoire à chaque fichier */
  splitStringInStringsArray(fileNames,vFileNames,SHADERS_DIRECTORY,"");

  cout << "Shader(s) ";
  for (vector<string>::const_iterator fileNamesIterator = vFileNames.begin ();
       fileNamesIterator != vFileNames.end (); fileNamesIterator++)
  {
    cout << *fileNamesIterator << " ";
  }
  cout << "compiling...";

  const GLchar* source[vFileNames.size()];
  string s[vFileNames.size()];

  uint i=0;
  for (vector<string>::const_iterator fileNamesIterator = vFileNames.begin ();
       fileNamesIterator != vFileNames.end (); fileNamesIterator++, i++)
  {
    getFileContents(*fileNamesIterator,s[i]);
    if (!macros.empty())
      addMacros(macros,s[i]);
    source[i] = s[i].c_str();
  }
  glShaderSource(m_shader, vFileNames.size(), source, NULL);

  glCompileShader(m_shader);
  glGetShaderiv(m_shader, GL_COMPILE_STATUS, &compiled);

  if (compiled)
  {
    cout << "OK" << endl;
  }
  else{
    glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 0)
    {
      char infoLog[infologLength];
      int charsWritten  = 0;
      glGetShaderInfoLog(m_shader, infologLength, &charsWritten, infoLog);
      cerr << endl << "(EE)" << endl << infoLog << endl;
    }
  }
}

void IShaderProgram::addMacros(const string& macros, string& source) const
{
  vector<string> vMacros;

  /** On splitte d'abord les macros en un tableau de macros */
  /** En ajoutant les directives et le marqueur de fin de ligne */
  splitStringInStringsArray(macros,vMacros,"#define ","\n");

  for (vector<string>::const_reverse_iterator vMacrosIterator = vMacros.rbegin ();
       vMacrosIterator != vMacros.rend (); vMacrosIterator++)
    source.insert(0,*vMacrosIterator);
}

void IShaderProgram::getFileContents(const string& fileName, string& source) const
{
  string buffer;

  ifstream input(fileName.c_str());

  if (!input.is_open()){
    cerr << "Could not open shader named " << fileName << endl;
    return;
  }

  getline(input, source, '\0');

  input.close();
}

void IShaderProgram::splitStringInStringsArray(const string& names, vector<string>& splitNames,
    const string& prefix, const string& suffix) const
{
  bool find = true;
  uint start=0,index=0;

  while (find)
  {
    string str,str2;

    index=names.find(SEPARATOR,start);

    str2=prefix;
    if (index != string::npos)
    {
      str = names.substr(start,index);
      start = index+1;
    }
    else
    {
      str = names.substr(start);
      find = false;
    }
    str2.append(str);
    str2.append(suffix);
    splitNames.push_back(str2);
  }
}

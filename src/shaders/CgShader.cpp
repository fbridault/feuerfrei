#include "CgShader.hpp"

#include <iostream>
#include "../common.hpp"

using namespace std;


CgShader::CgShader (const wxString& sourceName, const wxString& shaderName, const CGcontext* const context, CGGLenum type, 
		    bool recompile, const wxString& extraParameters)
{
  wxString sourcePath = _("src/shaders/") + sourceName;
  wxString shaderPath = _("src/shaders/") + shaderName;
  wxString compiledName = SHADERS_OBJECTS_PATH + shaderName + _(".o");
  
  char buffer[255];
  
  //   wxString cmd; 
  //   wxArrayString output;
  //   cmd += (_("cgc -q  -o "));
  //   cmd += compiledName;
  //   cmd += _(" -entry ");
  //   cmd += shaderName;
   
  //   // Création et chargement du vertex program
  //   if(type == CG_GL_VERTEX){
  //     cmd += (_(" -profile arbvp1 "));
  //   }else{
  //     cmd += (_(" -profile arbfp1 "));
  //   }
  
  //   cmd += sourceName;
  
  //   cout << cmd.mb_str() << endl;
  
  //system(cmd.ToAscii());
  //    int code = wxExecute( cmd, wxEXEC_SYNC|wxEXEC_RELATIVE_PATH );
  
  if(type == CG_GL_VERTEX){
    profile = CG_PROFILE_VP40;
    if(recompile) {
      cerr << "Compiling Cg shader : " << shaderPath.ToAscii() << endl;
      sprintf(buffer,"cgc -q -fastmath -fastprecision -o %s -entry %s -profile vp40 %s %s",
	      (const char*)compiledName.ToAscii(),(const char*)shaderName.ToAscii(),(const char*)extraParameters.ToAscii(),
	      (const char*)sourcePath.ToAscii());
      system(buffer);
    }
  }else{
    profile = CG_PROFILE_FP40;
    if(recompile) {
      cerr << "Compiling Cg shader : " << shaderPath.ToAscii() << endl;
      sprintf(buffer,"cgc -q -fastmath -fastprecision -o %s -entry %s -profile fp40 %s %s",
	      (const char*)compiledName.ToAscii(),(const char*)shaderName.ToAscii(),(const char*)extraParameters.ToAscii(),
	      (const char*)sourcePath.ToAscii());
      system(buffer);
    }
  }
  
  program = cgCreateProgramFromFile (*context, CG_OBJECT, compiledName.mb_str(), profile, shaderPath.mb_str(), 0);
  
  // Initialiser les profils et les options du compilateur CG
  //    profile = cgGLGetLatestProfile(type);
  //    if(profile==CG_PROFILE_UNKNOWN)
  //      cerr << "Cg Error : Unknown Profile for shader " << shaderPath.mb_str() << endl;
  //    else
  //      cerr << "profile for shader " <<  shaderPath.mb_str() << " : " << cgGetProfileString(profile) << endl;
  
  //   cgGLSetOptimalOptions(profile);
  //   program = cgCreateProgramFromFile (*context, CG_SOURCE, sourceName.mb_str(), profile, shaderPath.mb_str(), 0);
  
  cgGLLoadProgram (program);
}

CgShader::~CgShader ()
{
  if (program)
    cgDestroyProgram (program);
}

CgBasicVertexShader::CgBasicVertexShader(const wxString& sourceName, const wxString& shaderName, const CGcontext* const context, bool recompile) : 
  CgShader (sourceName, shaderName, context, CG_GL_VERTEX, recompile)
{
  // Récupération des paramètres
  modelViewProjectionMatrix = cgGetNamedParameter(program, "ModelViewProj");
}

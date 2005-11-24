#include "CgShader.hpp"

void fini()
{
  cout << "fini !" << endl;
}

CgShader::CgShader (const wxString& sourceName, const wxString& shaderName, CGcontext *context, CGGLenum type, bool recompile)
{
  wxString compiledName = shaderName + _(".o");

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
    profile = CG_PROFILE_ARBVP1;
    if(recompile) {
      cerr << "RECOMPILE" << endl;
      sprintf(buffer,"cgc -q -fastmath -fastprecision -o %s -entry %s -profile arbvp1 %s",
	      (const char*)compiledName.ToAscii(),(const char*)shaderName.ToAscii(),(const char*)sourceName.ToAscii());
      system(buffer);
    }
  }else{
    profile = CG_PROFILE_ARBFP1;
    if(recompile) {
      cerr << "RECOMPILE" << endl;
      sprintf(buffer,"cgc -q -fastmath -fastprecision -o %s -entry %s -profile arbfp1 %s",
	      (const char*)compiledName.ToAscii(),(const char*)shaderName.ToAscii(),(const char*)sourceName.ToAscii());
      system(buffer);
    }
  }
     
  program = cgCreateProgramFromFile (*context, CG_OBJECT, compiledName.mb_str(), profile, shaderName.mb_str(), 0);
  
  // Initialiser les profils et les options du compilateur CG
  //    profile = cgGLGetLatestProfile(type);
  //    if(profile==CG_PROFILE_UNKNOWN)
  //      cerr << "Cg Error : Unknown Profile for shader " << shaderName.mb_str() << endl;
  //    else
  //      cerr << "profile for shader " <<  shaderName.mb_str() << " : " << cgGetProfileString(profile) << endl;
  
  //   cgGLSetOptimalOptions(profile);
  //   program = cgCreateProgramFromFile (*context, CG_SOURCE, sourceName.mb_str(), profile, shaderName.mb_str(), 0);
  
  cgGLLoadProgram (program);
}

CgShader::~CgShader ()
{
  if (program)
    cgDestroyProgram (program);
}

CgBasicVertexShader::CgBasicVertexShader(const wxString& sourceName, const wxString& shaderName, CGcontext *context, bool recompile) : 
  CgShader (sourceName, shaderName, context, CG_GL_VERTEX, recompile)
{
  // Récupération des paramètres
  modelViewProjectionMatrix = cgGetNamedParameter(program, "ModelViewProj");
}

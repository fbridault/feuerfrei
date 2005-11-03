#include "CgShader.hpp"

CgShader::CgShader (const wxString& sourceName, const wxString& shaderName, CGcontext *context, CGGLenum type)
{
  // Initialiser les profils et les options du compilateur CG
  profile = cgGLGetLatestProfile(type);
  if(profile==CG_PROFILE_UNKNOWN)
    cerr << "Cg Error : Unknown Profile for shader " << shaderName.fn_str() << endl;
  else
    cerr << "profile for shader " <<  shaderName.fn_str() << " : " << cgGetProfileString(profile) << endl;

  cgGLSetOptimalOptions(profile);
  
  // Création et chargement du vertex program
  program = cgCreateProgramFromFile (*context, CG_SOURCE, sourceName.fn_str(), profile, shaderName.fn_str(), 0);
  
  cgGLLoadProgram (program);
}

CgShader::~CgShader ()
{
  if (program)
    cgDestroyProgram (program);
}

#include "CgSPFragmentShader.hpp"

CgSPFragmentShader::CgSPFragmentShader(const wxString& sourceName, const wxString& shaderName, 
				       CGcontext *context, IESList *ieslist, uint type, bool recompile) :
  CgShader (sourceName, shaderName, context, CG_GL_FRAGMENT, recompile)
{
  interp = type & 1;
  iesList = ieslist;
  
  // Récupération des paramètres
  paramTextureSP = cgGetNamedParameter(program, "textureSP");
  if(interp)
    lazimut_lzenith_denom = cgGetNamedParameter(program, "lazimut_lzenith_denom");
  else
    lazimut_lzenith = cgGetNamedParameter(program, "lazimut_lzenith");
  TextureSPMatrix = cgGetNamedParameter(program, "TextureSPMatrix");
  paramCentreSP = cgGetNamedParameter(program, "centreSP");
  paramFluctuationIntensite = cgGetNamedParameter(program, "fluctuationIntensite");
  if(type & 2)
    paramIsTextured = cgGetNamedParameter(program, "isTextured");
}

CgSPFragmentShader::~CgSPFragmentShader()
{
}

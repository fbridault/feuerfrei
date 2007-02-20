#include "CgSPFragmentShader.hpp"

CgSPFragmentShader::CgSPFragmentShader(const wxString& sourceName, const wxString& shaderName, uint nbFlames, 
				       const CGcontext* const context, uint type, bool recompile) :
  CgShader (sourceName, shaderName, context, CG_GL_FRAGMENT, recompile, wxString::Format(_("%s%d"),_("-DNB_SOURCES="), nbFlames))
{  
  interp = type & 1;
  m_nbFlames = nbFlames;
  
  // Récupération des paramètres
  paramTextureSP = cgGetNamedParameter(program, "textureSP");
  if(interp)
    lazimut_lzenith_denom = cgGetNamedParameter(program, "lazimut_lzenith_denom");
  else
    lazimut_lzenith = cgGetNamedParameter(program, "lazimut_lzenith");
//   TextureSPMatrix = cgGetNamedParameter(program, "TextureSPMatrix");
  paramCentreSP = cgGetNamedParameter(program, "centreSP");
  paramFluctuationIntensite = cgGetNamedParameter(program, "fluctuationIntensite");
  if(type & 2)
    paramIsTextured = cgGetNamedParameter(program, "isTextured");

  paramIncR = cgGetNamedParameter(program, "incr");
}

CgSPFragmentShader::~CgSPFragmentShader()
{
}

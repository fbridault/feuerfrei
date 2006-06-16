#include "CgSPFragmentShader.hpp"

CgSPFragmentShader::CgSPFragmentShader(const wxString& sourceName, const wxString& shaderName, uint nbFlames, CGcontext *context,
				       uint type, const wxString& extraParameters, bool recompile) :
  CgShader (sourceName, shaderName, context, CG_GL_FRAGMENT, recompile, extraParameters)
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

  paramTex2DSize = cgGetNamedParameter(program, "tailleTex2D");
}

CgSPFragmentShader::~CgSPFragmentShader()
{
}

#include "CgSPFragmentShader.hpp"

CgSPFragmentShader::CgSPFragmentShader(const wxString& sourceName, const wxString& shaderName, 
				       CGcontext *context, IESList *ieslist, bool type) :
  CgShader (sourceName, shaderName, context, CG_GL_FRAGMENT)
{
  interp = type;
  iesList = ieslist;
  // Chargement et creation de la texture du solide photometrique (image test)
  
  // Creation de la texture du solide photometrique
  texture_solide_photometrique = new Texture(iesList->getCurrentIESfile()->getNbazimut(),
					     iesList->getCurrentIESfile()->getNbzenith(),
					     iesList->getCurrentIESfile()->getIntensites());
  
  // Récupération des paramètres
  paramTextureSP = cgGetNamedParameter(program, "textureSP");
  if(interp)
    lazimut_lzenith_denom = cgGetNamedParameter(program, "lazimut_lzenith_denom");
  else
    lazimut_lzenith = cgGetNamedParameter(program, "lazimut_lzenith");
  TextureSPMatrix = cgGetNamedParameter(program, "TextureSPMatrix");
  paramCentreSP = cgGetNamedParameter(program, "centreSP");
  paramFluctuationIntensite = cgGetNamedParameter(program, "fluctuationIntensite");
}

CgSPFragmentShader::~CgSPFragmentShader()
{
  delete texture_solide_photometrique;
}

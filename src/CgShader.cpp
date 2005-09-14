#include "CgShader.hpp"

CgShader::CgShader (char *sourceName, char *shaderName, CGcontext *context, CGGLenum type)
{
  // Initialiser les profils et les options du compilateur CG
  profile = cgGLGetLatestProfile(type);
  //printf("profile : %s\n",cgGetProfileString(shadowProfile));
  cgGLSetOptimalOptions(profile);
  
  // Création et chargement du vertex program
  program =
    cgCreateProgramFromFile (*context, CG_SOURCE, sourceName, profile, shaderName, 0);
//   if (vertexProgram == 0) /* test la compilation du programme cg */
//   {
//       /* recuperer le code d'erreur */
//       CGerror Error = cgGetError();
//       /* afficher le message d'erreur correspondant a ce code */
//       printf("*** Erreur de compilation du vertex program vpSPTEX ***\n");
//       printf("%s \n",cgGetErrorString(Error));
//       printf("%s \n",cgGetLastListing(*context));
//       exit(-1);
//     }
  cgGLLoadProgram (program);
}

CgShader::~CgShader ()
{
  if (program)
    cgDestroyProgram (program);
}

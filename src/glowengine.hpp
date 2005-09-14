#if !defined(GLOWENGINE_H)
#define GLOWENGINE_H

class GlowEngine;

#include <header.h>

#include "extgl.h"
#include "pbuffer.h"
#include <Cg/cgGL.h>
#include <GL/glut.h>


class GlowEngine
{
private: 
  int				_iWidth, _iHeight; // dimensions des textures
  int				win; // handler sur le contexte d'afichage
    
  unsigned int	_iTexture[2]; // textures codants les origines [0] et les directions [1] des rayons

  // prog Cgx
  
  void init_glutnCg();

  PBuffer		pbuffer;
  CGcontext		cgContext;
  CGprofile		cgvpProfile,cgfpProfile;

  float			imgdata[TAILLE_PBFX*TAILLE_PBFY*3];
  float			imgdata2[TAILLE_PBFX*TAILLE_PBFY*3];
  float			imgdataout[TAILLE_PBFX*TAILLE_PBFY*4];
  //	float			depthdata[TAILLE_PBFX*TAILLE_PBFY];
    
public:
  
  GlowEngine();
  virtual ~GlowEngine();

  

};

#endif

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
  int width, height; // dimensions de la texture
  
  /** Pbuffer */
  PBuffer pbuffer;
  /** Pointeur sur le contexte CG */
  CGcontext *context;
  /** Vertex Shader pour le blur */
  CgBlurVertexShader blurVertexShaderX, blurVertexShaderY;
  /** Fragment Shader pour le blur */
  CgBlurFragmentShader blurFragmentShader;
  /** Indice de la texture servant à réaliser le blur */
  GLuint texblur;
    
public:
  
  GlowEngine();
  virtual ~GlowEngine();

  

};

#endif

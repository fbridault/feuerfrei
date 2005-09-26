#if !defined(GLOWENGINE_H)
#define GLOWENGINE_H

class GlowEngine;

#include <header.h>

#include <Cg/cgGL.h>
#include <GL/glut.h>
#include "pbuffer.hpp"
#include "CgBlurShaders.hpp"
#include "scene.hpp"
#include "eyeball.hpp"

class GlowEngine
{
public:
  
  GlowEngine(CScene *s, Eyeball *e, CGcontext *cgcontext, int w, int h);
  virtual ~GlowEngine();

  void activate();
  void render();
  void deactivate();
  void drawBlur();

private: 
  int width, height; // dimensions de la texture
  int scaleFactor;
  
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
  
  CScene *scene;
  Eyeball *eyeball;
  Texture *textest;
};

#endif

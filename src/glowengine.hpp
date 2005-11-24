#if !defined(GLOWENGINE_H)
#define GLOWENGINE_H

class GlowEngine;

#include <header.h>

#include <Cg/cgGL.h>

#include "pbuffer.hpp"
#include "CgBlurShaders.hpp"
#include "scene.hpp"
#include "camera.hpp"


class GlowEngine
{
public:
  
  GlowEngine(CScene *s, Camera *e, CGcontext *cgcontext, int w, int h, int sc, bool recompileShaders);
  virtual ~GlowEngine();

  /** Active le rendu du Glow, c'est-à-dire que toutes ce qui sera dessiné après l'appel à cette
   * fonction sera considérée comme étant une source de glow, donc rendue dans le pbuffer
   */
  void activate();
  /** Effectue le blur en trois passes */
  void blur();
  /** Désactive le glow, les appels suivants dessineront dans le color buffer */
  void deactivate();
  /** Plaque le blur à l'écran */
  void drawBlur(double alpha);

  void setGaussSigma(double sigma)
  {
    blurFragmentShader.computeWeights(sigma);
  }

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
  Camera *camera;
  //Texture *textest;
};

#endif

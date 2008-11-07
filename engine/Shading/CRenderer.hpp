#ifndef CRENDERER_H
#define CRENDERER_H

#include "IRenderer.hpp"
#include "../Common.hpp"

class CRenderTarget;
class CCamera;

// ====================================================
// renderer for forward rendering
//
class CForwardRenderer : public IRenderer
{
public: // ---------------------------------------------------

  CForwardRenderer(CScene& a_rScene);             // Default Constructor, ctl is a pointer to OpenGL controller necessary for scene drawing.
  virtual ~CForwardRenderer(){};

  void drawDirect(const ILight& src) const;   // Drawing method for direct lighting.
  void drawBrdf() const;                                		// Drawing method for brdf factor.
};



// ====================================================
// renderer for deferred rendering
//
class CDeferredRenderer : public IRenderer
{
public: // ---------------------------------------------------

  CDeferredRenderer(CScene&a_rScene, uint width, uint height, const CCamera& camera); // Default Constructor, ctl is a pointer to OpenGL controller necessary for scene drawing.
  virtual ~CDeferredRenderer();

  void drawDirect(const ILight& src) const;                     // Drawing method for direct lighting.
  void drawBrdf() const;                                        // Drawing method for brdf factor.

  void genGBuffer() const;                                      // Generate GBuffer

  void drawColorsTex(uint width, uint height) const;
  void drawNormalsTex(uint width, uint height) const;
  void drawPositionsTex(uint width, uint height) const;

  void resizeGBuffer(uint width, uint height)
  {
    destroyGBuffer();
    initGBuffer(width,height);
  }

private:
  void drawTex(uint width, uint height, uint i) const;
  void initGBuffer(uint width, uint height);
  void destroyGBuffer();

private:
  CShader *m_gBufferShader;                                 // GBuffer
  CRenderTarget *m_gBufferTargets;
  const CCamera& m_camera;
};

#endif // CRENDERER_H

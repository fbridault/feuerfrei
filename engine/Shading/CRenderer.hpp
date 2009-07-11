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

  CForwardRenderer(CScene& a_rScene);             // Default Constructor
  virtual ~CForwardRenderer(){};

  void drawDirect(const ILight& src, CRenderList const& a_rRenderList) const;    // Drawing method for direct lighting.
  void drawBrdf(CRenderList const& a_rRenderList) const;                         // Drawing method for brdf factor.
};



// ====================================================
// renderer for deferred rendering
//
class CDeferredRenderer : public IRenderer
{
public: // ---------------------------------------------------

  CDeferredRenderer(CScene&a_rScene, uint width, uint height, const CCamera& camera); // Default Constructor
  virtual ~CDeferredRenderer();

  void drawDirect(const ILight& src, CRenderList const& a_rRenderList) const;    // Drawing method for direct lighting.
  void drawBrdf(CRenderList const& a_rRenderList) const;                         // Drawing method for brdf factor.

  void genGBuffer(CRenderList const& a_rRenderList) const;                       // Generate GBuffer

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
  CShader *m_pGBufferShader;                                 // GBuffer
  CRenderTarget *m_pGBufferTargets;
  const CCamera& m_rCamera;
};

#endif // CRENDERER_H

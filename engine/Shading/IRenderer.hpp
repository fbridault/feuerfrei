#ifndef IRENDERER_H
#define IRENDERER_H

class CScene;
class ILight;
class CShader;
class CRenderList;

// ====================================================
// renderer interface
//
class IRenderer
{
protected: // ---------------------------------------------------

  IRenderer();                         // Default Constructor.
  virtual ~IRenderer();

public: // ---------------------------------------------------

  virtual void drawDirect(const ILight& src, CRenderList const& a_rRenderList) const = 0;  // Drawing method for direct lighting.
  virtual void drawBrdf(CRenderList const& a_rRenderList) const = 0;                       // Drawing method for brdf factor.

  void toggleSeeShadows() { m_bShadows = !m_bShadows; };

protected: // ---------------------------------------------------

  CShader *m_pBrdfShader, *m_pDirectSpotShader, *m_pDirectOmniShader, *m_pIndirectShader;
  bool m_bShadows;
};

#endif // IRENDERER_H

#ifndef IRENDERER_H
#define IRENDERER_H

class CScene;
class ILight;
class CShader;

// ====================================================
// renderer interface
//
class IRenderer
{
protected: // ---------------------------------------------------

  IRenderer(CScene& a_rScene);                            // Default Constructor, ctl is a pointer to OpenGL controller necessary for scene drawing.
  virtual ~IRenderer();

public: // ---------------------------------------------------

  virtual void drawDirect(const ILight& src) const = 0;         // Drawing method for direct lighting.
  virtual void drawBrdf() const = 0;                            // Drawing method for brdf factor.

  void toggleSeeShadows() { m_bShadows = !m_bShadows; };

protected: // ---------------------------------------------------

  CScene &m_rScene;

  CShader *m_brdfShader, *m_directSpotShader, *m_directOmniShader, *m_indirectShader;
  bool m_bShadows;
};

#endif // IRENDERER_H

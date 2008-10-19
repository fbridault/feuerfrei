#ifndef GAMMASHADER_H
#define GAMMASHADER_H

class CGammaFX;

#include "../Shading/Glsl.hpp"
#include "../Shading/CRenderTarget.hpp"
#include "../Scene/Texture.hpp"

/** Classe servant d'interface avec le fragment program de la correction gamma.
 *
 * @author	Flavien Bridault
 */
class CGammaFX
{
public:
    /** Constructeur par défaut.
   * @param sourceName Nom du fichier source.
   * @param shaderName Nom du programme Cg.
   * @param context CPointeur vers le contexte Cg (il doit être déjà créé).
   * @param recompile Indique s'il faut recompiler le shader à partir du fichier glsl ou si le .o est déjà compilé.
   */
  CGammaFX(uint width, uint height,  bool recompile=true);
  /** Destructeur. */
  virtual ~CGammaFX();

  void setSize(uint width, uint height)
  {
    m_width = width; m_height=height;
    delete m_renderTarget;
    m_renderTarget = new CRenderTarget("color rect rgba depthbuffer nearest",width, height,0);
  }

  /** Affecte la correction gamma.
   * @param gamma Valeur du gamma.
   */
  void SetGamma(GLfloat gamma){
    m_gamma = gamma;
  };

  void enableGamma()
  {
    m_renderTarget->bindTarget();
    /* Effacement de la texture */
    glClear(GL_COLOR_BUFFER_BIT);
  };

  void disableGamma(){
    m_renderTarget->bindDefaultTarget();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glActiveTexture(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_RECTANGLE_ARB);

    m_oShader.Enable();
    m_oShader.SetUniform1f("gamma", m_gamma);
    m_renderTarget->drawTextureOnScreen(m_width,m_height, 0);
    m_oShader.Disable();

    glDisable(GL_TEXTURE_RECTANGLE_ARB);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
  };

private:
  CShader m_oShader;
  GLfloat m_gamma;
  CRenderTarget *m_renderTarget;
  uint m_width, m_height;
};

#endif

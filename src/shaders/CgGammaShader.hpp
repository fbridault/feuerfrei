#ifndef CGGAMMASHADER_H
#define CGGAMMASHADER_H

class CgGammaShader;

#include "CgShader.hpp"

#include "../flames/fbo.hpp"
#include "../scene/texture.hpp"
#include <GL/glu.h>

class CgShader;

/** Classe servant d'interface avec le fragment program de la correction gamma.
 *
 * @author	Flavien Bridault
 */
class CgGammaShader : public CgShader
{
public:
    /** Constructeur par défaut.
   * @param sourceName Nom du fichier source.
   * @param shaderName Nom du programme Cg.
   * @param context Pointeur vers le contexte Cg (il doit être déjà créé).
   * @param recompile Indique s'il faut recompiler le shader à partir du fichier .cg ou si le .o est déjà compilé.
   */
  CgGammaShader(const wxString& sourceName, const wxString& shaderName, uint width, uint height, const CGcontext* const context, bool recompile=true);
  /** Destructeur. */
  virtual ~CgGammaShader();
  
  /** Affecte la correction gamma.
   * @param gamma Valeur du gamma.
   */
  void SetGamma(GLdouble gamma){
    m_gamma = gamma;
  };
  
  void enableGamma()
  {
    m_fbo.Activate();
    /* Effacement de la texture */
    glClear(GL_COLOR_BUFFER_BIT);
  };
  
  void disableGamma(){
    m_fbo.Deactivate();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glEnable(GL_TEXTURE_RECTANGLE_ARB);
  
    cgGLSetParameter1d(m_paramGamma, m_gamma);
    enableShader();
    m_renderTex->drawOnScreen(m_width,m_height);
    disableShader();

    glDisable(GL_TEXTURE_RECTANGLE_ARB);  
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    glEnable(GL_DEPTH_TEST);
  };
  
private:
  CGparameter m_paramGamma;  
  GLdouble m_gamma;
  FBO m_fbo;
  Texture *m_renderTex;
  uint m_width, m_height;
};

#endif

#if !defined(GLOWENGINE_H)
#define GLOWENGINE_H

class GlowEngine;

#include "flames.hpp"
#include "fbo.hpp"
#include "../shaders/CgBlurShaders.hpp"
#include "../scene/scene.hpp"
#include "../scene/camera.hpp"

#define GLOW_LEVELS 2

/** Classe regroupant des méthodes pour réaliser un glow. Après l'appel au constructeur,
 * qui va instancier deux FBOs, la fonction de dessin pour obtenir un blur se décompose comme ceci :<br><br>
 * glowEngine.activate();<br>
 * // Réglages de la largeur de la Gaussienne utilisée pour le blur<br>
 * m_glowEngine->setGaussSigma(4.5);<br>
 * // Opérations de dessin - 1ère passe<br>
 * ......................<br>
 * // Réalisation du blur<br>
 * glowEngine.blur();<br>
 * glowEngine.deactivate();<br>
 *<br>
 * // Opérations de dessin - 2ème passe<br>
 * ......................<br>
 *<br>
 * // Dessin du blur<br>
 * glBlendFunc (GL_ONE, GL_ONE);<br>
 * glDisable (GL_DEPTH_TEST);<br>
 * glowEngine.drawBlur(1.0);<br>
 * glEnable (GL_DEPTH_TEST); 
 */
class GlowEngine
{
public:
  /** Contructeur par défaut.
   * @param w largeur du viewport
   * @param h hauteur du viewport
   * @param scaleFactor rapport entre le viewport et la texture utilisée, typiquement > 1
   * @param recompileShaders indique s'il faut compiler ou non les shaders
   * @param cgcontext contexte Cg
   */
  GlowEngine(unsigned int w, unsigned int h, int scaleFactor[GLOW_LEVELS], bool recompileShaders, CGcontext *cgcontext );
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
    m_blurFragmentShader.computeWeights(sigma);
  }

private: 
  /** Dimensions de la texture */
  unsigned int m_width[GLOW_LEVELS], m_height[GLOW_LEVELS];
  unsigned int m_initialWidth, m_initialHeight;
  
  /** Rapport d'échelle entre la taille du viewport et de la texture du blur */
  unsigned int m_scaleFactor[GLOW_LEVELS];
  
  /** Pbuffer */
  //  PBuffer m_pbuffer;
  FBO m_firstPassFBOs[GLOW_LEVELS], m_secondPassFBOs[GLOW_LEVELS];
  /** Vertex Shader pour le blur en X */
  CgBlurVertexShader m_blurVertexShaderX;
  /** Vertex Shader pour le blur en Y */
  CgBlurVertexShader m_blurVertexShaderY;
  /** Fragment Shader pour le blur */
  CgBlurFragmentShader m_blurFragmentShader;
  /** Textures servant à réaliser le blur */
  Texture *m_firstPassTex[GLOW_LEVELS], *m_secondPassTex[GLOW_LEVELS];
};

#endif


#if !defined(GLOWENGINE_H)
#define GLOWENGINE_H

class GlowEngine;

#include "flames.hpp"
#include "pbuffer.hpp"
#include "../shaders/CgBlurShaders.hpp"
#include "../scene/scene.hpp"
#include "../scene/camera.hpp"

/** Classe regroupant des méthodes pour réaliser un glow. Après l'appel au constructeur,
 * qui va instancier un pbuffer, la fonction de dessin pour obtenir un blur se décompose comme ceci :<br><br>
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
  GlowEngine(int w, int h, int scaleFactor, bool recompileShaders, CGcontext *cgcontext );
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
  int m_width, m_height;
  /** Rapport d'échelle entre la taille du viewport et de la texture du blur */
  int m_scaleFactor;
  
  /** Pbuffer */
  PBuffer m_pbuffer;
  /** Vertex Shader pour le blur en X */
  CgBlurVertexShader m_blurVertexShaderX;
  /** Vertex Shader pour le blur en Y */
  CgBlurVertexShader m_blurVertexShaderY;
  /** Fragment Shader pour le blur */
  CgBlurFragmentShader m_blurFragmentShader;
  /** Indice de la texture servant à réaliser le blur */
  GLuint m_texblur;
};

#endif

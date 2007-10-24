#if !defined(GLOWENGINE_H)
#define GLOWENGINE_H

class GlowEngine;

#include "flames.hpp"
#include "fbo.hpp"
#include "../shaders/glsl.hpp"
#include "../scene/camera.hpp"
#include "../scene/texture.hpp"
#include "abstractFires.hpp"

#define GLOW_LEVELS 2
#define FILTER_SIZE 8

class GLFlameCanvas;

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
   * @param  indique s'il faut compiler ou non les shaders
   * @param cgcontext contexte Cg
   */
  GlowEngine(uint w, uint h, uint scaleFactor[GLOW_LEVELS-1] );
  virtual ~GlowEngine();

  /** Active le rendu du Glow, c'est-à-dire que toutes ce qui sera dessiné après l'appel à cette
   * fonction sera considérée comme étant une source de glow, donc rendue dans le FBO
   */
  void activate(){
    /* On dessine dans le FBO #1 */
    m_firstPassFBOs[0].Activate();
    /* On prend la résolution la plus grande */
    glViewport (0, 0, m_width[0], m_height[0]);
  }
  /** Désactive le glow, les appels suivants dessineront dans le color buffer */
  void deactivate()
  {
    m_firstPassFBOs[GLOW_LEVELS].Deactivate();
    glViewport (0, 0, m_initialWidth, m_initialHeight);
  };

  /** Effectue le blur en trois passes */
  void blur(GLFlameCanvas* const glBuffer);
  /** Plaque le blur à l'écran */
  void drawBlur(GLFlameCanvas* const glBuffer, bool glowOnly=false);
  
  /** Effectue le blur en trois passes */
  void blurFS(GLFlameCanvas* const glBuffer);
  /** Plaque le blur à l'écran */
  void drawBlurFS(GLFlameCanvas* const glBuffer, bool glowOnly=false);
  
  void computeWeights(uint index, float sigma);
  
  void deleteTex();
  void generateTex();
  
  void setSize(uint width, uint height)
  {
    m_initialWidth = width; m_initialHeight = height;
    deleteTex();
    generateTex();
  }
  
private: 
  /** Dimensions de la texture */
  uint m_width[GLOW_LEVELS], m_height[GLOW_LEVELS];
  uint m_initialWidth, m_initialHeight;
  
  /** Rapport d'échelle entre la taille du viewport et de la texture du blur */
  uint m_scaleFactor[GLOW_LEVELS];
  
  /** Tableau contenant la largeur du filtre */
  GLfloat m_offsets[6][FILTER_SIZE];
  /** Tableau contenant les poids des pixels du filtre */
  GLfloat m_weights[2][FILTER_SIZE];
  /** Diviseur correspondant à la somme des poids */
  GLfloat m_divide[3];
  
  /** FBOs */
  FBO m_firstPassFBOs[GLOW_LEVELS], m_secondPassFBOs[GLOW_LEVELS];
  GLSLProgram m_programX, m_programY, m_blurRendererProgram;
  /** Fragment Shader pour le blur */
  GLSLFragmentShader m_blurFragmentShader8X, m_blurFragmentShader8Y, m_blurRendererShader;
  /** Textures servant à réaliser le blur */
  Texture *m_firstPassTex[GLOW_LEVELS], *m_secondPassTex[GLOW_LEVELS];
};

#endif


#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "../scene/texture.hpp"

#include <assert.h>

#include <string>
#include <vector>

using namespace std;

/** La classe RenderTarget est une abstraction du m�canisme des FBOs, g�rant dans le m�me temps
 *  les textures utilis�es comme cibles pour le rendu.
 */
class RenderTarget
{
public:
  RenderTarget(uint width, uint height);
  /** Constructeur de RenderTarget.
   *
   * @param format  Param�tre comprenant un ensemble de cha�nes s�par�es par des espaces indiquant le format
   *                de la cible. Les choix sont les suivants : "color" ou "depth" pour rendre respectivement
   *                la couleur ou la profondeur. Si "color", "rgba" utilise 4 entiers non sign�s, "rgba16f"
   *                4 r�els sur 16 bits. Si "color", "depth buffer" permet d'instancier un render buffer pour
   *                la profondeur. Si "color", "cube" peut-�tre pr�cis� pour instancier une cube map. Attention
   *                dans ce cas, il ne peut y avoir qu'une seule texture cible, dont nbTexUnits doit �tre �gal � 1.
   *                Si "depth", "shadow" peut �tre indiqu� pour instancier une texture permettant les comparaisons
   *                de profondeur. "rect" ou "2D" d�terminent le type de texture et le filtrage peut-�tre pr�cis�
   *                gr�ce � "linear" ou "nearest".
   * @param width   Largeur de la cible.
   * @param height  Hauteur de la cible.
   * @param texUnit Num�ro de base de l'unit� de texture. Exemple, si texUnit=1 et nbTexUnits=2, alors les unit�s
   *                de texture utilis�es par bindTexture() seront successivements GL_TEXTURE1 et GL_TEXTURE2.
   * @param nbTex   Nombre de textures attach�es � la RenderTarget. Ne peut �tre sup�rieur � 1 si format contient
   *                "cube". Il doit �galement �tre inf�rieur � GL_MAX_COLOR_ATTACHMENTS_EXT dans tous les cas.
   */
  RenderTarget(const string& format, uint width, uint height, uint texUnit, uint attachment=-1);
  void addTarget(const string& format, uint texUnit, int attachment=-1);
  void addDepthRenderBuffer(void);

  virtual ~RenderTarget();

  /** Active la cible pour le rendu */
  void bindTarget() const
  {
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_frameBuffer );
    if (m_hasDepthRenderBuffer)
      glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_depthRenderBuffer );
  }

  /** Active la cible en tant que texture */
  void bindTexture() const
  {
    for (uint i=0; i < m_vTexUnits.size(); i++)
    {
      m_vRenderTextures[i]->bind(m_vTexUnits[i]);
    }
  }

  /** Active la cible en tant que texture */
  void bindTexture(uint i) const
  {
      assert(i < m_vTexUnits.size());
      m_vRenderTextures[i]->bind(m_vTexUnits[i]);
  }

  /** Rend la texture sur un quad � la taille de l'�cran. */
  void drawTextureOnScreen(uint width, uint height, uint i) const
  {
    m_vRenderTextures[i]->drawOnScreen(width, height);
  }

  /** Active uniquement le rendu dans un attachement dont le rang est pass� en param�tre. */
  void bindChannel(uint i) const
  {
#ifdef DEBUG
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &n);
    assert( i < (uint)n);
#endif
    glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT+i);
  }

  /** Active le framebuffer par d�faut, c'est � dire directement vers le back buffer. */
  void bindDefaultTarget() const
  {
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
    if (m_hasDepthRenderBuffer)
      glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );
  }

private:
  bool CheckStatus(void) const;

private:
  GLuint m_frameBuffer;
  GLuint m_depthRenderBuffer;

  vector<Texture *> m_vRenderTextures;

  uint m_width;
  uint m_height;

  bool m_hasDepthRenderBuffer, m_hasDepthTexture;
  vector<GLenum> m_vTexUnits;
};

#endif

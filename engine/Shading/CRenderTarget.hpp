#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "../Common.hpp"
#include "../Scene/Texture.hpp"

#include <string>
#include <vector>

using namespace std;

/** La classe CRenderTarget est une abstraction du mécanisme des FBOs, gérant dans le même temps
 *  les textures utilisées comme cibles pour le rendu.
 */
class CRenderTarget
{
public:
	CRenderTarget(uint width, uint height);
	/** Constructeur de CRenderTarget.
	 *
	 * @param format  Paramètre comprenant un ensemble de chaînes séparées par des espaces indiquant le format
	 *                de la cible. Les choix sont les suivants : "color" ou "depth" pour rendre respectivement
	 *                la couleur ou la profondeur. Si "color", "rgba" utilise 4 entiers non signés, "rgba16f"
	 *                4 réels sur 16 bits. Si "color", "depth buffer" permet d'instancier un render buffer pour
	 *                la profondeur. Si "color", "cube" peut-être précisé pour instancier une cube map. Attention
	 *                dans ce cas, il ne peut y avoir qu'une seule texture cible, dont nbTexUnits doit être égal à 1.
	 *                Si "depth", "shadow" peut être indiqué pour instancier une texture permettant les comparaisons
	 *                de profondeur. "rect" ou "2D" déterminent le type de texture et le filtrage peut-être précisé
	 *                grâce à "linear" ou "nearest".
	 * @param width   Largeur de la cible.
	 * @param height  Hauteur de la cible.
	 * @param texUnit Numéro de base de l'unité de texture. Exemple, si texUnit=1 et nbTexUnits=2, alors les unités
	 *                de texture utilisées par bindTexture() seront successivements GL_TEXTURE1 et GL_TEXTURE2.
	 * @param nbTex   Nombre de textures attachées à la CRenderTarget. Ne peut être supérieur à 1 si format contient
	 *                "cube". Il doit également être inférieur à GL_MAX_COLOR_ATTACHMENTS_EXT dans tous les cas.
	 */
	CRenderTarget(const string& format, uint width, uint height, uint texUnit, uint attachment=-1);
	void addTarget(const string& format, uint texUnit, int attachment=-1);
	void addDepthRenderBuffer(void);

	virtual ~CRenderTarget();

	/** Active la cible pour le rendu */
	void bindTarget() const
	{
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_uiFrameBuffer );
		if (m_bDepthRenderBuffer)
			glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_uiDepthRenderBuffer );
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

	/** Rend la texture sur un quad à la taille de l'écran. */
	void drawTextureOnScreen(uint width, uint height, uint i) const
	{
		m_vRenderTextures[i]->drawOnScreen(width, height);
	}

	/** Active uniquement le rendu dans un attachement dont le rang est passé en paramètre. */
	void bindChannel(uint i) const
	{
#ifdef DEBUG
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &n);
		assert( i < (uint)n);
#endif
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT+i);
	}

	/** Active le framebuffer par défaut, c'est à dire directement vers le back buffer. */
	void bindDefaultTarget() const
	{
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
		if (m_bDepthRenderBuffer)
			glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );
	}

private:
	bool CheckStatus(void) const;

private:
	GLuint m_uiFrameBuffer;
	GLuint m_uiDepthRenderBuffer;

	vector<ITexture *> m_vRenderTextures;

	uint m_uiWidth;
	uint m_uiHeight;

	bool m_bDepthRenderBuffer, m_bDepthTexture;
	vector<GLenum> m_vTexUnits;
};

#endif

#if !defined(GLOWENGINE_H)
#define GLOWENGINE_H

#include <engine/Shading/CRenderTarget.hpp>
#include <engine/Shading/Glsl.hpp>

#include "flames.hpp"

#define GLOW_LEVELS 2

class CRenderList;

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
	void activate()
	{
		/* On dessine dans le FBO #1 */
		m_apFirstPassRT[0]->BindTarget();
		/* On prend la résolution la plus grande */
		glViewport (0, 0, m_auiWidth[0], m_auiHeight[0]);
	}
	/** Désactive le glow, les appels suivants dessineront dans le color buffer */
	void deactivate()
	{
		CRenderTarget::BindDefaultTarget();
		glViewport (0, 0, m_uiInitialWidth, m_uiInitialHeight);
	};

	/** Effectue le blur en trois passes */
	void Blur(CRenderList const& a_rRenderList);

	/** Plaque le blur à l'écran */
	void DrawBlur(CRenderList const& a_rRenderList, bool glowOnly=false);

	void deleteTex();
	void generateTex();

	void setSize(uint width, uint height)
	{
		m_uiInitialWidth = width;
		m_uiInitialHeight = height;
		deleteTex();
		generateTex();
	}

private:
	/** Dimensions de la texture */
	uint m_auiWidth[GLOW_LEVELS], m_auiHeight[GLOW_LEVELS];
	uint m_uiInitialWidth, m_uiInitialHeight;

	/** Rapport d'échelle entre la taille du viewport et de la texture du blur */
	uint m_auiScaleFactor[GLOW_LEVELS];

	/** Tableau contenant la largeur du filtre */
	GLfloat m_afOffsets[6][FILTER_SIZE];

	/** CRenderTarget */
	CRenderTarget *m_apFirstPassRT[GLOW_LEVELS], *m_apSecondPassRT[GLOW_LEVELS];
	/** Shaders pour le blur */
	CShader m_oShaderX, m_oShaderY, m_oBlurRendererShader;
};

/*********************************************************************************************************************/
/**		ClassCGlowState	          			  	 																 */
/*********************************************************************************************************************/
class CGlowState : public ITSingleton<CGlowState>
{
	friend class ITSingleton<CGlowState>;
	friend class GlowEngine;

private:
	/**
	 * Constructeur par défaut.
	 */
	CGlowState() :
		m_bGlowEnabled(false),
		m_uiGlowPass(0)
	{}

public:

	uint GetPassNumber() const { return m_uiGlowPass; }
	bool IsEnabled() const { return m_bGlowEnabled; }

private:

//---------------------------------------------------------------------------------------------------------------------
//  Private methods - only CGlowEngine is allowed to use them
//---------------------------------------------------------------------------------------------------------------------

	void Enable() { m_bGlowEnabled = true; }
	void Disable() { m_bGlowEnabled = false; }
	void SetPassNumber(uint a_uiGlowPass) { m_uiGlowPass = a_uiGlowPass; }

	/** Glow actif ou non */
	bool m_bGlowEnabled;

	uint m_uiGlowPass;
};
#endif


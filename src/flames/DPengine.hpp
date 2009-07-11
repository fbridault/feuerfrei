#if !defined(DPENGINE_H)
#define DPENGINE_H

class GLFlameCanvas;
class DepthPeelingEngine;
class CRenderTarget;
class CRenderList;

#include "flames.hpp"
#include <engine/Scene/CScene.hpp>
#include <engine/Scene/CCamera.hpp>


/** Classe regroupant des méthodes implémentant le Depth Peeling.
 */
class DepthPeelingEngine
{
public:
	/** Contructeur par défaut.
	 * @param w largeur du viewport
	 * @param h hauteur du viewport
	 * @param scaleFactor rapport entre le viewport et la texture utilisée, typiquement > 1
	 * @param  indique s'il faut compiler ou non les shaders
	 * @param cgcontext contexte Cg
	 */
	DepthPeelingEngine(uint width, uint height, uint nbLayers);
	virtual ~DepthPeelingEngine();

	void deleteTex();
	void generateTex();

	/** Epluche les flammes en fonction de leur profondeur en plusieurs calques. Une fois cette méthode appelée,
	 * il faut utiliser la méthode render() pour afficher les flammes.<br>
	 * @param flames Vecteur contenant les flammes.
	 * @param displayFlames Indique si les flammes doivent être affichées.
	 * @param displayParticles Indique si les particules doivent être affichées.
	 * @param boundingVolume Le cas échéant, volume englobant à afficher à la place des flammes.
	 * (le paramètre displayFlames doit tout de même être à <i>true</i> pour les visualiser)
	 */
	void makePeels(CRenderList const& a_rRenderList);
	void render(CRenderList const& a_rRenderList);
	void renderFS();

	void addLayer()
	{
		m_nbLayers = (m_nbLayers < m_nbLayersMax) ? m_nbLayers + 1 : m_nbLayers;
	};

	void removeLayer()
	{
		m_nbLayers = (m_nbLayers > 0) ? m_nbLayers - 1 : m_nbLayers;
	};

	/** Met à jour le nombre de layers utilisés pour le rendu.
	 * On ne vérifie pas si ce nombre est correct pour le moment puisque
	 * la fonction est appelée par le slider de l'interface qui est borné
	 */
	void setNbLayers(uint value)
	{
		m_nbLayers = value;
	};

	void setSize(uint width, uint height)
	{
		m_width = width;
		m_height = height;
		deleteTex();
		generateTex();
	}

private:
	/** Dimensions de la texture */
	uint m_width, m_height;

	uint m_nbLayersMax, m_nbLayers;
	CDepthTexture *m_alwaysTrueDepthTex;
	CRenderTarget *m_renderTarget[2], *m_sceneDepthRenderTarget;
	GLuint m_curDepthTex;

	CShader m_oDpShader, m_oDpRendererShader;

	GLuint m_flamesDisplayList;
};

#endif

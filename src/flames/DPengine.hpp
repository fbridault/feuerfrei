#if !defined(DPENGINE_H)
#define DPENGINE_H

class GLFlameCanvas;
class DepthPeelingEngine;
class CRenderTarget;

#include "flames.hpp"
#include "fbo.hpp"
#include "../scene/scene.hpp"
#include "../scene/camera.hpp"

class ARBFragmentShader
{
public:
  ARBFragmentShader() { glGenProgramsARB(1, &program); };

  virtual ~ARBFragmentShader()
  {
    glDeleteProgramsARB(1, &program);
  }

  void enableShader()
  {
    glEnable(GL_FRAGMENT_PROGRAM_ARB);
    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, program);
  }

  void disableShader()
  {
    glDisable(GL_FRAGMENT_PROGRAM_ARB);
  }

  void load(const char * prog_text)
  {
    enableShader();
    glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, (GLuint)strlen(prog_text), prog_text);
    GLint errpos;
    glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errpos);
    if(errpos != -1)
      {
	fprintf(stderr, "program error:\n");
	int bgn = errpos - 10;

	const char * c = (const char *)(prog_text + bgn);
	for(int i = 0; i < 30; i++)
	  {
	    if(bgn+i >= int(strlen(prog_text)-1))
	      break;
	    fprintf(stderr, "%c", *c++);
	  }
	fprintf(stderr, "\n");
      }
    disableShader();
  }
private:
  GLuint program;

};

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
  void makePeels(GLFlameCanvas* const glBuffer, const Scene* const scene);
  void render(GLFlameCanvas* const glBuffer);
  void renderFS();

  void addLayer() { m_nbLayers = (m_nbLayers < m_nbLayersMax) ? m_nbLayers+1 : m_nbLayers;};
  void removeLayer() { m_nbLayers = (m_nbLayers > 0) ? m_nbLayers-1 : m_nbLayers;};

  /** Met à jour le nombre de layers utilisés pour le rendu.
   * On ne vérifie pas si ce nombre est correct pour le moment puisque
   * la fonction est appelée par le slider de l'interface qui est borné
   */
  void setNbLayers(uint value) { m_nbLayers=value; };

  void setSize(uint width, uint height)
  {
    m_width = width; m_height=height;
    deleteTex();
    generateTex();
  }

private:
  /** Dimensions de la texture */
  uint m_width, m_height;

  uint m_nbLayersMax, m_nbLayers;
  CDepthTexture *m_alwaysTrueDepthTex;
  CRenderTarget *m_renderTarget[2], *m_sceneDepthCRenderTarget;
  GLuint m_curDepthTex;

  ARBFragmentShader m_peelProgram;
  GLSLProgram m_dpRendererProgram;
  GLSLFragmentShader m_dpRendererShader;

  GLuint m_flamesDisplayList;
};

#endif

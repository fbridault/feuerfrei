#if !defined(DPENGINE_H)
#define DPENGINE_H


class DepthPeelingEngine;

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

  void setParameter4d(double a){
    glProgramLocalParameter4dARB(GL_FRAGMENT_PROGRAM_ARB, 1, a, a, a, a);
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
   * @param recompileShaders indique s'il faut compiler ou non les shaders
   * @param cgcontext contexte Cg
   */
  DepthPeelingEngine(uint width, uint height, uint nbLayers, Scene *scene, FireSource **flames, uint nbFlames, CGcontext *cgcontext );
  virtual ~DepthPeelingEngine();
  
  void makePeels(bool displayParticles);
  void render();
  
  void addLayer() { m_nbLayers = (m_nbLayers < m_nbLayersMax) ? m_nbLayers+1 : m_nbLayers;};
  void removeLayer() { m_nbLayers = (m_nbLayers > 0) ? m_nbLayers-1 : m_nbLayers;};

  /** Met à jour le nombre de layers utilisés pour le rendu.
   * On ne vérifie pas si ce nombre est correct pour le moment puisque
   * la fonction est appelée par le slider de l'interface qui est borné
   */
  void setNbLayers(uint value) { m_nbLayers=value; };
  
private: 
  /** Dimensions de la texture */
  uint m_width, m_height;
  
  uint m_nbLayersMax, m_nbLayers;
  FBO m_fbo;
  Texture *m_colorTex;
  Texture *m_depthTex[3], *m_sceneDepthTex;
  GLuint m_curDepthTex;

  Scene *m_scene;
  FireSource **m_flames;
  uint m_nbFlames;
  ARBFragmentShader m_peelProgram;

  GLuint m_flamesDisplayList;
};

#endif

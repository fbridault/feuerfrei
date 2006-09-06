#if !defined(DPENGINE_H)
#define DPENGINE_H


class DepthPeelingEngine;

#include "flames.hpp"
#include "fbo.hpp"
#include "../scene/scene.hpp"
#include "../scene/camera.hpp"

/** Abstraction d'un fragment program ARB. */
class ARBFragmentShader
{
public:
  /** Constructeur par défaut. */
  ARBFragmentShader() { glGenProgramsARB(1, &program); };
  
  /** Destructeur. */
  virtual ~ARBFragmentShader()
  { 
    glDeleteProgramsARB(1, &program);
  }
  
  /** Activation du shader. */
  void enableShader()
  {
    glEnable(GL_FRAGMENT_PROGRAM_ARB);
    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, program);
  }
  
  /** Désactivation du shader. */
  void disableShader()
  {
    glDisable(GL_FRAGMENT_PROGRAM_ARB);
  }
  
  /** Chargement du shader dont le listing est fourni en paramètre.
   * @param prog Listing du fragment program en assembleur.
   */
  void load(const char * prog)
  {
    enableShader();
    glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, (GLuint)strlen(prog), prog);
    GLint errpos;
    glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errpos);
    if(errpos != -1)
      {
	fprintf(stderr, "program error:\n");
	int bgn = errpos - 10;
	
	const char * c = (const char *)(prog + bgn);
	for(int i = 0; i < 30; i++)
	  {
	    if(bgn+i >= int(strlen(prog)-1))
	      break;
	    fprintf(stderr, "%c", *c++);
	  }
	fprintf(stderr, "\n");
      }
    disableShader();
  }
  
  /** Affectation d'un paramètre de type double. */
  void setParameter4d(double a){
    glProgramLocalParameter4dARB(GL_FRAGMENT_PROGRAM_ARB, 1, a, a, a, a);
  }
private:
  /** Identifiant du programme. */
  GLuint program;
  
};

/** Classe permettant d'afficher les flammes selon la technique du Depth Peeling.
 */
class DepthPeelingEngine
{
public:
  /** Contructeur par défaut.
   * @param width Largeur du viewport.
   * @param height Hauteur du viewport.
   * @param nbLayers Nombre de calques de profondeur maximum.
   * @param scene Pointeur sur la scène.
   * @param flames Tableau contenant les flammes.
   * @param nbFlames Nombre de flammes dans le tableau.
   * @param cgcontext Pointeur sur le contexte Cg.
   */
  DepthPeelingEngine(uint width, uint height, uint nbLayers, Scene *scene, FireSource **flames, uint nbFlames, CGcontext *cgcontext );
  /** Destructeur. */
  virtual ~DepthPeelingEngine();
  
  /** Décortiquage des flammes dans les calques selon la profondeur. */
  void makePeels(bool displayParticles);
  
  /** Affichage de tous les calques à plat. */
  void render();
  
  /** Ajoute un calque. Le nombre de calque ne peut excéder celui passé en paramètre au constructeur. */
  void addLayer() { m_nbLayers = (m_nbLayers < m_nbLayersMax) ? m_nbLayers+1 : m_nbLayers;};
  /** Supprime un calque. */
  void removeLayer() { m_nbLayers = (m_nbLayers > 0) ? m_nbLayers-1 : m_nbLayers;};
  
  /** Met à jour le nombre de calques utilisés pour le rendu. On ne vérifie pas si ce nombre est correct 
   * pour le moment puisque la fonction est appelée par le slider de l'interface qui est borné.
   * @param value Nombre de calques.
   */
  void setNbLayers(uint value) { m_nbLayers=value; };
  
private: 
  /** Largeur du viewport et donc des textures qui seront utilisées. */
  uint m_width;
  /** Hauteur du viewport et donc des textures qui seront utilisées. */
  uint m_height; 
  
  /** Nombre de calques maximum. */
  uint m_nbLayersMax;  
  /** Nombre de calques courant. */
  uint m_nbLayers;
  
  /** Frame Buffer Object utilisé pour le rendu off-screen. */
  FBO m_fbo;
  /** Texture dans laquelle on affiche le rendu au fur et à mesure du peeling. */
  Texture *m_colorTex;
  /** Tableau de textures de profondeur utilisées pour l'extraction. */
  Texture *m_depthTex[3];
  /** Texture contenant la profondeur de la scène, elle est utilisée pour obtenir les occlusions des objets
   * de la scène sur les flammes. 
   */
  Texture *m_sceneDepthTex;

  /** Indice de la texture de profondeur courante dans le tableau m_depthTex. */
  GLuint m_curDepthTex;
  
  /** Pointeur sur la scène. */
  Scene *m_scene;

  /** Tableau contenant les flammes. */
  FireSource **m_flames;
  /** Nombre de flammes dans le tabeau. */
  uint m_nbFlames;
  /** Fragment program utilisé pour l'extraction de la profondeur d'un calque. */
  ARBFragmentShader m_peelProgram;
  
  /** Identifiant de la display list contenant les flammes. */
  GLuint m_flamesDisplayList;
};

#endif

#ifndef WXGLBUFFER_H
#define WXGLBUFFER_H

class GLFlameCanvas;

#include "../common.hpp"
#include "interface.hpp"

#include <wx/glcanvas.h>

#include "../shaders/CgSVShader.hpp"

#include "../scene/camera.hpp"
#include "../scene/scene.hpp"
#include "../scene/graphicsFn.hpp"

#include "../flames/firesources.hpp"
#include "../flames/glowengine.hpp"
#include "../flames/solidePhoto.hpp"

#include "../solvers/GSsolver.hpp"
#include "../solvers/GCSSORsolver.hpp"
#include "../solvers/HybridSolver.hpp"
#include "../solvers/logResSolver.hpp"
#include "../solvers/logResAvgSolver.hpp"
#include "../solvers/logResAvgTimeSolver.hpp"

class GLFlameCanvas : public wxGLCanvas
{
public:
  GLFlameCanvas(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int* attribList = 0,  
	     long style=0, const wxString& name=_("GLCanvas"), const wxPalette& palette = wxNullPalette);
  
  ~GLFlameCanvas();
  
  void OnIdle(wxIdleEvent& event);
  void OnPaint(wxPaintEvent& event);
  
  /** D�fini l'action � effectuer lorsque la souris se d�place */
  void OnMouseMotion(wxMouseEvent& event);
  /** D�fini l'action � effectuer lorsqu'un bouton de la souris est enfonc� */
  void OnMouseClick(wxMouseEvent& event);
  void OnMouseWheel(wxMouseEvent& event);
  void OnKeyPressed(wxKeyEvent& event);

  /** Initialisations relatives � l'environnement OpenGL */
  void InitGL(bool recompileShaders);
  /** Initialisations relatives aux flammes */
  void InitFlames(void);
  /** Initialisations relatives aux solveurs */
  void InitSolvers(void);
  /** Initialisations relatives � la sc�ne */
  void InitScene(bool recompileShaders);
  /** Initialisations relatives aux param�tres de visualisation */
  void InitUISettings(void);
  void Restart (void);
  void DestroyScene(void);
  /** Initialisation globale du contr�le */
  void Init(FlameAppConfig *config, bool recompileShaders);

  bool IsRunning(void) { return m_run; };
  /** Lance/arr�te l'animation */
  void ToggleRun(void) { m_run=!m_run; };
  /** Active/D�sactive le glow */
  void ToggleGridDisplay(void) { m_displayGrid=!m_displayGrid; };
  void ToggleBaseDisplay(void) { m_displayBase=!m_displayBase; };
  void ToggleVelocityDisplay(void) { m_displayVelocity=!m_displayVelocity; };
  void ToggleParticlesDisplay(void) { m_displayParticles=!m_displayParticles; };
  void ToggleWickBoxesDisplay(void) { m_displayWickBoxes=!m_displayWickBoxes; };
  void ToggleFlamesDisplay(void) { m_displayFlame=!m_displayFlame; };
  void ToggleShadowVolumesDisplay(void) { m_drawShadowVolumes=!m_drawShadowVolumes; };
  void ToggleSmoothShading(void) { 
    for (int f = 0; f < m_currentConfig->nbFlames; f++)
    m_flames[f]->toggleSmoothShading ();
  };
  void ToggleSaveImages(void) { m_saveImages = !m_saveImages; };
  void Swap(void) { m_photoSolid->swap(); };
  void moveSolver(int selectedSolver, Point& pt){ m_solvers[selectedSolver]->moveTo(pt); };
  void setBuoyancy(int index, double value){ m_solvers[index]->setBuoyancy(value); };
  void setFlameForces(int index, double value){ m_flames[index]->setForces(value); };
  
  void UpdateShadowsFatness(void){ m_SVShader->setFatness(m_currentConfig->fatness); };
  void UpdateShadowsExtrudeDist(void){ m_SVShader->setShadowExtrudeDist(m_currentConfig->extrudeDist); };
private:
  void WriteFPS ();
  void DrawVelocity (void);
  
  void cast_shadows_double_multiple();
  void cast_shadows_double();
  
  /** Configuration de l'application */
  FlameAppConfig *m_currentConfig;
  /********* Variables relatives au contr�le de l'affichage **************/
  /* true si la simulation est en cours, 0 sinon */
  bool m_run, m_saveImages;
  bool m_displayVelocity, m_displayBase, m_displayGrid, m_displayFlame, m_displayParticles, m_displayWickBoxes;
  bool m_drawShadowVolumes, m_glowOnly;
  /** true si l'application est correctement initialis�e, 0 sinon */
  bool m_init;

  /********* Variables relatives � la fen�tre d'affichage ****************/
  int m_width, m_height;
  int prevNbSolvers, prevNbFlames;
  
  CGcontext m_context;
  Camera *m_camera;
  /* Pour le compte des frames */
  int m_framesCount, m_globalFramesCount;
  int m_t;
  
  /* Tableau de pixels pour la sauvegarde des images */
  unsigned char *m_pixels;

  /********* Variables relatives aux solides photom�triques **************/
  SolidePhotometrique *m_photoSolid;

  /********* Variables relatives au glow *********************************/
  GlowEngine *m_glowEngine;
  GlowEngine *m_glowEngine2;

  /********* Variables relatives au solveur ******************************/
  Solver **m_solvers;
    
  /********* Variables relatives � la simulation *************************/
  FireSource **m_flames;
  Scene *m_scene;
  CgSVShader *m_SVShader;
  
  double *intensities;

  const static int m_nbIterFlickering = 20;
  DECLARE_EVENT_TABLE()
};

#endif
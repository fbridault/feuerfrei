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
#include "../flames/DPengine.hpp"
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
  void drawScene(void);
  
  /** Défini l'action à effectuer lorsque la souris se déplace */
  void OnMouseMotion(wxMouseEvent& event);
  /** Défini l'action à effectuer lorsqu'un bouton de la souris est enfoncé */
  void OnMouseClick(wxMouseEvent& event);
  void OnMouseWheel(wxMouseEvent& event);
  void OnKeyPressed(wxKeyEvent& event);
  void OnSize(wxSizeEvent& event);
  
  /** Initialisations relatives à l'environnement OpenGL */
  void InitGL(bool recompileShaders);
  /** Initialisations relatives aux flammes */
  void InitFlames(void);
  /** Initialisations relatives aux solveurs */
  void InitSolvers(void);
  /** Initialisations relatives à la scène */
  void InitScene(bool recompileShaders);
  /** Initialisations relatives aux paramètres de visualisation */
  void InitUISettings(void);
  void Restart (void);
  void DestroyScene(void);
  /** Initialisation globale du contrôle */
  void Init(FlameAppConfig *config, bool recompileShaders);
  
  bool IsRunning(void) { return m_run; };
  /** Lance/arrête l'animation */
  void ToggleRun(void) { m_run=!m_run; };
  
  /** Active/Désactive le glow seul */
  void ToggleGlowOnlyDisplay(void) { m_glowOnly=!m_glowOnly; };
  void ToggleGridDisplay(void) { m_displayGrid=!m_displayGrid; };
  void ToggleBaseDisplay(void) { m_displayBase=!m_displayBase; };
  void ToggleVelocityDisplay(void) { m_displayVelocity=!m_displayVelocity; };
  void ToggleParticlesDisplay(void) { m_displayParticles=!m_displayParticles; };
  void ToggleWickBoxesDisplay(void) { m_displayWickBoxes=!m_displayWickBoxes; };
  void ToggleFlamesDisplay(void) { m_displayFlame=!m_displayFlame; };
  void ToggleShadowVolumesDisplay(void) { m_drawShadowVolumes=!m_drawShadowVolumes; };
  void ToggleSmoothShading(void) { 
    for (uint f = 0; f < m_currentConfig->nbFlames; f++)
    m_flames[f]->toggleSmoothShading ();
  };  
//   void ToggleDepthPeeling(void) { 
//     if(m_currentConfig->depthPeelingEnabled)
//       for (uint f = 0; f < m_currentConfig->nbFlames; f++)
// 	m_flames[f]-> setTesselateMode();
//     else
//       for (uint f = 0; f < m_currentConfig->nbFlames; f++)
// 	m_flames[f]-> setRenderMode();
//   };
  void ToggleSaveImages(void) { m_saveImages = !m_saveImages; };
  void moveSolver(int selectedSolver, Point& pt, bool move){ m_solvers[selectedSolver]->addExternalForces(pt,move); };
  void addPermanentExternalForcesToSolver(int selectedSolver, Point &pt){ m_solvers[selectedSolver]->addPermanentExternalForces(pt); };
  void setBuoyancy(int index, double value){ m_solvers[index]->setBuoyancy(value); };
  void setFlameForces(int index, double value){ m_flames[index]->setForces(value); };
  void setFlameSamplingTolerance(int index, double value){ m_flames[index]->setSamplingTolerance(value); };
  void setNbDepthPeelingLayers(uint value){ m_depthPeelingEngine->setNbLayers(value); };
  void UpdateShadowsFatness(void){ m_SVShader->setFatness(m_currentConfig->fatness); };
  void UpdateShadowsExtrudeDist(void){ m_SVShader->setShadowExtrudeDist(m_currentConfig->extrudeDist); };
  void RegeneratePhotometricSolids(uint flameIndex, wxString IESFileName);
  
private:
  void WriteFPS ();
  void DrawVelocity (void);
  
  void cast_shadows_double_multiple();
  void cast_shadows_double();
  
  /** Configuration de l'application */
  FlameAppConfig *m_currentConfig;
  /********* Variables relatives au contrôle de l'affichage **************/
  /* true si la simulation est en cours, 0 sinon */
  bool m_run, m_saveImages;
  bool m_displayVelocity, m_displayBase, m_displayGrid, m_displayFlame, m_displayParticles, m_displayWickBoxes;
  bool m_drawShadowVolumes, m_glowOnly;
  /** true si l'application est correctement initialisée, 0 sinon */
  bool m_init;

  /********* Variables relatives à la fenêtre d'affichage ****************/
  uint m_width, m_height;
  uint prevNbSolvers, prevNbFlames;
  
  CGcontext m_context;
  Camera *m_camera;
  /* Pour le compte des frames */
  uint m_framesCount, m_globalFramesCount;

  uint m_framesCountForSwitch;
  bool m_switch;
  int m_t;
  
  /* Tableau de pixels pour la sauvegarde des images */
  u_char *m_pixels;

  /********* Variables relatives aux solides photométriques **************/
  PhotometricSolidsRenderer *m_photoSolid;

  /********* Variables relatives au glow *********************************/
  GlowEngine *m_glowEngine;
  
  DepthPeelingEngine *m_depthPeelingEngine;
  /********* Variables relatives au solveur ******************************/
  Solver **m_solvers;
    
  /********* Variables relatives à la simulation *************************/
  FireSource **m_flames;
  Scene *m_scene;
  CgSVShader *m_SVShader;
  
  double *intensities;

  const static int m_nbIterFlickering = 20;
  DECLARE_EVENT_TABLE()
};

#endif

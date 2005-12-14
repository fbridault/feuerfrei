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

#include "../flames/bougie.hpp"
#include "../flames/firmalampe.hpp"
#include "../flames/glowengine.hpp"
#include "../flames/solidePhoto.hpp"

#include "../solvers/GSsolver.hpp"
#include "../solvers/GCSSORsolver.hpp"
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
  /** Lance/arr�te l'animation */
  void ToggleFlickering(void) { m_flickering=!m_flickering; };
  /** Active/D�sactive le solide photom�trique */
  void ToggleSP(void) { m_currentConfig->PSEnabled=!m_currentConfig->PSEnabled; };
  /** Active/D�sactive le glow */
  void ToggleGlow(void) { m_currentConfig->glowEnabled=!m_currentConfig->glowEnabled; };
  void ToggleGridDisplay(void) { m_displayGrid=!m_displayGrid; };
  void ToggleBaseDisplay(void) { m_displayBase=!m_displayBase; };
  void ToggleVelocityDisplay(void) { m_displayVelocity=!m_displayVelocity; };
  void ToggleParticlesDisplay(void) { m_displayParticles=!m_displayParticles; };
  void ToggleFlamesDisplay(void) { m_displayFlame=!m_displayFlame; };
  void ToggleSmoothShading(void) { 
    for (int f = 0; f < m_currentConfig->nbFlames; f++)
    m_flames[f]->toggleSmoothShading (); };
  void ToggleBlendedSP(void) { m_currentConfig->BPSEnabled = 2-m_currentConfig->BPSEnabled; };
  void ToggleInterpolationSP(void) { m_currentConfig->IPSEnabled = 1-m_currentConfig->IPSEnabled; };

  void Swap(void) { m_photoSolid->swap(); };
  void moveSolver(int selectedSolver, CPoint& pt){ m_solvers[selectedSolver]->moveTo(pt); };
  void setFlameForces(int index, double valField, double valInner){ m_flames[index]->setForces(valField, valInner); };

private:
  void WriteFPS ();
  void DrawVelocity (void);
  
  /** Configuration de l'application */
  FlameAppConfig *m_currentConfig;
  /********* Variables relatives au contr�le de l'affichage **************/
  /* true si la simulation est en cours, 0 sinon */
  bool m_run;
  bool m_displayVelocity, m_displayBase, m_displayGrid, m_displayFlame, m_displayParticles;
  bool m_flickering, m_shadowsEnabled, m_shadowVolumesEnabled, m_glowOnly;
  /** true si l'application est correctement initialis�e, 0 sinon */
  bool m_init;

  /********* Variables relatives � la fen�tre d'affichage ****************/
  int m_width, m_height;
  int prevNbSolvers, prevNbFlames;
  
  CGcontext m_context;
  Camera *m_camera;
  /* Pour le compte des frames */
  int m_framesCount;
  int m_t;
  
  /********* Variables relatives aux solides photom�triques **************/
  SolidePhotometrique *m_photoSolid;

  /********* Variables relatives au glow *********************************/
  GlowEngine *m_glowEngine;
  GlowEngine *m_glowEngine2;

  /********* Variables relatives au solveur ******************************/
  Solver **m_solvers;
    
  /********* Variables relatives � la simulation *************************/
  Flame **m_flames;
  CScene *m_scene;
  CgSVShader *m_SVShader;

  DECLARE_EVENT_TABLE()
};

#endif

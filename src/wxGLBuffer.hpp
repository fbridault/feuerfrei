#ifndef WXGLBUFFER_H
#define WXGLBUFFER_H

class wxGLBuffer;

#include "header.h"

#include <wx/glcanvas.h>

#include "CgSVShader.hpp"
#include "eyeball.hpp"

#include "scene.hpp"
#include "solidePhoto.hpp"
#include "graphicsFn.hpp"
#include "bougie.hpp"
#include "firmalampe.hpp"
#include "solver.hpp"
#include "benchsolver.hpp"
#include "glowengine.hpp"

class wxGLBuffer : public wxGLCanvas
{
public:
  wxGLBuffer() {}
  wxGLBuffer(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int* attribList = 0,  
	     long style=0, const wxString& name=_("GLCanvas"), const wxPalette& palette = wxNullPalette);
  virtual ~wxGLBuffer();
  
  void OnIdle(wxIdleEvent& event);
  void OnPaint(wxPaintEvent& event);
  
  /** D�fini l'action � effectuer lorsque la souris se d�place */
  void OnMouseMotion(wxMouseEvent& event);
  /** D�fini l'action � effectuer lorsqu'un bouton de la souris est enfonc� */
  void OnMouseClick(wxMouseEvent& event);
    
  void InitGL(void);
  void InitScene(void);
  void InitUISettings(void);
  void Restart (void);
  void DestroyScene(void);
  /** Initialisation globale du contr�le */
  void Init(flameAppConfig *config);
  /** Initialisation des variables d'affichage de l'interface */
  

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
    for (int f = 0; f < m_nbFlames; f++)
    m_flames[f]->toggleSmoothShading (); };
  void ToggleBlendedSP(void) { m_currentConfig->BPSEnabled = 2-m_currentConfig->BPSEnabled; };
  void ToggleInterpolationSP(void) { m_currentConfig->IPSEnabled = 1-m_currentConfig->IPSEnabled; };

  void Swap(void) { m_photoSolid->swap(); };

private:
  void WriteFPS ();
  void DrawVelocity (void);
  
  /** Configuration de l'application */
  flameAppConfig *m_currentConfig;
  /********* Variables relatives au contr�le de l'affichage **************/
  /* true si la simulation est en cours, 0 sinon */
  bool m_run;
  bool m_displayVelocity, m_displayBase, m_displayGrid,
    m_displayFlame, m_displayParticles;
  bool m_flickering, m_shadowsEnabled, m_shadowVolumesEnabled, m_glowOnly;
  /** true si l'application est correctement initialis�e, 0 sinon */
  bool m_init;

  /********* Variables relatives � la fen�tre d'affichage ****************/
  int m_width, m_height;
  
  CGcontext m_context;
  Eyeball *m_eyeball;
  /* Pour le compte des frames */
  int m_framesCount;
  int m_t;
  
  /********* Variables relatives aux solides photom�triques **************/
  SolidePhotometrique *m_photoSolid;

  /********* Variables relatives au glow *********************************/
  GlowEngine *m_glowEngine;
  GlowEngine *m_glowEngine2;

  /********* Variables relatives au solveur ******************************/
  Solver *m_solver;
    
  /********* Variables relatives � la simulation *************************/
  Flame **m_flames;
  int m_nbFlames;
  CScene *m_scene;
  CgSVShader *m_SVShader;

  DECLARE_EVENT_TABLE()
};

#endif

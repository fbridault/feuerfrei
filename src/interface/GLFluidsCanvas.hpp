#ifndef GLFLUIDSCANVAS_H
#define GLFLUIDSCANVAS_H

class GLFluidsCanvas;

#include "../common.hpp"
#include "interface.hpp"

#include <wx/glcanvas.h>

#include <engine/Scene/CCamera.hpp>
#include <engine/Utility/GraphicsFn.hpp>

#include "../solvers/field.hpp"

class GLFluidsCanvas : public wxGLCanvas
{
public:
  GLFluidsCanvas(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int* attribList = 0,
		long style=0, const wxString& name=_("GLCanvas"), const wxPalette& palette = wxNullPalette);

  ~GLFluidsCanvas();

  void OnIdle(wxIdleEvent& event);
  void OnPaint(wxPaintEvent& event);

  /** Défini l'action à effectuer lorsque la souris se déplace */
  void OnMouseMotion(wxMouseEvent& event);
  /** Défini l'action à effectuer lorsqu'un bouton de la souris est enfoncé */
  void OnMouseClick(wxMouseEvent& event);
  void OnMouseWheel(wxMouseEvent& event);
  void OnKeyPressed(wxKeyEvent& event);
  void OnSize(wxSizeEvent& event);

  /** Initialisations relatives à l'environnement OpenGL */
  void InitGL();
  /** Initialisations relatives aux solveurs */
  void InitSolvers(void);
  /** Initialisations relatives à la scène */
  void InitScene();
  /** Initialisations relatives aux paramètres de visualisation */
  void InitUISettings(void);
  void Restart (void);
  void DestroyScene(void);
  /** Initialisation globale du contrôle */
  void Init(FluidsAppConfig *config);

  bool IsRunning(void) { return m_run; };
  /** Lance/arrête l'animation */
  void setRunningState(bool run) { m_run=run; };

  void ToggleGridDisplay(void) { m_displayGrid=!m_displayGrid; };
  void ToggleBaseDisplay(void) { m_displayBase=!m_displayBase; };
  void ToggleVelocityDisplay(void) { m_displayVelocity=!m_displayVelocity; };
  void ToggleDensityDisplay(void) { m_displayDensity=!m_displayDensity; };
  void ToggleSaveImages(void) { m_saveImages = !m_saveImages; };
  void moveSolver(int selectedSolver, CPoint& pt, bool move){ m_solvers[selectedSolver]->addTemporaryExternalForces(pt); };
  void addPermanentExternalForcesToField(int selectedSolver, CPoint &pt){ m_solvers[selectedSolver]->addPermanentExternalForces(pt); };
  void setBuoyancy(int index, float value){ m_solvers[index]->setBuoyancy(value); };
  void setVorticity(int index, float value){ m_solvers[index]->setVorticity(value); };
  void addDensityInSolver(int index, int id){ m_solvers[index]->addDensity(id); };

private:
  void WriteFPS ();
  void DrawVelocity (void);

  /** Configuration de l'application */
  FluidsAppConfig *m_currentConfig;
  /********* Variables relatives au contrôle de l'affichage **************/
  /* true si la simulation est en cours, 0 sinon */
  bool m_run, m_saveImages;
  bool m_displayVelocity, m_displayDensity, m_displayBase, m_displayGrid;
  /** true si l'application est correctement initialisée, 0 sinon */
  bool m_init;

  /********* Variables relatives à la fenêtre d'affichage ****************/
  uint m_width, m_height;
  uint prevNbSolvers;

  CCamera *m_camera;
  /* Pour le compte des frames */
  wxStopWatch *m_swatch;
  uint m_framesCount, m_globalFramesCount;

  uint m_framesCountForSwitch;
  bool m_switch;
  bool m_2DDisplay;
  /** Indique que l'on cherche à mesurer les performances du solveur, donc on n'affiche rien
   * pour éviter les parasites
   */
  bool m_benchTime;
  int m_t;

  /* Tableau de pixels pour la sauvegarde des images */
  u_char *m_pixels;

  /********* Variables relatives au solveur ******************************/
  Field **m_solvers;

  const static int m_nbIterFlickering = 20;
  DECLARE_EVENT_TABLE()
};

#endif

#ifndef FLUIDSFRAME_H
#define FLUIDSFRAME_H

class FluidsFrame;

#if wxUSE_IOSTREAMH
    #include <fstream.h>
#else
    #include <fstream>
#endif

#include <wx/glcanvas.h>
#include <wx/fileconf.h>
#include <wx/stream.h>
#include <wx/wfstream.h>
#include <wx/notebook.h>

#include "interface.hpp"
#include "GLFluidsCanvas.hpp"
#include "mainPanels.hpp"

enum
  {
    IDCHK_SaveImages=1,
  };

enum
  {
    IDB_Run = 1,
    IDB_Restart,
  };

enum
  {
    IDM_LoadParam = 1,
    IDM_SaveSettings,
    IDM_SaveSettingsAs,
    IDM_Quit,
    IDM_About,
    IDM_Grid,
    IDM_Base,
    IDM_Velocity,
    IDM_Density,
    IDM_SolversSettings,
  };

/** Fenêtre principale */
class FluidsFrame: public wxFrame
{
public:
  FluidsFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const wxString& configFileName=_("param.ini")); 
  
  void GetSettingsFromConfigFile (void);
  void InitGLBuffer (bool recompileShaders);
  void InitSolversPanels();
  
  void OnClose(wxCloseEvent& event);
  /** Actions des boutons */
  void OnClickButtonRun(wxCommandEvent& event);
  void OnClickButtonRestart(wxCommandEvent& event);
  void OnClickButtonFlickering(wxCommandEvent& event);
  void OnClickButtonSwap(wxCommandEvent& event);
  
  /** Actions des menus */
  void OnLoadParamMenu(wxCommandEvent& event);
  void OnOpenSceneMenu(wxCommandEvent& event);
  void OnSaveSettingsMenu(wxCommandEvent& event);
  void OnSaveSettingsAsMenu(wxCommandEvent& event);
  void OnQuitMenu(wxCommandEvent& event);
  void OnAboutMenu(wxCommandEvent& event);
  void OnGlowOnlyMenu(wxCommandEvent &event);
  void OnGridMenu(wxCommandEvent& event);
  void OnBaseMenu(wxCommandEvent& event);
  void OnVelocityMenu(wxCommandEvent& event);
  void OnDensityMenu(wxCommandEvent& event);
  void OnSolversMenu(wxCommandEvent& event);
  /** Actions des checkboxes et autres */
  void OnCheckSaveImages(wxCommandEvent& event);
  void OnSelectSolver(wxCommandEvent& event);
  void OnSize(wxSizeEvent& event);
  void SetFPS(int fps);
  
private:
  wxString m_configFileName;
  /** Pointeur sur le fichier de configuration */
  wxFileConfig *m_config;
  /** Zone d'affichage OpenGL */
  GLFluidsCanvas *m_glBuffer;
  /** Boutons */
  wxButton *m_buttonRun, *m_buttonRestart;
  /** Menus */
  wxMenu *m_menuFile, *m_menuDisplay, *m_menuDisplayFlames, *m_menuSettings;
  /** Barre de menu */
  wxMenuBar *m_menuBar;
  
  wxCheckBox *m_saveImagesCheckBox;
  
  wxStaticBoxSizer *m_globalSizer,*m_solversSizer;  
  wxBoxSizer *m_mainSizer, *m_rightSizer, *m_globalTopSizer;

  SolverMainPanel* m_solverPanels[NB_MAXSOLVERS];
  
  wxNotebook *m_solversNotebook;
  
  FluidsAppConfig m_currentConfig;
  
  /** Nombre maximum de flammes utilisées durant la session, variable utilisée */
  /* Pour savoir combien de groupes /Flame# supprimer dans le fichier de configuration */
  uint m_nbSolversMax;
  
  DECLARE_EVENT_TABLE()
};

#endif

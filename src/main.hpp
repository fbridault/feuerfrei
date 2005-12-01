#ifndef MAIN_H
#define MAIN_H

#if wxUSE_IOSTREAMH
    #include <fstream.h>
#else
    #include <fstream>
#endif

#include <wx/glcanvas.h>
#include <wx/fileconf.h>
#include <wx/stream.h>
#include <wx/wfstream.h>

#include "header.h"
#include "wxGLBuffer.hpp"
#include "mainPanels.hpp"


enum
  {
    IDCHK_IS = 1,
    IDCHK_BS,
    IDCHK_ES,
    IDCHK_Glow,
  };

enum
  {
    IDB_Run = 1,
    IDB_Restart,
    IDB_Swap,
  };

enum
  {
    IDM_OpenScene = 1,
    IDM_SaveSettings,
    IDM_Quit,
    IDM_About,
    IDM_Grid,
    IDM_Base,
    IDM_Velocity,
    IDM_Particles,
    IDM_Flames,
    IDM_Hide,
    IDM_Wired,
    IDM_Shaded,
    IDM_SolversSettings,
    IDM_FlamesSettings,
  };

/** Fenêtre principale */
class MainFrame: public wxFrame
{
public:
  MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size); 
  
  void GetSettingsFromConfigFile (void);
  void InitGLBuffer (bool recompileShaders);
  
  void OnClose(wxCloseEvent& event);
  /** Actions des boutons */
  void OnClickButtonRun(wxCommandEvent& event);
  void OnClickButtonRestart(wxCommandEvent& event);
  void OnClickButtonFlickering(wxCommandEvent& event);
  void OnClickButtonSwap(wxCommandEvent& event);
  
  /** Actions des menus */
  void OnOpenSceneMenu(wxCommandEvent& event);
  void OnSaveSettingsMenu(wxCommandEvent& event);
  void OnQuitMenu(wxCommandEvent& event);
  void OnAboutMenu(wxCommandEvent& event);
  void OnGridMenu(wxCommandEvent& event);
  void OnBaseMenu(wxCommandEvent& event);
  void OnVelocityMenu(wxCommandEvent& event);
  void OnParticlesMenu(wxCommandEvent& event);
  void OnHideMenu(wxCommandEvent& event);
  void OnWiredMenu(wxCommandEvent& event);
  void OnShadedMenu(wxCommandEvent& event);
  void OnSolversMenu(wxCommandEvent& event);
  void OnFlamesMenu(wxCommandEvent& event);
  void OnCheckBS(wxCommandEvent& event);
  void OnCheckIS(wxCommandEvent& event);
  void OnCheckGlow(wxCommandEvent& event);
  void OnCheckES(wxCommandEvent& event);
  void OnSelectSolver(wxCommandEvent& event);
  void SetFPS(int fps);

private:
  /** Pointeur sur le fichier de configuration */
  wxFileConfig *m_config;
  /** Zone d'affichage OpenGL */
  wxGLBuffer *m_glBuffer;
  /** Boutons */
  wxButton *m_buttonRun, *m_buttonRestart, *m_buttonSwap;
  /** Menus */
  wxMenu *m_menuFile, *m_menuDisplay, *m_menuDisplayFlames, *m_menuSettings;
  /** Barre de menu */
  wxMenuBar *m_menuBar;
  
  wxCheckBox *m_interpolatedSolidCheckBox, *m_blendedSolidCheckBox;
  wxCheckBox *m_enableSolidCheckBox, *m_glowEnabledCheckBox;
    
  wxStaticBoxSizer *m_globalSizer,*m_solidSizer,*m_glowSizer,*m_solversSizer;  
  wxBoxSizer *m_topSizer, *m_mainSizer, *m_rightSizer;
  
  SolverMainPanel* m_solverPanels[NB_MAXSOLVERS];
  wxNotebook* m_solversNotebook;
  
  FlameAppConfig m_currentConfig;
  
  /** Nombre maximum de flammes utilisées durant la session, variable utilisée */
  /* Pour savoir combien de groupes /Flame# supprimer dans le fichier de configuration */
  int m_nbSolversMax;
  /** Nombre maximum de flammes utilisées durant la session, variable utilisée */
  /* Pour savoir combien de groupes /Flame# supprimer dans le fichier de configuration */
  int m_nbFlamesMax;
  
  DECLARE_EVENT_TABLE()
};

#endif

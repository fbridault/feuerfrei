#ifndef MAIN_H
#define MAIN_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

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
    IDB_Flickering,
    IDB_Swap,
  };

enum
  {
    IDM_SaveSettings = 1,
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
  };

/** Fenêtre principale */
class MainFrame: public wxFrame
{
public:
  MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size); 
  
  void OnClose(wxCloseEvent& event);
  /** Actions des boutons */
  void OnClickButtonRun(wxCommandEvent& event);
  void OnClickButtonRestart(wxCommandEvent& event);
  void OnClickButtonFlickering(wxCommandEvent& event);
  void OnClickButtonSwap(wxCommandEvent& event);
  
  /** Actions des menus */
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
  void OnCheckBS(wxCommandEvent& event);
  void OnCheckIS(wxCommandEvent& event);
  void OnCheckGlow(wxCommandEvent& event);
  void OnCheckES(wxCommandEvent& event);
  
  void SetFPS(int fps);
  void GetSettingsFromConfigFile ();
  
private:
  /** Pointeur sur le fichier de configuration */
  wxFileConfig *m_config;
  /** Zone d'affichage OpenGL */
  wxGLBuffer *m_glBuffer;
  /** Boutons */
  wxButton *m_buttonRun, *m_buttonRestart, *m_buttonFlickering, *m_buttonSwap;
  /** Menus */
  wxMenu *m_menuFile, *m_menuDisplay, *m_menuDisplayFlames;
  /** Barre de menu */
  wxMenuBar *m_menuBar;
  
  wxCheckBox *m_interpolatedSolidCheckBox, *m_blendedSolidCheckBox;
  wxCheckBox *m_enableSolidCheckBox, *m_glowEnabledCheckBox;
  
  wxStaticBoxSizer *m_globalSizer,*m_solidSizer,*m_glowSizer;
  wxBoxSizer *m_mainSizer, *m_rightSizer;
  flameAppConfig m_currentConfig;
  
  DECLARE_EVENT_TABLE()
};

#endif

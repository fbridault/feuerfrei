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
#include <wx/spinctrl.h>

#include "header.h"
#include "wxGLBuffer.hpp"


enum
  {
    IDSL_FXAP = 1,
    IDSL_FYAP,
    IDSL_FZAP,
  };

enum
  {
    IDT_FXAPMIN = 1,  
    IDT_FXAPMAX,    
    IDT_FYAPMIN,
    IDT_FYAPMAX,
    IDT_FZAPMIN,
    IDT_FZAPMAX,
  };

enum
  {
    IDST_FXAP = 1,
    IDST_FYAP,
    IDST_FZAP,
    IDT_SELECT,
  };

enum
  {
    IDCB_SELECT = 1,
  };

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
    IDM_Settings,
  };

/** Fenêtre principale */
class MainFrame: public wxFrame
{
public:
  MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size); 
  
  void GetSettingsFromConfigFile (bool recompileShaders);
  
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
  void OnCheckBS(wxCommandEvent& event);
  void OnCheckIS(wxCommandEvent& event);
  void OnCheckGlow(wxCommandEvent& event);
  void OnCheckES(wxCommandEvent& event);
  void OnSelectFlame(wxCommandEvent& event);
  void OnScrollPosition(wxScrollEvent& event);
  void OnFXAPMINEnter(wxCommandEvent& event);
  void OnFXAPMAXEnter(wxCommandEvent& event);
  void OnFYAPMINEnter(wxCommandEvent& event);
  void OnFYAPMAXEnter(wxCommandEvent& event);
  void OnFZAPMINEnter(wxCommandEvent& event);
  void OnFZAPMAXEnter(wxCommandEvent& event);
  void ComputeSlidersValues(void);
  void SetFPS(int fps);

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
  
  wxSlider *m_flameXAxisPositionSlider, *m_flameYAxisPositionSlider, *m_flameZAxisPositionSlider;
  wxTextCtrl *m_flameXAxisPositionSliderMax, *m_flameYAxisPositionSliderMax, *m_flameZAxisPositionSliderMax,
    *m_flameXAxisPositionSliderMin, *m_flameYAxisPositionSliderMin, *m_flameZAxisPositionSliderMin;
  wxStaticText *m_flameXAxisPositionLabel, *m_flameYAxisPositionLabel, *m_flameZAxisPositionLabel;
  wxStaticText *m_selectFlameLabel;
  
  wxComboBox *m_selectFlameComboBox;
  
  wxStaticBoxSizer *m_globalSizer,*m_solidSizer,*m_glowSizer, *m_flamesSizer;
  wxBoxSizer *m_topSizer, *m_mainSizer, *m_rightSizer;
  wxBoxSizer *m_flameSelectSizer;
  wxBoxSizer *m_flamesXAxisPositionSizer, *m_flamesYAxisPositionSizer, *m_flamesZAxisPositionSizer;
  wxBoxSizer *m_flamesXAxisPositionRangeSizer, *m_flamesYAxisPositionRangeSizer, *m_flamesZAxisPositionRangeSizer;
  
  FlameAppConfig m_currentConfig;
  
  /** Nombre maximum de flammes utilisées durant la session, variable utilisée */
  /* Pour savoir combien de groupes /Flame# supprimer dans le fichier de configuration */
  int m_nbFlamesMax;
  int m_selectedFlame;
  double SLIDER_SENSIBILITY;
  int SLIDER_RANGE;
  DECLARE_EVENT_TABLE()
};

#endif

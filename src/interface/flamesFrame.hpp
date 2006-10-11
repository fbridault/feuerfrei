#ifndef FLAMESFRAME_H
#define FLAMESFRAME_H

class FlamesFrame;

#if wxUSE_IOSTREAMH
    #include <fstream.h>
#else
    #include <fstream>
#endif

#include <wx/glcanvas.h>
#include <wx/fileconf.h>
#include <wx/stream.h>
#include <wx/wfstream.h>

#include "interface.hpp"
#include "GLFlameCanvas.hpp"
#include "mainPanels.hpp"

enum
  {
    IDCHK_BS = 1,
    IDCHK_Shadows,
    IDCHK_Glow,
    IDCHK_DP,
    IDCHK_SaveImages,
  };

enum
  {
    IDSL_DP = 1,
  };

enum
  {
    IDB_Run = 1,
    IDB_Restart,
    IDRB_Lighting,
  };

enum
  {
    IDM_LoadParam = 1,
    IDM_OpenScene,
    IDM_SaveSettings,
    IDM_SaveSettingsAs,
    IDM_Quit,
    IDM_About,
    IDM_GlowOnly,
    IDM_Grid,
    IDM_Base,
    IDM_Velocity,
    IDM_Particles,
    IDM_WickBoxes,
    IDM_Flames,
    IDM_ShadowVolumes,
    IDM_Hide,
    IDM_Wired,
    IDM_Shaded,
    IDM_SolversSettings,
    IDM_FlamesSettings,
    IDM_ShadowVolumesSettings,
  };

/** Fenêtre principale */
class FlamesFrame: public wxFrame
{
public:
  FlamesFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const wxString& configFileName=_("param.ini")); 
  
  void GetSettingsFromConfigFile (void);
  void InitGLBuffer (bool recompileShaders);
  void InitSolversPanels();
  void InitFlamesPanels();
  
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
  void OnParticlesMenu(wxCommandEvent& event);
  void OnWickBoxesMenu(wxCommandEvent& event);
  void OnHideMenu(wxCommandEvent& event);
  void OnWiredMenu(wxCommandEvent& event);
  void OnShadedMenu(wxCommandEvent& event);
  void OnSolversMenu(wxCommandEvent& event);
  void OnFlamesMenu(wxCommandEvent& event);
  void OnShadowsMenu(wxCommandEvent& event);
  void OnShadowVolumesMenu(wxCommandEvent& event);
  void OnShadowVolumesSettingsMenu(wxCommandEvent& event);
  /** Actions des checkboxes et autres */
  void OnCheckBS(wxCommandEvent& event);
  void OnCheckShadows(wxCommandEvent& event);
  void OnCheckGlow(wxCommandEvent& event);
  void OnCheckDepthPeeling(wxCommandEvent& event);
  void OnCheckSaveImages(wxCommandEvent& event);
  void OnSelectLighting(wxCommandEvent& event);
  void OnSelectSolver(wxCommandEvent& event);
  void OnScrollPosition(wxScrollEvent& event);
  void OnSize(wxSizeEvent& event);
  void SetFPS(int fps);

private:
  wxString m_configFileName;
  /** Pointeur sur le fichier de configuration */
  wxFileConfig *m_config;
  /** Zone d'affichage OpenGL */
  GLFlameCanvas *m_glBuffer;
  /** Boutons */
  wxButton *m_buttonRun, *m_buttonRestart, *m_buttonSwap;
  /** Menus */
  wxMenu *m_menuFile, *m_menuDisplay, *m_menuDisplayFlames, *m_menuSettings;
  /** Barre de menu */
  wxMenuBar *m_menuBar;
  
  wxRadioBox *m_lightingRadioBox;

  wxCheckBox *m_blendedSolidCheckBox;
  wxCheckBox *m_shadowsEnabledCheckBox, *m_glowEnabledCheckBox;
  wxCheckBox *m_saveImagesCheckBox, *m_depthPeelingEnabledCheckBox;
  
  wxStaticBoxSizer *m_lightingSizer, *m_globalSizer,*m_multiSizer,*m_solversSizer, *m_flamesSizer;  
  wxBoxSizer *m_leftSizer, *m_bottomSizer, *m_mainSizer, *m_rightSizer, *m_lightingBottomSizer, *m_multiTopSizer, *m_globalTopSizer;
  wxSlider *m_depthPeelingSlider;

  SolverMainPanel* m_solverPanels[NB_MAXSOLVERS];
  FlameMainPanel* m_flamePanels[NB_MAXFLAMMES];
  
  wxNotebook *m_solversNotebook, *m_flamesNotebook;
  
  FlameAppConfig m_currentConfig;
  
  /** Nombre maximum de flammes utilisées durant la session, variable utilisée */
  /* Pour savoir combien de groupes /Flame# supprimer dans le fichier de configuration */
  uint m_nbSolversMax;
  /** Nombre maximum de flammes utilisées durant la session, variable utilisée */
  /* Pour savoir combien de groupes /Flame# supprimer dans le fichier de configuration */
  uint m_nbFlamesMax;
  
  DECLARE_EVENT_TABLE()
};

#endif

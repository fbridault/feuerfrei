#ifndef MAINPANELS_H
#define MAINPANELS_H

#include "interface.hpp"
#include "../common.hpp"

#include <wx/spinctrl.h>
#ifdef RTFLAMES_BUILD
  class GLFlameCanvas;
#else
  class GLFluidsCanvas;
#endif

enum
  {
    IDSL_FXAP = 1,
    IDSL_FYAP,
    IDSL_FZAP,
    IDSL_SF,
    IDSL_FF,
    IDSL_IC,
    IDSL_NLP,
    IDSL_NPP,
    IDSL_SPTOL,
    IDRB_FLICK,
    IDRB_FDF,
    IDB_BROWSE,
    IDCHK_MOVE,
  };

enum
  {
    IDST_FXAP = 1,
    IDST_FYAP,
    IDST_FZAP,
  };

enum
  {
    IDT_FXAPMIN = 1,  
    IDT_FXAPMAX,    
    IDT_FYAPMIN,
    IDT_FYAPMAX,
    IDT_FZAPMIN,
    IDT_FZAPMAX,
    IDT_PHOTO
  };

#ifdef RTFLUIDS_BUILD
enum
  {
    IDB_LEFT = 1,
    IDB_RIGHT,
    IDB_TOP,
    IDB_BOTTOM,
  };
#endif

/** Panneau pour les onglets des solveurs dans la fenêtre principale */
class SolverMainPanel: public wxPanel 
{
public:
#ifdef RTFLAMES_BUILD
  SolverMainPanel(wxWindow* parent, int id, SolverConfig* const solverConfig, int index, GLFlameCanvas* const glBuffer, 
		  char type, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
#else
  SolverMainPanel(wxWindow* parent, int id, SolverConfig* const solverConfig, int index, GLFluidsCanvas* const glBuffer, 
		  const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
#endif
  virtual ~SolverMainPanel(){};
private:
  void OnScrollPosition(wxScrollEvent& event);
  void OnFXAPMINEnter(wxCommandEvent& event);
  void OnFXAPMAXEnter(wxCommandEvent& event);
  void OnFYAPMINEnter(wxCommandEvent& event);
  void OnFYAPMAXEnter(wxCommandEvent& event);
  void OnFZAPMINEnter(wxCommandEvent& event);
  void OnFZAPMAXEnter(wxCommandEvent& event);
  void ComputeSlidersValues(void);
  void OnCheckMove(wxCommandEvent& event);
#ifdef RTFLUIDS_BUILD
  void OnClickDensities(wxCommandEvent& event);
#endif
  
  wxSlider *m_solverXAxisPositionSlider, *m_solverYAxisPositionSlider, *m_solverZAxisPositionSlider;
  wxSlider *m_buoyancySlider;
  wxStaticText *m_buoyancyLabel;
  
  wxTextCtrl *m_solverXAxisPositionSliderMax, *m_solverYAxisPositionSliderMax, *m_solverZAxisPositionSliderMax,
    *m_solverXAxisPositionSliderMin, *m_solverYAxisPositionSliderMin, *m_solverZAxisPositionSliderMin;
  wxStaticText *m_solverXAxisPositionLabel, *m_solverYAxisPositionLabel, *m_solverZAxisPositionLabel;
  
  wxBoxSizer *m_panelSizer, *m_forcesSizer;
  wxBoxSizer *m_solversXAxisPositionSizer, *m_solversYAxisPositionSizer, *m_solversZAxisPositionSizer;
  wxBoxSizer *m_solversXAxisPositionRangeSizer, *m_solversYAxisPositionRangeSizer, *m_solversZAxisPositionRangeSizer;
  wxCheckBox *m_moveCheckBox;
  
  SolverConfig *m_solverConfig;
  /** Index du solveur */
  int m_index;
#ifdef RTFLAMES_BUILD
  GLFlameCanvas *m_glBuffer;
  /** 0 si l'on peut bouger et appliquer des forces, -1 pour uniquement les forces, 1 pour uniquement le déplacement */
  char m_type;
#else
  GLFluidsCanvas *m_glBuffer;
  wxStaticBoxSizer *m_densitiesSizer;
  wxButton *m_densityLButton, *m_densityRButton, *m_densityTButton, *m_densityBButton;
#endif
  Point m_saveSliderValues;
  
  double SLIDER_SENSIBILITY;
  double FORCE_SENSIBILITY;
  int SLIDER_RANGE;
  
  DECLARE_EVENT_TABLE()
};


#ifdef RTFLAMES_BUILD
/** Panneau pour les onglets des solveurs dans la fenêtre principale */
class FlameMainPanel: public wxPanel 
{
public:
  FlameMainPanel(wxWindow* parent, int id, FlameConfig* const flameConfig, int index, GLFlameCanvas* const glBuffer, 
		 const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
  virtual ~FlameMainPanel(){};
private:
  void OnScrollPosition(wxScrollEvent& event);
  void OnSelectType(wxCommandEvent& event);
  void OnSelectFDF(wxCommandEvent& event);
  
  void OnClickButtonBrowse(wxCommandEvent& event);
  void OnPhotoSolidEnter(wxCommandEvent& event);
  
  wxSlider *m_innerForceSlider, *m_samplingToleranceSlider, *m_leadLifeSlider, *m_periLifeSlider;
  wxStaticText *m_innerForceLabel, *m_samplingToleranceLabel, *m_leadLifeLabel, *m_periLifeLabel,
    *m_photoSolidLabel;
  
  wxButton *m_photoSolidBrowseButton;
  wxTextCtrl *m_photoSolidTextCtrl;
  
  wxBoxSizer *m_panelSizer, *m_photoSolidSizer;
  wxFlexGridSizer *m_slidersSizer;
  wxRadioBox *m_flickeringRadioBox, *m_FDFRadioBox;
  
  FlameConfig *m_flameConfig;
  /* Index du solveur */
  int m_index;
  GLFlameCanvas *m_glBuffer;
  
  double FORCE_SENSIBILITY;
  double LIGHT_SENSIBILITY;
  int SLIDER_RANGE;
  
  DECLARE_EVENT_TABLE()
};
#endif

#endif

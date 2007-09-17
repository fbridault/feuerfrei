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
    IDSL_VC,
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

#ifdef RTFLAMES_BUILD
/** Panneau pour les onglets des solveurs dans la fenêtre principale */
class LuminaryMainPanel: public wxPanel 
{
public:
  LuminaryMainPanel(wxWindow* parent, int id, LuminaryConfig* const luminaryConfig, int index, GLFlameCanvas* const glBuffer, 
		    const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);

  virtual ~LuminaryMainPanel(){};
private:
  void OnScrollPosition(wxScrollEvent& event);
  void OnFXAPMINEnter(wxCommandEvent& event);
  void OnFXAPMAXEnter(wxCommandEvent& event);
  void OnFYAPMINEnter(wxCommandEvent& event);
  void OnFYAPMAXEnter(wxCommandEvent& event);
  void OnFZAPMINEnter(wxCommandEvent& event);
  void OnFZAPMAXEnter(wxCommandEvent& event);
  void SetSlidersValues();
  
  wxSlider *m_solverXAxisPositionSlider, *m_solverYAxisPositionSlider, *m_solverZAxisPositionSlider;
  
  wxTextCtrl *m_solverXAxisPositionSliderMax, *m_solverYAxisPositionSliderMax, *m_solverZAxisPositionSliderMax,
    *m_solverXAxisPositionSliderMin, *m_solverYAxisPositionSliderMin, *m_solverZAxisPositionSliderMin;
  wxStaticText *m_solverXAxisPositionLabel, *m_solverYAxisPositionLabel, *m_solverZAxisPositionLabel;
  
  wxBoxSizer *m_panelSizer, *m_forcesSizer;
  wxBoxSizer *m_solversXAxisPositionSizer, *m_solversYAxisPositionSizer, *m_solversZAxisPositionSizer;
  wxBoxSizer *m_solversXAxisPositionRangeSizer, *m_solversYAxisPositionRangeSizer, *m_solversZAxisPositionRangeSizer;
  
  LuminaryConfig *m_luminaryConfig;
  /** Index du solveur */
  int m_index;
  GLFlameCanvas *m_glBuffer;
  
  float SLIDER_SENSIBILITY;
  float FORCE_SENSIBILITY;
  int SLIDER_RANGE;
  
  DECLARE_EVENT_TABLE()
};
#endif

/** Panneau pour les onglets des solveurs dans la fenêtre principale */
class SolverMainPanel: public wxPanel 
{
public:
#ifdef RTFLAMES_BUILD
  SolverMainPanel(wxWindow* parent, int id, float buoyancy, float vorticity, int index, GLFlameCanvas* const glBuffer, 
 		  const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
#else
  SolverMainPanel(wxWindow* parent, int id, float buoyancy, float vorticity, int index, GLFluidsCanvas* const glBuffer, 
 		  const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
#endif
  virtual ~SolverMainPanel(){};
  void getCtrlValues(SolverConfig& solverConfig);
  
private:
  void OnScrollPosition(wxScrollEvent& event);
#ifdef RTFLUIDS_BUILD
  void OnClickDensities(wxCommandEvent& event);
#endif
  
  wxStaticText *m_solverXAxisPositionLabel, *m_solverYAxisPositionLabel, *m_solverZAxisPositionLabel;
  wxSlider *m_solverXAxisPositionSlider, *m_solverYAxisPositionSlider, *m_solverZAxisPositionSlider;
  wxSlider *m_buoyancySlider;
  wxStaticText *m_buoyancyLabel;
  wxSlider *m_vorticitySlider;
  wxStaticText *m_vorticityLabel;
  
  wxBoxSizer *m_solversXAxisPositionSizer, *m_solversYAxisPositionSizer, *m_solversZAxisPositionSizer;
  wxBoxSizer *m_panelSizer, *m_forcesSizer, *m_vorticitySizer;
  
  /** Index du solveur */
  int m_index;
#ifdef RTFLAMES_BUILD
  GLFlameCanvas *m_glBuffer;
#else
  GLFluidsCanvas *m_glBuffer;
  wxStaticBoxSizer *m_densitiesSizer;
  wxButton *m_densityLButton, *m_densityRButton, *m_densityTButton, *m_densityBButton;
#endif
  Point m_saveSliderValues;
  
  float SLIDER_SENSIBILITY;
  float FORCE_SENSIBILITY;
  float BUOYANCY_SENSIBILITY;
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
  void getCtrlValues(FlameConfig& flameConfig);

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
  
  LuminaryConfig *m_luminaryConfig;
  /* Index du solveur */
  int m_index;
  GLFlameCanvas *m_glBuffer;
  
  float FORCE_SENSIBILITY;
  float LIGHT_SENSIBILITY;
  int SLIDER_RANGE;
  
  DECLARE_EVENT_TABLE()
};
#endif

#endif

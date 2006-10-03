#ifndef MAINPANELS_H
#define MAINPANELS_H

#include "interface.hpp"
#include <wx/spinctrl.h>
#include "GLFlameCanvas.hpp"

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

/** Panneau pour les onglets des solveurs dans la fenêtre principale */
class SolverMainPanel: public wxPanel 
{
public:
  SolverMainPanel(wxWindow* parent, int id, SolverConfig *solverConfig, int index, GLFlameCanvas *glBuffer, 
		  const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
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
  
  wxSlider *m_solverXAxisPositionSlider, *m_solverYAxisPositionSlider, *m_solverZAxisPositionSlider;
  wxSlider *m_buoyancySlider;
  wxStaticText *m_buoyancyLabel;
  
  wxTextCtrl *m_solverXAxisPositionSliderMax, *m_solverYAxisPositionSliderMax, *m_solverZAxisPositionSliderMax,
    *m_solverXAxisPositionSliderMin, *m_solverYAxisPositionSliderMin, *m_solverZAxisPositionSliderMin;
  wxStaticText *m_solverXAxisPositionLabel, *m_solverYAxisPositionLabel, *m_solverZAxisPositionLabel;
  
  wxBoxSizer *m_panelSizer;
  wxBoxSizer *m_solversXAxisPositionSizer, *m_solversYAxisPositionSizer, *m_solversZAxisPositionSizer;
  wxBoxSizer *m_solversXAxisPositionRangeSizer, *m_solversYAxisPositionRangeSizer, *m_solversZAxisPositionRangeSizer;
  wxBoxSizer *m_forcesSizer;
  wxCheckBox *m_moveCheckBox;
  
  SolverConfig *m_solverConfig;
  /* Index du solveur */
  int m_index;
  GLFlameCanvas *m_glBuffer;
  Point saveSliderValues;
  
  double SLIDER_SENSIBILITY;
  int SLIDER_RANGE;
  
  DECLARE_EVENT_TABLE()
};


/** Panneau pour les onglets des solveurs dans la fenêtre principale */
class FlameMainPanel: public wxPanel 
{
public:
  FlameMainPanel(wxWindow* parent, int id, FlameConfig *flameConfig, int index, GLFlameCanvas *glBuffer, 
		  const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
private:
  void OnScrollPosition(wxScrollEvent& event);
  void OnSelectType(wxCommandEvent& event);
  void OnSelectFDF(wxCommandEvent& event);

  void OnClickButtonBrowse(wxCommandEvent& event);
  void OnPhotoSolidEnter(wxCommandEvent& event);
  
  wxSlider *m_innerForceSlider, *m_samplingToleranceSlider, *m_leadLifeSlider, *m_periLifeSlider, *m_intensityCoefSlider;
  wxStaticText *m_innerForceLabel, *m_samplingToleranceLabel, *m_leadLifeLabel, *m_periLifeLabel,
    *m_photoSolidLabel, *m_intensityCoefLabel;
  
  wxButton *m_photoSolidBrowseButton;
  wxTextCtrl *m_photoSolidTextCtrl;
  
  wxBoxSizer *m_panelSizer, *m_photoSolidSizer;
  wxFlexGridSizer *m_slidersSizer;
  wxRadioBox *m_flickeringRadioBox, *m_FDFRadioBox;

  FlameConfig *m_flameConfig;
  /* Index du solveur */
  int m_index;
  GLFlameCanvas *m_glBuffer;
  
  double SLIDER_SENSIBILITY;
  int SLIDER_RANGE;
  
  DECLARE_EVENT_TABLE()
};


#endif

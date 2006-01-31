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
    IDRB_Flickering,
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
  
  SolverConfig *m_solverConfig;
  /* Index du solveur */
  int m_index;
  GLFlameCanvas *m_glBuffer;
  
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
  
  wxSlider *m_innerForceSlider;
  wxStaticText *m_innerForceLabel;
  
  wxBoxSizer *m_panelSizer, *m_forcesSizer;
  wxRadioBox *m_flickeringRadioBox;

  FlameConfig *m_flameConfig;
  /* Index du solveur */
  int m_index;
  GLFlameCanvas *m_glBuffer;
  
  double SLIDER_SENSIBILITY;
  int SLIDER_RANGE;
  
  DECLARE_EVENT_TABLE()
};


#endif

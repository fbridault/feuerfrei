#ifndef MAINPANELS_H
#define MAINPANELS_H

#include <header.h>
#include <wx/spinctrl.h>
#include "wxGLBuffer.hpp"

enum
  {
    IDSL_FXAP = 1,
    IDSL_FYAP,
    IDSL_FZAP,
    IDB_Flickering,
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

/** Panneau pour les onglets de la bo�te de dialogue des solveurs */
class SolverMainPanel: public wxPanel 
{
public:
  SolverMainPanel(wxWindow* parent, int id, SolverConfig *solverConfig, int index, wxGLBuffer *glBuffer, 
		  const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
  void setCtrlValues(SolverConfig* solverConfig);
  void getCtrlValues(SolverConfig* solverConfig);
private:

  void OnScrollPosition(wxScrollEvent& event);
  void OnFXAPMINEnter(wxCommandEvent& event);
  void OnFXAPMAXEnter(wxCommandEvent& event);
  void OnFYAPMINEnter(wxCommandEvent& event);
  void OnFYAPMAXEnter(wxCommandEvent& event);
  void OnFZAPMINEnter(wxCommandEvent& event);
  void OnFZAPMAXEnter(wxCommandEvent& event);
  void ComputeSlidersValues(void);

  void setProperties();
  void doLayout();
  
  wxButton *m_buttonFlickering;
  wxSlider *m_solverXAxisPositionSlider, *m_solverYAxisPositionSlider, *m_solverZAxisPositionSlider;
  wxTextCtrl *m_solverXAxisPositionSliderMax, *m_solverYAxisPositionSliderMax, *m_solverZAxisPositionSliderMax,
    *m_solverXAxisPositionSliderMin, *m_solverYAxisPositionSliderMin, *m_solverZAxisPositionSliderMin;
  wxStaticText *m_solverXAxisPositionLabel, *m_solverYAxisPositionLabel, *m_solverZAxisPositionLabel;
  
  wxBoxSizer *m_panelSizer;
  wxBoxSizer *m_solversXAxisPositionSizer, *m_solversYAxisPositionSizer, *m_solversZAxisPositionSizer;
  wxBoxSizer *m_solversXAxisPositionRangeSizer, *m_solversYAxisPositionRangeSizer, *m_solversZAxisPositionRangeSizer;
  
  SolverConfig *m_solverConfig;
  /* Index du solveur */
  int m_index;
  wxGLBuffer *m_glBuffer;
  
  double SLIDER_SENSIBILITY;
  int SLIDER_RANGE;
  
  DECLARE_EVENT_TABLE()
};


#endif

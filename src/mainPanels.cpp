#include "mainPanels.hpp"

BEGIN_EVENT_TABLE(SolverMainPanel, wxPanel)
  EVT_SCROLL(SolverMainPanel::OnScrollPosition)
  EVT_TEXT_ENTER(IDT_FXAPMIN, SolverMainPanel::OnFXAPMINEnter)
  EVT_TEXT_ENTER(IDT_FXAPMAX, SolverMainPanel::OnFXAPMAXEnter)
  EVT_TEXT_ENTER(IDT_FYAPMIN, SolverMainPanel::OnFYAPMINEnter)
  EVT_TEXT_ENTER(IDT_FYAPMAX, SolverMainPanel::OnFYAPMAXEnter)
  EVT_TEXT_ENTER(IDT_FZAPMIN, SolverMainPanel::OnFZAPMINEnter)
  EVT_TEXT_ENTER(IDT_FZAPMAX, SolverMainPanel::OnFZAPMAXEnter)
END_EVENT_TABLE();


SolverMainPanel::SolverMainPanel(wxWindow* parent, int id, SolverConfig *solverConfig, int index, 
				 wxGLBuffer *glBuffer, const wxPoint& pos, const wxSize& size, long style):
  wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
  SLIDER_SENSIBILITY=100.0;
  SLIDER_RANGE=500;
  
  m_solverConfig = solverConfig;
  m_index = index;
  m_glBuffer = glBuffer;
  
  m_solverXAxisPositionSlider = new wxSlider(this,IDSL_FXAP,0,-SLIDER_RANGE,SLIDER_RANGE, wxDefaultPosition, 
					    wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  m_solverYAxisPositionSlider = new wxSlider(this,IDSL_FYAP,0,-SLIDER_RANGE,SLIDER_RANGE, wxDefaultPosition,
					    wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  m_solverZAxisPositionSlider = new wxSlider(this,IDSL_FZAP,0,-SLIDER_RANGE,SLIDER_RANGE, wxDefaultPosition, 
					    wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  m_solverXAxisPositionLabel = new wxStaticText(this,IDST_FXAP,_("X"));
  m_solverYAxisPositionLabel = new wxStaticText(this,IDST_FYAP,_("Y"));
  m_solverZAxisPositionLabel = new wxStaticText(this,IDST_FZAP,_("Z"));
  m_solverXAxisPositionSliderMin = new wxTextCtrl(this,IDT_FXAPMIN,_(""),
						 wxDefaultPosition,wxSize(45,22),wxTE_PROCESS_ENTER);
  m_solverXAxisPositionSliderMax = new wxTextCtrl(this,IDT_FXAPMAX,_(""),
						 wxDefaultPosition,wxSize(45,22),wxTE_PROCESS_ENTER);
  m_solverYAxisPositionSliderMin = new wxTextCtrl(this,IDT_FYAPMIN,_(""),
						 wxDefaultPosition,wxSize(45,22),wxTE_PROCESS_ENTER);
  m_solverYAxisPositionSliderMax = new wxTextCtrl(this,IDT_FYAPMAX,_(""),
						 wxDefaultPosition,wxSize(45,22),wxTE_PROCESS_ENTER);
  m_solverZAxisPositionSliderMin = new wxTextCtrl(this,IDT_FZAPMIN,_(""),
						 wxDefaultPosition,wxSize(45,22),wxTE_PROCESS_ENTER);
  m_solverZAxisPositionSliderMax = new wxTextCtrl(this,IDT_FZAPMAX,_(""),
						 wxDefaultPosition,wxSize(45,22),wxTE_PROCESS_ENTER);

  (*m_solverXAxisPositionSliderMin) << -SLIDER_RANGE;
  (*m_solverXAxisPositionSliderMax) << SLIDER_RANGE;
  (*m_solverYAxisPositionSliderMin) << -SLIDER_RANGE;
  (*m_solverYAxisPositionSliderMax) << SLIDER_RANGE;
  (*m_solverZAxisPositionSliderMin) << -SLIDER_RANGE;
  (*m_solverZAxisPositionSliderMax) << SLIDER_RANGE;
  
  m_buttonFlickering = new wxButton(this,IDB_Flickering,_("Flickering"));      
  
  /* Réglages des solveurs */  
  m_solversXAxisPositionSizer = new wxBoxSizer(wxHORIZONTAL);
  m_solversYAxisPositionSizer = new wxBoxSizer(wxHORIZONTAL);
  m_solversZAxisPositionSizer = new wxBoxSizer(wxHORIZONTAL);

  m_solversXAxisPositionSizer->Add(m_solverXAxisPositionLabel, 1, wxTOP|wxLEFT, 4);
  m_solversXAxisPositionSizer->Add(m_solverXAxisPositionSlider, 18, 0, 0);
  m_solversYAxisPositionSizer->Add(m_solverYAxisPositionLabel, 1, wxTOP|wxLEFT, 4);
  m_solversYAxisPositionSizer->Add(m_solverYAxisPositionSlider, 18, 0, 0);
  m_solversZAxisPositionSizer->Add(m_solverZAxisPositionLabel, 1, wxTOP|wxLEFT, 4);
  m_solversZAxisPositionSizer->Add(m_solverZAxisPositionSlider, 18, 0, 0);
  
  m_solversXAxisPositionRangeSizer = new wxBoxSizer(wxHORIZONTAL);
  m_solversYAxisPositionRangeSizer = new wxBoxSizer(wxHORIZONTAL);
  m_solversZAxisPositionRangeSizer = new wxBoxSizer(wxHORIZONTAL);

  m_solversXAxisPositionRangeSizer->Add(m_solverXAxisPositionSliderMin, 0, wxADJUST_MINSIZE|wxLEFT, 15);
  m_solversXAxisPositionRangeSizer->AddStretchSpacer(1);
  m_solversXAxisPositionRangeSizer->Add(m_solverXAxisPositionSliderMax, 0, wxADJUST_MINSIZE|wxRIGHT, 5);
  m_solversYAxisPositionRangeSizer->Add(m_solverYAxisPositionSliderMin, 0, wxADJUST_MINSIZE|wxLEFT, 15);
  m_solversYAxisPositionRangeSizer->AddStretchSpacer(1);
  m_solversYAxisPositionRangeSizer->Add(m_solverYAxisPositionSliderMax, 0, wxADJUST_MINSIZE|wxRIGHT, 5);
  m_solversZAxisPositionRangeSizer->Add(m_solverZAxisPositionSliderMin, 0, wxADJUST_MINSIZE|wxLEFT, 15);
  m_solversZAxisPositionRangeSizer->AddStretchSpacer(1);
  m_solversZAxisPositionRangeSizer->Add(m_solverZAxisPositionSliderMax, 0, wxADJUST_MINSIZE|wxRIGHT, 5);

  m_panelSizer = new wxBoxSizer(wxVERTICAL);
  m_panelSizer->Add(m_solversXAxisPositionSizer, 1, wxEXPAND, 0);
  m_panelSizer->Add(m_solversXAxisPositionRangeSizer, 1, wxEXPAND, 0);
  m_panelSizer->Add(m_solversYAxisPositionSizer, 1, wxEXPAND, 0);
  m_panelSizer->Add(m_solversYAxisPositionRangeSizer, 1, wxEXPAND, 0);
  m_panelSizer->Add(m_solversZAxisPositionSizer, 1, wxEXPAND, 0);
  m_panelSizer->Add(m_solversZAxisPositionRangeSizer, 1, wxEXPAND, 0);
  m_panelSizer->Add(m_buttonFlickering, 0, 0, 0);
  
  SetSizer(m_panelSizer);

  ComputeSlidersValues();
}

void SolverMainPanel::OnScrollPosition(wxScrollEvent& event)
{
  CPoint pt(m_solverXAxisPositionSlider->GetValue()/SLIDER_SENSIBILITY,
	    m_solverYAxisPositionSlider->GetValue()/SLIDER_SENSIBILITY,
	    m_solverZAxisPositionSlider->GetValue()/SLIDER_SENSIBILITY);

  m_glBuffer->moveSolver(m_index, pt);
  
  m_solverConfig->position = pt;
}

void SolverMainPanel::OnFXAPMINEnter(wxCommandEvent& event)
{
  long val;
  m_solverXAxisPositionSliderMin->GetValue().ToLong(&val);
  m_solverXAxisPositionSlider->SetRange(val, m_solverXAxisPositionSlider->GetMax());
}

void SolverMainPanel::OnFXAPMAXEnter(wxCommandEvent& event)
{
  long val;
  m_solverXAxisPositionSliderMax->GetValue().ToLong(&val);
  m_solverXAxisPositionSlider->SetRange(m_solverXAxisPositionSlider->GetMin(), val);
}

void SolverMainPanel::OnFYAPMINEnter(wxCommandEvent& event)
{
  long val;
  m_solverYAxisPositionSliderMin->GetValue().ToLong(&val);
  m_solverYAxisPositionSlider->SetRange(val, m_solverYAxisPositionSlider->GetMax());
}

void SolverMainPanel::OnFYAPMAXEnter(wxCommandEvent& event)
{
 long val;
  m_solverYAxisPositionSliderMax->GetValue().ToLong(&val);
  m_solverYAxisPositionSlider->SetRange(m_solverYAxisPositionSlider->GetMin(), val);
}

void SolverMainPanel::OnFZAPMINEnter(wxCommandEvent& event)
{
  long val;
  m_solverZAxisPositionSliderMin->GetValue().ToLong(&val);
  m_solverZAxisPositionSlider->SetRange(val, m_solverZAxisPositionSlider->GetMax());
}

void SolverMainPanel::OnFZAPMAXEnter(wxCommandEvent& event)
{
  long val;
  m_solverZAxisPositionSliderMax->GetValue().ToLong(&val);
  m_solverZAxisPositionSlider->SetRange(m_solverZAxisPositionSlider->GetMin(), val);
}

void SolverMainPanel::ComputeSlidersValues(void)
{
  int valx = (int)(m_solverConfig->position.x*SLIDER_SENSIBILITY);
  int valy = (int)(m_solverConfig->position.y*SLIDER_SENSIBILITY);
  int valz = (int)(m_solverConfig->position.z*SLIDER_SENSIBILITY);
  
  m_solverXAxisPositionSlider->SetValue(valx);
  m_solverYAxisPositionSlider->SetValue(valy);
  m_solverZAxisPositionSlider->SetValue(valz);
  m_solverXAxisPositionSlider->SetRange(valx-SLIDER_RANGE,valx+SLIDER_RANGE);
  m_solverYAxisPositionSlider->SetRange(valy-SLIDER_RANGE,valy+SLIDER_RANGE);
  m_solverZAxisPositionSlider->SetRange(valz-SLIDER_RANGE,valz+SLIDER_RANGE);

  m_solverXAxisPositionSliderMin->Clear();
  m_solverXAxisPositionSliderMax->Clear();
  m_solverYAxisPositionSliderMin->Clear();
  m_solverYAxisPositionSliderMax->Clear();
  m_solverZAxisPositionSliderMin->Clear();
  m_solverZAxisPositionSliderMax->Clear();
  
  (*m_solverXAxisPositionSliderMin) << valx-SLIDER_RANGE;
  (*m_solverXAxisPositionSliderMax) << valx+SLIDER_RANGE;
  (*m_solverYAxisPositionSliderMin) << valy-SLIDER_RANGE;
  (*m_solverYAxisPositionSliderMax) << valy+SLIDER_RANGE;
  (*m_solverZAxisPositionSliderMin) << valz-SLIDER_RANGE;
  (*m_solverZAxisPositionSliderMax) << valz+SLIDER_RANGE;
}

/************************ FlameMainPanel class definition *********************************************/

BEGIN_EVENT_TABLE(FlameMainPanel, wxPanel)
  EVT_SCROLL(FlameMainPanel::OnScrollPosition)
END_EVENT_TABLE();


FlameMainPanel::FlameMainPanel(wxWindow* parent, int id, FlameConfig *flameConfig, int index, 
				 wxGLBuffer *glBuffer, const wxPoint& pos, const wxSize& size, long style):
  wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
  SLIDER_SENSIBILITY=1000.0;
  SLIDER_RANGE=200;
  
  m_flameConfig = flameConfig;
  m_index = index;
  m_glBuffer = glBuffer;
  
  m_fieldForcesSlider = new wxSlider(this,-1,0,-SLIDER_RANGE,SLIDER_RANGE, wxDefaultPosition, 
					    wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  m_innerForceSlider = new wxSlider(this,-1,0,-SLIDER_RANGE,SLIDER_RANGE, wxDefaultPosition,
					    wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  m_fieldForcesLabel = new wxStaticText(this,-1,_("Field forces"));
  m_innerForceLabel = new wxStaticText(this,-1,_("Inner force"));
  
  m_forcesSizer = new wxFlexGridSizer(2,2);
  m_forcesSizer->AddGrowableCol(1,3);
  m_forcesSizer->Add(m_fieldForcesLabel, 1, wxTOP|wxLEFT, 4);
  m_forcesSizer->Add(m_fieldForcesSlider, 10, wxEXPAND, 0);
  m_forcesSizer->Add(m_innerForceLabel, 1, wxTOP|wxLEFT, 4);
  m_forcesSizer->Add(m_innerForceSlider, 10, wxEXPAND, 0);
  
  m_panelSizer = new wxBoxSizer(wxVERTICAL);
  m_panelSizer->Add(m_forcesSizer, 1, wxEXPAND, 0);
  
  m_fieldForcesSlider->SetValue((int)(m_flameConfig->fieldForces*SLIDER_SENSIBILITY));
  m_innerForceSlider->SetValue((int)(m_flameConfig->innerForce*SLIDER_SENSIBILITY));

  SetSizer(m_panelSizer);
}

void FlameMainPanel::OnScrollPosition(wxScrollEvent& event)
{
  double valField = m_fieldForcesSlider->GetValue()/SLIDER_SENSIBILITY;
  double valInner = m_innerForceSlider->GetValue()/SLIDER_SENSIBILITY;

  m_glBuffer->setFlameForces(m_index, valField, valInner);
  
  m_flameConfig->fieldForces = valField;
  m_flameConfig->innerForce = valInner;
}

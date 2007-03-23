#include "mainPanels.hpp"
#include <wx/gbsizer.h>
#include <wx/file.h>

#ifdef RTFLAMES_BUILD
  #include "GLFlameCanvas.hpp"
#else
  #include "GLFluidsCanvas.hpp"
#endif

BEGIN_EVENT_TABLE(SolverMainPanel, wxPanel)
  EVT_SCROLL(SolverMainPanel::OnScrollPosition)
  EVT_TEXT_ENTER(IDT_FXAPMIN, SolverMainPanel::OnFXAPMINEnter)
  EVT_TEXT_ENTER(IDT_FXAPMAX, SolverMainPanel::OnFXAPMAXEnter)
  EVT_TEXT_ENTER(IDT_FYAPMIN, SolverMainPanel::OnFYAPMINEnter)
  EVT_TEXT_ENTER(IDT_FYAPMAX, SolverMainPanel::OnFYAPMAXEnter)
  EVT_TEXT_ENTER(IDT_FZAPMIN, SolverMainPanel::OnFZAPMINEnter)
  EVT_TEXT_ENTER(IDT_FZAPMAX, SolverMainPanel::OnFZAPMAXEnter)
  EVT_CHECKBOX(IDCHK_MOVE, SolverMainPanel::OnCheckMove)
#ifdef RTFLUIDS_BUILD
  EVT_BUTTON(IDB_LEFT, SolverMainPanel::OnClickDensities)
  EVT_BUTTON(IDB_RIGHT, SolverMainPanel::OnClickDensities)
  EVT_BUTTON(IDB_TOP, SolverMainPanel::OnClickDensities)
  EVT_BUTTON(IDB_BOTTOM, SolverMainPanel::OnClickDensities)
#endif
END_EVENT_TABLE();

#ifdef RTFLAMES_BUILD
SolverMainPanel::SolverMainPanel(wxWindow* parent, int id, SolverConfig* const solverConfig, int index, 
				 GLFlameCanvas* const glBuffer, char type, const wxPoint& pos, 
				 const wxSize& size, long style):
#else
SolverMainPanel::SolverMainPanel(wxWindow* parent, int id, SolverConfig* const solverConfig, int index, 
				 GLFluidsCanvas* const glBuffer, const wxPoint& pos, const wxSize& size, long style):
#endif
  wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
  SLIDER_SENSIBILITY=100.0;
  FORCE_SENSIBILITY=100.0;
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
  
  m_buoyancyLabel = new wxStaticText(this,-1,_("Buoyancy"));
  m_buoyancySlider = new wxSlider(this,IDSL_SF,0,-SLIDER_RANGE/10,2*SLIDER_RANGE/10, wxDefaultPosition, 
				  wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  
  m_buoyancySlider->SetValue((int)(m_solverConfig->buoyancy*FORCE_SENSIBILITY));
  
  (*m_solverXAxisPositionSliderMin) << -SLIDER_RANGE;
  (*m_solverXAxisPositionSliderMax) << SLIDER_RANGE;
  (*m_solverYAxisPositionSliderMin) << -SLIDER_RANGE;
  (*m_solverYAxisPositionSliderMax) << SLIDER_RANGE;
  (*m_solverZAxisPositionSliderMin) << -SLIDER_RANGE;
  (*m_solverZAxisPositionSliderMax) << SLIDER_RANGE;
  
  /* Réglages des solveurs */  
  m_solversXAxisPositionSizer = new wxBoxSizer(wxHORIZONTAL);
  m_solversYAxisPositionSizer = new wxBoxSizer(wxHORIZONTAL);
  m_solversZAxisPositionSizer = new wxBoxSizer(wxHORIZONTAL);
  
  m_solversXAxisPositionSizer->Add(m_solverXAxisPositionLabel, 1, wxLEFT, 4);
  m_solversXAxisPositionSizer->Add(m_solverXAxisPositionSlider, 18, 0, 0);
  m_solversYAxisPositionSizer->Add(m_solverYAxisPositionLabel, 1, wxLEFT, 4);
  m_solversYAxisPositionSizer->Add(m_solverYAxisPositionSlider, 18, 0, 0);
  m_solversZAxisPositionSizer->Add(m_solverZAxisPositionLabel, 1, wxLEFT, 4);
  m_solversZAxisPositionSizer->Add(m_solverZAxisPositionSlider, 18, 0, 0);
  
  m_solversXAxisPositionRangeSizer = new wxBoxSizer(wxHORIZONTAL);  
  m_solversXAxisPositionRangeSizer->Add(m_solverXAxisPositionSliderMin, 0, wxADJUST_MINSIZE|wxLEFT, 15);
  m_solversXAxisPositionRangeSizer->AddStretchSpacer(1);
  m_solversXAxisPositionRangeSizer->Add(m_solverXAxisPositionSliderMax, 0, wxADJUST_MINSIZE|wxRIGHT, 5);
  
  m_solversYAxisPositionRangeSizer = new wxBoxSizer(wxHORIZONTAL);
  m_solversYAxisPositionRangeSizer->Add(m_solverYAxisPositionSliderMin, 0, wxADJUST_MINSIZE|wxLEFT, 15);
  m_solversYAxisPositionRangeSizer->AddStretchSpacer(1);
  m_solversYAxisPositionRangeSizer->Add(m_solverYAxisPositionSliderMax, 0, wxADJUST_MINSIZE|wxRIGHT, 5);
  
  m_solversZAxisPositionRangeSizer = new wxBoxSizer(wxHORIZONTAL);
  m_solversZAxisPositionRangeSizer->Add(m_solverZAxisPositionSliderMin, 0, wxADJUST_MINSIZE|wxLEFT, 15);
  m_solversZAxisPositionRangeSizer->AddStretchSpacer(1);
  m_solversZAxisPositionRangeSizer->Add(m_solverZAxisPositionSliderMax, 0, wxADJUST_MINSIZE|wxRIGHT, 5);
    
  m_forcesSizer = new wxBoxSizer(wxHORIZONTAL);
  m_forcesSizer->Add(m_buoyancyLabel, 1, wxTOP|wxLEFT, 4);
  m_forcesSizer->Add(m_buoyancySlider, 2, wxEXPAND, 0);
  
  m_panelSizer = new wxBoxSizer(wxVERTICAL);

#ifdef RTFLUIDS_BUILD
  m_moveCheckBox = new wxCheckBox(this,IDCHK_MOVE,_("Moving mode"));
  m_panelSizer->Add(m_moveCheckBox, 0, wxEXPAND, 0);
#else
  m_type = type;
  if(!m_type){
    m_moveCheckBox = new wxCheckBox(this,IDCHK_MOVE,_("Moving mode"));
    m_panelSizer->Add(m_moveCheckBox, 0, wxEXPAND, 0);
  }
#endif
  m_panelSizer->Add(m_solversXAxisPositionSizer, 0, wxEXPAND, 0);
  m_panelSizer->Add(m_solversXAxisPositionRangeSizer, 0, wxEXPAND, 0);
  m_panelSizer->Add(m_solversYAxisPositionSizer, 0, wxEXPAND, 0);
  m_panelSizer->Add(m_solversYAxisPositionRangeSizer, 0, wxEXPAND, 0);
  m_panelSizer->Add(m_solversZAxisPositionSizer, 0, wxEXPAND, 0);
  m_panelSizer->Add(m_solversZAxisPositionRangeSizer, 0, wxEXPAND, 0);
  m_panelSizer->Add(m_forcesSizer, 0, wxEXPAND, 0);
#ifdef RTFLUIDS_BUILD
  m_densityLButton = new wxButton(this, IDB_LEFT, _("Left"));
  m_densityRButton = new wxButton(this, IDB_RIGHT, _("Right"));
  m_densityTButton = new wxButton(this, IDB_TOP, _("Top"));
  m_densityBButton = new wxButton(this, IDB_BOTTOM, _("Bottom"));
  
  m_densitiesSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Density"));
  m_densitiesSizer->Add(m_densityLButton, 0, 0, 0);
  m_densitiesSizer->Add(m_densityRButton, 0, 0, 0);
  m_densitiesSizer->Add(m_densityTButton, 0, 0, 0);
  m_densitiesSizer->Add(m_densityBButton, 0, 0, 0);
  m_panelSizer->Add(m_densitiesSizer, 0, 0, 0);
#endif
  
  SetSlidersValues(type);
  
  SetSizerAndFit(m_panelSizer);
}

void SolverMainPanel::SetSlidersValues(char type)
{  
  m_saveSliderValues.x = m_solverConfig->position.x;
  m_saveSliderValues.y = m_solverConfig->position.y;
  m_saveSliderValues.z = m_solverConfig->position.z;
  
  m_solverXAxisPositionSliderMin->Clear();
  m_solverXAxisPositionSliderMax->Clear();
  m_solverYAxisPositionSliderMin->Clear();
  m_solverYAxisPositionSliderMax->Clear();
  m_solverZAxisPositionSliderMin->Clear();
  m_solverZAxisPositionSliderMax->Clear();
  
  (*m_solverXAxisPositionSliderMin) << m_saveSliderValues.x-SLIDER_RANGE;
  (*m_solverXAxisPositionSliderMax) << m_saveSliderValues.x+SLIDER_RANGE;
  (*m_solverYAxisPositionSliderMin) << m_saveSliderValues.y-SLIDER_RANGE;
  (*m_solverYAxisPositionSliderMax) << m_saveSliderValues.y+SLIDER_RANGE;
  (*m_solverZAxisPositionSliderMin) << m_saveSliderValues.z-SLIDER_RANGE;
  (*m_solverZAxisPositionSliderMax) << m_saveSliderValues.z+SLIDER_RANGE;

  if(type < 1){
    m_solverXAxisPositionSliderMin->Disable();
    m_solverXAxisPositionSliderMax->Disable();
    m_solverYAxisPositionSliderMin->Disable();
    m_solverYAxisPositionSliderMax->Disable();
    m_solverZAxisPositionSliderMin->Disable();
    m_solverZAxisPositionSliderMax->Disable();
  }else{
    m_solverXAxisPositionSlider->SetValue((int)(m_saveSliderValues.x*SLIDER_SENSIBILITY));
    m_solverYAxisPositionSlider->SetValue((int)(m_saveSliderValues.y*SLIDER_SENSIBILITY));
    m_solverZAxisPositionSlider->SetValue((int)(m_saveSliderValues.z*SLIDER_SENSIBILITY));
  }
}

void SolverMainPanel::OnCheckMove(wxCommandEvent& event)
{
  Point oldValues(m_solverXAxisPositionSlider->GetValue()/SLIDER_SENSIBILITY,
		  m_solverYAxisPositionSlider->GetValue()/SLIDER_SENSIBILITY,
		  m_solverZAxisPositionSlider->GetValue()/SLIDER_SENSIBILITY);

  m_solverXAxisPositionSlider->SetValue((int)(m_saveSliderValues.x*SLIDER_SENSIBILITY));
  m_solverYAxisPositionSlider->SetValue((int)(m_saveSliderValues.y*SLIDER_SENSIBILITY));
  m_solverZAxisPositionSlider->SetValue((int)(m_saveSliderValues.z*SLIDER_SENSIBILITY));
  
  m_saveSliderValues = oldValues;
  
  if(m_moveCheckBox->IsChecked())
    {
      long min, max;
      
      m_solverXAxisPositionSliderMin->GetValue().ToLong(&min);
      m_solverXAxisPositionSliderMax->GetValue().ToLong(&max);
      m_solverXAxisPositionSlider->SetRange(min, max);
      m_solverYAxisPositionSliderMin->GetValue().ToLong(&min);
      m_solverYAxisPositionSliderMax->GetValue().ToLong(&max);
      m_solverYAxisPositionSlider->SetRange(min, max);
      m_solverZAxisPositionSliderMin->GetValue().ToLong(&min);
      m_solverZAxisPositionSliderMax->GetValue().ToLong(&max);
      m_solverZAxisPositionSlider->SetRange(min, max);
      
      m_solverXAxisPositionSliderMin->Enable();
      m_solverXAxisPositionSliderMax->Enable();
      m_solverYAxisPositionSliderMin->Enable();
      m_solverYAxisPositionSliderMax->Enable();
      m_solverZAxisPositionSliderMin->Enable();
      m_solverZAxisPositionSliderMax->Enable();
    }
  else
    {
      m_solverXAxisPositionSliderMin->Disable();
      m_solverXAxisPositionSliderMax->Disable();
      m_solverYAxisPositionSliderMin->Disable();
      m_solverYAxisPositionSliderMax->Disable();
      m_solverZAxisPositionSliderMin->Disable();
      m_solverZAxisPositionSliderMax->Disable();
      
      m_solverXAxisPositionSlider->SetRange(-SLIDER_RANGE, SLIDER_RANGE);
      m_solverYAxisPositionSlider->SetRange(-SLIDER_RANGE, SLIDER_RANGE);
      m_solverZAxisPositionSlider->SetRange(-SLIDER_RANGE, SLIDER_RANGE);
    }
}

void SolverMainPanel::OnScrollPosition(wxScrollEvent& event)
{
  if(event.GetId() == IDSL_SF)
    {
      double value = m_buoyancySlider->GetValue()/(FORCE_SENSIBILITY);
      
      m_glBuffer->setBuoyancy(m_index, value);
      m_solverConfig->buoyancy = value;
    }
  else
    {
      Point pt(m_solverXAxisPositionSlider->GetValue()/SLIDER_SENSIBILITY,
	       m_solverYAxisPositionSlider->GetValue()/SLIDER_SENSIBILITY,
	       m_solverZAxisPositionSlider->GetValue()/SLIDER_SENSIBILITY);
      
#ifdef RTFLUIDS_BUILD
      if(m_moveCheckBox->IsChecked()){
#else
      if( ( (!m_type) ? m_moveCheckBox->IsChecked() : (m_type > 0)) ){
#endif
	m_glBuffer->moveSolver(m_index, pt, true);
	m_solverConfig->position = pt;
      }else
	m_glBuffer->addPermanentExternalForcesToSolver(m_index,pt);
    }
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

#ifdef RTFLUIDS_BUILD
void SolverMainPanel::OnClickDensities(wxCommandEvent& event)
{
  m_glBuffer->addDensityInSolver(m_index, event.GetId());
}
#endif

#ifdef RTFLAMES_BUILD
/************************ FlameMainPanel class definition *********************************************/

BEGIN_EVENT_TABLE(FlameMainPanel, wxPanel)
  EVT_SCROLL(FlameMainPanel::OnScrollPosition)
  EVT_RADIOBOX(IDRB_FLICK, FlameMainPanel::OnSelectType)
  EVT_RADIOBOX(IDRB_FDF, FlameMainPanel::OnSelectFDF)
  EVT_TEXT_ENTER(IDT_PHOTO, FlameMainPanel::OnPhotoSolidEnter)
  EVT_BUTTON(IDB_BROWSE, FlameMainPanel::OnClickButtonBrowse)
END_EVENT_TABLE();


FlameMainPanel::FlameMainPanel(wxWindow* parent, int id, FlameConfig* const flameConfig, int index, 
			       GLFlameCanvas* const glBuffer, const wxPoint& pos, const wxSize& size, long style):
  wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{ 
  const wxString m_flickeringRadioBoxChoices[] = {
    _("None"),
    _("Vertical"),
    _("From right"),
    _("Random1"),
    _("Random2"),
    _("Noise")
  };
  const wxString m_FDFRadioBoxChoices[] = {
    _("Linear"),
    _("Bilinear"),
    _("Exponential"),
    _("Gauss"),
    _("Random")
  };
  
  LIGHT_SENSIBILITY=100.0;
  FORCE_SENSIBILITY=500.0;
  SLIDER_RANGE=50;
  
  m_flameConfig = flameConfig;
  m_index = index;
  m_glBuffer = glBuffer;
  
  m_innerForceSlider = new wxSlider(this,IDSL_FF,0,-SLIDER_RANGE,2*SLIDER_RANGE, wxDefaultPosition,
				    wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  m_innerForceLabel = new wxStaticText(this,-1,_("Force"));
  
  m_samplingToleranceSlider = new wxSlider(this,IDSL_SPTOL,0,0,2000, wxDefaultPosition,
					   wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  m_samplingToleranceLabel = new wxStaticText(this,-1,_("Sampling"));
  m_leadLifeSlider = new wxSlider(this,IDSL_NLP,0,0,NB_PARTICLES_MAX, wxDefaultPosition,
				  wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  m_leadLifeLabel = new wxStaticText(this,-1,_("Lead life span"));
  m_periLifeSlider = new wxSlider(this,IDSL_NPP,0,0,NB_PARTICLES_MAX, wxDefaultPosition,
				  wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  m_periLifeLabel = new wxStaticText(this,-1,_("Peri life span"));
  
  m_slidersSizer = new wxFlexGridSizer(2);
  m_slidersSizer->AddGrowableCol(1,0);
  m_slidersSizer->Add(m_innerForceLabel, 1, wxADJUST_MINSIZE, 0);
  m_slidersSizer->Add(m_innerForceSlider, 6, wxEXPAND, 0);
  m_slidersSizer->Add(m_leadLifeLabel, 1, wxADJUST_MINSIZE, 0);
  m_slidersSizer->Add(m_leadLifeSlider, 6, wxEXPAND, 0);
  m_slidersSizer->Add(m_periLifeLabel, 1, wxADJUST_MINSIZE, 0);
  m_slidersSizer->Add(m_periLifeSlider, 6, wxEXPAND, 0);
  m_slidersSizer->Add(m_samplingToleranceLabel, 1, wxADJUST_MINSIZE, 0);
  m_slidersSizer->Add(m_samplingToleranceSlider, 6, wxEXPAND, 0);
  
  m_flickeringRadioBox = new wxRadioBox(this, IDRB_FLICK, _("Flickering"), wxDefaultPosition, wxDefaultSize, 
					6, m_flickeringRadioBoxChoices, 2, wxRA_SPECIFY_ROWS);
  
  m_photoSolidLabel = new wxStaticText(this, -1, _("Ph. Solid"));
  m_photoSolidTextCtrl = new wxTextCtrl(this, IDT_PHOTO, m_flameConfig->IESFileName,
					wxDefaultPosition, wxSize(100,22), wxTE_PROCESS_ENTER);
  m_photoSolidBrowseButton = new wxButton(this, IDB_BROWSE, _("Browse..."));
  
  m_photoSolidSizer = new wxBoxSizer(wxHORIZONTAL);
  m_photoSolidSizer->Add(m_photoSolidLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 5);
  m_photoSolidSizer->Add(m_photoSolidTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 10);
  m_photoSolidSizer->Add(m_photoSolidBrowseButton, 0, wxADJUST_MINSIZE, 0);  
  
  m_panelSizer = new wxBoxSizer(wxVERTICAL);
  m_panelSizer->Add(m_flickeringRadioBox,  0, wxADJUST_MINSIZE, 0);
  
  switch(flameConfig->type){
  case FIRMALAMPE :
  case TORCH :
  case CAMPFIRE :
    m_FDFRadioBox = new wxRadioBox(this, IDRB_FDF, _("Fuel Distribution Function"), wxDefaultPosition, wxDefaultSize, 
				   5, m_FDFRadioBoxChoices, 2, wxRA_SPECIFY_ROWS);
  m_panelSizer->Add(m_FDFRadioBox,  0, wxADJUST_MINSIZE, 0);
  m_FDFRadioBox->SetSelection(m_flameConfig->fdf);
  }
  
  m_panelSizer->Add(m_slidersSizer, 0, wxEXPAND, 0);
  m_panelSizer->Add(m_photoSolidSizer, 0, wxEXPAND, 0);
  
  m_innerForceSlider->SetValue((int)(m_flameConfig->innerForce*FORCE_SENSIBILITY));
  m_leadLifeSlider->SetValue((int)(m_flameConfig->leadLifeSpan));
  m_periLifeSlider->SetValue((int)(m_flameConfig->periLifeSpan));
  m_flickeringRadioBox->SetSelection(m_flameConfig->flickering);
  m_samplingToleranceSlider->SetValue((int)(m_flameConfig->samplingTolerance));
  
  SetSizerAndFit(m_panelSizer);
}

void FlameMainPanel::OnScrollPosition(wxScrollEvent& event)
{
  double val;
  
  switch(event.GetId()){
  case IDSL_FF :
    val = m_innerForceSlider->GetValue()/FORCE_SENSIBILITY;
    m_glBuffer->setFlameForces(m_index, val);
    m_flameConfig->innerForce = val;
    break;
  case IDSL_SPTOL:
    m_glBuffer->setFlameSamplingTolerance(m_index, m_samplingToleranceSlider->GetValue());
    m_flameConfig->samplingTolerance = m_samplingToleranceSlider->GetValue();
    break;
  case IDSL_NLP:
    m_flameConfig->leadLifeSpan = m_leadLifeSlider->GetValue();
    break;
  case IDSL_NPP:
    m_flameConfig->periLifeSpan = m_periLifeSlider->GetValue();
    break;
  }
}

void FlameMainPanel::OnSelectType(wxCommandEvent& event)
{
  m_flameConfig->flickering = event.GetSelection();
}

void FlameMainPanel::OnSelectFDF(wxCommandEvent& event)
{
  m_flameConfig->fdf = event.GetSelection();
}

void FlameMainPanel::OnClickButtonBrowse(wxCommandEvent& event)
{
  wxString filename;
  wxString pwd=wxGetCwd();
  pwd <<_("/IES");
  
  wxFileDialog fileDialog(this, _("Choose a IES file for this flame"), pwd, _(""), _("*.ies"), wxOPEN|wxFILE_MUST_EXIST);
  if(fileDialog.ShowModal() == wxID_OK){
    filename = fileDialog.GetFilename();
    
    filename = _("IES/") + filename;
    m_photoSolidTextCtrl->SetValue(filename);
  }
  OnPhotoSolidEnter(event);
}

void FlameMainPanel::OnPhotoSolidEnter(wxCommandEvent& event)
{
  bool restart = false;
  wxString filename = m_photoSolidTextCtrl->GetValue();
  wxString pwd=wxGetCwd();
  
  filename.Replace(pwd,_(""),false);
  if(!wxFile::Exists(filename)){
    wxString message;
    message <<  _("File ") << filename << _(" doesn't exist !");
    wxMessageDialog *errorDialog = new wxMessageDialog(this,message, _("Error"),wxOK|wxICON_ERROR);
    errorDialog->ShowModal();
    errorDialog->Destroy();
    return;
  }
  if(m_glBuffer->IsRunning()){
    restart = true;
    m_glBuffer->setRunningState(false);
  }
  m_flameConfig->IESFileName = filename;
  m_glBuffer->RegeneratePhotometricSolids(m_index,filename);
  if(restart)
    m_glBuffer->setRunningState(true);
}

#endif

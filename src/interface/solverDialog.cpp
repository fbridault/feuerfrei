#include "solverDialog.hpp"

/**************************************** SolverPanel Class methods **************************************/
BEGIN_EVENT_TABLE(SolverPanel, wxPanel)
  EVT_RADIOBOX(IDRS_Type, SolverPanel::OnSelectType)
END_EVENT_TABLE();

SolverPanel::SolverPanel(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style):
  wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{ 
  m_posLabel = new wxStaticText(this, -1, _("Position"));
  m_posXTextCtrl = new DoubleTextCtrl(this, -1, -100, 100, _("0"));
  m_posYTextCtrl = new DoubleTextCtrl(this, -1, -100, 100, _("0"));
  m_posZTextCtrl = new DoubleTextCtrl(this, -1, -100, 100, _("0"));
  m_dimLabel = new wxStaticText(this, -1, _("Dimension"));
  m_dimTextCtrl = new DoubleTextCtrl(this, -1, -10, 10, _("1"));
  m_scaleLabel = new wxStaticText(this, -1, _("Scale"));
  m_scaleXTextCtrl = new DoubleTextCtrl(this, -1, -10, 10, _("1"));
  m_scaleYTextCtrl = new DoubleTextCtrl(this, -1, -10, 10, _("1"));
  m_scaleZTextCtrl = new DoubleTextCtrl(this, -1, -10, 10, _("1"));
  m_resLabel = new wxStaticText(this, -1, _("Resolution"));
  m_resXTextCtrl = new LongTextCtrl(this, -1, 0, 100, _("15"));
  m_resYTextCtrl = new LongTextCtrl(this, -1, 0, 100, _("15"));
  m_resZTextCtrl = new LongTextCtrl(this, -1, 0, 100, _("15"));
  m_timeStepLabel = new wxStaticText(this, -1, _("Time step"));
  m_timeStepTextCtrl = new DoubleTextCtrl(this, -1, 0, 2, _("0,4"));

#ifdef RTFLAMES_BUILD
  const wxString m_solverTypeRadioBoxChoices[] = {
    _("Gauss-Seidel"),
    _("Preconditioned Conjugated Gradient"),
    _("Hybrid"),
    _("LOD Hybrid"),
    _("Simple field"),
    _("Fake field"),
    _("LOD field")
  };
  m_solverTypeRadioBox = new wxRadioBox(this, IDRS_Type, _("Type"), wxDefaultPosition, wxDefaultSize, 
					7, m_solverTypeRadioBoxChoices, 2, wxRA_SPECIFY_COLS);
#else
  const wxString m_solverTypeRadioBoxChoices[] = {
    _("Gauss-Seidel"),
    _("Preconditioned Conjugated Gradient"),
    _("Hybrid"),
    _("LOD Hybrid"),
    _("Simple field"),
    _("Fake field"),
    _("LOD field"),
    _("Both - log residuals"),
    _("Both - log residuals averages"),
    _("Both - log residuals averages + time"),
    _("Gauss-Seidel 2D")
  };
  m_solverTypeRadioBox = new wxRadioBox(this, IDRS_Type, _("Type"), wxDefaultPosition, wxDefaultSize, 
					11, m_solverTypeRadioBoxChoices, 2, wxRA_SPECIFY_COLS);
#endif
  m_omegaDiffLabel = new wxStaticText(this, -1, _("Omega in diffusion"));
  m_omegaDiffTextCtrl = new DoubleTextCtrl(this, -1, 0, 2, _("1,5"));
  m_omegaProjLabel = new wxStaticText(this, -1, _("Omega in projection"));
  m_omegaProjTextCtrl = new DoubleTextCtrl(this, -1, 0, 2, _("1,5"));
  m_epsilonLabel = new wxStaticText(this, -1, _("Error threshold"));
  m_epsilonTextCtrl = new DoubleTextCtrl(this, -1, 0, 0.1, _("0,00001"));
  m_nbMaxIterLabel = new wxStaticText(this, -1, _("Max iterations number"));
  m_nbMaxIterTextCtrl = new LongTextCtrl(this, -1, 0, 1000, _("100"));
  
  setProperties();
  doLayout();

  m_omegaDiffLabel->Disable();
  m_omegaDiffTextCtrl->Disable();
  m_omegaProjLabel->Disable();
  m_omegaProjTextCtrl->Disable();
  m_epsilonLabel->Disable();
  m_epsilonTextCtrl->Disable();
  m_nbMaxIterLabel->Disable();
  m_nbMaxIterTextCtrl->Disable();
}

void SolverPanel::setProperties()
{
  m_posXTextCtrl->SetMinSize(wxSize(50, 22));
  m_posYTextCtrl->SetMinSize(wxSize(50, 22));
  m_posZTextCtrl->SetMinSize(wxSize(50, 22));
  m_dimTextCtrl->SetMinSize(wxSize(50, 22));
  m_scaleXTextCtrl->SetMinSize(wxSize(50, 22));
  m_scaleYTextCtrl->SetMinSize(wxSize(50, 22));
  m_scaleZTextCtrl->SetMinSize(wxSize(50, 22));
  m_resXTextCtrl->SetMinSize(wxSize(50, 22));
  m_resYTextCtrl->SetMinSize(wxSize(50, 22));
  m_resZTextCtrl->SetMinSize(wxSize(50, 22));
  m_timeStepTextCtrl->SetMinSize(wxSize(60, 22));
  m_solverTypeRadioBox->SetSelection(0);
  m_omegaDiffTextCtrl->SetMinSize(wxSize(60, 22));
  m_omegaProjTextCtrl->SetMinSize(wxSize(60, 22));
  m_epsilonTextCtrl->SetMinSize(wxSize(100, 22));
  m_nbMaxIterTextCtrl->SetMinSize(wxSize(50, 22));
}

void SolverPanel::doLayout()
{
  wxBoxSizer* m_panelSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* m_timeStepSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_resSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_dimSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_scaleSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_posSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_omegaDiffSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_omegaProjSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_epsilonSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_nbMaxIterSizer = new wxBoxSizer(wxHORIZONTAL);
  
  m_posSizer->Add(m_posLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_posSizer->Add(m_posXTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_posSizer->Add(m_posYTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_posSizer->Add(m_posZTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 10);
  m_panelSizer->Add(m_posSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);

  m_dimSizer->Add(m_dimLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_dimSizer->Add(m_dimTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 8);
  m_panelSizer->Add(m_dimSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  
  m_scaleSizer->Add(m_scaleLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_scaleSizer->Add(m_scaleXTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_scaleSizer->Add(m_scaleYTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_scaleSizer->Add(m_scaleZTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_panelSizer->Add(m_scaleSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);

  m_resSizer->Add(m_resLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_resSizer->Add(m_resXTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_resSizer->Add(m_resYTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_resSizer->Add(m_resZTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_panelSizer->Add(m_resSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);

  m_timeStepSizer->Add(m_timeStepLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_timeStepSizer->Add(m_timeStepTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 12);
  m_panelSizer->Add(m_timeStepSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);

  m_panelSizer->Add(m_solverTypeRadioBox, 0, wxADJUST_MINSIZE, 0);

  m_omegaDiffSizer->Add(m_omegaDiffLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_omegaDiffSizer->Add(m_omegaDiffTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_panelSizer->Add(m_omegaDiffSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);

  m_omegaProjSizer->Add(m_omegaProjLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_omegaProjSizer->Add(m_omegaProjTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_panelSizer->Add(m_omegaProjSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);

  m_epsilonSizer->Add(m_epsilonLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_epsilonSizer->Add(m_epsilonTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_panelSizer->Add(m_epsilonSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  
  m_nbMaxIterSizer->Add(m_nbMaxIterLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_nbMaxIterSizer->Add(m_nbMaxIterTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_panelSizer->Add(m_nbMaxIterSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  
  SetAutoLayout(true);
  SetSizerAndFit(m_panelSizer);
}

void SolverPanel::setCtrlValues(SolverConfig* const solverConfig)
{
  wxString tmp;
  
  m_posXTextCtrl->Clear();
  m_posYTextCtrl->Clear();
  m_posZTextCtrl->Clear();
  m_dimTextCtrl->Clear();
  m_scaleXTextCtrl->Clear();
  m_scaleYTextCtrl->Clear();
  m_scaleZTextCtrl->Clear();
  m_resXTextCtrl->Clear();
  m_resYTextCtrl->Clear();
  m_resZTextCtrl->Clear();
  m_timeStepTextCtrl->Clear();
  m_omegaDiffTextCtrl->Clear();
  m_omegaProjTextCtrl->Clear();
  m_epsilonTextCtrl->Clear();
  m_nbMaxIterTextCtrl->Clear();

  (*m_posXTextCtrl) << solverConfig->position.x;
  (*m_posYTextCtrl) << solverConfig->position.y;
  (*m_posZTextCtrl) << solverConfig->position.z;
  (*m_dimTextCtrl) << solverConfig->dim;
  (*m_scaleXTextCtrl) << solverConfig->scale.x;
  (*m_scaleYTextCtrl) << solverConfig->scale.y;
  (*m_scaleZTextCtrl) << solverConfig->scale.z;
  (*m_resXTextCtrl) << (int)solverConfig->resx;
  (*m_resYTextCtrl) << (int)solverConfig->resy;
  (*m_resZTextCtrl) << (int)solverConfig->resz;
  tmp.Printf(_("%.4lf"), solverConfig->timeStep);
  (*m_timeStepTextCtrl) << tmp;
  m_solverTypeRadioBox->SetSelection(solverConfig->type);
  tmp.Printf(_("%.5lf"), solverConfig->omegaDiff);
  (*m_omegaDiffTextCtrl) << tmp;
  tmp.Printf(_("%.5lf"), solverConfig->omegaProj);
  (*m_omegaProjTextCtrl) << tmp;
  tmp.Printf(_("%.10lf"), solverConfig->epsilon);
  (*m_epsilonTextCtrl) << tmp;
  (*m_nbMaxIterTextCtrl) << (int)solverConfig->nbMaxIter;

  if(solverConfig->type == GS_SOLVER || solverConfig->type == FAKE_FIELD)
    {
      m_omegaDiffLabel->Disable();
      m_omegaDiffTextCtrl->Disable();
      m_omegaProjLabel->Disable();
      m_omegaProjTextCtrl->Disable();
      m_epsilonLabel->Disable();
      m_epsilonTextCtrl->Disable();
    }
  else
    {
      m_omegaDiffLabel->Enable();
      m_omegaDiffTextCtrl->Enable();
      m_omegaProjLabel->Enable();
      m_omegaProjTextCtrl->Enable();
      m_epsilonLabel->Enable();
      m_epsilonTextCtrl->Enable();
    }
    
  if(solverConfig->type != LOGRES_SOLVER && solverConfig->type != LOGRESAVG_SOLVER && solverConfig->type != LOGRESAVGTIME_SOLVER)
    {
      m_nbMaxIterLabel->Disable();
      m_nbMaxIterTextCtrl->Disable();
    }
  else
    {
      m_nbMaxIterLabel->Enable();
      m_nbMaxIterTextCtrl->Enable();
    }
}

bool SolverPanel::getCtrlValues(SolverConfig* const solverConfig)
{
  /* Une exception est levée si l'un des contrôles n'a pas une valeur conforme */
  try
    {
      solverConfig->position.x = m_posXTextCtrl->GetSafelyValue();
      solverConfig->position.y = m_posYTextCtrl->GetSafelyValue();
      solverConfig->position.z = m_posZTextCtrl->GetSafelyValue();
      solverConfig->dim = m_dimTextCtrl->GetSafelyValue();
      solverConfig->scale.x = m_scaleXTextCtrl->GetSafelyValue();
      solverConfig->scale.y = m_scaleYTextCtrl->GetSafelyValue();
      solverConfig->scale.z = m_scaleZTextCtrl->GetSafelyValue();
      solverConfig->resx = m_resXTextCtrl->GetSafelyValue();
      solverConfig->resy = m_resYTextCtrl->GetSafelyValue();
      solverConfig->resz = m_resZTextCtrl->GetSafelyValue();
      solverConfig->timeStep = m_timeStepTextCtrl->GetSafelyValue();
      solverConfig->omegaDiff = m_omegaDiffTextCtrl->GetSafelyValue();
      solverConfig->omegaProj = m_omegaProjTextCtrl->GetSafelyValue();
      solverConfig->epsilon = m_epsilonTextCtrl->GetSafelyValue();
      solverConfig->nbMaxIter = m_nbMaxIterTextCtrl->GetSafelyValue();
    }
  catch(wxString s)
    {
      return false;
    }

  solverConfig->type = m_solverTypeRadioBox->GetSelection();
  
  return true;
}

void SolverPanel::OnSelectType(wxCommandEvent& event)
{
  if(event.GetSelection() != GS_SOLVER && event.GetSelection() != GS_SOLVER2D && event.GetSelection() != SIMPLE_FIELD && event.GetSelection() != FAKE_FIELD)
    {
      m_omegaDiffLabel->Enable();
      m_omegaDiffTextCtrl->Enable();
      m_omegaProjLabel->Enable();
      m_omegaProjTextCtrl->Enable();
      m_epsilonLabel->Enable();
      m_epsilonTextCtrl->Enable();
    } 
  else
    {
      m_omegaDiffLabel->Disable();
      m_omegaDiffTextCtrl->Disable();
      m_omegaProjLabel->Disable();
      m_omegaProjTextCtrl->Disable();
      m_epsilonLabel->Disable();
      m_epsilonTextCtrl->Disable();
    }
  if(event.GetSelection() != LOGRES_SOLVER && event.GetSelection() != LOGRESAVG_SOLVER && event.GetSelection() != LOGRESAVGTIME_SOLVER)
    {
      m_nbMaxIterLabel->Disable();
      m_nbMaxIterTextCtrl->Disable();
    }
  else
    {
      m_nbMaxIterLabel->Enable();
      m_nbMaxIterTextCtrl->Enable();
    }
}

/**************************************** SolverDialog Class methods **************************************/
BEGIN_EVENT_TABLE(SolverDialog, wxDialog)
  EVT_BUTTON(IDB_Add, SolverDialog::OnClickButtonAdd)
  EVT_BUTTON(IDB_Delete, SolverDialog::OnClickButtonDelete)
  EVT_BUTTON(IDB_OK, SolverDialog::OnOK)
  EVT_BUTTON(IDB_Cancel, SolverDialog::OnCancel)
  EVT_NOTEBOOK_PAGE_CHANGING(IDNB_Solvers, SolverDialog::OnPageChanging)
END_EVENT_TABLE();


#ifdef RTFLAMES_BUILD
SolverDialog::SolverDialog(wxWindow* parent, int id, const wxString& title,  FlameAppConfig *config, 
			   const wxPoint& pos, const wxSize& size, long style):
#else
SolverDialog::SolverDialog(wxWindow* parent, int id, const wxString& title,  FluidsAppConfig *config, 
			   const wxPoint& pos, const wxSize& size, long style):
#endif
  wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{
  m_solverNotebook = new wxNotebook(this, IDNB_Solvers, wxDefaultPosition, wxDefaultSize, 0);
  m_currentConfig = config;
  m_nbPanels = m_currentConfig->nbSolvers;
  for(uint i = 0; i < m_currentConfig->nbSolvers; i++)
    {
      m_solverPanels[i] = new SolverPanel(m_solverNotebook, -1);
      m_solverPanels[i]->setCtrlValues(&m_currentConfig->solvers[i]);
    }
  //     m_nbMaxPanels = 5;
  m_addSolverButton = new wxButton(this, IDB_Add, _("Add a solver"));
  m_deleteSolverButton = new wxButton(this, IDB_Delete, _("Delete a solver"));
  m_okButton = new wxButton(this, IDB_OK, _("OK"));
  m_cancelButton = new wxButton(this, IDB_Cancel, _("Cancel"));

  checkSolverUsage(0);
  
  doLayout();
}

void SolverDialog::doLayout()
{
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  for(uint i = 0; i < m_currentConfig->nbSolvers; i++)
    {
      wxString tabName(_("Solver #")); tabName << i+1;
      m_solverNotebook->AddPage(m_solverPanels[i], tabName);
    }
  sizer->Add(m_solverNotebook, 1, wxEXPAND, 0);
  wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
  buttonSizer->Add(m_addSolverButton, 0, wxADJUST_MINSIZE, 0);
  buttonSizer->Add(m_deleteSolverButton, 0, wxADJUST_MINSIZE, 0);
  buttonSizer->Add(m_okButton, 0, wxADJUST_MINSIZE|wxLEFT, 20);
  buttonSizer->Add(m_cancelButton, 0, wxADJUST_MINSIZE, 0);
  sizer->Add(buttonSizer, 0, wxEXPAND, 0);
  SetAutoLayout(true);
  SetSizer(sizer);
  sizer->Fit(this);
  sizer->SetSizeHints(this);
  Layout();
}

void SolverDialog::OnClickButtonAdd(wxCommandEvent& event)
{
  if(m_nbPanels < NB_MAXSOLVERS){
    m_solverPanels[m_nbPanels] = new SolverPanel(m_solverNotebook, -1);
    m_nbPanels++;
    wxString tabName(_("Solver #")); tabName << m_nbPanels;
    m_solverNotebook->AddPage(m_solverPanels[m_nbPanels-1], tabName);
    m_solverNotebook->SetSelection(m_nbPanels-1);
  }
}

void SolverDialog::OnClickButtonDelete(wxCommandEvent& event)
{
  int sel = m_solverNotebook->GetSelection();
  if(sel != -1){
    m_solverNotebook->DeletePage(sel);
    m_nbPanels--;
    /* Décalage vers la gauche pour combler le trou dans le tableau */
    for(int i=sel; i < m_nbPanels; i++)
      m_solverPanels[i] = m_solverPanels[i+1];
#ifdef RTFLAMES_BUILD
    /* Réindexage des index des solveurs des flammes en conséquence */
    for(uint i = 0; i < m_currentConfig->nbFlames; i++)
      if( m_currentConfig->flames[i].solverIndex > sel)
	m_currentConfig->flames[i].solverIndex--;
#endif
  }
}

void SolverDialog::OnOK(wxCommandEvent& event)
{ 
  SolverConfig* newConfig;
  uint newNb;
  
  newNb = m_nbPanels;  
  newConfig = new SolverConfig[newNb];
  
  for(uint i = 0; i < newNb; i++)
    {
      if(!m_solverPanels[i]->getCtrlValues(&newConfig[i])){
	delete [] newConfig;
	return;
      }else
	/* On recopie l'ancienne buoyancy si elle existe */
	if(m_currentConfig->nbSolvers > i)
	  newConfig[i].buoyancy = m_currentConfig->solvers[i].buoyancy;
	else
	  newConfig[i].buoyancy = .2;
    }
  
  delete [] m_currentConfig->solvers;
  m_currentConfig->solvers = newConfig;
  m_currentConfig->nbSolvers = newNb;
}

void SolverDialog::OnCancel(wxCommandEvent& event)
{
  Destroy();
}

void SolverDialog::OnPageChanging(wxNotebookEvent& event)
{
  checkSolverUsage(event.GetSelection());
}

void SolverDialog::checkSolverUsage(int solverIndex)
{
#ifdef RTFLAMES_BUILD
  for(uint i = 0; i < m_currentConfig->nbFlames; i++)
    {
      if(m_currentConfig->flames[i].solverIndex == solverIndex){
	m_deleteSolverButton->Disable();
	return;
      }
    }
#endif
  m_deleteSolverButton->Enable();
}

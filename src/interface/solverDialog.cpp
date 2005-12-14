#include "solverDialog.hpp"

/**************************************** SolverPanel Class methods **************************************/

SolverPanel::SolverPanel(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style):
  wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
  m_posLabel = new wxStaticText(this, -1, _("Position"));
  m_posXTextCtrl = new DoubleTextCtrl(this, -1, -100, 100, _("0"));
  m_posYTextCtrl = new DoubleTextCtrl(this, -1, -100, 100, _("0"));
  m_posZTextCtrl = new DoubleTextCtrl(this, -1, -100, 100, _("0"));
  m_dimLabel = new wxStaticText(this, -1, _("Dimension"));
  m_dimTextCtrl = new DoubleTextCtrl(this, -1, -10, 10, _("1"));
  m_resLabel = new wxStaticText(this, -1, _("Resolution"));
  m_resXTextCtrl = new LongTextCtrl(this, -1, 0, 100, _("15"));
  m_resYTextCtrl = new LongTextCtrl(this, -1, 0, 100, _("15"));
  m_resZTextCtrl = new LongTextCtrl(this, -1, 0, 100, _("15"));
  m_timeStepLabel = new wxStaticText(this, -1, _("Time step"));
  m_timeStepTextCtrl = new DoubleTextCtrl(this, -1, 0, 2, _("0,4"));
    
  const wxString m_solverTypeRadioBoxChoices[] = {
    _("Gauss-Seidel"),
    _("Preconditioned Conjugated Gradient")
  };
  m_solverTypeRadioBox = new wxRadioBox(this, -1, _("Type"), wxDefaultPosition, wxDefaultSize, 
					2, m_solverTypeRadioBoxChoices, 0, wxRA_SPECIFY_COLS);

  setProperties();
  doLayout();
}


void SolverPanel::setProperties()
{
  m_posXTextCtrl->SetMinSize(wxSize(50, 22));
  m_posYTextCtrl->SetMinSize(wxSize(50, 22));
  m_posZTextCtrl->SetMinSize(wxSize(50, 22));
  m_dimTextCtrl->SetMinSize(wxSize(50, 22));
  m_resXTextCtrl->SetMinSize(wxSize(50, 22));
  m_resYTextCtrl->SetMinSize(wxSize(50, 22));
  m_resZTextCtrl->SetMinSize(wxSize(50, 22));
  m_timeStepTextCtrl->SetMinSize(wxSize(50, 22));
  m_solverTypeRadioBox->SetSelection(0);
}


void SolverPanel::doLayout()
{
  wxBoxSizer* m_panelSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* m_timeStepSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_resSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_dimSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_posSizer = new wxBoxSizer(wxHORIZONTAL);
  m_posSizer->Add(m_posLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_posSizer->Add(m_posXTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_posSizer->Add(m_posYTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_posSizer->Add(m_posZTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 10);
  m_panelSizer->Add(m_posSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  m_dimSizer->Add(m_dimLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_dimSizer->Add(m_dimTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 8);
  m_panelSizer->Add(m_dimSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  m_resSizer->Add(m_resLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_resSizer->Add(m_resXTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_resSizer->Add(m_resYTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_resSizer->Add(m_resZTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_panelSizer->Add(m_resSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  m_timeStepSizer->Add(m_timeStepLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_timeStepSizer->Add(m_timeStepTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 12);
  m_panelSizer->Add(m_timeStepSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  m_panelSizer->Add(m_solverTypeRadioBox, 0, wxADJUST_MINSIZE, 0);
  SetAutoLayout(true);
  SetSizer(m_panelSizer);
  m_panelSizer->Fit(this);
  m_panelSizer->SetSizeHints(this);
}

void SolverPanel::setCtrlValues(SolverConfig* solverConfig)
{
  m_posXTextCtrl->Clear();
  m_posYTextCtrl->Clear();
  m_posZTextCtrl->Clear();
  m_dimTextCtrl->Clear();
  m_resXTextCtrl->Clear();
  m_resYTextCtrl->Clear();
  m_resZTextCtrl->Clear();
  m_timeStepTextCtrl->Clear();

  (*m_posXTextCtrl) << solverConfig->position.x;
  (*m_posYTextCtrl) << solverConfig->position.y;
  (*m_posZTextCtrl) << solverConfig->position.z;
  (*m_dimTextCtrl) << solverConfig->dim;
  (*m_resXTextCtrl) << solverConfig->resx;
  (*m_resYTextCtrl) << solverConfig->resy;
  (*m_resZTextCtrl) << solverConfig->resz;
  (*m_timeStepTextCtrl) << solverConfig->timeStep;
  m_solverTypeRadioBox->SetSelection(solverConfig->type);
}

bool SolverPanel::getCtrlValues(SolverConfig* solverConfig)
{
  try
    {
      solverConfig->position.x = m_posXTextCtrl->GetSafelyValue();
      solverConfig->position.y = m_posYTextCtrl->GetSafelyValue();
      solverConfig->position.z = m_posZTextCtrl->GetSafelyValue();
      solverConfig->dim = m_dimTextCtrl->GetSafelyValue();
      solverConfig->resx = m_resXTextCtrl->GetSafelyValue();
      solverConfig->resy = m_resYTextCtrl->GetSafelyValue();
      solverConfig->resz = m_resZTextCtrl->GetSafelyValue();
      solverConfig->timeStep = m_timeStepTextCtrl->GetSafelyValue();
    }
  catch(wxString s)
    {
      return false;
    }

  solverConfig->type = m_solverTypeRadioBox->GetSelection();
  
  return true;
}

/**************************************** SolverDialog Class methods **************************************/
BEGIN_EVENT_TABLE(SolverDialog, wxDialog)
  EVT_BUTTON(IDB_Add, SolverDialog::OnClickButtonAdd)
  EVT_BUTTON(IDB_Delete, SolverDialog::OnClickButtonDelete)
  EVT_BUTTON(IDB_OK, SolverDialog::OnOK)
  EVT_BUTTON(IDB_Cancel, SolverDialog::OnCancel)
  EVT_NOTEBOOK_PAGE_CHANGING(IDNB_Solvers, SolverDialog::OnPageChanging)
END_EVENT_TABLE();


SolverDialog::SolverDialog(wxWindow* parent, int id, const wxString& title,  FlameAppConfig *config, 
			   const wxPoint& pos, const wxSize& size, long style):
  wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{
  m_solverNotebook = new wxNotebook(this, IDNB_Solvers, wxDefaultPosition, wxDefaultSize, 0);
  m_currentConfig = config;
  m_nbPanels = m_currentConfig->nbSolvers;
  for(int i = 0; i < m_currentConfig->nbSolvers; i++)
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
  for(int i = 0; i < m_currentConfig->nbSolvers; i++)
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
    /* Réindexage des index des solveurs des flammes en conséquence */
    for(int i = 0; i < m_currentConfig->nbFlames; i++)
      if( m_currentConfig->flames[i].solverIndex > sel)
	m_currentConfig->flames[i].solverIndex--;
  }
}

void SolverDialog::OnOK(wxCommandEvent& event)
{
  delete [] m_currentConfig->solvers;
  
  m_currentConfig->nbSolvers = m_nbPanels;
  
  m_currentConfig->solvers = new SolverConfig[m_currentConfig->nbSolvers];
  
  for(int i = 0; i < m_currentConfig->nbSolvers; i++)
    {
      if(!m_solverPanels[i]->getCtrlValues(&m_currentConfig->solvers[i]))
	return;
    }
  wxDialog::OnOK(event);
}

void SolverDialog::OnPageChanging(wxNotebookEvent& event)
{
  checkSolverUsage(event.GetSelection());
}

void SolverDialog::checkSolverUsage(int solverIndex)
{
  for(int i = 0; i < m_currentConfig->nbFlames; i++)
    {
      if(m_currentConfig->flames[i].solverIndex == solverIndex){
	m_deleteSolverButton->Disable();
	return;
      }	
    }
  m_deleteSolverButton->Enable();
}

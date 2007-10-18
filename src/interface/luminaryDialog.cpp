#include "luminaryDialog.hpp"

/**************************************** LuminaryPanel Class methods **************************************/
BEGIN_EVENT_TABLE(LuminaryPanel, wxPanel)
  EVT_RADIOBOX(IDRS_Type, LuminaryPanel::OnSelectFieldType)
  EVT_BUTTON(IDBF_BrowseWick, LuminaryPanel::OnClickButtonBrowseWick)
END_EVENT_TABLE();


LuminaryPanel::LuminaryPanel(wxWindow* parent, int id, bool luminary, const wxPoint& pos, const wxSize& size, long style):
  wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
  wxString itemName;
  
  m_luminary = luminary;
  if(m_luminary){
    /* Field */
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
    m_resYTextCtrl = new LongTextCtrl(this, -1, 0, 100, _("15"));
    m_resZTextCtrl = new LongTextCtrl(this, -1, 0, 100, _("15"));
    
    /* Flame */
    m_skeletonsNumberLabel = new wxStaticText(this, -1, _("Skeletons number"));
    m_skeletonsNumberCtrl = new LongTextCtrl(this, -1, 0, 100, _("4"));

    const wxString m_luminaryTypeRadioBoxChoices[] = {
      _("Candle"),
      _("Oil Lamp"),
      _("Torch"),
      _("Camp Fire"),
      _("Candles Set"),
      _("Candlestick")
    };
    m_luminaryTypeRadioBox = new wxRadioBox(this, IDRF_Type, _("Flame Type"), wxDefaultPosition, wxDefaultSize, 
					    6, m_luminaryTypeRadioBoxChoices, 0, wxRA_SPECIFY_COLS);
    m_wickLabel = new wxStaticText(this, -1, _("Luminary file"));
    m_wickTextCtrl = new wxTextCtrl(this, -1, _("scenes/bougie.obj"));
    m_wickBrowseButton = new wxButton(this, IDBF_BrowseWick, _("Browse..."));
  }
  /* Field */
  m_resLabel = new wxStaticText(this, -1, _("Resolution"));
  m_resXTextCtrl = new LongTextCtrl(this, -1, 0, 100, _("15"));
  m_timeStepLabel = new wxStaticText(this, -1, _("Time step"));
  m_timeStepTextCtrl = new DoubleTextCtrl(this, -1, 0, 2, _("0.4"));

  const wxString m_solverTypeRadioBoxChoices[] = {
    _("Gauss-Seidel"),
    _("Preconditioned Conjugated Gradient"),
    _("Preconditioned Conjugated Gradient-SSE"),
    _("Hybrid"),
    _("LOD Solver"),
    _("Simple field"),
    _("Fake field"),
    _("LOD field"),
    _("LOD smooth field ")
  };
  m_solverTypeRadioBox = new wxRadioBox(this, IDRS_Type, _("Velocity Field Type"), wxDefaultPosition, wxDefaultSize, 
					9, m_solverTypeRadioBoxChoices, 2, wxRA_SPECIFY_COLS);

  m_omegaDiffLabel = new wxStaticText(this, -1, _("Omega in diffusion"));
  m_omegaDiffTextCtrl = new DoubleTextCtrl(this, -1, 0, 2, _("1.0"));
  m_omegaProjLabel = new wxStaticText(this, -1, _("Omega in projection"));
  m_omegaProjTextCtrl = new DoubleTextCtrl(this, -1, 0, 2, _("1.6"));
  m_epsilonLabel = new wxStaticText(this, -1, _("Error threshold"));
  m_epsilonTextCtrl = new DoubleTextCtrl(this, -1, 0, 0.1, _("0.00001"));
  
  setProperties();
  doLayout();
  
  m_omegaDiffLabel->Disable();
  m_omegaDiffTextCtrl->Disable();
  m_omegaProjLabel->Disable();
  m_omegaProjTextCtrl->Disable();
  m_epsilonLabel->Disable();
  m_epsilonTextCtrl->Disable();
}


void LuminaryPanel::setProperties()
{
  if(m_luminary){
    /* Field */
    m_posXTextCtrl->SetMinSize(wxSize(50, 22));
    m_posYTextCtrl->SetMinSize(wxSize(50, 22));
    m_posZTextCtrl->SetMinSize(wxSize(50, 22));
    m_dimTextCtrl->SetMinSize(wxSize(50, 22));
    m_scaleXTextCtrl->SetMinSize(wxSize(50, 22));
    m_scaleYTextCtrl->SetMinSize(wxSize(50, 22));
    m_scaleZTextCtrl->SetMinSize(wxSize(50, 22));
    m_resYTextCtrl->SetMinSize(wxSize(50, 22));
    m_resZTextCtrl->SetMinSize(wxSize(50, 22));
  
    /* Flame */
    m_wickTextCtrl->SetMinSize(wxSize(120, 22));
    m_luminaryTypeRadioBox->SetSelection(0);
  }
  m_resXTextCtrl->SetMinSize(wxSize(50, 22));
  m_timeStepTextCtrl->SetMinSize(wxSize(60, 22));
  m_solverTypeRadioBox->SetSelection(0);
  m_omegaDiffTextCtrl->SetMinSize(wxSize(70, 22));
  m_omegaProjTextCtrl->SetMinSize(wxSize(70, 22));
  m_epsilonTextCtrl->SetMinSize(wxSize(100, 22));
}


void LuminaryPanel::doLayout()
{
  wxBoxSizer* m_panelSizer = new wxBoxSizer(wxVERTICAL);  
  wxBoxSizer* m_timeStepSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_resSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_omegaDiffSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_omegaProjSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_epsilonSizer = new wxBoxSizer(wxHORIZONTAL);
  
  if(m_luminary){
    wxBoxSizer* m_posSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* m_wickSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* m_skeletonsNumberSizer = new wxBoxSizer(wxHORIZONTAL);
    
    /* Luminaire */
    m_posSizer->Add(m_posLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
    m_posSizer->Add(m_posXTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
    m_posSizer->Add(m_posYTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
    m_posSizer->Add(m_posZTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 10);
    m_panelSizer->Add(m_posSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
    
    m_wickSizer->Add(m_wickLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
    m_wickSizer->Add(m_wickTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 8);
    m_wickSizer->Add(m_wickBrowseButton, 0, wxADJUST_MINSIZE, 0);
    m_panelSizer->Add(m_wickSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
    
    /* Flame */
    m_panelSizer->Add(m_luminaryTypeRadioBox, 0, wxADJUST_MINSIZE, 0);
    m_skeletonsNumberSizer->Add(m_skeletonsNumberLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
    m_skeletonsNumberSizer->Add(m_skeletonsNumberCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 8);
    m_panelSizer->Add(m_skeletonsNumberSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  }
  /* Field */
  m_panelSizer->Add(m_solverTypeRadioBox, 0, wxADJUST_MINSIZE, 0);

  m_resSizer->Add(m_resLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_resSizer->Add(m_resXTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  if(m_luminary){
    m_resSizer->Add(m_resYTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
    m_resSizer->Add(m_resZTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  }
  m_panelSizer->Add(m_resSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  
  if(m_luminary){
    wxBoxSizer* m_dimSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* m_scaleSizer = new wxBoxSizer(wxHORIZONTAL);
    
    m_dimSizer->Add(m_dimLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
    m_dimSizer->Add(m_dimTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 8);
    m_panelSizer->Add(m_dimSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  
    m_scaleSizer->Add(m_scaleLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
    m_scaleSizer->Add(m_scaleXTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
    m_scaleSizer->Add(m_scaleYTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
    m_scaleSizer->Add(m_scaleZTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
    m_panelSizer->Add(m_scaleSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  }

  m_timeStepSizer->Add(m_timeStepLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_timeStepSizer->Add(m_timeStepTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 12);
  m_panelSizer->Add(m_timeStepSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);

  m_omegaDiffSizer->Add(m_omegaDiffLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_omegaDiffSizer->Add(m_omegaDiffTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_panelSizer->Add(m_omegaDiffSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);

  m_omegaProjSizer->Add(m_omegaProjLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_omegaProjSizer->Add(m_omegaProjTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_panelSizer->Add(m_omegaProjSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);

  m_epsilonSizer->Add(m_epsilonLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_epsilonSizer->Add(m_epsilonTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_panelSizer->Add(m_epsilonSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  
  SetAutoLayout(true);
  SetSizer(m_panelSizer);
  m_panelSizer->Fit(this);
  m_panelSizer->SetSizeHints(this);
}


void LuminaryPanel::setCtrlValues(const LuminaryConfig& luminaryConfig)
{  
  wxString tmp;
  
  /* Field */
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

  /* Flame */
  m_wickTextCtrl->Clear();
  m_skeletonsNumberCtrl->Clear();
  
  /* Field */
  (*m_posXTextCtrl) << luminaryConfig.position.x;
  (*m_posYTextCtrl) << luminaryConfig.position.y;
  (*m_posZTextCtrl) << luminaryConfig.position.z;
  (*m_dimTextCtrl) << luminaryConfig.fields[0].dim;
  (*m_scaleXTextCtrl) << luminaryConfig.fields[0].scale.x;
  (*m_scaleYTextCtrl) << luminaryConfig.fields[0].scale.y;
  (*m_scaleZTextCtrl) << luminaryConfig.fields[0].scale.z;
  (*m_resYTextCtrl) << (int)luminaryConfig.fields[0].resy;
  (*m_resZTextCtrl) << (int)luminaryConfig.fields[0].resz;
    
  (*m_resXTextCtrl) << (int)luminaryConfig.fields[0].resx;
  tmp.Printf(_("%.4lf"), luminaryConfig.fields[0].timeStep);
  (*m_timeStepTextCtrl) << tmp;
  m_solverTypeRadioBox->SetSelection(luminaryConfig.fields[0].type);
  tmp.Printf(_("%.5lf"), luminaryConfig.fields[0].omegaDiff);
  (*m_omegaDiffTextCtrl) << tmp;
  tmp.Printf(_("%.5lf"), luminaryConfig.fields[0].omegaProj);
  (*m_omegaProjTextCtrl) << tmp;
  tmp.Printf(_("%.10lf"), luminaryConfig.fields[0].epsilon);
  (*m_epsilonTextCtrl) << tmp;

  if(luminaryConfig.fields[0].type == GS_SOLVER || luminaryConfig.fields[0].type == FAKE_FIELD)
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

  /* Flame */
  (*m_wickTextCtrl) << luminaryConfig.fileName;
  
  m_luminaryTypeRadioBox->SetSelection(luminaryConfig.fires[0].type);

  (*m_skeletonsNumberCtrl) << (int)luminaryConfig.fires[0].skeletonsNumber;
}

void LuminaryPanel::setCtrlValues(const SolverConfig& solverConfig)
{  
  wxString tmp;
  
  m_resXTextCtrl->Clear();
  m_timeStepTextCtrl->Clear();
  m_omegaDiffTextCtrl->Clear();
  m_omegaProjTextCtrl->Clear();
  m_epsilonTextCtrl->Clear();
  
  (*m_resXTextCtrl) << (int)solverConfig.resx;
  tmp.Printf(_("%.4lf"), solverConfig.timeStep);
  (*m_timeStepTextCtrl) << tmp;
  m_solverTypeRadioBox->SetSelection(solverConfig.type);
  tmp.Printf(_("%.5lf"), solverConfig.omegaDiff);
  (*m_omegaDiffTextCtrl) << tmp;
  tmp.Printf(_("%.5lf"), solverConfig.omegaProj);
  (*m_omegaProjTextCtrl) << tmp;
  tmp.Printf(_("%.10lf"), solverConfig.epsilon);
  (*m_epsilonTextCtrl) << tmp;

  if(solverConfig.type == GS_SOLVER || solverConfig.type == FAKE_FIELD)
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
}


bool LuminaryPanel::getCtrlValues(LuminaryConfig& luminaryConfig)
{
  try
    {
      /* Field */
      luminaryConfig.position.x = m_posXTextCtrl->GetSafelyValue();
      luminaryConfig.position.y = m_posYTextCtrl->GetSafelyValue();
      luminaryConfig.position.z = m_posZTextCtrl->GetSafelyValue();
      luminaryConfig.fields[0].dim = m_dimTextCtrl->GetSafelyValue();
      luminaryConfig.fields[0].scale.x = m_scaleXTextCtrl->GetSafelyValue();
      luminaryConfig.fields[0].scale.y = m_scaleYTextCtrl->GetSafelyValue();
      luminaryConfig.fields[0].scale.z = m_scaleZTextCtrl->GetSafelyValue();
      luminaryConfig.fields[0].resx = m_resXTextCtrl->GetSafelyValue();
      luminaryConfig.fields[0].resy = m_resYTextCtrl->GetSafelyValue();
      luminaryConfig.fields[0].resz = m_resZTextCtrl->GetSafelyValue();
      luminaryConfig.fields[0].timeStep = m_timeStepTextCtrl->GetSafelyValue();
      luminaryConfig.fields[0].omegaDiff = m_omegaDiffTextCtrl->GetSafelyValue();
      luminaryConfig.fields[0].omegaProj = m_omegaProjTextCtrl->GetSafelyValue();
      luminaryConfig.fields[0].epsilon = m_epsilonTextCtrl->GetSafelyValue();
      /* Flame */
      luminaryConfig.fires[0].skeletonsNumber = m_skeletonsNumberCtrl->GetSafelyValue();
    }
  catch(wxString s)
    {
      return false;
    }
  if(!m_wickTextCtrl->GetValue().IsEmpty())
    luminaryConfig.fileName = m_wickTextCtrl->GetValue();
  luminaryConfig.fires[0].type = m_luminaryTypeRadioBox->GetSelection();
  
  luminaryConfig.fields[0].type = m_solverTypeRadioBox->GetSelection();
  
  return true;
}

bool LuminaryPanel::getCtrlValues(SolverConfig& solverConfig)
{
  try
    {
      solverConfig.type = m_solverTypeRadioBox->GetSelection();
      solverConfig.resx = m_resXTextCtrl->GetSafelyValue();
      solverConfig.timeStep = m_timeStepTextCtrl->GetSafelyValue();
      solverConfig.omegaDiff = m_omegaDiffTextCtrl->GetSafelyValue();
      solverConfig.omegaProj = m_omegaProjTextCtrl->GetSafelyValue();
      solverConfig.epsilon = m_epsilonTextCtrl->GetSafelyValue();
    }
  catch(wxString s)
    {
      return false;
    }
  return true;
}

void LuminaryPanel::OnSelectFieldType(wxCommandEvent& event)
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
}

void LuminaryPanel::OnClickButtonBrowseWick(wxCommandEvent& event)
{
  wxString filename;
  wxString pwd=wxGetCwd();
  pwd << _("/scenes");
  
  wxFileDialog fileDialog(this, _("Choose a OBJ file for the wick"), pwd, _(""), _("*.obj"), wxOPEN|wxFILE_MUST_EXIST);
  if(fileDialog.ShowModal() == wxID_OK){
    filename = fileDialog.GetPath();
    
    /* On récupère le chemin absolu vers la scène */
    filename.Replace(wxGetCwd(),_(""),false);
    filename=filename.Mid(1);
    
    if(!filename.IsEmpty())
      m_wickTextCtrl->SetValue(filename);    
  }
}

/**************************************** LuminaryDialog Class methods **************************************/
BEGIN_EVENT_TABLE(LuminaryDialog, wxDialog)
  EVT_BUTTON(IDBF_Add, LuminaryDialog::OnClickButtonAdd)
  EVT_BUTTON(IDBF_Delete, LuminaryDialog::OnClickButtonDelete)
  EVT_BUTTON(IDBF_OK, LuminaryDialog::OnOK)
  EVT_BUTTON(IDBF_Cancel, LuminaryDialog::OnCancel)
  EVT_CHECKBOX(IDCHK_GS, LuminaryDialog::OnCheckActivateGlobalSolver)
END_EVENT_TABLE();


LuminaryDialog::LuminaryDialog(wxWindow* parent, int id, const wxString& title, FlameAppConfig* const config, 
			 const wxPoint& pos, const wxSize& size, long style):
  wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{
  m_luminaryNotebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
  m_currentConfig = config;
  m_nbPanels = m_currentConfig->nbLuminaries;

  /* Solveur global */
  m_activateGlobalSolverCheckBox = new wxCheckBox(this,IDCHK_GS,_("Activate Global Field"));
  m_activateGlobalSolverCheckBox->SetValue(m_currentConfig->useGlobalField);
  
  m_globalSolverPanel = new LuminaryPanel(m_luminaryNotebook, -1, false);
  m_globalSolverPanel->setCtrlValues(m_currentConfig->globalField);
  
  /* Luminaires */
  for(uint i = 0; i < m_currentConfig->nbLuminaries; i++)
    {
      m_luminaryPanels[i] = new LuminaryPanel(m_luminaryNotebook, -1, true);
      m_luminaryPanels[i]->setCtrlValues(m_currentConfig->luminaries[i]);
    }
  m_addLuminaryButton = new wxButton(this, IDBF_Add, _("Add a luminary"));
  m_deleteLuminaryButton = new wxButton(this, IDBF_Delete, _("Delete a luminary"));
  m_okButton = new wxButton(this, IDBF_OK, _("OK"));
  m_cancelButton = new wxButton(this, IDBF_Cancel, _("Cancel"));

  doLayout();
}

void LuminaryDialog::doLayout()
{
  m_sizer = new wxBoxSizer(wxVERTICAL);
  
  m_sizer->Add(m_activateGlobalSolverCheckBox, 0, 0, 0);
  m_luminaryNotebook->AddPage(m_globalSolverPanel, _("Global Solver"));
  if(!m_currentConfig->useGlobalField)
    m_globalSolverPanel->Disable();
  
  for(uint i = 0; i < m_currentConfig->nbLuminaries; i++)
    {
      wxString tabName(_("Luminary #")); tabName << i+1;
      m_luminaryNotebook->AddPage(m_luminaryPanels[i], tabName);
    }
  m_sizer->Add(m_luminaryNotebook, 1, wxEXPAND, 0);
  wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
  buttonSizer->Add(m_addLuminaryButton, 0, wxADJUST_MINSIZE, 0);
  buttonSizer->Add(m_deleteLuminaryButton, 0, wxADJUST_MINSIZE, 0);
  buttonSizer->Add(m_okButton, 0, wxADJUST_MINSIZE|wxLEFT, 20);
  buttonSizer->Add(m_cancelButton, 0, wxADJUST_MINSIZE, 0);
  m_sizer->Add(buttonSizer, 0, wxEXPAND, 0);
  SetAutoLayout(true);
  SetSizerAndFit(m_sizer);
}

void LuminaryDialog::OnClickButtonAdd(wxCommandEvent& event)
{
  if(m_nbPanels < NB_MAXFLAMMES){
    m_luminaryPanels[m_nbPanels] = new LuminaryPanel(m_luminaryNotebook, -1, m_currentConfig->nbLuminaries);
    m_nbPanels++;
    wxString tabName(_("Luminary #")); tabName << m_nbPanels;
    m_luminaryNotebook->AddPage(m_luminaryPanels[m_nbPanels-1], tabName);
    m_luminaryNotebook->SetSelection(m_nbPanels);
  }
}

void LuminaryDialog::OnClickButtonDelete(wxCommandEvent& event)
{
  int sel = m_luminaryNotebook->GetSelection();
  if(sel != -1){
    m_luminaryNotebook->DeletePage(sel);
    m_nbPanels--;
    for(int i=sel; i < m_nbPanels; i++)
      m_luminaryPanels[i] = m_luminaryPanels[i+1];
  }
}

void LuminaryDialog::OnOK(wxCommandEvent& event)
{
  LuminaryConfig *newConfig;
  SolverConfig newGlobalConfig;
  uint newNb;
    
  newNb = m_nbPanels;
  newConfig = new LuminaryConfig[newNb];
  for(uint i = 0; i < newNb; i++){
    newConfig[i].fields = new SolverConfig[1];
    newConfig[i].fires = new FlameConfig[1];
  }
  
  if(!m_globalSolverPanel->getCtrlValues(newGlobalConfig))
    return;
  else
    {
      m_currentConfig->globalField = newGlobalConfig;
      m_currentConfig->useGlobalField = m_activateGlobalSolverCheckBox->GetValue();
      m_currentConfig->globalField.buoyancy=0; 
      m_currentConfig->globalField.vorticityConfinement=.1; 
    }
  
  for(uint i = 0; i < newNb; i++)
    {
      if( m_luminaryPanels[i]->getCtrlValues(newConfig[i]) ){
	if( newConfig[i].fires[0].type != CANDLE && newConfig[i].fires[0].type != CANDLESTICK ){
	  if( newConfig[i].fileName.IsEmpty() ){
	    wxMessageDialog errorDialog (this,_("You must provide a filename for the wick"),_("Error"),wxOK|wxICON_ERROR);
	    errorDialog.ShowModal();
	    delete [] newConfig;
	    return;
	  }
	}
	/* On recopie les anciens paramètres si ils existent */
	if( m_currentConfig->nbLuminaries > i )
	  {
	    newConfig[i].fires[0].innerForce = m_currentConfig->luminaries[i].fires[0].innerForce;
	    newConfig[i].fires[0].leadLifeSpan = m_currentConfig->luminaries[i].fires[0].leadLifeSpan;
	    newConfig[i].fires[0].periLifeSpan = m_currentConfig->luminaries[i].fires[0].periLifeSpan;
	    newConfig[i].fires[0].lod = m_currentConfig->luminaries[i].fires[0].lod;
	    newConfig[i].fires[0].flickering = m_currentConfig->luminaries[i].fires[0].flickering;
	    newConfig[i].fires[0].fdf = m_currentConfig->luminaries[i].fires[0].fdf;
	    newConfig[i].fires[0].IESFileName = m_currentConfig->luminaries[i].fires[0].IESFileName;
	    newConfig[i].fields[0].buoyancy = m_currentConfig->luminaries[i].fields[0].buoyancy;
	    newConfig[i].fields[0].vorticityConfinement = m_currentConfig->luminaries[i].fields[0].vorticityConfinement;
	  }
	else
	  {
	    newConfig[i].fires[0].innerForce = 0.005;
	    newConfig[i].fires[0].leadLifeSpan = 6;
	    newConfig[i].fires[0].periLifeSpan = 4;
	    newConfig[i].fires[0].lod = 100;
	    newConfig[i].fires[0].flickering = 0;
	    newConfig[i].fires[0].fdf = 0;
	    newConfig[i].fires[0].IESFileName = _("IES/test.ies");
	    newConfig[i].fields[0].buoyancy = .2;
	    newConfig[i].fields[0].vorticityConfinement = .1;
	  }
      }else{
	for(uint i = 0; i < newNb; i++){
	  delete [] newConfig[i].fields;
	  delete [] newConfig[i].fires;
	}
	delete [] newConfig;
	return;
      }
    }
  for(int unsigned i=0; i < m_currentConfig->nbLuminaries; i++)
    {
      delete [] m_currentConfig->luminaries[i].fires;
      delete [] m_currentConfig->luminaries[i].fields;
    }
  delete [] m_currentConfig->luminaries;
  m_currentConfig->nbLuminaries = newNb;
  m_currentConfig->luminaries = newConfig;
  EndModal(wxID_OK);
}

void LuminaryDialog::OnCancel(wxCommandEvent& event)
{
  EndModal(wxID_CANCEL);
}

void LuminaryDialog::OnCheckActivateGlobalSolver(wxCommandEvent& event)
{
  if(event.IsChecked())
    m_globalSolverPanel->Enable();
  else
    m_globalSolverPanel->Disable();
}

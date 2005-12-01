#include "flameDialog.hpp"

/**************************************** FlamePanel Class methods **************************************/
BEGIN_EVENT_TABLE(FlamePanel, wxPanel)
  EVT_RADIOBOX(IDRF_Type, FlamePanel::OnSelectType)
  EVT_BUTTON(IDBF_BrowseWick, FlamePanel::OnClickButtonBrowseWick)
END_EVENT_TABLE();

FlamePanel::FlamePanel(wxWindow* parent, int id, int nbSolvers, const wxPoint& pos, const wxSize& size, long style):
  wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
  wxString itemName;
  
  m_posLabel = new wxStaticText(this, -1, _("Position"));
  m_posXTextCtrl = new wxTextCtrl(this, -1, _("0"));
  m_posYTextCtrl = new wxTextCtrl(this, -1, _("0"));
  m_posZTextCtrl = new wxTextCtrl(this, -1, _("0"));

  m_solverLabel = new wxStaticText(this,-1,_("Use solver :"));
  m_solverComboBox = new wxComboBox(this,-1,_(""),wxDefaultPosition,wxDefaultSize,0,wxCB_READONLY);
  
  for(int i=0; i < nbSolvers; i++)
    {
      itemName.Printf(_("Solver #%d"),i+1);
      m_solverComboBox->Append(itemName);
    }   

  const wxString m_flameTypeRadioBoxChoices[] = {
    _("Candle"),
    _("Oil Lamp")
  };
  m_flameTypeRadioBox = new wxRadioBox(this, IDRF_Type, _("Type"), wxDefaultPosition, wxDefaultSize, 
					2, m_flameTypeRadioBoxChoices, 0, wxRA_SPECIFY_COLS);
  m_wickLabel = new wxStaticText(this, -1, _("Wick"));
  m_wickTextCtrl = new wxTextCtrl(this, -1, _("meche2.obj"));
  m_wickBrowseButton = new wxButton(this, IDBF_BrowseWick, _("Browse..."));

  setProperties();
  doLayout();
}


void FlamePanel::setProperties()
{
  m_posXTextCtrl->SetMinSize(wxSize(50, 22));
  m_posYTextCtrl->SetMinSize(wxSize(50, 22));
  m_posZTextCtrl->SetMinSize(wxSize(50, 22));
  m_wickTextCtrl->SetMinSize(wxSize(120, 22));
  m_flameTypeRadioBox->SetSelection(0);
}


void FlamePanel::doLayout()
{
  wxBoxSizer* m_panelSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* m_solverSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_wickSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* m_posSizer = new wxBoxSizer(wxHORIZONTAL);
  m_posSizer->Add(m_posLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_posSizer->Add(m_posXTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_posSizer->Add(m_posYTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  m_posSizer->Add(m_posZTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 10);
  m_panelSizer->Add(m_posSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  m_wickSizer->Add(m_wickLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_wickSizer->Add(m_wickTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 8);
  m_wickSizer->Add(m_wickBrowseButton, 0, wxADJUST_MINSIZE, 0);
  m_panelSizer->Add(m_wickSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);  
  m_solverSizer->Add(m_solverLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_solverSizer->Add(m_solverComboBox, 0, wxLEFT|wxADJUST_MINSIZE, 15);
  m_panelSizer->Add(m_solverSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  m_panelSizer->Add(m_flameTypeRadioBox, 0, wxADJUST_MINSIZE, 0);
  SetAutoLayout(true);
  SetSizer(m_panelSizer);
  m_panelSizer->Fit(this);
  m_panelSizer->SetSizeHints(this);
}

void FlamePanel::setCtrlValues(FlameConfig* flameConfig)
{  
  m_posXTextCtrl->Clear();
  m_posYTextCtrl->Clear();
  m_posZTextCtrl->Clear();
  m_wickTextCtrl->Clear();
  
  (*m_posXTextCtrl) << flameConfig->position.x;
  (*m_posYTextCtrl) << flameConfig->position.y;
  (*m_posZTextCtrl) << flameConfig->position.z;
  (*m_wickTextCtrl) << flameConfig->wickName;

  m_solverComboBox->SetSelection(flameConfig->solverIndex);
  m_flameTypeRadioBox->SetSelection(flameConfig->type);
  if(flameConfig->type != FIRMALAMPE){
    m_wickLabel->Disable();
    m_wickTextCtrl->Disable();
    m_wickBrowseButton->Disable();
  }
}

void FlamePanel::getCtrlValues(FlameConfig* flameConfig)
{
  flameConfig->position.x = getDoubleTextCtrlValue(m_posXTextCtrl);
  flameConfig->position.y = getDoubleTextCtrlValue(m_posYTextCtrl);
  flameConfig->position.z = getDoubleTextCtrlValue(m_posZTextCtrl);
  flameConfig->wickName = m_wickTextCtrl->GetValue();

  flameConfig->solverIndex = m_solverComboBox->GetSelection();
  flameConfig->type = m_flameTypeRadioBox->GetSelection();
}

void FlamePanel::OnSelectType(wxCommandEvent& event)
{
  if(event.GetSelection() == FIRMALAMPE)
    {
      m_wickLabel->Enable();
      m_wickTextCtrl->Enable();    
      m_wickBrowseButton->Enable();
    } 
  else 
    {
      m_wickLabel->Disable();
      m_wickTextCtrl->Disable();
      m_wickBrowseButton->Disable();
    }
}

void FlamePanel::OnClickButtonBrowseWick(wxCommandEvent& event)
{
  wxString filename;
  
  wxFileDialog fileDialog(this, _("Choose a OBJ file for the wick"), _("scenes"), _(""), _("*.obj"), wxOPEN|wxFILE_MUST_EXIST);
  if(fileDialog.ShowModal() == wxID_OK){
    filename = fileDialog.GetFilename();
    
    if(!filename.IsEmpty())
      m_wickTextCtrl->SetValue(filename);    
  }
}

/**************************************** FlameDialog Class methods **************************************/
BEGIN_EVENT_TABLE(FlameDialog, wxDialog)
  EVT_BUTTON(IDBF_Add, FlameDialog::OnClickButtonAdd)
  EVT_BUTTON(IDBF_Delete, FlameDialog::OnClickButtonDelete)
  EVT_BUTTON(IDBF_OK, FlameDialog::OnOK)
  EVT_BUTTON(IDBF_Cancel, FlameDialog::OnCancel)
END_EVENT_TABLE();


FlameDialog::FlameDialog(wxWindow* parent, int id, const wxString& title,  FlameAppConfig *config, 
			   const wxPoint& pos, const wxSize& size, long style):
  wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{
  m_flameNotebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
  m_currentConfig = config;
  m_nbPanels = m_currentConfig->nbFlames;
  for(int i = 0; i < m_currentConfig->nbFlames; i++)
    {
      m_flamePanels[i] = new FlamePanel(m_flameNotebook, -1, m_currentConfig->nbSolvers);
      m_flamePanels[i]->setCtrlValues(&m_currentConfig->flames[i]);
    }
  //     m_nbMaxPanels = 5;
  m_addFlameButton = new wxButton(this, IDBF_Add, _("Add a flame"));
  m_deleteFlameButton = new wxButton(this, IDBF_Delete, _("Delete a flame"));
  m_okButton = new wxButton(this, IDBF_OK, _("OK"));
  m_cancelButton = new wxButton(this, IDBF_Cancel, _("Cancel"));
  doLayout();
}

void FlameDialog::doLayout()
{
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  for(int i = 0; i < m_currentConfig->nbFlames; i++)
    {
      wxString tabName(_("Flame #")); tabName << i+1;
      m_flameNotebook->AddPage(m_flamePanels[i], tabName);
    }
  sizer->Add(m_flameNotebook, 1, wxEXPAND, 0);
  wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
  buttonSizer->Add(m_addFlameButton, 0, wxADJUST_MINSIZE, 0);
  buttonSizer->Add(m_deleteFlameButton, 0, wxADJUST_MINSIZE, 0);
  buttonSizer->Add(m_okButton, 0, wxADJUST_MINSIZE|wxLEFT, 20);
  buttonSizer->Add(m_cancelButton, 0, wxADJUST_MINSIZE, 0);
  sizer->Add(buttonSizer, 0, wxEXPAND, 0);
  SetAutoLayout(true);
  SetSizer(sizer);
  sizer->Fit(this);
  sizer->SetSizeHints(this);
  Layout();
}

void FlameDialog::OnClickButtonAdd(wxCommandEvent& event)
{
  if(m_nbPanels < m_nbMaxPanels){
    m_flamePanels[m_nbPanels] = new FlamePanel(m_flameNotebook, -1, m_currentConfig->nbSolvers);
    m_nbPanels++;
    wxString tabName(_("Flame #")); tabName << m_nbPanels;
    m_flameNotebook->AddPage(m_flamePanels[m_nbPanels-1], tabName);
    m_flameNotebook->SetSelection(m_nbPanels-1);
  }
}

void FlameDialog::OnClickButtonDelete(wxCommandEvent& event)
{
  int sel = m_flameNotebook->GetSelection();
  if(sel != -1){
    m_flameNotebook->DeletePage(sel);
    m_nbPanels--;
    for(int i=sel; i < m_nbPanels; i++)
      m_flamePanels[i] = m_flamePanels[i+1];
  }
}

void FlameDialog::OnOK(wxCommandEvent& event)
{
  delete [] m_currentConfig->flames;
  
  m_currentConfig->nbFlames = m_nbPanels;
  
  m_currentConfig->flames = new FlameConfig[m_currentConfig->nbFlames];
  
  for(int i = 0; i < m_currentConfig->nbFlames; i++)
    {
      m_flamePanels[i]->getCtrlValues(&m_currentConfig->flames[i]);
    }
  wxDialog::OnOK(event);
}

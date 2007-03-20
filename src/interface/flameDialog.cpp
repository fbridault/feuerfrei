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
  m_posXTextCtrl = new DoubleTextCtrl(this, -1, -100, 100, _("0,5"));
  m_posYTextCtrl = new DoubleTextCtrl(this, -1, -100, 100, _("0"));
  m_posZTextCtrl = new DoubleTextCtrl(this, -1, -100, 100, _("0,5"));
  
  m_skeletonsNumberLabel = new wxStaticText(this, -1, _("Skeletons number"));
  m_skeletonsNumberCtrl = new LongTextCtrl(this, -1, 0, 100, _("4"));
  
  m_solverLabel = new wxStaticText(this,-1,_("Use solver :"));
  m_solverComboBox = new wxComboBox(this,-1,_(""),wxDefaultPosition,wxDefaultSize,0,wxCB_READONLY);
  
  for(int i=0; i < nbSolvers; i++)
    {
      itemName.Printf(_("Solver #%d"),i+1);
      m_solverComboBox->Append(itemName);
    }
  
  const wxString m_flameTypeRadioBoxChoices[] = {
    _("Candle"),
    _("Oil Lamp"),
    _("Torch"),
    _("Camp Fire"),
    _("Candlestick")
  };
  m_flameTypeRadioBox = new wxRadioBox(this, IDRF_Type, _("Type"), wxDefaultPosition, wxDefaultSize, 
					5, m_flameTypeRadioBoxChoices, 0, wxRA_SPECIFY_COLS);
  m_wickLabel = new wxStaticText(this, -1, _("Wick"));
  m_wickTextCtrl = new wxTextCtrl(this, -1, _("meche2.obj"));
  m_wickBrowseButton = new wxButton(this, IDBF_BrowseWick, _("Browse..."));
  
  setProperties();
  doLayout();
  m_wickLabel->Disable();
  m_wickTextCtrl->Disable();
  m_wickBrowseButton->Disable();
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
  wxBoxSizer* m_skeletonsNumberSizer = new wxBoxSizer(wxHORIZONTAL);
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
  m_skeletonsNumberSizer->Add(m_skeletonsNumberLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_skeletonsNumberSizer->Add(m_skeletonsNumberCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 8);
  m_panelSizer->Add(m_skeletonsNumberSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);  
  m_solverSizer->Add(m_solverLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  m_solverSizer->Add(m_solverComboBox, 0, wxLEFT|wxADJUST_MINSIZE, 15);
  m_panelSizer->Add(m_solverSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  m_panelSizer->Add(m_flameTypeRadioBox, 0, wxADJUST_MINSIZE, 0);
  SetAutoLayout(true);
  SetSizer(m_panelSizer);
  m_panelSizer->Fit(this);
  m_panelSizer->SetSizeHints(this);
}


void FlamePanel::setCtrlValues(const FlameConfig* const flameConfig)
{  
  m_posXTextCtrl->Clear();
  m_posYTextCtrl->Clear();
  m_posZTextCtrl->Clear();
  m_wickTextCtrl->Clear();
  m_skeletonsNumberCtrl->Clear();
  
  (*m_posXTextCtrl) << flameConfig->position.x;
  (*m_posYTextCtrl) << flameConfig->position.y;
  (*m_posZTextCtrl) << flameConfig->position.z;
  (*m_wickTextCtrl) << flameConfig->wickName;

  m_solverComboBox->SetSelection(flameConfig->solverIndex);
  m_flameTypeRadioBox->SetSelection(flameConfig->type);

  (*m_skeletonsNumberCtrl) << (int)flameConfig->skeletonsNumber;
  
  if(flameConfig->type == CANDLE || flameConfig->type == CANDLESTICK){
    m_wickLabel->Disable();
    m_wickTextCtrl->Disable();
    m_wickBrowseButton->Disable();
  }else{
    m_wickLabel->Enable();
    m_wickTextCtrl->Enable();    
    m_wickBrowseButton->Enable();
  }
}


bool FlamePanel::getCtrlValues(FlameConfig* const flameConfig)
{
  try
    {
      flameConfig->position.x = m_posXTextCtrl->GetSafelyValue();
      flameConfig->position.y = m_posYTextCtrl->GetSafelyValue();
      flameConfig->position.z = m_posZTextCtrl->GetSafelyValue();
      flameConfig->skeletonsNumber = m_skeletonsNumberCtrl->GetSafelyValue();
    }
  catch(wxString s)
    {
      return false;
    }
  if(!m_wickTextCtrl->GetValue().IsEmpty())
    flameConfig->wickName = m_wickTextCtrl->GetValue();
  
  flameConfig->solverIndex = m_solverComboBox->GetSelection();
  flameConfig->type = m_flameTypeRadioBox->GetSelection();
  return true;
}


void FlamePanel::OnSelectType(wxCommandEvent& event)
{
  if(event.GetSelection() != CANDLE && event.GetSelection() != CANDLESTICK)
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

/**************************************** FlameDialog Class methods **************************************/
BEGIN_EVENT_TABLE(FlameDialog, wxDialog)
  EVT_BUTTON(IDBF_Add, FlameDialog::OnClickButtonAdd)
  EVT_BUTTON(IDBF_Delete, FlameDialog::OnClickButtonDelete)
  EVT_BUTTON(IDBF_OK, FlameDialog::OnOK)
  EVT_BUTTON(IDBF_Cancel, FlameDialog::OnCancel)
END_EVENT_TABLE();


FlameDialog::FlameDialog(wxWindow* parent, int id, const wxString& title,  FlameAppConfig* const config, 
			 const wxPoint& pos, const wxSize& size, long style):
  wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{
  m_flameNotebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
  m_currentConfig = config;
  m_nbPanels = m_currentConfig->nbFlames;
  for(uint i = 0; i < m_currentConfig->nbFlames; i++)
    {
      m_flamePanels[i] = new FlamePanel(m_flameNotebook, -1, m_currentConfig->nbSolvers);
      m_flamePanels[i]->setCtrlValues(&m_currentConfig->flames[i]);
    }
  m_addFlameButton = new wxButton(this, IDBF_Add, _("Add a flame"));
  m_deleteFlameButton = new wxButton(this, IDBF_Delete, _("Delete a flame"));
  m_okButton = new wxButton(this, IDBF_OK, _("OK"));
  m_cancelButton = new wxButton(this, IDBF_Cancel, _("Cancel"));
  doLayout();
}

void FlameDialog::doLayout()
{
  m_sizer = new wxBoxSizer(wxVERTICAL);
  for(uint i = 0; i < m_currentConfig->nbFlames; i++)
    {
      wxString tabName(_("Flame #")); tabName << i+1;
      m_flameNotebook->AddPage(m_flamePanels[i], tabName);
    }
  m_sizer->Add(m_flameNotebook, 1, wxEXPAND, 0);
  wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
  buttonSizer->Add(m_addFlameButton, 0, wxADJUST_MINSIZE, 0);
  buttonSizer->Add(m_deleteFlameButton, 0, wxADJUST_MINSIZE, 0);
  buttonSizer->Add(m_okButton, 0, wxADJUST_MINSIZE|wxLEFT, 20);
  buttonSizer->Add(m_cancelButton, 0, wxADJUST_MINSIZE, 0);
  m_sizer->Add(buttonSizer, 0, wxEXPAND, 0);
  SetAutoLayout(true);
  SetSizerAndFit(m_sizer);
}

void FlameDialog::OnClickButtonAdd(wxCommandEvent& event)
{
  if(m_nbPanels < NB_MAXFLAMMES){
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
  FlameConfig *newConfig;
  uint newNb;
    
  newNb = m_nbPanels;
  newConfig = new FlameConfig[newNb];
  
  for(uint i = 0; i < newNb; i++)
    {
      if( m_flamePanels[i]->getCtrlValues(&newConfig[i]) ){
	if( newConfig[i].type != CANDLE && newConfig[i].type != CANDLESTICK ){
	  if( newConfig[i].wickName.IsEmpty() ){
	    wxMessageDialog errorDialog (this,_("You must provide a filename for the wick"),_("Error"),wxOK|wxICON_ERROR);
	    errorDialog.ShowModal();
	    delete [] newConfig;
	    return;
	  }
	}
	/* On recopie les anciens paramètres si ils existent */
	if( m_currentConfig->nbFlames > i )
	  {
	    newConfig[i].innerForce = m_currentConfig->flames[i].innerForce;
	    newConfig[i].leadLifeSpan = m_currentConfig->flames[i].leadLifeSpan;
	    newConfig[i].periLifeSpan = m_currentConfig->flames[i].periLifeSpan;
	    newConfig[i].samplingTolerance = m_currentConfig->flames[i].samplingTolerance;
	    newConfig[i].flickering = m_currentConfig->flames[i].flickering;
	    newConfig[i].fdf = m_currentConfig->flames[i].fdf;
	    newConfig[i].IESFileName = m_currentConfig->flames[i].IESFileName;	      
	  }
	else
	  {
	    newConfig[i].innerForce = 0.005;
	    newConfig[i].leadLifeSpan = 6;
	    newConfig[i].periLifeSpan = 4;
	    newConfig[i].samplingTolerance = 100;
	    newConfig[i].flickering = 0;
	    newConfig[i].fdf = 0;
	    newConfig[i].IESFileName = _("IES/test.ies");
	  }
      }else{
	delete [] newConfig;
	return;
      }
    }
  delete [] m_currentConfig->flames;
  m_currentConfig->nbFlames = newNb;
  m_currentConfig->flames = newConfig;
  EndModal(wxID_OK);
}

void FlameDialog::OnCancel(wxCommandEvent& event)
{
  EndModal(wxID_CANCEL);
}

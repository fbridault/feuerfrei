#include "shadowsDialog.hpp"

BEGIN_EVENT_TABLE(ShadowsDialog, wxDialog)
  EVT_TEXT_ENTER(IDT_FATX, ShadowsDialog::OnFatnessEnter)
  EVT_TEXT_ENTER(IDT_FATY, ShadowsDialog::OnFatnessEnter)
  EVT_TEXT_ENTER(IDT_FATZ, ShadowsDialog::OnFatnessEnter)
  EVT_TEXT_ENTER(IDT_SEDX, ShadowsDialog::OnShadowsExtrudeDistEnter)
  EVT_TEXT_ENTER(IDT_SEDY, ShadowsDialog::OnShadowsExtrudeDistEnter)
  EVT_TEXT_ENTER(IDT_SEDZ, ShadowsDialog::OnShadowsExtrudeDistEnter)
END_EVENT_TABLE();

/**************************************** ShadowsDialog Class methods **************************************/

ShadowsDialog::ShadowsDialog(wxWindow* parent, int id, const wxString& title, FlameAppConfig* const config, 
			     GLFlameCanvas* const glBuffer, const wxPoint& pos, const wxSize& size, long style):
  wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{
  wxString tmp;
  
  m_currentConfig = config;
  
  m_fatnessLabel = new wxStaticText(this, -1, _("Fatness"));
  m_fatnessXTextCtrl = new DoubleTextCtrl(this, IDT_FATX, -1, 1, _(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  m_fatnessYTextCtrl = new DoubleTextCtrl(this, IDT_FATY, -1, 1, _(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  m_fatnessZTextCtrl = new DoubleTextCtrl(this, IDT_FATZ, -1, 1, _(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  m_shadowExtrudeDistLabel = new wxStaticText(this, -1, _("shadowExtrudeDist"));
  m_shadowExtrudeDistXTextCtrl = new DoubleTextCtrl(this, IDT_SEDX, 0, 100, _(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  m_shadowExtrudeDistYTextCtrl = new DoubleTextCtrl(this, IDT_SEDY, 0, 100, _(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  m_shadowExtrudeDistZTextCtrl = new DoubleTextCtrl(this, IDT_SEDZ, 0, 100, _(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

  tmp.Printf(_("%.5f"), m_currentConfig->fatness[0]);
  (*m_fatnessXTextCtrl) << tmp;
  tmp.Printf(_("%.5f"), m_currentConfig->fatness[1]);
  (*m_fatnessYTextCtrl) << tmp;
  tmp.Printf(_("%.5f"),  m_currentConfig->fatness[2]);
  (*m_fatnessZTextCtrl) << tmp;

  (*m_shadowExtrudeDistXTextCtrl) << m_currentConfig->extrudeDist[0];
  (*m_shadowExtrudeDistYTextCtrl) << m_currentConfig->extrudeDist[1];
  (*m_shadowExtrudeDistZTextCtrl) << m_currentConfig->extrudeDist[2];

  m_glBuffer = glBuffer;
    
  doLayout();
}

void ShadowsDialog::doLayout()
{
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* fatnessSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* shadowExtrudeDistSizer = new wxBoxSizer(wxHORIZONTAL);
  
  fatnessSizer->Add(m_fatnessLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  fatnessSizer->Add(m_fatnessXTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  fatnessSizer->Add(m_fatnessYTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  fatnessSizer->Add(m_fatnessZTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 10);
  sizer->Add(fatnessSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  
  shadowExtrudeDistSizer->Add(m_shadowExtrudeDistLabel, 0, wxLEFT|wxTOP|wxADJUST_MINSIZE, 3);
  shadowExtrudeDistSizer->Add(m_shadowExtrudeDistXTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  shadowExtrudeDistSizer->Add(m_shadowExtrudeDistYTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  shadowExtrudeDistSizer->Add(m_shadowExtrudeDistZTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
  sizer->Add(shadowExtrudeDistSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
  
  SetAutoLayout(true);
  SetSizer(sizer);
  sizer->Fit(this);
  sizer->SetSizeHints(this);
  Layout();
}

void ShadowsDialog::OnFatnessEnter(wxCommandEvent& event)
{
  switch(event.GetId())
    {
    case IDT_FATX : 
      m_currentConfig->fatness[0]=m_fatnessXTextCtrl->GetSafelyValue();
      break;
    case IDT_FATY : 
      m_currentConfig->fatness[1]=m_fatnessYTextCtrl->GetSafelyValue();
      break;
    case IDT_FATZ : 
      m_currentConfig->fatness[2]=m_fatnessZTextCtrl->GetSafelyValue();
      break;
    }
}

void ShadowsDialog::OnShadowsExtrudeDistEnter(wxCommandEvent& event)
{
  switch(event.GetId())
    {
    case IDT_SEDX : 
      m_currentConfig->extrudeDist[0]=m_shadowExtrudeDistXTextCtrl->GetSafelyValue();
      break;
    case IDT_SEDY : 
      m_currentConfig->extrudeDist[1]=m_shadowExtrudeDistYTextCtrl->GetSafelyValue();
      break;
    case IDT_SEDZ : 
      m_currentConfig->extrudeDist[2]=m_shadowExtrudeDistZTextCtrl->GetSafelyValue();
      break;
    }
}

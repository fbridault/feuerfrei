#ifndef FLAMEDIALOG_H
#define FLAMEDIALOG_H

#include <header.h>
#include <wx/notebook.h>

enum
  {
    IDBF_Add = 1,
    IDBF_Delete,
    IDBF_OK,
    IDBF_Cancel,
    IDBF_BrowseWick,
    IDRF_Type,
  };


/** Panneau pour les onglets de la boîte de dialogue des réglages des flammes */
class FlamePanel: public wxPanel 
{
public:
  FlamePanel(wxWindow* parent, int id, int nbSolvers, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
  void setCtrlValues(FlameConfig* solverConfig);
  void getCtrlValues(FlameConfig* solverConfig);
  long getLongTextCtrlValue(wxTextCtrl* textCtrl){
    wxString tmp; long val;
    tmp = textCtrl->GetValue();
    if(tmp.IsNumber()){
      tmp.ToLong(&val);
      return(val);
    }else{
      wxMessageDialog *errorDialog = new wxMessageDialog(this,_("Non-numeric field"),_("Error"),wxOK|wxICON_ERROR);
      errorDialog->ShowModal();
      errorDialog->Destroy();
      return -1;
    }
  };
  double getDoubleTextCtrlValue(wxTextCtrl* textCtrl){
    wxString tmp; double val;
    tmp = textCtrl->GetValue();
//     if(tmp.IsNumber())
    tmp.ToDouble(&val);
    return(val);
   //  else{
//       wxMessageDialog *errorDialog = new wxMessageDialog(this,_("Non-numeric field"),_("Error"),wxOK|wxICON_ERROR);
//       errorDialog->ShowModal();
//       errorDialog->Destroy();
//       return -1;
//     }
  };
private:
  void setProperties();
  void doLayout();
  void OnClickButtonBrowseWick(wxCommandEvent& event);
  void OnSelectType(wxCommandEvent& event);
  
  DECLARE_EVENT_TABLE()

protected:
  wxStaticText* m_posLabel;
  wxTextCtrl* m_posXTextCtrl;
  wxTextCtrl* m_posYTextCtrl;
  wxTextCtrl* m_posZTextCtrl;
  wxStaticText* m_wickLabel;
  wxTextCtrl* m_wickTextCtrl;
  wxButton* m_wickBrowseButton;
  wxRadioBox* m_flameTypeRadioBox;  
  wxStaticText* m_solverLabel;  
  wxComboBox* m_solverComboBox;
};

/** Boîte de dialogue pour les réglages des flammes */
class FlameDialog: public wxDialog 
{
public:
  FlameDialog(wxWindow* parent, int id, const wxString& title, FlameAppConfig *config, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);

private:
  void doLayout();
  void OnClickButtonAdd(wxCommandEvent& event);
  void OnClickButtonDelete(wxCommandEvent& event);
  void OnOK(wxCommandEvent& event);
  
  const static int m_nbMaxPanels=10;
  int m_nbPanels;
  FlamePanel* m_flamePanels[m_nbMaxPanels];
  wxNotebook* m_flameNotebook;
  wxButton *m_addFlameButton, *m_deleteFlameButton, *m_okButton, *m_cancelButton;
  FlameAppConfig *m_currentConfig;
  DECLARE_EVENT_TABLE()
};

#endif

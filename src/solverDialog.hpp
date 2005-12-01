#ifndef SOLVERDIALOG_H
#define SOLVERDIALOG_H

#include <header.h>
#include <wx/notebook.h>

enum
  {
    IDB_Add = 1,
    IDB_Delete,
    IDB_OK,
    IDB_Cancel,
    IDNB_Solvers,
  };


class SolverPanel: public wxPanel 
{
public:
  SolverPanel(wxWindow* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
  void setCtrlValues(SolverConfig* solverConfig);
  void getCtrlValues(SolverConfig* solverConfig);
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

protected:
    wxStaticText* m_posLabel;
    wxTextCtrl* m_posXTextCtrl;
    wxTextCtrl* m_posYTextCtrl;
    wxTextCtrl* m_posZTextCtrl;
    wxStaticText* m_dimLabel;
    wxTextCtrl* m_dimTextCtrl;
    wxStaticText* m_resLabel;
    wxTextCtrl* m_resXTextCtrl;
    wxTextCtrl* m_resYTextCtrl;
    wxTextCtrl* m_resZTextCtrl;
    wxStaticText* m_timeStepLabel;
    wxTextCtrl* m_timeStepTextCtrl;
    wxRadioBox* m_solverTypeRadioBox;
};

class SolverDialog: public wxDialog 
{
public:
  SolverDialog(wxWindow* parent, int id, const wxString& title, FlameAppConfig *config, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);

private:
  void doLayout();
  void OnClickButtonAdd(wxCommandEvent& event);
  void OnClickButtonDelete(wxCommandEvent& event);
  void OnOK(wxCommandEvent& event);
  void OnPageChanging(wxNotebookEvent& event);
  void checkSolverUsage(int solverIndex);
  
  const static int m_nbMaxPanels=5;
  int m_nbPanels;
  SolverPanel* m_solverPanels[m_nbMaxPanels];
  wxNotebook* m_solverNotebook;
  wxButton *m_addSolverButton, *m_deleteSolverButton, *m_okButton, *m_cancelButton;
  FlameAppConfig *m_currentConfig;
  DECLARE_EVENT_TABLE()
};

#endif

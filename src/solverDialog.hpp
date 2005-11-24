#ifndef SOLVERDIALOG_H
#define SOLVERDIALOG_H

#include <header.h>
#include <wx/notebook.h>


class SolverPanel: public wxPanel {
public:
    SolverPanel(wxWindow* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);

private:
    void set_properties();
    void do_layout();

protected:
    wxStaticBox* m_panelSizer_staticbox;
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

class SolverDialog: public wxDialog {
public:
  SolverDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);

private:
  void set_properties();
  void do_layout();
  
protected:
    wxStaticBox* m_staticbox;
    SolverPanel* m_solverPanel;
    wxNotebook* m_solverNotebook;
  wxButton* m_addSolverButton;
};

#endif

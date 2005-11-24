#include "solverDialog.hpp"

/**************************************** SolverPanel Class methods **************************************/

SolverPanel::SolverPanel(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style):
    wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
    m_panelSizer_staticbox = new wxStaticBox(this, -1, _("Solver #"));
    m_posLabel = new wxStaticText(this, -1, _("Position"));
    m_posXTextCtrl = new wxTextCtrl(this, -1, _(""));
    m_posYTextCtrl = new wxTextCtrl(this, -1, _(""));
    m_posZTextCtrl = new wxTextCtrl(this, -1, _(""));
    m_dimLabel = new wxStaticText(this, -1, _("Dimension"));
    m_dimTextCtrl = new wxTextCtrl(this, -1, _(""));
    m_resLabel = new wxStaticText(this, -1, _("Resolution"));
    m_resXTextCtrl = new wxTextCtrl(this, -1, _(""));
    m_resYTextCtrl = new wxTextCtrl(this, -1, _(""));
    m_resZTextCtrl = new wxTextCtrl(this, -1, _(""));
    m_timeStepLabel = new wxStaticText(this, -1, _("Time step"));
    m_timeStepTextCtrl = new wxTextCtrl(this, -1, _(""));
    const wxString m_solverTypeRadioBoxChoices[] = {
        _("Gauss-Seidel"),
        _("Preconditioned Conjugated Gradient")
    };
    m_solverTypeRadioBox = new wxRadioBox(this, -1, _("Type"), wxDefaultPosition, wxDefaultSize, 
					  2, m_solverTypeRadioBoxChoices, 0, wxRA_SPECIFY_COLS);

    set_properties();
    do_layout();
}


void SolverPanel::set_properties()
{
    m_posXTextCtrl->SetMinSize(wxSize(40, 22));
    m_posYTextCtrl->SetMinSize(wxSize(40, 22));
    m_posZTextCtrl->SetMinSize(wxSize(40, 22));
    m_dimTextCtrl->SetMinSize(wxSize(40, 22));
    m_resXTextCtrl->SetMinSize(wxSize(40, 22));
    m_resYTextCtrl->SetMinSize(wxSize(40, 22));
    m_resZTextCtrl->SetMinSize(wxSize(40, 22));
    m_timeStepTextCtrl->SetMinSize(wxSize(40, 22));
    m_solverTypeRadioBox->SetSelection(0);
}


void SolverPanel::do_layout()
{
    wxStaticBoxSizer* m_panelSizer = new wxStaticBoxSizer(m_panelSizer_staticbox, wxVERTICAL);
    wxBoxSizer* m_timeStepSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* m_resSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* m_dimSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* m_posSizer = new wxBoxSizer(wxHORIZONTAL);
    m_posSizer->Add(m_posLabel, 0, wxRIGHT|wxADJUST_MINSIZE, 15);
    m_posSizer->Add(m_posXTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
    m_posSizer->Add(m_posYTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
    m_posSizer->Add(m_posZTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 10);
    m_panelSizer->Add(m_posSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
    m_dimSizer->Add(m_dimLabel, 0, wxADJUST_MINSIZE, 0);
    m_dimSizer->Add(m_dimTextCtrl, 0, wxLEFT|wxADJUST_MINSIZE, 8);
    m_panelSizer->Add(m_dimSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
    m_resSizer->Add(m_resLabel, 0, wxADJUST_MINSIZE, 0);
    m_resSizer->Add(m_resXTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
    m_resSizer->Add(m_resYTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
    m_resSizer->Add(m_resZTextCtrl, 0, wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 10);
    m_panelSizer->Add(m_resSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
    m_timeStepSizer->Add(m_timeStepLabel, 0, wxRIGHT|wxADJUST_MINSIZE, 12);
    m_timeStepSizer->Add(m_timeStepTextCtrl, 0, wxADJUST_MINSIZE, 0);
    m_panelSizer->Add(m_timeStepSizer, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
    m_panelSizer->Add(m_solverTypeRadioBox, 0, wxADJUST_MINSIZE, 0);
    SetAutoLayout(true);
    SetSizer(m_panelSizer);
    m_panelSizer->Fit(this);
    m_panelSizer->SetSizeHints(this);
}

/**************************************** SolverDialog Class methods **************************************/

SolverDialog::SolverDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, 
			   const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{
    m_staticbox = new wxStaticBox(this, -1, _("sizer_12"));
    m_solverNotebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
    m_solverPanel = new SolverPanel(m_solverNotebook, -1);
    m_addSolverButton = new wxButton(this, -1, _("Add a solver"));

    set_properties();
    do_layout();
}


void SolverDialog::set_properties()
{
    SetTitle(_("Solver settings"));
}


void SolverDialog::do_layout()
{
    wxStaticBoxSizer* sizer = new wxStaticBoxSizer(m_staticbox, wxVERTICAL);
    m_solverNotebook->AddPage(m_solverPanel, _("tab1"));
    sizer->Add(m_solverNotebook, 1, wxEXPAND, 0);
    sizer->Add(m_addSolverButton, 0, wxADJUST_MINSIZE, 0);
    SetAutoLayout(true);
    SetSizer(sizer);
    sizer->Fit(this);
    sizer->SetSizeHints(this);
    Layout();
}

#ifndef LUMINARYDIALOG_H
#define LUMINARYDIALOG_H

#include "numTextCtrl.hpp"
#include <wx/notebook.h>

enum
  {
    IDBF_Add = 1,
    IDBF_Delete,
    IDBF_OK,
    IDBF_Cancel,
    IDBF_BrowseWick,
    IDRF_Type,
    IDRS_Type,
    IDCHK_GS,
  };


/** Panneau pour les onglets de la boîte de dialogue des réglages des flammes */
class LuminaryPanel: public wxPanel 
{
public:
  LuminaryPanel(wxWindow* parent, int id, bool localSolver, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
  void setCtrlValues(const LuminaryConfig& luminaryConfig);
  void setCtrlValues(const SolverConfig& solverConfig);
  bool getCtrlValues(LuminaryConfig& luminaryConfig);
  bool getCtrlValues(SolverConfig& solverConfig);
  
private:
  void setProperties();
  void doLayout();
  void OnClickButtonBrowseWick(wxCommandEvent& event);
  void OnSelectFlameType(wxCommandEvent& event);
  void OnSelectFieldType(wxCommandEvent& event);
  
  protected:
  wxStaticText* m_posLabel;
  DoubleTextCtrl* m_posXTextCtrl;
  DoubleTextCtrl* m_posYTextCtrl;
  DoubleTextCtrl* m_posZTextCtrl;
  wxStaticText* m_wickLabel;
  wxTextCtrl* m_wickTextCtrl;
  wxStaticText* m_skeletonsNumberLabel;
  LongTextCtrl* m_skeletonsNumberCtrl;
  wxButton* m_wickBrowseButton;
  wxRadioBox* m_luminaryTypeRadioBox;
  
  wxStaticText* m_dimLabel;
  DoubleTextCtrl* m_dimTextCtrl;
  wxStaticText* m_scaleLabel;
  DoubleTextCtrl* m_scaleXTextCtrl;
  DoubleTextCtrl* m_scaleYTextCtrl;
  DoubleTextCtrl* m_scaleZTextCtrl;
  wxStaticText* m_resLabel;
  LongTextCtrl* m_resXTextCtrl;
  LongTextCtrl* m_resYTextCtrl;
  LongTextCtrl* m_resZTextCtrl;
  wxStaticText* m_timeStepLabel;
  DoubleTextCtrl* m_timeStepTextCtrl;
  wxRadioBox* m_solverTypeRadioBox;
  wxStaticText* m_omegaDiffLabel;
  DoubleTextCtrl* m_omegaDiffTextCtrl;
  wxStaticText* m_omegaProjLabel;
  DoubleTextCtrl* m_omegaProjTextCtrl;
  wxStaticText* m_epsilonLabel;
  DoubleTextCtrl* m_epsilonTextCtrl;
  bool m_luminary;
  
  DECLARE_EVENT_TABLE()
};

/** Boîte de dialogue pour les réglages des flammes */
class LuminaryDialog: public wxDialog 
{
public:
  LuminaryDialog(wxWindow* parent, int id, const wxString& title, FlameAppConfig* const config, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);

private:
  void doLayout();
  void OnClickButtonAdd(wxCommandEvent& event);
  void OnClickButtonDelete(wxCommandEvent& event);
  void OnOK(wxCommandEvent& event);
  void OnCancel(wxCommandEvent& event);
  void OnCheckActivateGlobalSolver(wxCommandEvent& event);
  
  int m_nbPanels;
  wxCheckBox *m_activateGlobalSolverCheckBox;
  LuminaryPanel* m_luminaryPanels[NB_MAXFLAMMES];
  LuminaryPanel *m_globalSolverPanel;
  wxNotebook* m_luminaryNotebook;
  wxButton *m_addLuminaryButton, *m_deleteLuminaryButton, *m_okButton, *m_cancelButton;
  FlameAppConfig *m_currentConfig;
  wxBoxSizer* m_sizer;
  DECLARE_EVENT_TABLE()
};

#endif

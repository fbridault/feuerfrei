#ifndef SOLVERDIALOG_H
#define SOLVERDIALOG_H

#include <wx/notebook.h>
#include "interface.hpp"
#include "numTextCtrl.hpp"

enum
  {
    IDB_Add = 1,
    IDB_Delete,
    IDB_OK,
    IDB_Cancel,
    IDNB_Solvers,
    IDRS_Type,
    IDCHK_GS,
  };

/** Panneau pour les onglets de la boîte de dialogue des solveurs */
class SolverPanel: public wxPanel 
{
public:
  SolverPanel(wxWindow* parent, int id, bool localSolver=true, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
  void setCtrlValues(const SolverConfig& solverConfig);
  bool getCtrlValues(SolverConfig& solverConfig);

  void OnSelectType(wxCommandEvent& event);

private:
  void setProperties();
  void doLayout();

protected:
  wxStaticText* m_posLabel;
  DoubleTextCtrl* m_posXTextCtrl;
  DoubleTextCtrl* m_posYTextCtrl;
  DoubleTextCtrl* m_posZTextCtrl;
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
  wxStaticText* m_nbMaxIterLabel;
  LongTextCtrl* m_nbMaxIterTextCtrl;
  bool m_localSolver;
  DECLARE_EVENT_TABLE()
};

/** Boîte de dialogue pour les réglages du solveur */
class SolverDialog: public wxDialog 
{
public:
  SolverDialog(wxWindow* parent, int id, const wxString& title, FluidsAppConfig *config, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);
  
private:
  void doLayout();
  void OnClickButtonAdd(wxCommandEvent& event);
  void OnClickButtonDelete(wxCommandEvent& event);
  void OnOK(wxCommandEvent& event);
  void OnCancel(wxCommandEvent& event);
  void OnPageChanging(wxNotebookEvent& event);
  /** Vérifie si un solveur est utilisé par des flammes.
   * Si c'est le cas, le bouton d'effacement du solveur est désactivé
   * @param solverIndex index du solveur à vérifier
   */
  void checkSolverUsage(int solverIndex);
  
  int m_nbPanels;
  SolverPanel *m_solverPanels[NB_MAXSOLVERS];
  wxNotebook *m_solverNotebook;
  wxButton *m_addSolverButton, *m_deleteSolverButton, *m_okButton, *m_cancelButton;
  FluidsAppConfig *m_currentConfig;
  DECLARE_EVENT_TABLE()
};

#endif

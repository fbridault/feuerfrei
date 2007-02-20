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
  };

/** Panneau pour les onglets de la boîte de dialogue des solveurs */
class SolverPanel: public wxPanel 
{
public:
  SolverPanel(wxWindow* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
  void setCtrlValues(SolverConfig* const solverConfig);
  bool getCtrlValues(SolverConfig* const solverConfig);

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

  DECLARE_EVENT_TABLE()
};

/** Boîte de dialogue pour les réglages du solveur */
class SolverDialog: public wxDialog 
{
public:
#ifdef RTFLAMES_BUILD
  SolverDialog(wxWindow* parent, int id, const wxString& title, FlameAppConfig *config, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);
#else
  SolverDialog(wxWindow* parent, int id, const wxString& title, FluidsAppConfig *config, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);
#endif

private:
  void doLayout();
  void OnClickButtonAdd(wxCommandEvent& event);
  void OnClickButtonDelete(wxCommandEvent& event);
  void OnOK(wxCommandEvent& event);
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
#ifdef RTFLAMES_BUILD
  FlameAppConfig *m_currentConfig;
#else
  FluidsAppConfig *m_currentConfig;
#endif
  DECLARE_EVENT_TABLE()
};

#endif

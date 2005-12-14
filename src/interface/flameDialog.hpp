#ifndef FLAMEDIALOG_H
#define FLAMEDIALOG_H

#include "interface.hpp"
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
  };


/** Panneau pour les onglets de la boîte de dialogue des réglages des flammes */
class FlamePanel: public wxPanel 
{
public:
  FlamePanel(wxWindow* parent, int id, int nbSolvers, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
  void setCtrlValues(FlameConfig* solverConfig);
  bool getCtrlValues(FlameConfig* solverConfig);
  
private:
  void setProperties();
  void doLayout();
  void OnClickButtonBrowseWick(wxCommandEvent& event);
  void OnSelectType(wxCommandEvent& event);
  
  DECLARE_EVENT_TABLE()

protected:
  wxStaticText* m_posLabel;
  DoubleTextCtrl* m_posXTextCtrl;
  DoubleTextCtrl* m_posYTextCtrl;
  DoubleTextCtrl* m_posZTextCtrl;
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
  
  int m_nbPanels;
  FlamePanel* m_flamePanels[NB_MAXFLAMMES];
  wxNotebook* m_flameNotebook;
  wxButton *m_addFlameButton, *m_deleteFlameButton, *m_okButton, *m_cancelButton;
  FlameAppConfig *m_currentConfig;
  DECLARE_EVENT_TABLE()
};

#endif

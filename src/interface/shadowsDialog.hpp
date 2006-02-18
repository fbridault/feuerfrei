#ifndef SHADOWSDIALOG_H
#define SHADOWSDIALOG_H

#include "interface.hpp"
#include "numTextCtrl.hpp"
#include "GLFlameCanvas.hpp"

enum
  {
    IDT_FATX = 1,
    IDT_FATY,
    IDT_FATZ,
    IDT_SEDX,
    IDT_SEDY,
    IDT_SEDZ,
  };

/** Boîte de dialogue pour les réglages du solveur */
class ShadowsDialog: public wxDialog 
{
public:
  ShadowsDialog(wxWindow* parent, int id, const wxString& title, FlameAppConfig *config, GLFlameCanvas *glBuffer, 
		const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);

private:
  void OnFatnessEnter(wxCommandEvent& event);
  void OnShadowsExtrudeDistEnter(wxCommandEvent& event);
  void doLayout();
  
  wxStaticText *m_fatnessLabel;
  DoubleTextCtrl *m_fatnessXTextCtrl, *m_fatnessYTextCtrl, *m_fatnessZTextCtrl;
  wxStaticText *m_shadowExtrudeDistLabel;
  DoubleTextCtrl *m_shadowExtrudeDistXTextCtrl, *m_shadowExtrudeDistYTextCtrl, *m_shadowExtrudeDistZTextCtrl;
  
  FlameAppConfig *m_currentConfig;
  /** Zone d'affichage OpenGL */
  GLFlameCanvas *m_glBuffer;
  DECLARE_EVENT_TABLE()
};

#endif

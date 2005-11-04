#ifndef MAIN_H
#define MAIN_H

#include <wx/wxprec.h>
#include <wx/glcanvas.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wxGLBuffer.hpp"


#define LARGEUR  512
#define HAUTEUR  512

enum
{
ID_ButtonRun = 1,
ID_ButtonFlickering,
};

enum
{
IDM_Quit = 1,
IDM_About,
IDM_Glow,
IDM_SP
};

class MainFrame: public wxFrame
{
public:
  MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size); 
  
  /** Actions des boutons */
  void OnClickButtonRun(wxCommandEvent& event);
  void OnClickButtonFlickering(wxCommandEvent& event);
  
  /** Actions des menus */
  void OnQuitMenu(wxCommandEvent& event);
  void OnAboutMenu(wxCommandEvent& event);
  void OnGlowMenu(wxCommandEvent& event);
  void OnSPMenu(wxCommandEvent& event);

  void SetFPS(int fps);
  int GetSettingsFromFile (char *name);
  
private:
  
  // Boutton 1
  wxButton *buttonRun, *buttonFlickering;
  wxGLBuffer *glBuffer;
  wxMenu *menuFile, *menuDisplay;
  wxMenuBar *menuBar;
  wxStaticBoxSizer *sizerH;
  wxBoxSizer *sizerV;
  
  DECLARE_EVENT_TABLE()
};

#endif

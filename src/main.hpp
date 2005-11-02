// Fichier hworld.h

// Indispensable pour faire des wxwidgets :
#include <wx/wxprec.h>
#include <wx/glcanvas.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wxGLBuffer.h"


#define LARGEUR  512
#define HAUTEUR  512

// On doit créer un identifiant pour chaque évenement
// Ceci permettra, par exemple, d'associer un même
// évemenement à deux boutons
#define ID_Bt_Click 1

//Déclaration d'une classe MyApp (Mon application) dérivée de wxApp
class MyApp: public wxApp
{
    virtual bool OnInit();
};

enum
{
ID_Quit = 1,
ID_About,
};

//Déclaration d'une classe MainFrame (Ma fenetre principale) dérivée de wxFrame
class MainFrame: public wxFrame
{
public:
  //Constructeur de la fenetre :
  MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size); 
  
  //Fonction qui sera appelé lorsque l'utilisateur cliquera sur le MonBouton1
  void OnClickButton1(wxCommandEvent& event);

  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);

  int ParseInitFile (char *name);
  
  // Boutton 1
  wxButton *button1;
  wxGLBuffer *glBuffer;
  wxMenu *menuFile;
  wxMenuBar *menuBar;
  wxBoxSizer *sizer;
  // C'est la table qui est écrite dans le fichier cpp
  DECLARE_EVENT_TABLE()
};

// Sorte de main ...
IMPLEMENT_APP(MyApp)

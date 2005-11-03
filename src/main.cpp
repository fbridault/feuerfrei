#include "main.hpp"

// Déclarations de la table des événements
// Sorte de relation qui lit des identifiants d'événements aux fonctions
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_BUTTON(ID_ButtonRun, MainFrame::OnClickButtonRun)
  EVT_BUTTON(ID_ButtonFlickering, MainFrame::OnClickButtonFlickering)
  EVT_MENU(ID_Quit, MainFrame::OnQuit)
  EVT_MENU(ID_About, MainFrame::OnAbout)
END_EVENT_TABLE();

class MyApp: public wxApp
{
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)


// Code de l'initialisation de l'application
bool MyApp::OnInit()
{
  wxImage::AddHandler(new wxPNGHandler);
  wxImage::AddHandler(new wxJPEGHandler);
  
  MainFrame *frame = new MainFrame( _("Hello World"), wxPoint(0,0), wxSize(800,900) );
  
  frame->Show(TRUE);
  
#ifdef BOUGIE
  if (frame->GetSettingsFromFile ("param.ini"))
    exit (2);
#else
  if (frame->GetSettingsFromFile ("param2.ini"))
    exit (2);
#endif
  SetTopWindow(frame);

  return TRUE;
} 


// Construction de la fenêtre. Elle ne contient qu'un bouton.
MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{

  int attributelist[ 5 ] = { WX_GL_RGBA        ,
			     WX_GL_DOUBLEBUFFER,
			     WX_GL_STENCIL_SIZE,
			     1                 ,
			     0                  };
  
  glBuffer = new wxGLBuffer( this, wxID_ANY, wxPoint(0,0), wxSize(800,800),attributelist, wxSUNKEN_BORDER );
  
  // Création d'un bouton. Ce bouton est associé à l'identifiant 
  // événement ID_Bt_Click, en consultant, la table des événements
  // on en déduit que c'est la fonction OnClickButton qui sera 
  // appelée lors d'un click sur ce bouton
  buttonRun = new wxButton(this,ID_ButtonRun,_("Start/Pause"));
  buttonFlickering = new wxButton(this,ID_ButtonFlickering,_("Flickering"));

  sizerH = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Controls"));
  sizerH->Add(buttonRun, 0, 0, 0);
  sizerH->Add(buttonFlickering, 0, 0, 0);
  
  sizerV = new wxBoxSizer(wxVERTICAL);
  sizerV->Add(glBuffer, 0, 0, 0);
  sizerV->Add(sizerH, 0, 0, 0);
   
  SetSizer(sizerV);

  menuFile = new wxMenu;

  menuFile->Append( ID_About, _("&About...") );
  menuFile->AppendSeparator();
  menuFile->Append( ID_Quit, _("E&xit") );
  
  menuBar = new wxMenuBar;
  menuBar->Append( menuFile, _("&File") );
  
  SetMenuBar( menuBar );
  
  CreateStatusBar();
  SetStatusText( _("FPS will be here...") );
}

// Fonction qui est exécutée lors du click sur le bouton.
void MainFrame::OnClickButtonRun(wxCommandEvent& event)
{
  glBuffer->ToggleRun();
}

void MainFrame::OnClickButtonFlickering(wxCommandEvent& event)
{
  glBuffer->ToggleFlickering();
}


void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close(TRUE);
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(_("Real-time simulation of small flames\nOasis Team"),
	       _("About flames"), wxOK | wxICON_INFORMATION, this);
}

/* A convertir en wx */
int MainFrame::GetSettingsFromFile (char *name)
{
  FILE *f;
  char buffer[255];
  float timeStep;
  int solvx, solvy, solvz;
  double clipping;
  char meche_name[255];
  char scene_name[255];
  int largeur, hauteur;
  
  if ((f = fopen (name, "r")) == NULL)
    {
      cout << "Problème d'ouverture du fichier de paramétrage" <<
	endl;
      return 2;
    }

  do
    {
      fscanf (f, "%s", buffer);

      if (!strcmp (buffer, "GRID_SIZE"))
	fscanf (f, "%d %d %d", &solvx, &solvy, &solvz);
      else if (!strcmp (buffer, "TIME_STEP"))
	fscanf (f, "%f", &timeStep);
      else if (!strcmp (buffer, "SCENE"))
	fscanf (f, "%s", scene_name);
      else if (!strcmp (buffer, "WINDOW_SIZE"))
	fscanf (f, "%d %d", &largeur, &hauteur);
      else if (!strcmp (buffer, "CLIPPING"))
	fscanf (f, "%lf", &clipping);
      else if (!strcmp (buffer, "WICK"))
	fscanf (f, "%s", meche_name);

    }
  while (!feof (f));
  timeStep=0.4;
  cout << "Grille de " << solvx << "x" << solvy << "x" << solvz << endl;
  cout << "Pas de temps :" << timeStep << endl;
  cout << "Pas de temps :" << meche_name << endl;
  cout << "Pas de temps :" << largeur << hauteur << endl;
  cout << "Pas de temps :" << clipping << endl;
  cout << "Nom de la scène :" << scene_name << endl;

  fclose (f);

  glBuffer->SetSize(wxSize(largeur,hauteur));
  glBuffer->Init(largeur, hauteur, solvx, solvy, solvz, timeStep, scene_name, meche_name, clipping);
  
  return 0;
}

void MainFrame::SetFPS(int fps)
{
  wxString s;
  s += wxString::Format(_("%d FPS"), fps);
  
  SetStatusText(s);
}

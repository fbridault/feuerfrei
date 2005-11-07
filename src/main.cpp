#include "main.hpp"

// Déclarations de la table des événements
// Sorte de relation qui lit des identifiants d'événements aux fonctions
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_BUTTON(IDB_Run, MainFrame::OnClickButtonRun)
  EVT_BUTTON(IDB_Restart, MainFrame::OnClickButtonRestart)
  EVT_BUTTON(IDB_Flickering, MainFrame::OnClickButtonFlickering)
  EVT_BUTTON(IDB_Swap, MainFrame::OnClickButtonSwap)
  EVT_MENU(IDM_SaveSettings, MainFrame::OnSaveSettingsMenu)
  EVT_MENU(IDM_Quit, MainFrame::OnQuitMenu)
  EVT_MENU(IDM_About, MainFrame::OnAboutMenu)
  EVT_MENU(IDM_Grid, MainFrame::OnGridMenu)
  EVT_MENU(IDM_Base, MainFrame::OnBaseMenu)
  EVT_MENU(IDM_Velocity, MainFrame::OnVelocityMenu)
  EVT_MENU(IDM_Particles, MainFrame::OnParticlesMenu)
  EVT_MENU(IDM_Hide, MainFrame::OnHideMenu)
  EVT_MENU(IDM_Wired, MainFrame::OnWiredMenu)
  EVT_MENU(IDM_Shaded, MainFrame::OnShadedMenu)
  EVT_CHECKBOX(IDCHK_IS, MainFrame::OnCheckIS)
  EVT_CHECKBOX(IDCHK_BS, MainFrame::OnCheckBS)
  EVT_CHECKBOX(IDCHK_Glow, MainFrame::OnCheckGlow)
  EVT_CHECKBOX(IDCHK_ES, MainFrame::OnCheckES)
  EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE();

class FlamesApp : public wxApp
{
    virtual bool OnInit();
};

IMPLEMENT_APP(FlamesApp)


// Code de l'initialisation de l'application
bool FlamesApp::OnInit()
{
  wxImage::AddHandler(new wxPNGHandler);
  wxImage::AddHandler(new wxJPEGHandler);
  
  MainFrame *frame = new MainFrame( _("Hello World"), wxDefaultPosition, wxSize(950,860) );
  
  frame->Show(TRUE);  
  
  frame->GetSettingsFromConfigFile ();
  
  SetTopWindow(frame);
  return TRUE;
} 


// Construction de la fenêtre. Elle ne contient qu'un bouton.
MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{

  int attributelist[ 10 ] = { WX_GL_RGBA        ,
			      WX_GL_DOUBLEBUFFER,
			      WX_GL_STENCIL_SIZE,
			      1                 ,
			      0                  };
  
  m_glBuffer = new wxGLBuffer( this, wxID_ANY, wxPoint(0,0), wxSize(800,800),attributelist, wxSUNKEN_BORDER );
  
  // Création d'un bouton. Ce bouton est associé à l'identifiant 
  // événement ID_Bt_Click, en consultant, la table des événements
  // on en déduit que c'est la fonction OnClickButton qui sera 
  // appelée lors d'un click sur ce bouton
  m_buttonRun = new wxButton(this,IDB_Run,_("Pause"));
  m_buttonRestart = new wxButton(this,IDB_Restart,_("Restart"));
  m_buttonFlickering = new wxButton(this,IDB_Flickering,_("Flickering"));
  m_buttonSwap = new wxButton(this,IDB_Swap,_("Next IES file"));
  
  m_interpolatedSolidCheckBox = new wxCheckBox(this,IDCHK_IS,_("Interpolation"));
  m_blendedSolidCheckBox = new wxCheckBox(this,IDCHK_BS,_("Blended"));
  m_enableSolidCheckBox = new wxCheckBox(this,IDCHK_ES,_("Enabled"));
  m_glowEnabledCheckBox = new wxCheckBox(this,IDCHK_Glow,_("Enabled"));
  
  m_globalSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Global"));
  m_globalSizer->Add(m_buttonRun, 0, 0, 0);
  m_globalSizer->Add(m_buttonRestart, 0, 0, 0);
  m_globalSizer->Add(m_buttonFlickering, 0, 0, 0);  

  m_solidSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Photometric solid"));
  m_solidSizer->Add(m_enableSolidCheckBox, 0, 0, 0);
  m_solidSizer->Add(m_interpolatedSolidCheckBox, 0, 0, 0);
  m_solidSizer->Add(m_blendedSolidCheckBox, 0, 0, 0);
  m_solidSizer->Add(m_buttonSwap, 0, 0, 0);
  
  m_glowSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Glow"));
  m_glowSizer->Add(m_glowEnabledCheckBox, 0, 0, 0);
  
  m_rightSizer = new wxBoxSizer(wxVERTICAL);
  m_rightSizer->Add(m_globalSizer, 0, 0, 0);
  m_rightSizer->Add(m_solidSizer, 0, 0, 0);
  m_rightSizer->Add(m_glowSizer, 0, 0, 0);

  m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
  m_mainSizer->Add(m_glBuffer, 0, 0, 0);
  m_mainSizer->Add(m_rightSizer, 0, 0, 0);
   
  SetSizer(m_mainSizer);

  m_menuFile = new wxMenu;
  
  m_menuFile->Append( IDM_SaveSettings, _("&Save settings") );
  m_menuFile->Append( IDM_About, _("&About...") );
  m_menuFile->AppendSeparator();
  m_menuFile->Append( IDM_Quit, _("E&xit") );
  
  m_menuDisplayFlames = new wxMenu;
  m_menuDisplayFlames->AppendCheckItem( IDM_Hide, _("Hide"));
  m_menuDisplayFlames->AppendCheckItem( IDM_Wired, _("Wired"));
  m_menuDisplayFlames->AppendCheckItem( IDM_Shaded, _("Shaded"));
  m_menuDisplayFlames->Check(IDM_Shaded,true);
  
  m_menuDisplay = new wxMenu;
  m_menuDisplay->AppendCheckItem( IDM_Grid, _("Grid"));
  m_menuDisplay->AppendCheckItem( IDM_Base, _("Base"));
  m_menuDisplay->AppendCheckItem( IDM_Velocity, _("Velocity"));
  m_menuDisplay->AppendCheckItem( IDM_Particles, _("Particles"));
  m_menuDisplay->Append( IDM_Flames, _("Flames"), m_menuDisplayFlames);
  
  m_menuBar = new wxMenuBar;
  m_menuBar->Append( m_menuFile, _("&File") );
  m_menuBar->Append( m_menuDisplay, _("&Display") );
  
  SetMenuBar( m_menuBar );
  
  CreateStatusBar();
  SetStatusText( _("FPS will be here...") );
}

void MainFrame::OnClose(wxCloseEvent& event)
{
  delete m_config;
  
  Destroy();
}

// Fonction qui est exécutée lors du click sur le bouton.
void MainFrame::OnClickButtonRun(wxCommandEvent& event)
{
  if(m_glBuffer->IsRunning())
    m_buttonRun->SetLabel(_("Continue"));
  else
    m_buttonRun->SetLabel(_("Pause"));
  m_glBuffer->ToggleRun();
}

// Fonction qui est exécutée lors du click sur le bouton.
void MainFrame::OnClickButtonRestart(wxCommandEvent& event)
{
  m_glBuffer->Restart();
}

void MainFrame::OnClickButtonFlickering(wxCommandEvent& event)
{
  m_glBuffer->ToggleFlickering();
}

void MainFrame::OnClickButtonSwap(wxCommandEvent& event)
{
  m_glBuffer->Swap();
}

void MainFrame::OnGridMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleGridDisplay();
}

void MainFrame::OnBaseMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleBaseDisplay();
}

void MainFrame::OnVelocityMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleVelocityDisplay();
}

void MainFrame::OnParticlesMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleParticlesDisplay();
}

void MainFrame::OnHideMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleFlamesDisplay();
}

void MainFrame::OnWiredMenu(wxCommandEvent& event)
{
  if(m_menuDisplayFlames->IsChecked(IDM_Shaded)){
    m_glBuffer->ToggleSmoothShading();
    m_menuDisplayFlames->Check(IDM_Shaded,false);
  }else
    m_menuDisplayFlames->Check(IDM_Wired,true);
}

void MainFrame::OnShadedMenu(wxCommandEvent& event)
{
  if(m_menuDisplayFlames->IsChecked(IDM_Wired)){
    m_glBuffer->ToggleSmoothShading();
    m_menuDisplayFlames->Check(IDM_Wired,false);
  }else
    m_menuDisplayFlames->Check(IDM_Shaded,true);
}

void MainFrame::OnCheckBS(wxCommandEvent& event)
{
  m_glBuffer->ToggleBlendedSP();
}

void MainFrame::OnCheckIS(wxCommandEvent& event)
{
  m_glBuffer->ToggleInterpolationSP();
}

void MainFrame::OnCheckGlow(wxCommandEvent& event)
{
  m_glBuffer->ToggleGlow();
}

void MainFrame::OnCheckES(wxCommandEvent& event)
{
  if(m_enableSolidCheckBox->IsChecked()){
    m_interpolatedSolidCheckBox->Enable();
    m_blendedSolidCheckBox->Enable();
  }else{
    m_interpolatedSolidCheckBox->Disable();
    m_blendedSolidCheckBox->Disable();
  }
  m_glBuffer->ToggleSP(); 
}

void MainFrame::OnQuitMenu(wxCommandEvent& WXUNUSED(event))
{
  Close(TRUE);
}

void MainFrame::OnAboutMenu(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(_("Real-time simulation of small flames\nOasis Team"),
	       _("About flames"), wxOK | wxICON_INFORMATION, this);
}

void MainFrame::GetSettingsFromConfigFile ()
{
  wxFileInputStream* file = new wxFileInputStream( _("param.ini" ));
  m_config = new wxFileConfig( *file );
  
  m_currentConfig.solvx = m_config->Read(_("/Solver/X_res"), 15);
  m_currentConfig.solvy = m_config->Read(_("/Solver/Y_res"), 15);
  m_currentConfig.solvz = m_config->Read(_("/Solver/Z_res"), 15);
  m_config->Read(_("/Solver/TimeStep"),&m_currentConfig.timeStep,0.4);
  m_currentConfig.width = m_config->Read(_("/Display/Width"), 800);
  m_currentConfig.height = m_config->Read(_("/Display/Height"), 800);
  m_currentConfig.clipping = m_config->Read(_("/Display/Clipping"), 100);
  m_config->Read(_("/Display/PhotometricSolid"), &m_currentConfig.PSEnabled, false);
  m_config->Read(_("/Display/IPSEnabled"), &m_currentConfig.IPSEnabled, 0);
  m_config->Read(_("/Display/BPSEnabled"), &m_currentConfig.BPSEnabled, 0);
  m_config->Read(_("/Display/Glow"), &m_currentConfig.glowEnabled, false);
  m_currentConfig.sceneName = m_config->Read(_("/Scene/FileName"), _("scene2.obj"));
  /* A intégrer bientôt, mais il faut propager alors dans les squelettes */
  //  flameType = m_config->Read(_("/Flame/Type"), 1);
  m_currentConfig.mecheName = m_config->Read(_("/Flame/WickFileName"), _("meche2.obj"));
  
  m_interpolatedSolidCheckBox->SetValue(m_currentConfig.IPSEnabled);
  m_blendedSolidCheckBox->SetValue(m_currentConfig.BPSEnabled);
  m_enableSolidCheckBox->SetValue(m_currentConfig.PSEnabled);
  m_glowEnabledCheckBox->SetValue(m_currentConfig.glowEnabled);
  
  if(m_enableSolidCheckBox->IsChecked()){
    m_interpolatedSolidCheckBox->Enable();
    m_blendedSolidCheckBox->Enable();
  }else{
    m_interpolatedSolidCheckBox->Disable();
    m_blendedSolidCheckBox->Disable();
  }

  m_glBuffer->SetSize(wxSize(m_currentConfig.width,m_currentConfig.height));
  m_glBuffer->Init(&m_currentConfig);
  
  delete file;

  return;
}

void MainFrame::OnSaveSettingsMenu(wxCommandEvent& event)
{
  m_config->Write(_("/Solver/X_res"),m_currentConfig.solvx);
  m_config->Write(_("/Solver/Y_res"),m_currentConfig.solvy);
  m_config->Write(_("/Solver/Z_res"),m_currentConfig.solvz);
  m_config->Write(_("/Solver/TimeStep"),m_currentConfig.timeStep);
  m_config->Write(_("/Display/Width"), m_currentConfig.width);
  m_config->Write(_("/Display/Height"), m_currentConfig.height);
  m_config->Write(_("/Display/Clipping"), m_currentConfig.clipping);
  m_config->Write(_("/Display/PhotometricSolid"), m_currentConfig.PSEnabled);
  m_config->Write(_("/Display/IPSEnabled"), m_currentConfig.IPSEnabled);
  m_config->Write(_("/Display/BPSEnabled"), m_currentConfig.BPSEnabled);
  m_config->Write(_("/Display/Glow"), m_currentConfig.glowEnabled);
  m_config->Write(_("/Scene/FileName"), m_currentConfig.sceneName);
  m_config->Write(_("/Flame/WickFileName"), m_currentConfig.mecheName);
  
  wxFileOutputStream* file = new wxFileOutputStream( _("param.ini" ));
  
  if (m_config->Save(*file) )  
    wxMessageBox(_("Configuration for the current simulation have been saved"),
		 _("Save settings"), wxOK | wxICON_INFORMATION, this);

  delete file;
}

void MainFrame::SetFPS(int fps)
{
  wxString s;
  s += wxString::Format(_("%d FPS"), fps);
  
  SetStatusText(s);
}

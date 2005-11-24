#include "main.hpp"

#include "solverDialog.hpp"

// Déclarations de la table des événements
// Sorte de relation qui lit des identifiants d'événements aux fonctions
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_BUTTON(IDB_Run, MainFrame::OnClickButtonRun)
  EVT_BUTTON(IDB_Restart, MainFrame::OnClickButtonRestart)
  EVT_BUTTON(IDB_Flickering, MainFrame::OnClickButtonFlickering)
  EVT_BUTTON(IDB_Swap, MainFrame::OnClickButtonSwap)
  EVT_MENU(IDM_OpenScene, MainFrame::OnOpenSceneMenu)
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
  EVT_MENU(IDM_Solvers, MainFrame::OnSolversMenu)
  EVT_CHECKBOX(IDCHK_IS, MainFrame::OnCheckIS)
  EVT_CHECKBOX(IDCHK_BS, MainFrame::OnCheckBS)
  EVT_CHECKBOX(IDCHK_Glow, MainFrame::OnCheckGlow)
  EVT_CHECKBOX(IDCHK_ES, MainFrame::OnCheckES)
  EVT_COMBOBOX(IDCB_SELECT, MainFrame::OnSelectFlame)
  EVT_SCROLL(MainFrame::OnScrollPosition)
  EVT_SCROLL(MainFrame::OnScrollPosition)
  EVT_SCROLL(MainFrame::OnScrollPosition)
  EVT_TEXT_ENTER(IDT_FXAPMIN, MainFrame::OnFXAPMINEnter)
  EVT_TEXT_ENTER(IDT_FXAPMAX, MainFrame::OnFXAPMAXEnter)
  EVT_TEXT_ENTER(IDT_FYAPMIN, MainFrame::OnFYAPMINEnter)
  EVT_TEXT_ENTER(IDT_FYAPMAX, MainFrame::OnFYAPMAXEnter)
  EVT_TEXT_ENTER(IDT_FZAPMIN, MainFrame::OnFZAPMINEnter)
  EVT_TEXT_ENTER(IDT_FZAPMAX, MainFrame::OnFZAPMAXEnter)
  EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE();

/******************************** FlamesApp Class definition & methods ******g*************************/

class FlamesApp : public wxApp
{
    virtual bool OnInit();
};

IMPLEMENT_APP(FlamesApp)


bool FlamesApp::OnInit()
{
  bool recompileShaders=false;
  /* Déclaration des handlers pour la gestion des formats d'image */
  wxImage::AddHandler(new wxPNGHandler);
  wxImage::AddHandler(new wxJPEGHandler);
  
  MainFrame *frame = new MainFrame( _("Real-time Animation of small Flames"), wxDefaultPosition, wxSize(1060,860) );
  
  frame->Show(TRUE);
  
  if(argc == 2){
    if(wxString(argv[1]) == _("--recompile"))
      recompileShaders = true;
  }
  frame->GetSettingsFromConfigFile (recompileShaders);
  
  SetTopWindow(frame);
  return TRUE;
} 

/**************************************** MainFrame Class methods **************************************/

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  
  int attributelist[ 10 ] = { WX_GL_RGBA        ,
			      WX_GL_DOUBLEBUFFER,
			      WX_GL_STENCIL_SIZE,
			      1                 ,
			      0                  };
  m_selectedSolver = 0;
  SLIDER_SENSIBILITY=100.0;
  SLIDER_RANGE=500;
  /*********************************** Création des contrôles *************************************************/
  m_glBuffer = new wxGLBuffer( this, wxID_ANY, wxPoint(0,0), wxSize(800,800),attributelist, wxSUNKEN_BORDER );
  
  // Création d'un bouton. Ce bouton est associé à l'identifiant 
  // événement ID_Bt_Click, en consultant, la table des événements
  // on en déduit que c'est la fonction OnClickButton qui sera 
  // appelée lors d'un click sur ce bouton
  m_buttonRun = new wxButton(this,IDB_Run,_("Pause"));
  m_buttonRestart = new wxButton(this,IDB_Restart,_("Restart"));
  m_buttonSwap = new wxButton(this,IDB_Swap,_("Next IES file"));
  
  m_interpolatedSolidCheckBox = new wxCheckBox(this,IDCHK_IS,_("Interpolation"));
  m_blendedSolidCheckBox = new wxCheckBox(this,IDCHK_BS,_("Blended"));
  m_enableSolidCheckBox = new wxCheckBox(this,IDCHK_ES,_("Enabled"));
  m_glowEnabledCheckBox = new wxCheckBox(this,IDCHK_Glow,_("Enabled"));
  
  m_selectSolverLabel = new wxStaticText(this,IDT_SELECT,_("Selected solver :"));
  m_selectSolverComboBox = new wxComboBox(this,IDCB_SELECT,_(""),wxDefaultPosition,wxDefaultSize,0,wxCB_READONLY);

  m_solverXAxisPositionSlider = new wxSlider(this,IDSL_FXAP,0,-SLIDER_RANGE,SLIDER_RANGE, wxDefaultPosition, 
					    wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  m_solverYAxisPositionSlider = new wxSlider(this,IDSL_FYAP,0,-SLIDER_RANGE,SLIDER_RANGE, wxDefaultPosition,
					    wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  m_solverZAxisPositionSlider = new wxSlider(this,IDSL_FZAP,0,-SLIDER_RANGE,SLIDER_RANGE, wxDefaultPosition, 
					    wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  m_solverXAxisPositionLabel = new wxStaticText(this,IDST_FXAP,_("X"));
  m_solverYAxisPositionLabel = new wxStaticText(this,IDST_FYAP,_("Y"));
  m_solverZAxisPositionLabel = new wxStaticText(this,IDST_FZAP,_("Z"));
  m_solverXAxisPositionSliderMin = new wxTextCtrl(this,IDT_FXAPMIN,_(""),
						 wxDefaultPosition,wxSize(45,22),wxTE_PROCESS_ENTER);
  m_solverXAxisPositionSliderMax = new wxTextCtrl(this,IDT_FXAPMAX,_(""),
						 wxDefaultPosition,wxSize(45,22),wxTE_PROCESS_ENTER);
  m_solverYAxisPositionSliderMin = new wxTextCtrl(this,IDT_FYAPMIN,_(""),
						 wxDefaultPosition,wxSize(45,22),wxTE_PROCESS_ENTER);
  m_solverYAxisPositionSliderMax = new wxTextCtrl(this,IDT_FYAPMAX,_(""),
						 wxDefaultPosition,wxSize(45,22),wxTE_PROCESS_ENTER);
  m_solverZAxisPositionSliderMin = new wxTextCtrl(this,IDT_FZAPMIN,_(""),
						 wxDefaultPosition,wxSize(45,22),wxTE_PROCESS_ENTER);
  m_solverZAxisPositionSliderMax = new wxTextCtrl(this,IDT_FZAPMAX,_(""),
						 wxDefaultPosition,wxSize(45,22),wxTE_PROCESS_ENTER);

  (*m_solverXAxisPositionSliderMin) << -SLIDER_RANGE;
  (*m_solverXAxisPositionSliderMax) << SLIDER_RANGE;
  (*m_solverYAxisPositionSliderMin) << -SLIDER_RANGE;
  (*m_solverYAxisPositionSliderMax) << SLIDER_RANGE;
  (*m_solverZAxisPositionSliderMin) << -SLIDER_RANGE;
  (*m_solverZAxisPositionSliderMax) << SLIDER_RANGE;
  
  m_buttonFlickering = new wxButton(this,IDB_Flickering,_("Flickering"));      
  
  /* Réglages globaux */
  m_globalSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Global"));
  m_globalSizer->Add(m_buttonRun, 0, 0, 0);
  m_globalSizer->Add(m_buttonRestart, 0, 0, 0);
  
  /* Réglages du solide photométrique */
  m_solidSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Photometric solid"));
  m_solidSizer->Add(m_enableSolidCheckBox, 1, 0, 0);
  m_solidSizer->Add(m_interpolatedSolidCheckBox, 1, 0, 0);
  m_solidSizer->Add(m_blendedSolidCheckBox, 1, 0, 0);
  m_solidSizer->Add(m_buttonSwap, 1, 0, 0);
  
  m_topSizer = new wxBoxSizer(wxHORIZONTAL);
  m_topSizer->Add(m_globalSizer, 2, wxEXPAND, 0);
  m_topSizer->Add(m_solidSizer, 3, wxEXPAND, 0);
  
  /* Réglages du glow */
  m_glowSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Glow"));
  m_glowSizer->Add(m_glowEnabledCheckBox, 1, 0, 0);
  
  /* Réglages des flammes */
  m_solverSelectSizer = new wxBoxSizer(wxHORIZONTAL);
  m_solverSelectSizer->Add(m_selectSolverLabel,0,0,0);
  m_solverSelectSizer->Add(m_selectSolverComboBox,0,0,0);
  
  m_solversXAxisPositionSizer = new wxBoxSizer(wxHORIZONTAL);
  m_solversYAxisPositionSizer = new wxBoxSizer(wxHORIZONTAL);
  m_solversZAxisPositionSizer = new wxBoxSizer(wxHORIZONTAL);

  m_solversXAxisPositionSizer->Add(m_solverXAxisPositionLabel, 1, wxTOP|wxLEFT, 4);
  m_solversXAxisPositionSizer->Add(m_solverXAxisPositionSlider, 18, 0, 0);
  m_solversYAxisPositionSizer->Add(m_solverYAxisPositionLabel, 1, wxTOP|wxLEFT, 4);
  m_solversYAxisPositionSizer->Add(m_solverYAxisPositionSlider, 18, 0, 0);
  m_solversZAxisPositionSizer->Add(m_solverZAxisPositionLabel, 1, wxTOP|wxLEFT, 4);
  m_solversZAxisPositionSizer->Add(m_solverZAxisPositionSlider, 18, 0, 0);
  
  m_solversXAxisPositionRangeSizer = new wxBoxSizer(wxHORIZONTAL);
  m_solversYAxisPositionRangeSizer = new wxBoxSizer(wxHORIZONTAL);
  m_solversZAxisPositionRangeSizer = new wxBoxSizer(wxHORIZONTAL);

  m_solversXAxisPositionRangeSizer->Add(m_solverXAxisPositionSliderMin, 0, wxADJUST_MINSIZE|wxLEFT, 15);
  m_solversXAxisPositionRangeSizer->AddStretchSpacer(1);
  m_solversXAxisPositionRangeSizer->Add(m_solverXAxisPositionSliderMax, 0, wxADJUST_MINSIZE|wxRIGHT, 5);
  m_solversYAxisPositionRangeSizer->Add(m_solverYAxisPositionSliderMin, 0, wxADJUST_MINSIZE|wxLEFT, 15);
  m_solversYAxisPositionRangeSizer->AddStretchSpacer(1);
  m_solversYAxisPositionRangeSizer->Add(m_solverYAxisPositionSliderMax, 0, wxADJUST_MINSIZE|wxRIGHT, 5);
  m_solversZAxisPositionRangeSizer->Add(m_solverZAxisPositionSliderMin, 0, wxADJUST_MINSIZE|wxLEFT, 15);
  m_solversZAxisPositionRangeSizer->AddStretchSpacer(1);
  m_solversZAxisPositionRangeSizer->Add(m_solverZAxisPositionSliderMax, 0, wxADJUST_MINSIZE|wxRIGHT, 5);

  m_solversSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Flames settings"));
  m_solversSizer->Add(m_solverSelectSizer, 1, wxEXPAND, 0);
  m_solversSizer->Add(m_solversXAxisPositionSizer, 1, wxEXPAND, 0);
  m_solversSizer->Add(m_solversXAxisPositionRangeSizer, 1, wxEXPAND, 0);
  m_solversSizer->Add(m_solversYAxisPositionSizer, 1, wxEXPAND, 0);
  m_solversSizer->Add(m_solversYAxisPositionRangeSizer, 1, wxEXPAND, 0);
  m_solversSizer->Add(m_solversZAxisPositionSizer, 1, wxEXPAND, 0);
  m_solversSizer->Add(m_solversZAxisPositionRangeSizer, 1, wxEXPAND, 0);
  m_solversSizer->Add(m_buttonFlickering, 0, 0, 0);

  /* Placement des sizers principaux */
  m_rightSizer = new wxBoxSizer(wxVERTICAL);
  m_rightSizer->Add(m_topSizer, 0, wxEXPAND, 0);
  m_rightSizer->Add(m_glowSizer, 0, wxEXPAND, 0);
  m_rightSizer->Add(m_solversSizer, 0, wxEXPAND, 0);
  
  m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
  m_mainSizer->Add(m_glBuffer, 0, 0, 0);
  m_mainSizer->Add(m_rightSizer, 1, wxEXPAND, 0);
  
  SetSizer(m_mainSizer);
  
  m_menuFile = new wxMenu;
  
  m_menuFile->Append( IDM_OpenScene, _("&Open scene...") );
  m_menuFile->Append( IDM_SaveSettings, _("&Save settings") );
  m_menuFile->Append( IDM_About, _("&About...") );
  m_menuFile->AppendSeparator();
  m_menuFile->Append( IDM_Quit, _("E&xit") );
  
  m_menuDisplayFlames = new wxMenu;
  m_menuDisplayFlames->AppendCheckItem( IDM_Hide, _("&Hide"));
  m_menuDisplayFlames->AppendCheckItem( IDM_Wired, _("&Wired"));
  m_menuDisplayFlames->AppendCheckItem( IDM_Shaded, _("Shaded"));
  m_menuDisplayFlames->Check(IDM_Shaded,true);
  
  m_menuDisplay = new wxMenu;
  m_menuDisplay->AppendCheckItem( IDM_Grid, _("Grid"));
  m_menuDisplay->AppendCheckItem( IDM_Base, _("Base"));
  m_menuDisplay->AppendCheckItem( IDM_Velocity, _("Velocity"));
  m_menuDisplay->AppendCheckItem( IDM_Particles, _("Particles"));
  m_menuDisplay->Append( IDM_Flames, _("Flames"), m_menuDisplayFlames);
  
  m_menuSettings = new wxMenu;  
  m_menuSettings->Append( IDM_Solvers, _("&Solvers..."));

  m_menuBar = new wxMenuBar;
  m_menuBar->Append( m_menuFile, _("&File") );
  m_menuBar->Append( m_menuDisplay, _("&Display") );
  m_menuBar->Append( m_menuSettings, _("&Settings") );
  
  SetMenuBar( m_menuBar );
  
  CreateStatusBar();
  SetStatusText( _("FPS will be here...") );
}

void MainFrame::GetSettingsFromConfigFile (bool recompileShaders)
{
  wxFileInputStream* file = new wxFileInputStream( _("param.ini" ));
  m_config = new wxFileConfig( *file );
  
  m_currentConfig.width = m_config->Read(_("/Display/Width"), 800);
  m_currentConfig.height = m_config->Read(_("/Display/Height"), 800);
  m_currentConfig.clipping = m_config->Read(_("/Display/Clipping"), 100);
  m_config->Read(_("/Display/PhotometricSolid"), &m_currentConfig.PSEnabled, false);
  m_config->Read(_("/Display/IPSEnabled"), &m_currentConfig.IPSEnabled, 0);
  m_config->Read(_("/Display/BPSEnabled"), &m_currentConfig.BPSEnabled, 0);
  m_config->Read(_("/Display/Glow"), &m_currentConfig.glowEnabled, false);
  m_currentConfig.sceneName = m_config->Read(_("/Scene/FileName"), _("scene2.obj"));
  
  m_currentConfig.nbSolvers = m_config->Read(_("/Solvers/Number"), 1);
  m_currentConfig.solvers = new SolverConfig[m_currentConfig.nbSolvers];
  m_nbSolversMax = m_currentConfig.nbSolvers;

  wxString groupName,itemName;  
  for(int i=0; i < m_currentConfig.nbSolvers; i++)
    {
      groupName.Printf(_("/Solver%d/"),i);
      
      m_config->Read(groupName + _("Type"), (int *) &m_currentConfig.solvers[i].type, 1);

      m_config->Read(groupName + _("Pos.x"), &m_currentConfig.solvers[i].position.x, 0.0);
      m_config->Read(groupName + _("Pos.y"), &m_currentConfig.solvers[i].position.y, 0.0);
      m_config->Read(groupName + _("Pos.z"), &m_currentConfig.solvers[i].position.z, 0.0);

      m_currentConfig.solvers[i].resx = m_config->Read(groupName + _("X_res"), 15);
      m_currentConfig.solvers[i].resy = m_config->Read(groupName + _("Y_res"), 15);
      m_currentConfig.solvers[i].resz = m_config->Read(groupName + _("Z_res"), 15);

      m_config->Read(groupName + _("Dim"),&m_currentConfig.solvers[i].dim,1.0);
      m_config->Read(groupName + _("TimeStep"),&m_currentConfig.solvers[i].timeStep,0.4);
      
      itemName.Printf(_("Solver #%d"),i+1);
      m_selectSolverComboBox->Append(itemName);
    }

  m_currentConfig.nbFlames = m_config->Read(_("/Flames/Number"), 1);
  m_currentConfig.flames = new FlameConfig[m_currentConfig.nbFlames];
  m_nbFlamesMax = m_currentConfig.nbFlames;

  for(int i=0; i < m_currentConfig.nbFlames; i++)
    {
      groupName.Printf(_("/Flame%d/"),i);
      
      m_config->Read(groupName + _("Type"), (int *) &m_currentConfig.flames[i].type, 1);
      m_config->Read(groupName + _("Solver"), &m_currentConfig.flames[i].solverIndex, 0);
      m_config->Read(groupName + _("Pos.x"), &m_currentConfig.flames[i].position.x, 0.0);
      m_config->Read(groupName + _("Pos.y"), &m_currentConfig.flames[i].position.y, 0.0);
      m_config->Read(groupName + _("Pos.z"), &m_currentConfig.flames[i].position.z, 0.0);
      if(m_currentConfig.flames[i].type == FIRMALAMPE)
	m_currentConfig.flames[i].wickName = m_config->Read(groupName + _("WickFileName"), _("meche2.obj"));
    }
  ComputeSlidersValues();
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
  m_glBuffer->Init(&m_currentConfig,recompileShaders);
        
  delete file;

  return;
}

void MainFrame::ComputeSlidersValues(void)
{
  int valx = (int)(m_currentConfig.solvers[m_selectedSolver].position.x*SLIDER_SENSIBILITY);
  int valy = (int)(m_currentConfig.solvers[m_selectedSolver].position.y*SLIDER_SENSIBILITY);
  int valz = (int)(m_currentConfig.solvers[m_selectedSolver].position.z*SLIDER_SENSIBILITY);
  
  m_solverXAxisPositionSlider->SetValue(valx);
  m_solverYAxisPositionSlider->SetValue(valy);
  m_solverZAxisPositionSlider->SetValue(valz);
  m_solverXAxisPositionSlider->SetRange(valx-SLIDER_RANGE,valx+SLIDER_RANGE);
  m_solverYAxisPositionSlider->SetRange(valy-SLIDER_RANGE,valy+SLIDER_RANGE);
  m_solverZAxisPositionSlider->SetRange(valz-SLIDER_RANGE,valz+SLIDER_RANGE);

  m_solverXAxisPositionSliderMin->Clear();
  m_solverXAxisPositionSliderMax->Clear();
  m_solverYAxisPositionSliderMin->Clear();
  m_solverYAxisPositionSliderMax->Clear();
  m_solverZAxisPositionSliderMin->Clear();
  m_solverZAxisPositionSliderMax->Clear();
  
  (*m_solverXAxisPositionSliderMin) << valx-SLIDER_RANGE;
  (*m_solverXAxisPositionSliderMax) << valx+SLIDER_RANGE;
  (*m_solverYAxisPositionSliderMin) << valy-SLIDER_RANGE;
  (*m_solverYAxisPositionSliderMax) << valy+SLIDER_RANGE;
  (*m_solverZAxisPositionSliderMin) << valz-SLIDER_RANGE;
  (*m_solverZAxisPositionSliderMax) << valz+SLIDER_RANGE;
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

void MainFrame::OnSelectFlame(wxCommandEvent& event)
{
  cerr << "Selected : " << m_selectSolverComboBox->GetValue();
}

void MainFrame::OnScrollPosition(wxScrollEvent& event)
{
  CPoint pt(m_solverXAxisPositionSlider->GetValue()/SLIDER_SENSIBILITY,
	    m_solverYAxisPositionSlider->GetValue()/SLIDER_SENSIBILITY,
	    m_solverZAxisPositionSlider->GetValue()/SLIDER_SENSIBILITY);

  m_glBuffer->moveSolver(m_selectedSolver, pt);
  
  m_currentConfig.solvers[m_selectedSolver].position = pt;
}

void MainFrame::OnOpenSceneMenu(wxCommandEvent& event)
{
  wxString filename;
  
  wxFileDialog fileDialog(this, _("Choose a scene file"), _("scenes"), _(""), _("*.obj"), wxOPEN|wxFILE_MUST_EXIST);
  if(fileDialog.ShowModal() == wxID_OK){
    filename = fileDialog.GetFilename();
  
    cout << filename.fn_str() << endl;
    if(!filename.IsEmpty()){
      m_currentConfig.sceneName = filename;
      m_glBuffer->Restart();
    }
  }
}

void MainFrame::OnSaveSettingsMenu(wxCommandEvent& event)
{
  m_config->Write(_("/Display/Width"), m_currentConfig.width);
  m_config->Write(_("/Display/Height"), m_currentConfig.height);
  m_config->Write(_("/Display/Clipping"), m_currentConfig.clipping);
  m_config->Write(_("/Display/PhotometricSolid"), m_currentConfig.PSEnabled);
  m_config->Write(_("/Display/IPSEnabled"), m_currentConfig.IPSEnabled);
  m_config->Write(_("/Display/BPSEnabled"), m_currentConfig.BPSEnabled);
  m_config->Write(_("/Display/Glow"), m_currentConfig.glowEnabled);
  m_config->Write(_("/Scene/FileName"), m_currentConfig.sceneName);
  
  m_config->Write(_("/Solvers/Number"), m_currentConfig.nbSolvers);
  
  wxString groupName;
  for(int i=0; i < m_nbSolversMax; i++)
    {
      groupName.Printf(_("/Solver%d/"),i);

      m_config->DeleteGroup(groupName);
    }
 
  for(int i=0; i < m_currentConfig.nbSolvers; i++)
    {
      groupName.Printf(_("/Solver%d/"),i);
      
      m_config->Write(groupName + _("Type"), (int )m_currentConfig.solvers[i].type);

      m_config->Write(groupName + _("Pos.x"),m_currentConfig.solvers[i].position.x);
      m_config->Write(groupName + _("Pos.y"),m_currentConfig.solvers[i].position.y);
      m_config->Write(groupName + _("Pos.z"),m_currentConfig.solvers[i].position.z);

      m_config->Write(groupName + _("X_res"),m_currentConfig.solvers[i].resx);
      m_config->Write(groupName + _("Y_res"),m_currentConfig.solvers[i].resy);
      m_config->Write(groupName + _("Z_res"),m_currentConfig.solvers[i].resz);
      
      m_config->Write(groupName + _("Dim"),m_currentConfig.solvers[i].dim);
      m_config->Write(groupName + _("TimeStep"),m_currentConfig.solvers[i].timeStep);
    }
  
  m_config->Write(_("/Flames/Number"), m_currentConfig.nbFlames);

  for(int i=0; i < m_nbFlamesMax; i++)
    {
      groupName.Printf(_("/Flame%d/"),i);

      m_config->DeleteGroup(groupName);
    }
 
  for(int i=0; i < m_currentConfig.nbFlames; i++)
    {
      groupName.Printf(_("/Flame%d/"),i);       
      
      m_config->Write(groupName + _("Type"), (int )m_currentConfig.flames[i].type);
      m_config->Write(groupName + _("Solver"), m_currentConfig.flames[i].solverIndex);
      m_config->Write(groupName + _("Pos.x"),m_currentConfig.flames[i].position.x);
      m_config->Write(groupName + _("Pos.y"),m_currentConfig.flames[i].position.y);
      m_config->Write(groupName + _("Pos.z"),m_currentConfig.flames[i].position.z);
      if(m_currentConfig.flames[i].type == FIRMALAMPE)
	m_config->Write(groupName + _("WickFileName"),m_currentConfig.flames[i].wickName);
    }

  wxFileOutputStream* file = new wxFileOutputStream( _("param.ini" ));
  
  if (m_config->Save(*file) )  
    wxMessageBox(_("Configuration for the current simulation have been saved"),
		 _("Save settings"), wxOK | wxICON_INFORMATION, this);

  delete file;
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

void MainFrame::OnSolversMenu(wxCommandEvent& event)
{
  SolverDialog *solverDialog = new SolverDialog(GetParent(),-1,_("Solvers settings"));
  solverDialog->ShowModal();
}
void MainFrame::SetFPS(int fps)
{
  wxString s;
  s += wxString::Format(_("%d FPS"), fps);
  
  SetStatusText(s);
}

void MainFrame::OnFXAPMINEnter(wxCommandEvent& event)
{
  long val;
  m_solverXAxisPositionSliderMin->GetValue().ToLong(&val);
  m_solverXAxisPositionSlider->SetRange(val, m_solverXAxisPositionSlider->GetMax());
}

void MainFrame::OnFXAPMAXEnter(wxCommandEvent& event)
{
  long val;
  m_solverXAxisPositionSliderMax->GetValue().ToLong(&val);
  m_solverXAxisPositionSlider->SetRange(m_solverXAxisPositionSlider->GetMin(), val);
}

void MainFrame::OnFYAPMINEnter(wxCommandEvent& event)
{
  long val;
  m_solverYAxisPositionSliderMin->GetValue().ToLong(&val);
  m_solverYAxisPositionSlider->SetRange(val, m_solverYAxisPositionSlider->GetMax());
}

void MainFrame::OnFYAPMAXEnter(wxCommandEvent& event)
{
 long val;
  m_solverYAxisPositionSliderMax->GetValue().ToLong(&val);
  m_solverYAxisPositionSlider->SetRange(m_solverYAxisPositionSlider->GetMin(), val);
}

void MainFrame::OnFZAPMINEnter(wxCommandEvent& event)
{
  long val;
  m_solverZAxisPositionSliderMin->GetValue().ToLong(&val);
  m_solverZAxisPositionSlider->SetRange(val, m_solverZAxisPositionSlider->GetMax());
}

void MainFrame::OnFZAPMAXEnter(wxCommandEvent& event)
{
  long val;
  m_solverZAxisPositionSliderMax->GetValue().ToLong(&val);
  m_solverZAxisPositionSlider->SetRange(m_solverZAxisPositionSlider->GetMin(), val);
}

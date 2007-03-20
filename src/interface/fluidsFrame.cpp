#include "fluidsFrame.hpp"

#include "solverDialog.hpp"

// Déclarations de la table des événements
// Sorte de relation qui lit des identifiants d'événements aux fonctions
BEGIN_EVENT_TABLE(FluidsFrame, wxFrame)
  EVT_SIZE(FluidsFrame::OnSize)
  EVT_BUTTON(IDB_Run, FluidsFrame::OnClickButtonRun)
  EVT_BUTTON(IDB_Restart, FluidsFrame::OnClickButtonRestart)
  EVT_MENU(IDM_LoadParam, FluidsFrame::OnLoadParamMenu)
  EVT_MENU(IDM_SaveSettings, FluidsFrame::OnSaveSettingsMenu)
  EVT_MENU(IDM_SaveSettingsAs, FluidsFrame::OnSaveSettingsAsMenu)
  EVT_MENU(IDM_Quit, FluidsFrame::OnQuitMenu)
  EVT_MENU(IDM_About, FluidsFrame::OnAboutMenu)
  EVT_MENU(IDM_Grid, FluidsFrame::OnGridMenu)
  EVT_MENU(IDM_Base, FluidsFrame::OnBaseMenu)
  EVT_MENU(IDM_Velocity, FluidsFrame::OnVelocityMenu)
  EVT_MENU(IDM_Density, FluidsFrame::OnDensityMenu)
  EVT_MENU(IDM_SolversSettings, FluidsFrame::OnSolversMenu)
  EVT_CHECKBOX(IDCHK_SaveImages, FluidsFrame::OnCheckSaveImages)
  EVT_CLOSE(FluidsFrame::OnClose)
END_EVENT_TABLE();

/**************************************** FluidsFrame Class methods **************************************/

FluidsFrame::FluidsFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const wxString& configFileName)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  int attributelist[ 10 ] = { WX_GL_RGBA,
			      WX_GL_DOUBLEBUFFER,
			      WX_GL_STENCIL_SIZE,
			      1                 ,
			      0                  };
  
  /*********************************** Création des contrôles *************************************************/
  // Création d'un bouton. Ce bouton est associé à l'identifiant 
  // événement ID_Bt_Click, en consultant, la table des événements
  // on en déduit que c'est la fonction OnClickButton qui sera 
  // appelée lors d'un click sur ce bouton
  m_glBuffer = new GLFluidsCanvas( this, wxID_ANY, wxPoint(0,0), wxSize(1024,768),attributelist, wxSUNKEN_BORDER );
    
  m_buttonRun = new wxButton(this,IDB_Run,_("Pause"));
  m_buttonRestart = new wxButton(this,IDB_Restart,_("Restart"));
  
  m_saveImagesCheckBox =  new wxCheckBox(this,IDCHK_SaveImages,_("Save Images"));
  
  m_solversNotebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
  
  /* Réglages globaux */
  m_globalTopSizer = new wxBoxSizer(wxHORIZONTAL);
  m_globalTopSizer->Add(m_buttonRun, 0, 0, 0);
  m_globalTopSizer->Add(m_buttonRestart, 0, 0, 0);
  
  m_globalSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Global"));
  m_globalSizer->Add(m_globalTopSizer, 0, 0, 0);
  m_globalSizer->Add(m_saveImagesCheckBox, 0, 0, 0);
  
  m_solversSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Solvers settings"));
  m_solversSizer->Add(m_solversNotebook, 1, wxEXPAND, 0);
  
  /* Placement des sizers principaux */
  m_rightSizer = new wxBoxSizer(wxVERTICAL);
  m_rightSizer->Add(m_globalSizer, 0, wxEXPAND, 0);
  m_rightSizer->Add(m_solversSizer, 0, wxEXPAND, 0);
  
  m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
  m_mainSizer->Add(m_glBuffer, 0, 0, 0);
  m_mainSizer->Add(m_rightSizer, 1, 0, 0);
  
  /* Création des menus */
  m_menuFile = new wxMenu;
  
  m_menuFile->Append( IDM_LoadParam, _("&Load simulation file...") );
  m_menuFile->Append( IDM_SaveSettings, _("&Save settings") );
  m_menuFile->Append( IDM_SaveSettingsAs, _("&Save settings as...") );
  m_menuFile->Append( IDM_About, _("&About...") );
  m_menuFile->AppendSeparator();
  m_menuFile->Append( IDM_Quit, _("E&xit") );
  
  m_menuDisplay = new wxMenu;
  m_menuDisplay->AppendCheckItem( IDM_Grid, _("&Grid"));
  m_menuDisplay->AppendCheckItem( IDM_Base, _("&Base"));
  m_menuDisplay->AppendCheckItem( IDM_Velocity, _("&Velocity"));
  m_menuDisplay->AppendCheckItem( IDM_Density, _("&Density"));
  
  m_menuDisplay->Check(IDM_Base,true);
  m_menuDisplay->Check(IDM_Velocity,true);
  m_menuDisplay->Check(IDM_Density,true);

  m_menuSettings = new wxMenu;
  m_menuSettings->Append( IDM_SolversSettings, _("&Solvers..."));
  
  m_menuBar = new wxMenuBar;
  m_menuBar->Append( m_menuFile, _("&File") );
  m_menuBar->Append( m_menuDisplay, _("&Display") );
  m_menuBar->Append( m_menuSettings, _("&Settings") );
  
  SetMenuBar( m_menuBar );
  
  m_configFileName = configFileName;
  GetSettingsFromConfigFile();
  
  SetSizerAndFit(m_mainSizer);
  
  CreateStatusBar();
  SetStatusText( _("FPS will be here...") );
}

void FluidsFrame::OnSize(wxSizeEvent& event)
{
  // this is also necessary to update the context on some platforms
  wxFrame::OnSize(event);
  Layout();
}

void FluidsFrame::GetSettingsFromConfigFile (void)
{
  wxFileInputStream file( m_configFileName );
  //if(!wxFileInputStream::Ok())
  
  m_config = new wxFileConfig( file );
  
  m_currentConfig.width = m_config->Read(_("/Display/Width"), 1024);
  m_currentConfig.height = m_config->Read(_("/Display/Height"), 768);
  m_currentConfig.clipping = m_config->Read(_("/Display/Clipping"), 100);
    
  m_currentConfig.nbSolvers = m_config->Read(_("/Solvers/Number"), 1);
  m_currentConfig.solvers = new SolverConfig[m_currentConfig.nbSolvers];
  m_nbSolversMax = m_currentConfig.nbSolvers;
  
  wxString groupName,tabName;
  for(uint i=0; i < m_currentConfig.nbSolvers; i++)
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
      m_config->Read(groupName + _("Scale.x"),&m_currentConfig.solvers[i].scale.x,1.0);
      m_config->Read(groupName + _("Scale.y"),&m_currentConfig.solvers[i].scale.y,1.0);
      m_config->Read(groupName + _("Scale.z"),&m_currentConfig.solvers[i].scale.z,1.0);
            
      m_config->Read(groupName + _("TimeStep"),&m_currentConfig.solvers[i].timeStep,0.4);      
      m_config->Read(groupName + _("Buoyancy"), &m_currentConfig.solvers[i].buoyancy, 0.02);
      
      m_config->Read(groupName + _("omegaDiff"),&m_currentConfig.solvers[i].omegaDiff,1.5);
      m_config->Read(groupName + _("omegaProj"),&m_currentConfig.solvers[i].omegaProj,1.5);
      m_config->Read(groupName + _("epsilon"),&m_currentConfig.solvers[i].epsilon,0.00001);
      m_currentConfig.solvers[i].nbMaxIter = m_config->Read(groupName + _("nbMaxIter"), 100);
      
      tabName.Printf(_("Solver #%d"),i+1);
      
      m_solverPanels[i] = new SolverMainPanel(m_solversNotebook, -1, &m_currentConfig.solvers[i], i, m_glBuffer);
      m_solversNotebook->AddPage(m_solverPanels[i], tabName);
    }
  
  return;
}

void FluidsFrame::InitGLBuffer(bool recompileShaders)
{
  m_glBuffer->SetSize(wxSize(m_currentConfig.width,m_currentConfig.height));
  m_glBuffer->Init(&m_currentConfig,recompileShaders);
  m_mainSizer->Fit(this);
  m_mainSizer->SetSizeHints(this);
  Layout();
}

void FluidsFrame::InitSolversPanels()
{
  wxString tabName;
  
  m_solversNotebook->DeleteAllPages();
  
  for(int unsigned i=0; i < m_currentConfig.nbSolvers; i++)
    {
      m_solverPanels[i] = new SolverMainPanel(m_solversNotebook, -1, &m_currentConfig.solvers[i], i, m_glBuffer);	
      tabName.Printf(_("Solver #%d"),i+1);
      m_solversNotebook->AddPage(m_solverPanels[i], tabName);
    }
}

void FluidsFrame::OnClose(wxCloseEvent& event)
{
  delete [] m_currentConfig.solvers;
  delete m_config;
  
  Destroy();
}

// Fonction qui est exécutée lors du click sur le bouton.
void FluidsFrame::OnClickButtonRun(wxCommandEvent& event)
{
  if(m_glBuffer->IsRunning()){
    m_buttonRun->SetLabel(_("Continue"));
    m_glBuffer->setRunningState(false);
  }else{
    m_buttonRun->SetLabel(_("Pause"));
    m_glBuffer->setRunningState(true);
  }
}

// Fonction qui est exécutée lors du click sur le bouton.
void FluidsFrame::OnClickButtonRestart(wxCommandEvent& event)
{
  m_glBuffer->Restart();
}

void FluidsFrame::OnCheckSaveImages(wxCommandEvent& event)
{
  m_glBuffer->ToggleSaveImages();  
}

void FluidsFrame::OnLoadParamMenu(wxCommandEvent& event)
{
  wxString filename;
  wxString pwd=wxGetCwd();
  pwd << PARAMS_DIRECTORY;
  
  wxFileDialog fileDialog(this, _("Choose a simulation file"), pwd, _(""), _("*.slv"), wxOPEN|wxFILE_MUST_EXIST);
  if(fileDialog.ShowModal() == wxID_OK){
    filename = fileDialog.GetPath();
    
    if(!filename.IsEmpty()){
      /* Récupération le chemin absolu vers la scène */
      filename.Replace(wxGetCwd(),_(""),false);
      /* Suppression du premier slash */
      filename=filename.Mid(1);

      m_configFileName = filename;
      
      SetTitle(_("Real-time Fluids - ") + m_configFileName);

      delete [] m_currentConfig.solvers;
      m_solversNotebook->DeleteAllPages();
      
      GetSettingsFromConfigFile();
      m_glBuffer->Restart();
      m_glBuffer->SetSize(wxSize(m_currentConfig.width,m_currentConfig.height));
      m_mainSizer->Fit(this);
      m_mainSizer->SetSizeHints(this);
      Layout();
    }
  }
}

void FluidsFrame::OnSaveSettingsMenu(wxCommandEvent& event)
{
  m_config->Write(_("/Display/Width"), (int)m_currentConfig.width);
  m_config->Write(_("/Display/Height"), (int)m_currentConfig.height);
  m_config->Write(_("/Display/Clipping"), m_currentConfig.clipping);
  
  m_config->Write(_("/Solvers/Number"), (int)m_currentConfig.nbSolvers);
  
  wxString groupName;
  for(uint i=0; i < m_nbSolversMax; i++)
    {
      groupName.Printf(_("/Solver%d/"),i);

      m_config->DeleteGroup(groupName);
    }
 
  for(uint i=0; i < m_currentConfig.nbSolvers; i++)
    {
      groupName.Printf(_("/Solver%d/"),i);
      
      m_config->Write(groupName + _("Type"), (int)m_currentConfig.solvers[i].type);
      
      m_config->Write(groupName + _("Pos.x"),m_currentConfig.solvers[i].position.x);
      m_config->Write(groupName + _("Pos.y"),m_currentConfig.solvers[i].position.y);
      m_config->Write(groupName + _("Pos.z"),m_currentConfig.solvers[i].position.z);
      
      m_config->Write(groupName + _("X_res"),(int)m_currentConfig.solvers[i].resx);
      m_config->Write(groupName + _("Y_res"),(int)m_currentConfig.solvers[i].resy);
      m_config->Write(groupName + _("Z_res"),(int)m_currentConfig.solvers[i].resz);
      
      m_config->Write(groupName + _("Dim"),m_currentConfig.solvers[i].dim);
      m_config->Write(groupName + _("Scale.x"),m_currentConfig.solvers[i].scale.x);
      m_config->Write(groupName + _("Scale.y"),m_currentConfig.solvers[i].scale.y);
      m_config->Write(groupName + _("Scale.z"),m_currentConfig.solvers[i].scale.z);
      
      m_config->Write(groupName + _("TimeStep"),m_currentConfig.solvers[i].timeStep);      
      m_config->Write(groupName + _("Buoyancy"), m_currentConfig.solvers[i].buoyancy);
      
      m_config->Write(groupName + _("omegaDiff"),m_currentConfig.solvers[i].omegaDiff);
      m_config->Write(groupName + _("omegaProj"),m_currentConfig.solvers[i].omegaProj);
      m_config->Write(groupName + _("epsilon"),m_currentConfig.solvers[i].epsilon);
      
      m_config->Write(groupName + _("nbMaxIter"),(int)m_currentConfig.solvers[i].nbMaxIter);
    }
  
  wxFileOutputStream file ( m_configFileName );
  
  if (m_config->Save(file) )
    wxMessageBox(_("Configuration for the current simulation have been saved"),
		 _("Save settings"), wxOK | wxICON_INFORMATION, this);
}

void FluidsFrame::OnSaveSettingsAsMenu(wxCommandEvent& event)
{
  wxString filename;
  wxString pwd=wxGetCwd();
  pwd << PARAMS_DIRECTORY;
  
  wxFileDialog fileDialog(this, _("Enter a simulation file"), pwd, _(""), _("*.ini"), wxSAVE|wxOVERWRITE_PROMPT);
  if(fileDialog.ShowModal() == wxID_OK){
    filename = fileDialog.GetPath();
    /* Récupération le chemin absolu vers la scène */
    filename.Replace(wxGetCwd(),_(""),false);
    /* Suppression du premier slash */
    filename=filename.Mid(1);
  
    if(!filename.IsEmpty()){
      m_configFileName = filename;
      
      SetTitle(_("Real-time Fluids - ") + m_configFileName);
      OnSaveSettingsMenu(event);
    }
  }
}

void FluidsFrame::OnQuitMenu(wxCommandEvent& WXUNUSED(event))
{
  Close(TRUE);
}

void FluidsFrame::OnAboutMenu(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(_("Real-time simulation of flames\nOasis Team"),
	       _("About flames"), wxOK | wxICON_INFORMATION, this);
}

void FluidsFrame::OnGridMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleGridDisplay();
}

void FluidsFrame::OnBaseMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleBaseDisplay();
}

void FluidsFrame::OnVelocityMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleVelocityDisplay();
}

void FluidsFrame::OnDensityMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleDensityDisplay();
}

void FluidsFrame::OnSolversMenu(wxCommandEvent& event)
{
  SolverDialog solverDialog (GetParent(),-1,_("Solvers settings"),&m_currentConfig);
  if(solverDialog.ShowModal() == wxID_OK){
    InitSolversPanels();
    m_glBuffer->Restart();
  }
}

void FluidsFrame::SetFPS(int fps)
{
  wxString s;
  s += wxString::Format(_("%d FPS"), fps);
  
  SetStatusText(s);
}

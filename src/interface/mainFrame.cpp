#include "mainFrame.hpp"

#include "solverDialog.hpp"
#include "flameDialog.hpp"
#include "shadowsDialog.hpp"

// Déclarations de la table des événements
// Sorte de relation qui lit des identifiants d'événements aux fonctions
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_SIZE(MainFrame::OnSize)
  EVT_RADIOBOX(IDRB_Lighting, MainFrame::OnSelectLighting)
  EVT_BUTTON(IDB_Run, MainFrame::OnClickButtonRun)
  EVT_BUTTON(IDB_Restart, MainFrame::OnClickButtonRestart)
  EVT_MENU(IDM_LoadParam, MainFrame::OnLoadParamMenu)
  EVT_MENU(IDM_OpenScene, MainFrame::OnOpenSceneMenu)
  EVT_MENU(IDM_SaveSettings, MainFrame::OnSaveSettingsMenu)
  EVT_MENU(IDM_SaveSettingsAs, MainFrame::OnSaveSettingsAsMenu)
  EVT_MENU(IDM_Quit, MainFrame::OnQuitMenu)
  EVT_MENU(IDM_About, MainFrame::OnAboutMenu)
  EVT_MENU(IDM_GlowOnly, MainFrame::OnGlowOnlyMenu)
  EVT_MENU(IDM_Grid, MainFrame::OnGridMenu)
  EVT_MENU(IDM_Base, MainFrame::OnBaseMenu)
  EVT_MENU(IDM_Velocity, MainFrame::OnVelocityMenu)
  EVT_MENU(IDM_Particles, MainFrame::OnParticlesMenu)
  EVT_MENU(IDM_WickBoxes, MainFrame::OnWickBoxesMenu)
  EVT_MENU(IDM_ShadowVolumes, MainFrame::OnShadowVolumesMenu)
  EVT_MENU(IDM_Hide, MainFrame::OnHideMenu)
  EVT_MENU(IDM_Wired, MainFrame::OnWiredMenu)
  EVT_MENU(IDM_Shaded, MainFrame::OnShadedMenu)
  EVT_MENU(IDM_SolversSettings, MainFrame::OnSolversMenu)
  EVT_MENU(IDM_FlamesSettings, MainFrame::OnFlamesMenu)
  EVT_MENU(IDM_ShadowVolumesSettings, MainFrame::OnShadowVolumesSettingsMenu)
  EVT_CHECKBOX(IDCHK_BS, MainFrame::OnCheckBS)
  EVT_CHECKBOX(IDCHK_Shadows, MainFrame::OnCheckShadows)
  EVT_CHECKBOX(IDCHK_Glow, MainFrame::OnCheckGlow)
  EVT_CHECKBOX(IDCHK_DP, MainFrame::OnCheckDepthPeeling)
  EVT_CHECKBOX(IDCHK_SaveImages, MainFrame::OnCheckSaveImages)
  EVT_SCROLL(MainFrame::OnScrollPosition)
  EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE();

/**************************************** MainFrame Class methods **************************************/

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const wxString& configFileName)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  int attributelist[ 10 ] = { WX_GL_RGBA,
			      WX_GL_DOUBLEBUFFER,
			      WX_GL_STENCIL_SIZE,
			      1                 ,
			      0                  };
  
  const wxString m_lightingChoices[] = {
    _("Standard"),
    _("Photometric Solid")
  };
  /*********************************** Création des contrôles *************************************************/
  // Création d'un bouton. Ce bouton est associé à l'identifiant 
  // événement ID_Bt_Click, en consultant, la table des événements
  // on en déduit que c'est la fonction OnClickButton qui sera 
  // appelée lors d'un click sur ce bouton
  m_glBuffer = new GLFlameCanvas( this, wxID_ANY, wxPoint(0,0), wxSize(1024,768),attributelist, wxSUNKEN_BORDER );
   
  m_lightingRadioBox = new wxRadioBox(this, IDRB_Lighting, _("Lighting"), wxDefaultPosition, wxDefaultSize, 
				      2, m_lightingChoices, 2, wxRA_SPECIFY_COLS);
  
  m_buttonRun = new wxButton(this,IDB_Run,_("Pause"));
  m_buttonRestart = new wxButton(this,IDB_Restart,_("Restart"));
  
//   m_interpolatedSolidCheckBox = new wxCheckBox(this,IDCHK_IS,_("Interpolation"));
  m_blendedSolidCheckBox = new wxCheckBox(this,IDCHK_BS,_("Blended"));
  m_shadowsEnabledCheckBox = new wxCheckBox(this,IDCHK_Shadows,_("Shadows"));
  m_glowEnabledCheckBox = new wxCheckBox(this,IDCHK_Glow,_("Glow"));
  m_depthPeelingEnabledCheckBox = new wxCheckBox(this,IDCHK_DP,_("Depth Peeling"));
  m_depthPeelingSlider = new wxSlider(this,IDSL_DP,0,0,DEPTH_PEELING_LAYERS_MAX, wxDefaultPosition, 
				      wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  m_saveImagesCheckBox =  new wxCheckBox(this,IDCHK_SaveImages,_("Save Images"));
  
  m_solversNotebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
  m_flamesNotebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
  
  /* Réglages globaux */
  m_globalSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Global"));
  m_globalSizer->Add(m_buttonRun, 0, 0, 0);
  m_globalSizer->Add(m_buttonRestart, 0, 0, 0);
  m_globalSizer->Add(m_saveImagesCheckBox, 0, 0, 0);
  
  /* Réglages du solide photométrique */
  m_solidSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Photometric solid"));
//   m_solidSizer->Add(m_interpolatedSolidCheckBox, 1, 0, 0);
  m_solidSizer->Add(m_blendedSolidCheckBox, 1, 0, 0);
  
  m_topSizer = new wxBoxSizer(wxHORIZONTAL);
  m_topSizer->Add(m_globalSizer, 2, wxEXPAND, 0);
  m_topSizer->Add(m_solidSizer, 2, wxEXPAND, 0);
  
  /* Réglages du glow */
  m_multiSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Multi-pass Rendering"));
  m_multiTopSizer = new wxBoxSizer(wxHORIZONTAL);
  m_multiTopSizer->Add(m_shadowsEnabledCheckBox, 1, 0, 0);
  m_multiTopSizer->Add(m_glowEnabledCheckBox, 1, 0, 0);
  m_multiSizer->Add(m_multiTopSizer, 1, 0, 0);
  m_multiSizer->Add(m_depthPeelingEnabledCheckBox, 1, 0, 0);
  m_multiSizer->Add(m_depthPeelingSlider, 1, wxEXPAND, 0);
    
  m_solversSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Solvers settings"));
  m_solversSizer->Add(m_solversNotebook, 1, wxEXPAND, 0);

  m_flamesSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Flames settings"));
  m_flamesSizer->Add(m_flamesNotebook, 1, wxEXPAND, 0);
  
  /* Placement des sizers principaux */
  m_rightSizer = new wxBoxSizer(wxVERTICAL);
  m_rightSizer->Add(m_lightingRadioBox, 0, wxEXPAND, 0);
  m_rightSizer->Add(m_topSizer, 0, wxEXPAND, 0);
  m_rightSizer->Add(m_multiSizer, 0, wxEXPAND, 0);
  m_rightSizer->Add(m_solversSizer, 0, wxEXPAND, 0);
  m_rightSizer->Add(m_flamesSizer, 0, wxEXPAND, 0);  
  
  /* Création des menus */
  m_menuFile = new wxMenu;
  
  m_menuFile->Append( IDM_LoadParam, _("&Load simulation file...") );
  m_menuFile->Append( IDM_OpenScene, _("&Open scene...") );
  m_menuFile->Append( IDM_SaveSettings, _("&Save settings") );
  m_menuFile->Append( IDM_SaveSettingsAs, _("&Save settings as...") );
  m_menuFile->Append( IDM_About, _("&About...") );
  m_menuFile->AppendSeparator();
  m_menuFile->Append( IDM_Quit, _("E&xit") );
  
  m_menuDisplayFlames = new wxMenu;
  m_menuDisplayFlames->AppendCheckItem( IDM_Hide, _("&Hide"));
  m_menuDisplayFlames->AppendCheckItem( IDM_Wired, _("&Wired"));
  m_menuDisplayFlames->AppendCheckItem( IDM_Shaded, _("&Shaded"));
  m_menuDisplayFlames->Check(IDM_Shaded,true);
  
  m_menuDisplay = new wxMenu;
  m_menuDisplay->AppendCheckItem( IDM_Grid, _("&Grid"));
  m_menuDisplay->AppendCheckItem( IDM_Base, _("&Base"));
  m_menuDisplay->AppendCheckItem( IDM_Velocity, _("&Velocity"));
  m_menuDisplay->AppendCheckItem( IDM_Particles, _("&Particles"));
  m_menuDisplay->AppendCheckItem( IDM_WickBoxes, _("&Wick Boxes"));
  m_menuDisplay->Append( IDM_Flames, _("&Flames"), m_menuDisplayFlames);
  m_menuDisplay->AppendCheckItem( IDM_ShadowVolumes, _("&Shadow Volumes"));  
  m_menuDisplay->AppendCheckItem( IDM_GlowOnly, _("&Glow only"));
  
  m_menuSettings = new wxMenu;
  m_menuSettings->Append( IDM_SolversSettings, _("&Solvers..."));
  m_menuSettings->Append( IDM_FlamesSettings, _("&Flames..."));
  m_menuSettings->Append( IDM_ShadowVolumesSettings, _("S&hadows..."));
  
  m_menuBar = new wxMenuBar;
  m_menuBar->Append( m_menuFile, _("&File") );
  m_menuBar->Append( m_menuDisplay, _("&Display") );
  m_menuBar->Append( m_menuSettings, _("&Settings") );
  
  SetMenuBar( m_menuBar );
  
  m_configFileName = configFileName;
  GetSettingsFromConfigFile();

  m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
  m_mainSizer->Add(m_glBuffer, 0, 0, 0);
  m_mainSizer->Add(m_rightSizer, 1, 0, 0);
  
  SetSizerAndFit(m_mainSizer);
  
  CreateStatusBar();
  SetStatusText( _("FPS will be here...") );

}

void MainFrame::OnSize(wxSizeEvent& event)
{
  // this is also necessary to update the context on some platforms
  wxFrame::OnSize(event);
  Layout();
}

void MainFrame::GetSettingsFromConfigFile (void)
{
  wxFileInputStream* file = new wxFileInputStream( m_configFileName );
  //if(!wxFileInputStream::Ok())
    
  m_config = new wxFileConfig( *file );
  
  m_currentConfig.width = m_config->Read(_("/Display/Width"), 1024);
  m_currentConfig.height = m_config->Read(_("/Display/Height"), 768);
  m_currentConfig.clipping = m_config->Read(_("/Display/Clipping"), 100);
  m_config->Read(_("/Display/LightingMode"), &m_currentConfig.lightingMode, LIGHTING_STANDARD);
  m_config->Read(_("/Display/BPSEnabled"), &m_currentConfig.BPSEnabled, 0);
  m_config->Read(_("/Display/Shadows"), &m_currentConfig.shadowsEnabled, false);
  m_config->Read(_("/Display/Glow"), &m_currentConfig.glowEnabled, false);
  m_config->Read(_("/Display/DepthPeeling"), &m_currentConfig.depthPeelingEnabled, false);
  m_config->Read(_("/Display/NbDepthPeelingLayers"), (int *) &m_currentConfig.nbDepthPeelingLayers, 4);
  m_currentConfig.sceneName = m_config->Read(_("/Scene/FileName"), _("scene2.obj"));
  
  m_config->Read(_("/Shadows/Fatness.x"), &m_currentConfig.fatness[0], -.001);
  m_config->Read(_("/Shadows/Fatness.y"), &m_currentConfig.fatness[1], -.001);
  m_config->Read(_("/Shadows/Fatness.z"), &m_currentConfig.fatness[2], -.001);
  m_config->Read(_("/Shadows/ExtrudeDist.x"), &m_currentConfig.extrudeDist[0], 5);
  m_config->Read(_("/Shadows/ExtrudeDist.y"), &m_currentConfig.extrudeDist[1], 5);
  m_config->Read(_("/Shadows/ExtrudeDist.z"), &m_currentConfig.extrudeDist[2], 5);
  m_currentConfig.fatness[3] = 0;
  m_currentConfig.extrudeDist[3] = 0;
  
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
  
  m_currentConfig.nbFlames = m_config->Read(_("/Flames/Number"), 1);
  m_currentConfig.flames = new FlameConfig[m_currentConfig.nbFlames];
  m_nbFlamesMax = m_currentConfig.nbFlames;
  
  for(uint i=0; i < m_currentConfig.nbFlames; i++)
    {
      groupName.Printf(_("/Flame%d/"),i);
      
      m_config->Read(groupName + _("Type"), (int *) &m_currentConfig.flames[i].type, 1);
      m_config->Read(groupName + _("Solver"), &m_currentConfig.flames[i].solverIndex, 0);
      m_config->Read(groupName + _("Pos.x"), &m_currentConfig.flames[i].position.x, 0.0);
      m_config->Read(groupName + _("Pos.y"), &m_currentConfig.flames[i].position.y, 0.0);
      m_config->Read(groupName + _("Pos.z"), &m_currentConfig.flames[i].position.z, 0.0);
      if(m_currentConfig.flames[i].type != CANDLE){
	m_currentConfig.flames[i].wickName = m_config->Read(groupName + _("WickFileName"), _("meche2.obj"));
	m_config->Read(groupName + _("SkeletonsNumber"), (int *) &m_currentConfig.flames[i].skeletonsNumber, 5);
	m_config->Read(groupName + _("InnerForce"), &m_currentConfig.flames[i].innerForce, 0.005);
      }else{
	m_config->Read(groupName + _("SkeletonsNumber"), (int *) &m_currentConfig.flames[i].skeletonsNumber, 4);
	m_config->Read(groupName + _("InnerForce"), &m_currentConfig.flames[i].innerForce, 0.04);
      }
      m_config->Read(groupName + _("Flickering"), (int *) &m_currentConfig.flames[i].flickering, 0);
      m_config->Read(groupName + _("FDF"), (int *) &m_currentConfig.flames[i].fdf, 0);
      m_config->Read(groupName + _("SamplingTolerance"), &m_currentConfig.flames[i].samplingTolerance, 100);
      m_config->Read(groupName + _("intensityCoef"), &m_currentConfig.flames[i].intensityCoef, 5);
      m_config->Read(groupName + _("nbLeadParticles"), (int *) &m_currentConfig.flames[i].leadLifeSpan, 8);
      m_config->Read(groupName + _("nbPeriParticles"), (int *) &m_currentConfig.flames[i].periLifeSpan, 6);
      m_currentConfig.flames[i].IESFileName = m_config->Read(groupName + _("IESFileName"), _("IES/test.ies"));
      
      tabName.Printf(_("Flame #%d"),i+1);
      
      m_flamePanels[i] = new FlameMainPanel(m_flamesNotebook, -1, &m_currentConfig.flames[i], i, m_glBuffer);      
      m_flamesNotebook->AddPage(m_flamePanels[i], tabName);
    }
  
  m_blendedSolidCheckBox->SetValue(m_currentConfig.BPSEnabled);
  m_lightingRadioBox->SetSelection(m_currentConfig.lightingMode);
  m_glowEnabledCheckBox->SetValue(m_currentConfig.glowEnabled);
  m_shadowsEnabledCheckBox->SetValue(m_currentConfig.shadowsEnabled);
  m_depthPeelingEnabledCheckBox->SetValue(m_currentConfig.depthPeelingEnabled);
//   if(m_currentConfig.depthPeelingEnabled)
//     m_depthPeelingSlider->Enable();
//   else
    m_depthPeelingSlider->Disable();
  m_depthPeelingSlider->SetValue(m_currentConfig.nbDepthPeelingLayers);
  switch(m_currentConfig.lightingMode)
    {
    case LIGHTING_STANDARD : 
      m_menuDisplayFlames->Enable(IDM_ShadowVolumes,false);
      m_blendedSolidCheckBox->Disable();
      break;
    case LIGHTING_PHOTOMETRIC :
      m_menuDisplayFlames->Enable(IDM_ShadowVolumes,false);
      m_blendedSolidCheckBox->Enable();
      break;
    }
  
  delete file;
  
  return;
}

void MainFrame::InitGLBuffer(bool recompileShaders)
{
  m_glBuffer->SetSize(wxSize(m_currentConfig.width,m_currentConfig.height));
  m_glBuffer->Init(&m_currentConfig,recompileShaders);
  m_glBuffer->setNbDepthPeelingLayers(m_currentConfig.nbDepthPeelingLayers);
  m_mainSizer->Fit(this);
  m_mainSizer->SetSizeHints(this);
  Layout();
}

void MainFrame::InitSolversPanels()
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


void MainFrame::InitFlamesPanels()
{
  wxString tabName;
  
  m_flamesNotebook->DeleteAllPages();
  
  for(int unsigned i=0; i < m_currentConfig.nbFlames; i++)
    {
      m_flamePanels[i] = new FlameMainPanel(m_flamesNotebook, -1, &m_currentConfig.flames[i], i, m_glBuffer);     
      tabName.Printf(_("Flame #%d"),i+1);       
      m_flamesNotebook->AddPage(m_flamePanels[i], tabName);
    }
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

void MainFrame::OnCheckBS(wxCommandEvent& event)
{
  m_currentConfig.BPSEnabled = 1-m_currentConfig.BPSEnabled;
}

void MainFrame::OnSelectLighting(wxCommandEvent& event)
{
  m_currentConfig.lightingMode = event.GetSelection();
  
  switch(m_currentConfig.lightingMode)
    {
    case LIGHTING_STANDARD :
      m_blendedSolidCheckBox->Disable();
      break;
    case LIGHTING_PHOTOMETRIC :
      m_blendedSolidCheckBox->Enable();
      break;
    }
}

void MainFrame::OnCheckShadows(wxCommandEvent& event)
{
  m_currentConfig.shadowsEnabled = !m_currentConfig.shadowsEnabled;
}

void MainFrame::OnCheckGlow(wxCommandEvent& event)
{
  m_currentConfig.glowEnabled = !m_currentConfig.glowEnabled;
  m_menuDisplayFlames->Enable(IDM_GlowOnly,m_currentConfig.glowEnabled);
}

void MainFrame::OnCheckDepthPeeling(wxCommandEvent& event)
{
  m_currentConfig.depthPeelingEnabled = !m_currentConfig.depthPeelingEnabled;
  if(m_currentConfig.depthPeelingEnabled)
    m_depthPeelingSlider->Enable();
  else
    m_depthPeelingSlider->Disable();
//   m_glBuffer->ToggleDepthPeeling();
}

void MainFrame::OnScrollPosition(wxScrollEvent& event)
{
  if(event.GetId() == IDSL_DP){
    m_glBuffer->setNbDepthPeelingLayers(m_depthPeelingSlider->GetValue() );
    m_currentConfig.nbDepthPeelingLayers = m_depthPeelingSlider->GetValue();
  }
}

void MainFrame::OnCheckSaveImages(wxCommandEvent& event)
{
  m_glBuffer->ToggleSaveImages();  
}

void MainFrame::OnOpenSceneMenu(wxCommandEvent& event)
{
  wxString filename;
  wxString pwd=wxGetWorkingDirectory();
  pwd << SCENES_DIRECTORY;
  
  wxFileDialog fileDialog(this, _("Choose a scene file"), pwd, _(""), _("*.obj"), wxOPEN|wxFILE_MUST_EXIST);
  if(fileDialog.ShowModal() == wxID_OK){
    filename = fileDialog.GetPath();
    
    /* Récupération le chemin absolu vers la scène */
    filename.Replace(wxGetWorkingDirectory(),_(""),false);
    /* Suppression du premier slash */
    filename=filename.Mid(1);
    
    if(!filename.IsEmpty()){
      m_currentConfig.sceneName = filename;
      m_glBuffer->Restart();
    }
  }
}

void MainFrame::OnLoadParamMenu(wxCommandEvent& event)
{
  wxString filename;
  wxString pwd=wxGetWorkingDirectory();
  pwd << PARAMS_DIRECTORY;
  
  wxFileDialog fileDialog(this, _("Choose a simulation file"), pwd, _(""), _("*.ini"), wxOPEN|wxFILE_MUST_EXIST);
  if(fileDialog.ShowModal() == wxID_OK){
    filename = fileDialog.GetPath();
    
    if(!filename.IsEmpty()){
      /* Récupération le chemin absolu vers la scène */
      filename.Replace(wxGetWorkingDirectory(),_(""),false);
      /* Suppression du premier slash */
      filename=filename.Mid(1);

      m_configFileName = filename;
      
      SetTitle(_("Real-time Animation of small Flames - ") + m_configFileName);
      m_solversNotebook->DeleteAllPages();
      m_flamesNotebook->DeleteAllPages();
      GetSettingsFromConfigFile();
      m_glBuffer->Restart();
      m_glBuffer->SetSize(wxSize(m_currentConfig.width,m_currentConfig.height));
      m_mainSizer->Fit(this);
      m_mainSizer->SetSizeHints(this);
      Layout();
    }
  }
}

void MainFrame::OnSaveSettingsMenu(wxCommandEvent& event)
{
  m_config->Write(_("/Display/Width"), (int)m_currentConfig.width);
  m_config->Write(_("/Display/Height"), (int)m_currentConfig.height);
  m_config->Write(_("/Display/Clipping"), m_currentConfig.clipping);
  m_config->Write(_("/Display/LightingMode"), m_currentConfig.lightingMode);
  m_config->Write(_("/Display/BPSEnabled"), m_currentConfig.BPSEnabled);
  m_config->Write(_("/Display/Shadows"), m_currentConfig.shadowsEnabled);
  m_config->Write(_("/Display/DepthPeeling"), m_currentConfig.depthPeelingEnabled);
  m_config->Write(_("/Display/NbDepthPeelingLayers"), (int)m_currentConfig.nbDepthPeelingLayers);
  m_config->Write(_("/Display/Glow"), m_currentConfig.glowEnabled);
  m_config->Write(_("/Scene/FileName"), m_currentConfig.sceneName);
  
  m_config->Write(_("/Shadows/Fatness.x"), m_currentConfig.fatness[0]);
  m_config->Write(_("/Shadows/Fatness.y"), m_currentConfig.fatness[1]);
  m_config->Write(_("/Shadows/Fatness.z"), m_currentConfig.fatness[2]);
  m_config->Write(_("/Shadows/ExtrudeDist.x"), m_currentConfig.extrudeDist[0]);
  m_config->Write(_("/Shadows/ExtrudeDist.y"), m_currentConfig.extrudeDist[1]);
  m_config->Write(_("/Shadows/ExtrudeDist.z"), m_currentConfig.extrudeDist[2]);
  
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
      m_config->Write(groupName + _("TimeStep"),m_currentConfig.solvers[i].timeStep);
      
      m_config->Write(groupName + _("Buoyancy"), m_currentConfig.solvers[i].buoyancy);
      
      m_config->Write(groupName + _("omegaDiff"),m_currentConfig.solvers[i].omegaDiff);
      m_config->Write(groupName + _("omegaProj"),m_currentConfig.solvers[i].omegaProj);
      m_config->Write(groupName + _("epsilon"),m_currentConfig.solvers[i].epsilon);
      
      m_config->Write(groupName + _("nbMaxIter"),(int)m_currentConfig.solvers[i].nbMaxIter);
    }
  
  m_config->Write(_("/Flames/Number"), (int)m_currentConfig.nbFlames);
  
  for(uint i=0; i < m_nbFlamesMax; i++)
    {
      groupName.Printf(_("/Flame%d/"),i);

      m_config->DeleteGroup(groupName);
    }
  
  for(uint i=0; i < m_currentConfig.nbFlames; i++)
    {
      groupName.Printf(_("/Flame%d/"),i);
      
      m_config->Write(groupName + _("Type"), (int )m_currentConfig.flames[i].type);
      m_config->Write(groupName + _("Solver"), m_currentConfig.flames[i].solverIndex);
      m_config->Write(groupName + _("Pos.x"),m_currentConfig.flames[i].position.x);
      m_config->Write(groupName + _("Pos.y"),m_currentConfig.flames[i].position.y);
      m_config->Write(groupName + _("Pos.z"),m_currentConfig.flames[i].position.z);
      m_config->Write(groupName + _("SkeletonsNumber"),(int )m_currentConfig.flames[i].skeletonsNumber);
      m_config->Write(groupName + _("InnerForce"), m_currentConfig.flames[i].innerForce);
      if(m_currentConfig.flames[i].type != CANDLE)
	m_config->Write(groupName + _("WickFileName"),m_currentConfig.flames[i].wickName);
      m_config->Write(groupName + _("Flickering"), (int )m_currentConfig.flames[i].flickering);
      m_config->Write(groupName + _("FDF"), (int )m_currentConfig.flames[i].fdf);
      m_config->Write(groupName + _("SamplingTolerance"), m_currentConfig.flames[i].samplingTolerance);
      m_config->Write(groupName + _("intensityCoef"), m_currentConfig.flames[i].intensityCoef);
      m_config->Write(groupName + _("nbLeadParticles"), (int )m_currentConfig.flames[i].leadLifeSpan);
      m_config->Write(groupName + _("nbPeriParticles"), (int )m_currentConfig.flames[i].periLifeSpan);
      m_config->Write(groupName + _("IESFileName"),m_currentConfig.flames[i].IESFileName);
    }
  
  wxFileOutputStream* file = new wxFileOutputStream( m_configFileName );
  
  if (m_config->Save(*file) )
    wxMessageBox(_("Configuration for the current simulation have been saved"),
		 _("Save settings"), wxOK | wxICON_INFORMATION, this);
  
  delete file;
}

void MainFrame::OnSaveSettingsAsMenu(wxCommandEvent& event)
{
  
  wxString filename;
  wxString pwd=wxGetWorkingDirectory();
  pwd << PARAMS_DIRECTORY;
  
  wxFileDialog fileDialog(this, _("Enter a simulation file"), pwd, _(""), _("*.ini"), wxSAVE|wxHIDE_READONLY|wxOVERWRITE_PROMPT);
  if(fileDialog.ShowModal() == wxID_OK){
    filename = fileDialog.GetPath();
    /* Récupération le chemin absolu vers la scène */
    filename.Replace(wxGetWorkingDirectory(),_(""),false);
    /* Suppression du premier slash */
    filename=filename.Mid(1);
  
    if(!filename.IsEmpty()){
      m_configFileName = filename;
      
      SetTitle(_("Real-time Animation of small Flames - ") + m_configFileName);
      OnSaveSettingsMenu(event);
    }
  }
}

void MainFrame::OnQuitMenu(wxCommandEvent& WXUNUSED(event))
{
  Close(TRUE);
}

void MainFrame::OnAboutMenu(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(_("Real-time simulation of flames\nOasis Team"),
	       _("About flames"), wxOK | wxICON_INFORMATION, this);
}

void MainFrame::OnGlowOnlyMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleGlowOnlyDisplay();
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

void MainFrame::OnWickBoxesMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleWickBoxesDisplay();
}

void MainFrame::OnHideMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleFlamesDisplay();
}

void MainFrame::OnShadowVolumesMenu(wxCommandEvent& event)
{ 
  m_glBuffer->ToggleShadowVolumesDisplay();
}

void MainFrame::OnShadowVolumesSettingsMenu(wxCommandEvent& event)
{
  ShadowsDialog *shadowsDialog = new ShadowsDialog(GetParent(),-1,_("Shadows settings"),&m_currentConfig,m_glBuffer);
  shadowsDialog->ShowModal();  
  shadowsDialog->Destroy();
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
  SolverDialog *solverDialog = new SolverDialog(GetParent(),-1,_("Solvers settings"),&m_currentConfig);
  if(solverDialog->ShowModal() == wxID_OK){
    m_glBuffer->Restart();
    InitSolversPanels();
  }
  solverDialog->Destroy();
}

void MainFrame::OnFlamesMenu(wxCommandEvent& event)
{
  FlameDialog *flameDialog = new FlameDialog(GetParent(),-1,_("Flames settings"),&m_currentConfig);
  if(flameDialog->ShowModal() == wxID_OK){
    m_glBuffer->Restart();
    InitFlamesPanels();
  }
  flameDialog->Destroy();
}

void MainFrame::SetFPS(int fps)
{
  wxString s;
  s += wxString::Format(_("%d FPS"), fps);
  
  SetStatusText(s);
}

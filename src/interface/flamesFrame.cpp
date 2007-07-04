#include "flamesFrame.hpp"

#include "luminaryDialog.hpp"
#include "shadowsDialog.hpp"
#include "GLFlameCanvas.hpp"

// Déclarations de la table des événements
// Sorte de relation qui lit des identifiants d'événements aux fonctions
BEGIN_EVENT_TABLE(FlamesFrame, wxFrame)
  EVT_SIZE(FlamesFrame::OnSize)
  EVT_RADIOBOX(IDRB_Lighting, FlamesFrame::OnSelectLighting)
  EVT_BUTTON(IDB_Run, FlamesFrame::OnClickButtonRun)
  EVT_BUTTON(IDB_Restart, FlamesFrame::OnClickButtonRestart)
  EVT_MENU(IDM_LoadParam, FlamesFrame::OnLoadParamMenu)
  EVT_MENU(IDM_OpenScene, FlamesFrame::OnOpenSceneMenu)
  EVT_MENU(IDM_SaveSettings, FlamesFrame::OnSaveSettingsMenu)
  EVT_MENU(IDM_SaveSettingsAs, FlamesFrame::OnSaveSettingsAsMenu)
  EVT_MENU(IDM_Quit, FlamesFrame::OnQuitMenu)
  EVT_MENU(IDM_About, FlamesFrame::OnAboutMenu)
  EVT_MENU(IDM_GlowOnly, FlamesFrame::OnGlowOnlyMenu)
  EVT_MENU(IDM_BDS, FlamesFrame::OnBDSMenu)
  EVT_MENU(IDM_Grid, FlamesFrame::OnGridMenu)
  EVT_MENU(IDM_Base, FlamesFrame::OnBaseMenu)
  EVT_MENU(IDM_Velocity, FlamesFrame::OnVelocityMenu)
  EVT_MENU(IDM_Particles, FlamesFrame::OnParticlesMenu)
  EVT_MENU(IDM_WickBoxes, FlamesFrame::OnWickBoxesMenu)
  EVT_MENU(IDM_ShadowVolumes, FlamesFrame::OnShadowVolumesMenu)
  EVT_MENU(IDM_Hide, FlamesFrame::OnHideMenu)
  EVT_MENU(IDM_FBDS, FlamesFrame::OnFBDSMenu)
  EVT_MENU(IDM_FBDB, FlamesFrame::OnFBDBMenu)
  EVT_MENU(IDM_Wired, FlamesFrame::OnWiredMenu)
  EVT_MENU(IDM_Shaded, FlamesFrame::OnShadedMenu)
  EVT_MENU(IDM_Settings, FlamesFrame::OnSettingsMenu)
  EVT_MENU(IDM_ShadowVolumesSettings, FlamesFrame::OnShadowVolumesSettingsMenu)
  EVT_CHECKBOX(IDCHK_BS, FlamesFrame::OnCheckBS)
  EVT_CHECKBOX(IDCHK_Shadows, FlamesFrame::OnCheckShadows)
  EVT_CHECKBOX(IDCHK_Glow, FlamesFrame::OnCheckGlow)
  EVT_CHECKBOX(IDCHK_DP, FlamesFrame::OnCheckDepthPeeling)
  EVT_CHECKBOX(IDCHK_SaveImages, FlamesFrame::OnCheckSaveImages)
  EVT_CHECKBOX(IDCHK_Gamma, FlamesFrame::OnCheckGamma)
  EVT_COMMAND_SCROLL(IDSL_DP, FlamesFrame::OnScrollDP)
  EVT_COMMAND_SCROLL(IDSL_Gamma, FlamesFrame::OnScrollGamma)
  EVT_CLOSE(FlamesFrame::OnClose)
END_EVENT_TABLE();

/**************************************** FlamesFrame Class methods **************************************/

FlamesFrame::FlamesFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const wxString& configFileName)
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
  m_glBuffer = new GLFlameCanvas( this, wxID_ANY, wxPoint(0,0), wxSize(1024,768),attributelist, wxSUNKEN_BORDER);
  m_glBuffer->SetExtraStyle(wxWS_EX_PROCESS_IDLE);
  
  m_lightingRadioBox = new wxRadioBox(this, IDRB_Lighting, _("Type"), wxDefaultPosition, wxDefaultSize, 
				      2, m_lightingChoices, 2, wxRA_SPECIFY_COLS);
  
  m_buttonRun = new wxButton(this,IDB_Run,_("Pause"));
  m_buttonRestart = new wxButton(this,IDB_Restart,_("Restart"));
  
  m_blendedSolidCheckBox = new wxCheckBox(this,IDCHK_BS,_("Show PS"));
  m_shadowsEnabledCheckBox = new wxCheckBox(this,IDCHK_Shadows,_("Shadows"));
  m_glowEnabledCheckBox = new wxCheckBox(this,IDCHK_Glow,_("Glow"));
  m_depthPeelingEnabledCheckBox = new wxCheckBox(this,IDCHK_DP,_("Depth Peeling"));
  m_depthPeelingSlider = new wxSlider(this,IDSL_DP,0,0,DEPTH_PEELING_LAYERS_MAX, wxDefaultPosition, 
				      wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  m_saveImagesCheckBox =  new wxCheckBox(this,IDCHK_SaveImages,_("Save Images"));
  
  m_luminariesNotebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
  m_solversNotebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
  m_flamesNotebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
  
  m_gammaCheckBox =  new wxCheckBox(this,IDCHK_Gamma,_("Enable"));
  m_gammaSlider = new wxSlider(this,IDSL_Gamma,0,40,200, wxDefaultPosition, wxDefaultSize, wxSL_LABELS|wxSL_AUTOTICKS);
  
  SetToolTips();
  DoLayout();
  CreateMenuBar();
  
  m_configFileName = configFileName;
  LoadSettings();
  
  m_currentConfig.gammaCorrection = 1;
  m_gammaSlider->SetValue((int)m_currentConfig.gammaCorrection*100);
  
  CreateStatusBar();
  SetStatusText( _("FPS will be here...") );
}

void FlamesFrame::SetToolTips()
{
  m_blendedSolidCheckBox->SetToolTip(_("Click to see the values of the photometric solid mapped on the scene"));
  m_saveImagesCheckBox->SetToolTip(_("Click to save the simulation as PNG images (stored in the directory captures/)"));
  m_depthPeelingSlider->SetToolTip(_("Change the number of layers used in the depth peeling process"));
}

void FlamesFrame::DoLayout()
{
  wxStaticBoxSizer *lightingSizer, *globalSizer,*multiSizer, *luminariesSizer, *solversSizer, *flamesSizer, *gammaSizer;  
  wxBoxSizer *leftSizer, *bottomSizer, *rightSizer, *lightingBottomSizer, *multiTopSizer, *globalTopSizer;
  
  /* Réglages globaux */
  globalTopSizer = new wxBoxSizer(wxHORIZONTAL);
  globalTopSizer->Add(m_buttonRun, 0, 0, 0);
  globalTopSizer->Add(m_buttonRestart, 0, 0, 0);
  
  globalSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Global"));
  globalSizer->Add(globalTopSizer, 0, 0, 0);
  globalSizer->Add(m_saveImagesCheckBox, 0, 0, 0);
  
  /* Réglages du glow */
  multiTopSizer = new wxBoxSizer(wxHORIZONTAL);
  multiTopSizer->Add(m_glowEnabledCheckBox, 0, 0, 0);
  multiTopSizer->Add(m_depthPeelingEnabledCheckBox, 0, 0, 0);
  multiSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Multi-pass Rendering"));
  multiSizer->Add(multiTopSizer, 0, 0, 0);
  multiSizer->Add(m_depthPeelingSlider, 0, wxEXPAND, 0);
    
  /* Réglages de l'éclairage */
  lightingBottomSizer = new wxBoxSizer(wxHORIZONTAL);
  lightingBottomSizer->Add(m_blendedSolidCheckBox, 1, 0, 0);
  lightingBottomSizer->Add(m_shadowsEnabledCheckBox, 1, 0, 0);
  
  lightingSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Lighting"));
  lightingSizer->Add(m_lightingRadioBox, 0, wxEXPAND, 0);
  lightingSizer->Add(lightingBottomSizer, 1, 0, 0);
  
  /* Réglages de la correction Gamma */
  gammaSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Gamma correction"));
  gammaSizer->Add(m_gammaCheckBox, 0, wxEXPAND, 0);
  gammaSizer->Add(m_gammaSlider, 0, wxEXPAND, 0);
  
  bottomSizer = new wxBoxSizer(wxHORIZONTAL);
  bottomSizer->Add(globalSizer, 1, wxEXPAND, 0);
  bottomSizer->Add(multiSizer, 1, wxEXPAND, 0);
  bottomSizer->Add(lightingSizer, 1, wxEXPAND, 0);
  bottomSizer->Add(gammaSizer, 1, wxEXPAND, 0);
  
  luminariesSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Luminaries settings"));
  luminariesSizer->Add(m_luminariesNotebook, 1, wxEXPAND, 0);
  
  solversSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Solvers settings"));
  solversSizer->Add(m_solversNotebook, 1, wxEXPAND, 0);

  flamesSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Flames settings"));
  flamesSizer->Add(m_flamesNotebook, 1, wxEXPAND, 0);
  
  /* Placement des sizers principaux */
  rightSizer = new wxBoxSizer(wxVERTICAL);
  rightSizer->Add(luminariesSizer, 0, wxEXPAND, 0);
  rightSizer->Add(solversSizer, 0, wxEXPAND, 0);
  rightSizer->Add(flamesSizer, 0, wxEXPAND, 0);  
  
  leftSizer = new wxBoxSizer(wxVERTICAL);
  leftSizer->Add(m_glBuffer, 0, 0, 0);
  leftSizer->Add(bottomSizer, 1, 0, 0);

  m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
  m_mainSizer->Add(leftSizer, 0, 0, 0);
  m_mainSizer->Add(rightSizer, 1, 0, 0);
  
  SetSizerAndFit(m_mainSizer);
}

void FlamesFrame::CreateMenuBar()
{
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
  m_menuDisplayFlames->AppendCheckItem( IDM_FBDS, _("&Bounding Spheres"));
  m_menuDisplayFlames->AppendCheckItem( IDM_FBDB, _("&Bounding Boxes"));
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
  m_menuDisplay->AppendCheckItem( IDM_BDS, _("&Bounding spheres"));
  
  m_menuSettings = new wxMenu;
  m_menuSettings->Append( IDM_Settings, _("&Settings..."));
  m_menuSettings->Append( IDM_ShadowVolumesSettings, _("S&hadows..."));
  
  m_menuBar = new wxMenuBar;
  m_menuBar->Append( m_menuFile, _("&File") );
  m_menuBar->Append( m_menuDisplay, _("&Display") );
  m_menuBar->Append( m_menuSettings, _("&Settings") );
  
  SetMenuBar( m_menuBar );
}

void FlamesFrame::OnSize(wxSizeEvent& event)
{
  // this is also necessary to update the context on some platforms
  wxFrame::OnSize(event);
  Layout();
}

void FlamesFrame::InitGLBuffer()
{
  m_glBuffer->SetSize(wxSize(m_currentConfig.width,m_currentConfig.height));
  m_glBuffer->Init(&m_currentConfig);
  m_glBuffer->setNbDepthPeelingLayers(m_currentConfig.nbDepthPeelingLayers);
  
  m_mainSizer->Fit(this);
  m_mainSizer->SetSizeHints(this);
  Layout();
}

void FlamesFrame::InitLuminariesPanels()
{
  wxString tabName;
  char type=0;
  
  m_luminariesNotebook->DeleteAllPages();
  
  for(int unsigned i=0; i < m_currentConfig.nbLuminaries; i++)
    {
      m_luminaryPanels[i+type] = new LuminaryMainPanel(m_luminariesNotebook, -1, &m_currentConfig.luminaries[i], 
						       i, m_glBuffer);
      tabName.Printf(_("Luminary #%d"),i+1);
      m_luminariesNotebook->AddPage(m_luminaryPanels[i+type], tabName);
    }
}

void FlamesFrame::InitSolversPanels()
{
  wxString tabName;
  char type=0;
  
  m_solversNotebook->DeleteAllPages();
  
  if(m_currentConfig.useGlobalField)
    {
      m_solverPanels[0] = new SolverMainPanel(m_solversNotebook, -1, m_currentConfig.globalField.buoyancy, -1, m_glBuffer);
      m_solversNotebook->AddPage(m_solverPanels[0], _("Global Field"));
      type = 1;
    }
  for(int unsigned i=0; i < m_currentConfig.nbLuminaries; i++)
    {
      m_solverPanels[i+type] = new SolverMainPanel(m_solversNotebook, -1, 
						   m_currentConfig.luminaries[i].fields[0].buoyancy, i, m_glBuffer);
      tabName.Printf(_("Solver #%d"),i+1);
      m_solversNotebook->AddPage(m_solverPanels[i+type], tabName);
    }
}

void FlamesFrame::InitFlamesPanels()
{
  wxString tabName;
  
  m_flamesNotebook->DeleteAllPages();
  
  for(int unsigned i=0; i < m_currentConfig.nbLuminaries; i++)
    {
      m_flamePanels[i] = new FlameMainPanel(m_flamesNotebook, -1, &m_currentConfig.luminaries[i].fires[0], i, m_glBuffer);
      tabName.Printf(_("Flame #%d"),i+1);       
      m_flamesNotebook->AddPage(m_flamePanels[i], tabName);
    }
}

void FlamesFrame::OnClose(wxCloseEvent& event)
{
  m_glBuffer->DeleteThreads();
  m_glBuffer->setRunningState(false);
  for(int unsigned i=0; i < m_currentConfig.nbLuminaries; i++)
    {
      delete [] m_currentConfig.luminaries[i].fires;
      delete [] m_currentConfig.luminaries[i].fields;
    }
  delete [] m_currentConfig.luminaries;
  delete m_config;
  
  Destroy();
}

// Fonction qui est exécutée lors du click sur le bouton.
void FlamesFrame::OnClickButtonRun(wxCommandEvent& event)
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
void FlamesFrame::OnClickButtonRestart(wxCommandEvent& event)
{
  m_glBuffer->Restart();
}

void FlamesFrame::OnCheckBS(wxCommandEvent& event)
{
  m_currentConfig.BPSEnabled = 1-m_currentConfig.BPSEnabled;
}

void FlamesFrame::OnSelectLighting(wxCommandEvent& event)
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

void FlamesFrame::OnCheckShadows(wxCommandEvent& event)
{
  m_currentConfig.shadowsEnabled = !m_currentConfig.shadowsEnabled;
}

void FlamesFrame::OnCheckGlow(wxCommandEvent& event)
{
  m_currentConfig.glowEnabled = !m_currentConfig.glowEnabled;
  m_menuDisplayFlames->Enable(IDM_GlowOnly,m_currentConfig.glowEnabled);
}

void FlamesFrame::OnCheckGamma(wxCommandEvent& event)
{
  m_glBuffer->setGammaCorrectionState( event.IsChecked() );
  if(event.IsChecked())
    m_gammaSlider->Enable();
  else
    m_gammaSlider->Disable();
}

void FlamesFrame::OnCheckDepthPeeling(wxCommandEvent& event)
{
  m_currentConfig.depthPeelingEnabled = !m_currentConfig.depthPeelingEnabled;
  if(m_currentConfig.depthPeelingEnabled)
    m_depthPeelingSlider->Enable();
  else
    m_depthPeelingSlider->Disable();
}

void FlamesFrame::OnScrollDP(wxScrollEvent& event)
{
  m_glBuffer->setNbDepthPeelingLayers(m_depthPeelingSlider->GetValue() );
  m_currentConfig.nbDepthPeelingLayers = m_depthPeelingSlider->GetValue();
}

void FlamesFrame::OnScrollGamma(wxScrollEvent& event)
{
  m_currentConfig.gammaCorrection = m_gammaSlider->GetValue()/100.0f;
  m_glBuffer->setGammaCorrection( m_currentConfig.gammaCorrection );
}

void FlamesFrame::OnCheckSaveImages(wxCommandEvent& event)
{
  m_glBuffer->ToggleSaveImages();  
}

void FlamesFrame::OnOpenSceneMenu(wxCommandEvent& event)
{
  wxString filename;
  wxString pwd=wxGetCwd();
  pwd << SCENES_DIRECTORY;
  
  wxFileDialog fileDialog(this, _("Choose a scene file"), pwd, _(""), _("*.obj"), wxOPEN|wxFILE_MUST_EXIST);
  if(fileDialog.ShowModal() == wxID_OK){
    m_glBuffer->setRunningState(false);
    filename = fileDialog.GetPath();
    
    /* Récupération le chemin absolu vers la scène */
    filename.Replace(wxGetCwd(),_(""),false);
    /* Suppression du premier slash */
    filename=filename.Mid(1);
    
    if(!filename.IsEmpty()){
      m_currentConfig.sceneName = filename;
      m_glBuffer->Restart();
    }
  }
}

void FlamesFrame::OnLoadParamMenu(wxCommandEvent& event)
{
  wxString filename;
  wxString pwd=wxGetCwd();
  pwd << PARAMS_DIRECTORY;
  
  wxFileDialog fileDialog(this, _("Choose a simulation file"), pwd, _(""), _("*.ini"), wxOPEN|wxFILE_MUST_EXIST);
  if(fileDialog.ShowModal() == wxID_OK){
    filename = fileDialog.GetPath();
    
    if(!filename.IsEmpty()){
      m_glBuffer->setRunningState(false);
      Disable();
      /* Récupération le chemin absolu vers la scène */
      filename.Replace(wxGetCwd(),_(""),false);
      /* Suppression du premier slash */
      filename=filename.Mid(1);

      m_configFileName = filename;
      
      SetTitle(_("Real-time Animation of small Flames - ") + m_configFileName);
      
      for(int unsigned i=0; i < m_currentConfig.nbLuminaries; i++)
	{
	  delete [] m_currentConfig.luminaries[i].fires;
	  delete [] m_currentConfig.luminaries[i].fields;
	}
      delete [] m_currentConfig.luminaries;
      
      m_flamesNotebook->DeleteAllPages();      
      m_solversNotebook->DeleteAllPages();

      LoadSettings();
      m_glBuffer->Restart();
      m_mainSizer->Fit(this);
      m_mainSizer->SetSizeHints(this);
      Layout();
      Enable();
    }
  }
}

/**************************************************************************************************/
/****************** METHODES DE CHARGEMENT DES PARAMETRES DE LA SIMULATION ************************/
/**************************************************************************************************/

void FlamesFrame::LoadSolverSettings(wxString& groupName, SolverConfig& solverConfig)
{
  double tmp;
  
  m_config->Read(groupName + _("FieldType"), (int *) &solverConfig.type, HYBRID_SOLVER);
  
  solverConfig.resx = m_config->Read(groupName + _("X_res"), 15);
  solverConfig.resy = m_config->Read(groupName + _("Y_res"), 15);
  solverConfig.resz = m_config->Read(groupName + _("Z_res"), 15);
  
  m_config->Read(groupName + _("Dim"),&tmp, 1.0);
  solverConfig.dim = (float)tmp;
  m_config->Read(groupName + _("Scale.x"),&tmp,1.0);
  solverConfig.scale.x = (float)tmp;
  m_config->Read(groupName + _("Scale.y"),&tmp,1.0);
  solverConfig.scale.y = (float)tmp;
  m_config->Read(groupName + _("Scale.z"),&tmp,1.0);
  solverConfig.scale.z = (float)tmp;
  
  m_config->Read(groupName + _("TimeStep"), &tmp, 0.4);
  solverConfig.timeStep = (float)tmp;
  m_config->Read(groupName + _("Buoyancy"), &tmp, 0.02);
  solverConfig.buoyancy = (float)tmp;
  
  m_config->Read(groupName + _("omegaDiff"),&tmp, 1.5);
  solverConfig.omegaDiff = (float)tmp;
  m_config->Read(groupName + _("omegaProj"),&tmp, 1.5);
  solverConfig.omegaProj = (float)tmp;
  m_config->Read(groupName + _("epsilon"),&tmp, 0.00001);
  solverConfig.epsilon = (float)tmp;
  solverConfig.nbMaxIter = m_config->Read(groupName + _("nbMaxIter"), 100);
}

void FlamesFrame::LoadFireSettings(wxString& groupName, FlameConfig& fireConfig)
{
  double tmp;
  
  m_config->Read(groupName + _("Type"), (int *) &fireConfig.type, CANDLE);
  if(fireConfig.type != CANDLE){
    m_config->Read(groupName + _("SkeletonsNumber"), (int *) &fireConfig.skeletonsNumber, 5);
    m_config->Read(groupName + _("InnerForce"), &tmp, 0.005f);
    fireConfig.innerForce = (float)tmp;
  }else{
    m_config->Read(groupName + _("SkeletonsNumber"), (int *) &fireConfig.skeletonsNumber, 4);
    m_config->Read(groupName + _("InnerForce"), &tmp, 0.04f);
    fireConfig.innerForce = (float)tmp;
  }
  m_config->Read(groupName + _("Flickering"), (int *) &fireConfig.flickering, 0);
  m_config->Read(groupName + _("FDF"), (int *) &fireConfig.fdf, 0);
  m_config->Read(groupName + _("SamplingTolerance"), &tmp, 100);
  fireConfig.samplingTolerance = (float)tmp;
  m_config->Read(groupName + _("nbLeadParticles"), (int *) &fireConfig.leadLifeSpan, 8);
  m_config->Read(groupName + _("nbPeriParticles"), (int *) &fireConfig.periLifeSpan, 6);
  fireConfig.IESFileName = m_config->Read(groupName + _("IESFileName"), _("IES/test.ies"));
}

void FlamesFrame::LoadSettings (void)
{
  wxString groupName;
  double tmp;
  wxFileInputStream file( m_configFileName );
  //if(!wxFileInputStream::Ok())
  
  m_config = new wxFileConfig( file );
  
  m_currentConfig.width = m_config->Read(_("/Display/Width"), 1024);
  m_currentConfig.height = m_config->Read(_("/Display/Height"), 768);
  m_currentConfig.clipping = m_config->Read(_("/Display/Clipping"), 100);
  m_config->Read(_("/Display/LightingMode"), &m_currentConfig.lightingMode, LIGHTING_STANDARD);
  m_config->Read(_("/Display/BPSEnabled"), &m_currentConfig.BPSEnabled, 0);
  m_config->Read(_("/Display/Shadows"), &m_currentConfig.shadowsEnabled, false);
  m_config->Read(_("/Display/Glow"), &m_currentConfig.glowEnabled, false);
  m_config->Read(_("/Display/DepthPeeling"), &m_currentConfig.depthPeelingEnabled, false);
  m_config->Read(_("/Display/NbDepthPeelingLayers"), (int *) &m_currentConfig.nbDepthPeelingLayers, 4);
  m_currentConfig.sceneName = m_config->Read(_("/Scene/FileName"), _("scenes/scene2.obj"));
  
  m_config->Read(_("/Shadows/Fatness.x"), &tmp, -.001);
  m_currentConfig.fatness[0] = (float)tmp;
  m_config->Read(_("/Shadows/Fatness.y"), &tmp, -.001);
  m_currentConfig.fatness[1] = (float)tmp;
  m_config->Read(_("/Shadows/Fatness.z"), &tmp, -.001);
  m_currentConfig.fatness[2] = (float)tmp;
  m_config->Read(_("/Shadows/ExtrudeDist.x"), &tmp, 5);
  m_currentConfig.extrudeDist[0] = (float)tmp;
  m_config->Read(_("/Shadows/ExtrudeDist.y"), &tmp, 5);
  m_currentConfig.extrudeDist[1] = (float)tmp;
  m_config->Read(_("/Shadows/ExtrudeDist.z"), &tmp, 5);
  m_currentConfig.extrudeDist[2] = (float)tmp;
  m_currentConfig.fatness[3] = 0.0f;
  m_currentConfig.extrudeDist[3] = 0.0f;
  
  m_config->Read(_("/GlobalField/Enabled"), &m_currentConfig.useGlobalField, 0);
  groupName << _("/GlobalField/");
  LoadSolverSettings(groupName,m_currentConfig.globalField);
  
  m_currentConfig.nbLuminaries = m_config->Read(_("/Luminaries/Number"), 1);
  m_currentConfig.luminaries = new LuminaryConfig[m_currentConfig.nbLuminaries];
  m_nbLuminariesMax = m_currentConfig.nbLuminaries;
  
  for(uint i=0; i < m_currentConfig.nbLuminaries; i++)
    {
      groupName.Printf(_("/Luminary%d/"), i);
      m_currentConfig.luminaries[i].fileName = m_config->Read(groupName + _("FileName"), _("scenes/bougie.obj"));
      
      m_config->Read(groupName + _("Pos.x"), &tmp, 0.0);
      m_currentConfig.luminaries[i].position.x = (float)tmp;
      m_config->Read(groupName + _("Pos.y"), &tmp, 0.0);
      m_currentConfig.luminaries[i].position.y = (float)tmp;
      m_config->Read(groupName + _("Pos.z"), &tmp, 0.0);
      m_currentConfig.luminaries[i].position.z = (float)tmp;
      
      m_currentConfig.luminaries[i].nbFields = m_config->Read(_("/Luminary%d/NbFields"), 1);
      m_currentConfig.luminaries[i].fields = new SolverConfig[m_currentConfig.luminaries[i].nbFields];
      
      /** A modifier pour prendre en compte la sauvegarde de plusieurs solveurs et flammes par luminaire */
      groupName.Printf(_("/Luminary%d/Solver0/"), i);
      LoadSolverSettings(groupName, m_currentConfig.luminaries[i].fields[0]);
      
      m_currentConfig.luminaries[i].nbFires = m_config->Read(_("/Luminary%d/NbFires"), 1);
      m_currentConfig.luminaries[i].fires = new FlameConfig[m_currentConfig.luminaries[i].nbFires];
      
      groupName.Printf(_("/Luminary%d/Flame0/"), i);
      LoadFireSettings(groupName, m_currentConfig.luminaries[i].fires[0]);
    }
  InitLuminariesPanels();
  InitSolversPanels();
  InitFlamesPanels();
  
  m_blendedSolidCheckBox->SetValue(!m_currentConfig.BPSEnabled);
  m_lightingRadioBox->SetSelection(m_currentConfig.lightingMode);
  m_glowEnabledCheckBox->SetValue(m_currentConfig.glowEnabled);
  m_shadowsEnabledCheckBox->SetValue(m_currentConfig.shadowsEnabled);
  
  m_depthPeelingEnabledCheckBox->SetValue(m_currentConfig.depthPeelingEnabled);
  m_depthPeelingSlider->SetValue(m_currentConfig.nbDepthPeelingLayers);
    
  if(m_currentConfig.depthPeelingEnabled)
    m_depthPeelingSlider->Enable();
  else
    m_depthPeelingSlider->Disable();
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
  
  return;
}

/**************************************************************************************************/
/****************** METHODES DE SAUVEGARDE DES PARAMETRES DE LA SIMULATION ************************/
/**************************************************************************************************/

void FlamesFrame::SaveSolverSettings(wxString& groupName, SolverConfig& solverConfig)
{
//   solverConfig.buoyancy = ;
  m_config->Write(groupName + _("FieldType"), (int)solverConfig.type);
  
  m_config->Write(groupName + _("X_res"),(int)solverConfig.resx);
  m_config->Write(groupName + _("Y_res"),(int)solverConfig.resy);
  m_config->Write(groupName + _("Z_res"),(int)solverConfig.resz);
  
  m_config->Write(groupName + _("Dim"),solverConfig.dim);
  m_config->Write(groupName + _("Scale.x"),solverConfig.scale.x);
  m_config->Write(groupName + _("Scale.y"),solverConfig.scale.y);
  m_config->Write(groupName + _("Scale.z"),solverConfig.scale.z);
  
  m_config->Write(groupName + _("TimeStep"),solverConfig.timeStep);      
  m_config->Write(groupName + _("Buoyancy"), solverConfig.buoyancy);
  
  m_config->Write(groupName + _("omegaDiff"),solverConfig.omegaDiff);
  m_config->Write(groupName + _("omegaProj"),solverConfig.omegaProj);
  m_config->Write(groupName + _("epsilon"),solverConfig.epsilon);
  
  m_config->Write(groupName + _("nbMaxIter"),(int)solverConfig.nbMaxIter);
}

void FlamesFrame::SaveFireSettings(wxString& groupName, FlameConfig& fireConfig)
{
  m_config->Write(groupName + _("Type"), (int )fireConfig.type);
  m_config->Write(groupName + _("SkeletonsNumber"),(int )fireConfig.skeletonsNumber);
  m_config->Write(groupName + _("InnerForce"), fireConfig.innerForce);
  m_config->Write(groupName + _("Flickering"), (int )fireConfig.flickering);
  m_config->Write(groupName + _("FDF"), (int )fireConfig.fdf);
  m_config->Write(groupName + _("SamplingTolerance"), fireConfig.samplingTolerance);
  m_config->Write(groupName + _("nbLeadParticles"), (int )fireConfig.leadLifeSpan);
  m_config->Write(groupName + _("nbPeriParticles"), (int )fireConfig.periLifeSpan);
  m_config->Write(groupName + _("IESFileName"),fireConfig.IESFileName);
}

void FlamesFrame::OnSaveSettingsMenu(wxCommandEvent& event)
{
  wxString groupName;
  
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
  
  m_config->Write(_("/Shadows/Fatness.x"), (GLfloat)m_currentConfig.fatness[0]);
  m_config->Write(_("/Shadows/Fatness.y"), (GLfloat)m_currentConfig.fatness[1]);
  m_config->Write(_("/Shadows/Fatness.z"), (GLfloat)m_currentConfig.fatness[2]);
  m_config->Write(_("/Shadows/ExtrudeDist.x"), (GLfloat)m_currentConfig.extrudeDist[0]);
  m_config->Write(_("/Shadows/ExtrudeDist.y"), (GLfloat)m_currentConfig.extrudeDist[1]);
  m_config->Write(_("/Shadows/ExtrudeDist.z"), (GLfloat)m_currentConfig.extrudeDist[2]);
  
  m_config->Write(_("/Luminaries/Number"), (int)m_currentConfig.nbLuminaries);

  m_config->Write(_("/GlobalField/Enabled"), m_currentConfig.useGlobalField);
  groupName << _("/GlobalField/");
  m_config->DeleteGroup(groupName);
  SaveSolverSettings(groupName,m_currentConfig.globalField);
  
  for(uint i=0; i < m_nbLuminariesMax; i++)
    {
      groupName.Printf(_("/Luminary%d"),i);

      m_config->DeleteGroup(groupName);
    }
  
  for(uint i=0; i < m_currentConfig.nbLuminaries; i++)
    {
      groupName.Printf(_("/Luminary%d/"),i);
      
      m_config->Write(groupName + _("FileName"),m_currentConfig.luminaries[i].fileName);
      m_config->Write(groupName + _("Pos.x"),m_currentConfig.luminaries[i].position.x);
      m_config->Write(groupName + _("Pos.y"),m_currentConfig.luminaries[i].position.y);
      m_config->Write(groupName + _("Pos.z"),m_currentConfig.luminaries[i].position.z);
      
      m_config->Write(_("/Luminaries/NbFields"), (int)m_currentConfig.luminaries[i].nbFields);
      groupName.Printf(_("/Luminary%d/Solver0/"),i);
      m_solverPanels[i]->getCtrlValues(m_currentConfig.luminaries[i].fields[0]);
      SaveSolverSettings(groupName,m_currentConfig.luminaries[i].fields[0]);
      
      m_config->Write(_("/Luminaries/NbFires"), (int)m_currentConfig.luminaries[i].nbFires);
      groupName.Printf(_("/Luminary%d/Flame0/"),i);
      m_flamePanels[i]->getCtrlValues(m_currentConfig.luminaries[i].fires[0]);
      SaveFireSettings(groupName,m_currentConfig.luminaries[i].fires[0]);  
    }
  
  wxFileOutputStream file( m_configFileName );
  
  if (m_config->Save(file) )
    wxMessageBox(_("Configuration for the current simulation have been saved"),
		 _("Save settings"), wxOK | wxICON_INFORMATION, this);
}

void FlamesFrame::OnSaveSettingsAsMenu(wxCommandEvent& event)
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
      
      SetTitle(_("Real-time Animation of small Flames - ") + m_configFileName);
      OnSaveSettingsMenu(event);
    }
  }
}

void FlamesFrame::OnQuitMenu(wxCommandEvent& WXUNUSED(event))
{
  Close(TRUE);
}

void FlamesFrame::OnAboutMenu(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(_("Real-time simulation of flames\nOasis Team"),
	       _("About flames"), wxOK | wxICON_INFORMATION, this);
}

void FlamesFrame::OnGlowOnlyMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleGlowOnlyDisplay();
}

void FlamesFrame::OnBDSMenu(wxCommandEvent& event)
{
  m_glBuffer->setBoundingSphereMode(event.IsChecked());
}

void FlamesFrame::OnGridMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleGridDisplay();
}

void FlamesFrame::OnBaseMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleBaseDisplay();
}

void FlamesFrame::OnVelocityMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleVelocityDisplay();
}

void FlamesFrame::OnParticlesMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleParticlesDisplay();
}

void FlamesFrame::OnWickBoxesMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleWickBoxesDisplay();
}

void FlamesFrame::OnHideMenu(wxCommandEvent& event)
{
  m_glBuffer->ToggleFlamesDisplay();
}

void FlamesFrame::OnShadowVolumesMenu(wxCommandEvent& event)
{ 
  m_glBuffer->ToggleShadowVolumesDisplay();
}

void FlamesFrame::OnShadowVolumesSettingsMenu(wxCommandEvent& event)
{
  ShadowsDialog shadowsDialog(GetParent(),-1,_("Shadows settings"),&m_currentConfig,m_glBuffer);
  shadowsDialog.ShowModal();  
}

void FlamesFrame::OnFBDSMenu(wxCommandEvent& event)
{
  m_menuDisplayFlames->Check(IDM_Shaded,false);
  m_menuDisplayFlames->Check(IDM_Wired,false);
  m_menuDisplayFlames->Check(IDM_FBDB,false);
  m_menuDisplayFlames->Check(IDM_FBDS,true);
  m_glBuffer->setBoundingVolumesDisplay(BOUNDING_SPHERE);
}

void FlamesFrame::OnFBDBMenu(wxCommandEvent& event)
{
  m_menuDisplayFlames->Check(IDM_Shaded,false);
  m_menuDisplayFlames->Check(IDM_Wired,false);
  m_menuDisplayFlames->Check(IDM_FBDB,true);
  m_menuDisplayFlames->Check(IDM_FBDS,false);
  m_glBuffer->setBoundingVolumesDisplay(BOUNDING_BOX);
}

void FlamesFrame::OnWiredMenu(wxCommandEvent& event)
{
  m_menuDisplayFlames->Check(IDM_Shaded,false);
  m_menuDisplayFlames->Check(IDM_Wired,true);
  m_menuDisplayFlames->Check(IDM_FBDS,false);
  m_menuDisplayFlames->Check(IDM_FBDB,false);
  m_glBuffer->setSmoothShading(false);
  m_glBuffer->setBoundingVolumesDisplay(NO_BOUNDING_VOLUME);
}

void FlamesFrame::OnShadedMenu(wxCommandEvent& event)
{
  m_menuDisplayFlames->Check(IDM_Shaded,true);
  m_menuDisplayFlames->Check(IDM_Wired,false);
  m_menuDisplayFlames->Check(IDM_FBDS,false);
  m_menuDisplayFlames->Check(IDM_FBDB,false);
  m_glBuffer->setSmoothShading(true);
  m_glBuffer->setBoundingVolumesDisplay(false);
}

void FlamesFrame::OnSettingsMenu(wxCommandEvent& event)
{
  m_glBuffer->setRunningState(false);
  LuminaryDialog solverDialog (GetParent(),-1,_("Simulation settings"),&m_currentConfig);
  if(solverDialog.ShowModal() == wxID_OK){
    InitLuminariesPanels();
    InitSolversPanels();
    InitFlamesPanels();
    m_glBuffer->ReloadFieldsAndFires();
  }
  m_glBuffer->setRunningState(true);
}

void FlamesFrame::SetFPS(int fps, int rps)
{
  wxString s;
  s += wxString::Format(_("%d FPS %d RPS"), fps, rps);
  
  SetStatusText(s);
}

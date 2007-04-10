#include "GLFlameCanvas.hpp"

#include "flamesFrame.hpp"
#include <iostream>

#include "../solvers/GSSolver3D.hpp"
#include "../solvers/GCSSORSolver3D.hpp"
#include "../solvers/HybridSolver3D.hpp"
#include "../solvers/fakeField3D.hpp"
#include "../solvers/LODField3D.hpp"

#include "../flames/solidePhoto.hpp"

BEGIN_EVENT_TABLE(GLFlameCanvas, wxGLCanvas)
  EVT_SIZE(GLFlameCanvas::OnSize)
  EVT_PAINT(GLFlameCanvas::OnPaint)
  EVT_IDLE(GLFlameCanvas::OnIdle)
  EVT_MOTION(GLFlameCanvas::OnMouseMotion)
  EVT_MIDDLE_DOWN(GLFlameCanvas::OnMouseClick)
  EVT_MIDDLE_UP(GLFlameCanvas::OnMouseClick)
  EVT_RIGHT_DOWN(GLFlameCanvas::OnMouseClick)
  EVT_RIGHT_UP(GLFlameCanvas::OnMouseClick)
  EVT_MOUSEWHEEL(GLFlameCanvas::OnMouseWheel)
  EVT_CHAR(GLFlameCanvas::OnKeyPressed)
END_EVENT_TABLE();


GLFlameCanvas::GLFlameCanvas(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int* attribList, 
		       long style, const wxString& name, const wxPalette& palette)
  : wxGLCanvas(parent, id, pos, size, style, name, attribList, palette)
{
  m_init = false;
  m_run = false;
  m_pixels = new u_char[size.GetWidth()*size.GetHeight()*3];
  m_framesCount = 0;
  /* Pour éviter de faire un calcul pour ajouter des 0... */
  /* Un jour je ferais mieux, promis... */
  m_globalFramesCount = 1000000;
  m_intensities = NULL;
  m_switch = false;
  m_currentConfig = NULL;
  m_globalField = NULL;
  //m_framesCountForSwitch = 0;
  
  srand(clock());
}

GLFlameCanvas::~GLFlameCanvas()
{
  DestroyScene();
  delete [] m_pixels;
  if( m_intensities ) delete [] m_intensities;
  delete m_gammaEngine;
  delete m_SVShader;
  delete m_SVProgram;
}

void GLFlameCanvas::InitUISettings(void)
{
  /* Pour l'affichage */
  m_run = true;
  m_saveImages = false;
  m_glowOnly = false;
  m_displayBase = m_displayVelocity = m_displayParticles = m_displayGrid = m_displayWickBoxes = false;
  m_displayFlame = true;
  m_drawShadowVolumes = false;
  m_gammaCorrection = false;
}

void GLFlameCanvas::InitGL()
{
  m_width = m_currentConfig->width; m_height = m_currentConfig->height;
  
  glClearColor (0.0, 0.0, 0.0, 0.0);
  /* Restriction de la zone d'affichage */
  glViewport (0, 0, m_width, m_height);
  
  glEnable (GL_DEPTH_TEST);
  glEnable (GL_BLEND);
  //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glShadeModel (GL_SMOOTH);
  glEnable (GL_LINE_SMOOTH);
  /*glEnable(GL_POLYGON_SMOOTH); */
  
  glEnable (GL_CULL_FACE);
  glCullFace (GL_BACK);
  
  glEnable (GL_AUTO_NORMAL);
  glEnable (GL_NORMALIZE);
  
  glPolygonMode(GL_FRONT,GL_FILL);
  //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  
  glDisable (GL_LIGHTING);
  
  m_SVShader = new GLSLVertexShader();
  m_SVProgram = new GLSLProgram();
  m_SVShader->load ("shadowVolume.vp");
  m_SVProgram->attachShader(*m_SVShader);
  m_SVProgram->link();
  
  m_gammaEngine = new GammaEngine (m_width, m_height);
  setGammaCorrection( m_currentConfig->gammaCorrection );
  
}

void GLFlameCanvas::InitFlames(void)
{
  vector <Field3D *> extraSolvers;
  for(uint i=0 ; i < m_currentConfig->nbFlames; i++){
    switch(m_currentConfig->flames[i].type){
    case CANDLE :
      m_flames.push_back( new Candle (&m_currentConfig->flames[i], m_solvers[m_currentConfig->flames[i].solverIndex],
				      m_scene, "scenes/bougie.obj", i, m_SVProgram, 1/ 8.0));
      break;
    case FIRMALAMPE :
      m_flames.push_back( new Firmalampe(&m_currentConfig->flames[i], m_solvers[m_currentConfig->flames[i].solverIndex],
					 m_scene, "scenes/firmalampe.obj", i, m_SVProgram,
					 m_currentConfig->flames[i].wickName.fn_str()));
      break;
    case TORCH :
      m_flames.push_back( new Torch(&m_currentConfig->flames[i], m_solvers[m_currentConfig->flames[i].solverIndex], 
				    m_scene, m_currentConfig->flames[i].wickName.fn_str(), i, m_SVProgram));
      break;
    case CAMPFIRE :
      m_flames.push_back( new CampFire(&m_currentConfig->flames[i], m_solvers[m_currentConfig->flames[i].solverIndex],
				       m_scene, m_currentConfig->flames[i].wickName.fn_str(), i, m_SVProgram));
      break;
    case CANDLESSET :
      m_flames.push_back( new CandlesSet (&m_currentConfig->flames[i], m_solvers[m_currentConfig->flames[i].solverIndex],
					  extraSolvers, m_scene, m_currentConfig->flames[i].wickName.fn_str(),
					  i, m_SVProgram, m_currentConfig->solvers[m_currentConfig->flames[i].solverIndex].scale));
      m_solvers.insert(m_solvers.end(),extraSolvers.begin(),extraSolvers.end());
      break;
    case CANDLESTICK :
      m_flames.push_back( new CandleStick (&m_currentConfig->flames[i], m_solvers[m_currentConfig->flames[i].solverIndex],
					   m_scene, "scenes/bougie.obj", i, m_SVProgram, 1/ 8.0));
      break;
    default :
      cerr << "Unknown flame type : " << (int)m_currentConfig->flames[i].type << endl;
      ::wxExit();
    }
  }
  prevNbFlames = m_currentConfig->nbFlames;
  
  if( m_intensities ) delete [] m_intensities;
  m_intensities = new double[m_currentConfig->nbFlames];
}

void GLFlameCanvas::InitSolvers(void)
{
  for(uint i=0 ; i < m_currentConfig->nbSolvers; i++){
    switch(m_currentConfig->solvers[i].type){
    case GS_SOLVER :
      m_solvers.push_back( new GSSolver3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx,
					  m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz,
					  m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].scale,
					  m_currentConfig->solvers[i].timeStep, m_currentConfig->solvers[i].buoyancy));
      break;
    case GCSSOR_SOLVER :
      m_solvers.push_back( new GCSSORSolver3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx,
					      m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz,
					      m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].scale,
					      m_currentConfig->solvers[i].timeStep, m_currentConfig->solvers[i].buoyancy,
					      m_currentConfig->solvers[i].omegaDiff, m_currentConfig->solvers[i].omegaProj,
					      m_currentConfig->solvers[i].epsilon));
      break;
    case HYBRID_SOLVER :
      m_solvers.push_back( new HybridSolver3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx,
					      m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz,
					      m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].scale,
					      m_currentConfig->solvers[i].timeStep,
					      m_currentConfig->solvers[i].buoyancy, m_currentConfig->solvers[i].omegaDiff,
					      m_currentConfig->solvers[i].omegaProj, m_currentConfig->solvers[i].epsilon));
      break;
    case LOD_HYBRID_SOLVER :
      m_solvers.push_back( new LODHybridSolver3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx,
						 m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz,
						 m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].scale,
						 m_currentConfig->solvers[i].timeStep,
						 m_currentConfig->solvers[i].buoyancy, m_currentConfig->solvers[i].omegaDiff,
						 m_currentConfig->solvers[i].omegaProj, m_currentConfig->solvers[i].epsilon));
      break;
    case SIMPLE_FIELD :
      m_solvers.push_back( new RealField3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx,
					   m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz,
					   m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].scale,
					   m_currentConfig->solvers[i].timeStep, m_currentConfig->solvers[i].buoyancy));
      break;
    case FAKE_FIELD :
      m_solvers.push_back( new FakeField3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx,
					   m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz,
					   m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].scale,
					   m_currentConfig->solvers[i].timeStep, m_currentConfig->solvers[i].buoyancy));
      break;
    case LOD_FIELD :
      m_solvers.push_back( new LODField3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx,
					  m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz,
					  m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].scale,
					  m_currentConfig->solvers[i].timeStep, m_currentConfig->solvers[i].buoyancy,
					  m_currentConfig->solvers[i].omegaDiff, m_currentConfig->solvers[i].omegaProj,
					  m_currentConfig->solvers[i].epsilon));
      break;
    default :
      cerr << "Unknown solver type : " << (int)m_currentConfig->solvers[i].type << endl;
      ::wxExit();
    }
  }
  prevNbSolvers = m_currentConfig->nbSolvers;
}

void GLFlameCanvas::InitScene()
{  
  uint glowScales[2] = { 1, 4 };
  
  InitSolvers();
  
  m_scene = new Scene (m_currentConfig->sceneName.fn_str(), &m_flames);
  
  InitFlames();
  
  if(m_currentConfig->useGlobalField)
    m_globalField = new GlobalField(m_solvers, m_scene, m_currentConfig->globalField.type,
				    m_currentConfig->globalField.resx, m_currentConfig->globalField.timeStep,
				    m_currentConfig->globalField.omegaDiff, m_currentConfig->globalField.omegaProj, 
				    m_currentConfig->globalField.epsilon);
  
  m_photoSolid = new PhotometricSolidsRenderer(m_scene, &m_flames);
  
  m_scene->createVBOs();
  
  m_camera = new Camera (m_width, m_height, m_currentConfig->clipping, m_scene);
  
  m_glowEngine  = new GlowEngine (m_width, m_height, glowScales);
  m_depthPeelingEngine = new DepthPeelingEngine(m_width, m_height, DEPTH_PEELING_LAYERS_MAX, m_scene, &m_flames);
}

void GLFlameCanvas::Init (FlameAppConfig *config)
{  
  m_currentConfig = config;
  
  InitUISettings();
  SetCurrent();
  InitGL();
  
  InitScene();
  
  ::wxStartTimer();
  
  m_init = true;
  
  cerr << "Initialization over" << endl;
}

void GLFlameCanvas::Restart (void)
{
  Disable();
  m_init = false;
  DestroyScene();
  
  m_width = m_currentConfig->width; m_height = m_currentConfig->height;
  glViewport (0, 0, m_width, m_height);
  
//   InitUISettings();
  InitScene();
  setNbDepthPeelingLayers(m_currentConfig->nbDepthPeelingLayers);
  ::wxStartTimer();
  m_run = true;
  m_init = true;
  cerr << "Initialization over" << endl;
  Enable();
}

void GLFlameCanvas::ReloadSolversAndFlames (void)
{
  Disable();
  m_init = false;
  
  delete m_photoSolid;
  for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
       flamesIterator != m_flames.end (); flamesIterator++)
    delete (*flamesIterator);
  m_flames.clear();
  
  for (vector < Field3D* >::iterator solversIterator = m_solvers.begin ();
       solversIterator != m_solvers.end (); solversIterator++)
    delete (*solversIterator);
  m_solvers.clear();
  
  if(m_globalField){
    delete m_globalField;
    m_globalField = NULL;
  }
  
  InitSolvers();
  
  InitFlames();
  
  m_photoSolid = new PhotometricSolidsRenderer(m_scene, &m_flames);
  
  if(m_currentConfig->useGlobalField)
    m_globalField = new GlobalField(m_solvers, m_scene, m_currentConfig->globalField.type,
				    m_currentConfig->globalField.resx, m_currentConfig->globalField.timeStep,
				    m_currentConfig->globalField.omegaDiff, m_currentConfig->globalField.omegaProj, 
				    m_currentConfig->globalField.epsilon);
  
  ::wxStartTimer();
  m_run = true;
  m_init = true;
  cerr << "Initialization over" << endl;
  Enable();
}

void GLFlameCanvas::RegeneratePhotometricSolids(uint flameIndex, wxString IESFileName)
{
  m_flames[flameIndex]->useNewIESFile(IESFileName.ToAscii());
  m_photoSolid->deleteTexture();
  m_photoSolid->generateTexture();
}

void GLFlameCanvas::DestroyScene(void)
{ 
  delete m_depthPeelingEngine;
  delete m_glowEngine;
  delete m_camera;
  delete m_scene;
  delete m_photoSolid;
  
  for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
       flamesIterator != m_flames.end (); flamesIterator++)
    delete (*flamesIterator);
  m_flames.clear();
  
  if(m_globalField){
    delete m_globalField;
    m_globalField = NULL;
  }
  for (vector < Field3D* >::iterator solversIterator = m_solvers.begin ();
       solversIterator != m_solvers.end (); solversIterator++)
    delete (*solversIterator);
  m_solvers.clear();
}

void GLFlameCanvas::OnIdle(wxIdleEvent& event)
{
  if(m_run && m_init){
    if(m_currentConfig->useGlobalField)
      m_globalField->cleanSources ();
    for (vector < Field3D* >::iterator solversIterator = m_solvers.begin ();
	 solversIterator != m_solvers.end (); solversIterator++)
      (*solversIterator)->cleanSources ();
    
    for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
	 flamesIterator != m_flames.end (); flamesIterator++)
      (*flamesIterator)->addForces ();
    
    if(m_currentConfig->useGlobalField) m_globalField->iterate();
    for (vector < Field3D* >::iterator solversIterator = m_solvers.begin ();
	 solversIterator != m_solvers.end (); solversIterator++)
      (*solversIterator)->iterate ();
  }
  
  /* Force à redessiner */
  this->Refresh();
  
  /*  draw();*/
  //event.RequestMore();
}

void GLFlameCanvas::OnMouseMotion(wxMouseEvent& event)
{
  m_camera->OnMouseMotion(event);
}

void GLFlameCanvas::OnMouseClick(wxMouseEvent& event)
{
  m_camera->OnMouseClick(event);
}

void GLFlameCanvas::OnMouseWheel(wxMouseEvent& event)
{
  m_camera->moveOnFrontOrBehind(-event.GetWheelRotation()/1000.0);
}

void GLFlameCanvas::OnKeyPressed(wxKeyEvent& event)
{
  double step=0.1;
  switch(event.GetKeyCode())
    {      
    case WXK_LEFT: m_camera->moveOnSides(step); break;
    case WXK_RIGHT: m_camera->moveOnSides(-step); break;
    case WXK_UP: m_camera->moveOnFrontOrBehind(-step*10); break;
    case WXK_DOWN: m_camera->moveOnFrontOrBehind(step*10); break;
    case WXK_HOME: m_camera->moveUpOrDown(-step); break;
    case WXK_END: m_camera->moveUpOrDown(step); break;
    case 'l':
      //       m_framesCountForSwitch = 1;
//       m_switch = true;
      for (vector < Field3D* >::iterator solversIterator = m_solvers.begin ();
	   solversIterator != m_solvers.end (); solversIterator++)
	(*solversIterator)->decreaseRes ();
      break;
      
    case 'L': 
      //       m_framesCountForSwitch = 1;
      //       m_switch = true;
      for (vector < Field3D* >::iterator solversIterator = m_solvers.begin ();
	   solversIterator != m_solvers.end (); solversIterator++)
	(*solversIterator)->increaseRes ();
      break;
    case WXK_SPACE : m_run = !m_run; break;
    }
  event.Skip();
}

/** Fonction de dessin global */
void GLFlameCanvas::OnPaint (wxPaintEvent& event)
{
  if(!m_init)
    return;
  
  wxPaintDC dc(this);
  
  if(!GetContext())
    return;
  
  SetCurrent();
  
  /* Déplacement du camera */
  m_camera->setView();
  
  /********** CONSTRUCTION DES FLAMMES *******************************/
  // SDL_mutexP (lock);
//   if(m_framesCountForSwitch){
//     if(m_framesCountForSwitch == 6){
//       m_switch = false;
//       m_framesCountForSwitch = 0;
//     }else
//       m_framesCountForSwitch++;
//   }
  if(m_run && !m_switch)
    for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
	 flamesIterator != m_flames.end (); flamesIterator++)
      (*flamesIterator)->build();
  // SDL_mutexV (lock);
  
  /********** RENDU DES FLAMMES AVEC LE GLOW  *******************************/
  m_visibility = false;
  if(m_displayFlame){
    for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
	 flamesIterator != m_flames.end (); flamesIterator++)
      if((*flamesIterator)->isVisible()){
	m_visibility = true;
	break;
      }
  }
  
  if(m_visibility || m_displayParticles){
    if(m_currentConfig->glowEnabled ){
      //    GLdouble m[4][4];
      //    double dist, sigma;
    
      /* Adaptation du flou en fonction de la distance */
      /* On module la largeur de la gaussienne */    
      //     glGetDoublev (GL_MODELVIEW_MATRIX, &m[0][0]);
    
      //     Point position(m[3][0], m[3][1], m[3][2]);
      //     Vector direction = position;
      //     dist = direction.length();
    
      /* Définition de la largeur de la gaussienne en fonction de la distance */
      /* A définir de manière plus précise par la suite */
      //     sigma = dist > 0.1 ? -log(4*dist)+6 : 6.0;
      //sigma = dist > 0.1 ? -log(dist)+6 : 6.0;
      //     sigma = 2;
      
      if(m_currentConfig->depthPeelingEnabled){
	/* On décortique dans les calques */
	m_depthPeelingEngine->makePeels(m_displayFlame, m_displayParticles, m_displayFlamesBoundingSpheres);
	
	m_glowEngine->activate();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	/* On affiche la superposition des calques que l'on vient de décortiquer */
	m_depthPeelingEngine->render(m_flames);
      }else{
	m_glowEngine->activate();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBlendColor(.2,.2,.2,1.0);
	glBlendFunc (GL_ONE, GL_CONSTANT_COLOR);
	
	/* Dessin de la scène sans les textures pour avoir les occlusions sur les flammes */
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	m_scene->drawSceneWT ();
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	/* Dessin de la flamme */
	for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
	     flamesIterator != m_flames.end (); flamesIterator++)
	  (*flamesIterator)->drawFlame (m_displayFlame, m_displayParticles, m_displayFlamesBoundingSpheres);
      }
      m_glowEngine->blur(m_flames);
    
      m_glowEngine->deactivate();
    }else
      if(m_currentConfig->depthPeelingEnabled)
	/* On effectue l'épluchage avant d'activer le gamma car tous les deux utilisent un FBO */
	m_depthPeelingEngine->makePeels(m_displayFlame, m_displayParticles, m_displayFlamesBoundingSpheres);  
  }
  if(m_gammaCorrection)
    m_gammaEngine->enableGamma();
  if(!m_glowOnly){
    drawScene();
    /********************* DESSINS DES FLAMMES SANS GLOW **********************************/
    if((m_visibility || m_displayParticles) && !m_currentConfig->glowEnabled )
      if(m_currentConfig->depthPeelingEnabled)
	m_depthPeelingEngine->render(m_flames);
      else
	for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
	     flamesIterator != m_flames.end (); flamesIterator++)
	  (*flamesIterator)->drawFlame (m_displayFlame, m_displayParticles, m_displayFlamesBoundingSpheres);
  }
  if((m_visibility || m_displayParticles) && m_currentConfig->glowEnabled )
    m_glowEngine->drawBlur(m_flames);
  if(m_gammaCorrection)
    m_gammaEngine->disableGamma();
  
  /******** A VERIFIER *******/
  //   glFlush();
  //glFinish();
  /***************************/
  SwapBuffers ();
  
  //event.Skip();
  
  /******************** CALCUL DU FRAMERATE *************************************/
  m_framesCount++;
  m_globalFramesCount++;
  
  m_t = ::wxGetElapsedTime (false);
  if (m_t >= 2000){    
    ((FlamesFrame *)GetParent())->SetFPS( m_framesCount / (m_t/1000) );
    ::wxStartTimer();
    m_framesCount = 0;
  }
  
  if(m_saveImages){
    wxString filename;
    wxString zeros;
    
    glReadPixels (0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, m_pixels);
    
    filename << _("captures/capture") << m_globalFramesCount << _(".png");
    /* Création d'une image, le dernier paramètre précise que wxImage ne doit pas détruire */
    /* le tableau de données dans son destructeur */
    wxImage image(m_width,m_height,m_pixels,true),image2;
    image2 = image.Mirror(false);
    if(!image2.SaveFile(filename,wxBITMAP_TYPE_PNG))
      cerr << "Image saving error !!" << endl;
  }
}

void GLFlameCanvas::drawScene()
{
  Point position, scale;
  
  if(m_currentConfig->lightingMode == LIGHTING_PHOTOMETRIC)
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  else
    glBlendFunc (GL_ONE, GL_ZERO);
  
  if (m_currentConfig->shadowsEnabled)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  else
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /******************* AFFICHAGE DE LA SCENE *******************************/
  for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
       flamesIterator != m_flames.end (); flamesIterator++)
    (*flamesIterator)->drawWick (m_displayWickBoxes);
  
  /**** Affichage de la scène ****/  
  if (m_drawShadowVolumes)
    for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
	 flamesIterator != m_flames.end (); flamesIterator++)
      (*flamesIterator)->drawShadowVolume ((float *)m_currentConfig->fatness, (float *)m_currentConfig->extrudeDist);
  
  if (m_currentConfig->shadowsEnabled)
    castShadows();  
  else
    if(m_currentConfig->lightingMode == LIGHTING_PHOTOMETRIC)
      m_photoSolid->draw(m_currentConfig->BPSEnabled);
    else{
      glEnable (GL_LIGHTING);
      for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
	   flamesIterator != m_flames.end (); flamesIterator++)
	(*flamesIterator)->switchOn ();
      m_scene->drawScene();
      glDisable (GL_LIGHTING);
    }
  
  /************ Affichage des outils d'aide à la visu (grille, etc...) *********/
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  if(m_currentConfig->useGlobalField){
    position = m_globalField->getPosition ();
    glPushMatrix ();
    glTranslatef (position.x, position.y, position.z);
    if (m_displayBase)
      m_globalField->displayBase();
    if (m_displayGrid)
      m_globalField->displayGrid();
    if (m_displayVelocity)
      m_globalField->displayVelocityField();
    glPopMatrix ();
  }
  for (vector < Field3D* >::iterator solversIterator = m_solvers.begin ();
       solversIterator != m_solvers.end (); solversIterator++)
    {
      position = (*solversIterator)->getPosition ();
      scale =  (*solversIterator)->getScale ();
      
      glPushMatrix ();
      glTranslatef (position.x, position.y, position.z);
      glScalef (scale.x, scale.y, scale.z);
      if (m_displayBase)
	(*solversIterator)->displayBase();
      if (m_displayGrid)
	(*solversIterator)->displayGrid();
      if (m_displayVelocity)
	(*solversIterator)->displayVelocityField();
      glPopMatrix ();
    }
}
// void
// GLFlameCanvas::cast_shadows_double_multiple ()
// {
//   switch_off_lights ();
//   m_scene->drawSceneWTEX ();

//   glBlendFunc (GL_ONE, GL_ONE);
//   for (int i = 0; i < 1 /*m_nbLights *//**SHADOW_SAMPLE_PER_LIGHT*/ ; i++)
//     {
//       enable_only_ambient_light (i);
//       glClear (GL_STENCIL_BUFFER_BIT);
//       glDepthFunc (GL_LESS);
//       glPushAttrib (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
// 		    GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT);

//       glColorMask (0, 0, 0, 0);
//       glDepthMask (0);

//       glDisable (GL_CULL_FACE);
//       glEnable (GL_STENCIL_TEST);
//       glEnable (GL_STENCIL_TEST_TWO_SIDE_EXT);

//       glActiveStencilFaceEXT (GL_BACK);
//       glStencilOp (GL_KEEP,	// stencil test fail
// 		   GL_DECR_WRAP_EXT,	// depth test fail
// 		   GL_KEEP);	// depth test pass
//       glStencilMask (~0);
//       glStencilFunc (GL_ALWAYS, 0, ~0);

//       glActiveStencilFaceEXT (GL_FRONT);
//       glStencilOp (GL_KEEP,	// stencil test fail
// 		   GL_INCR_WRAP_EXT,	// depth test fail
// 		   GL_KEEP);	// depth test pass
//       glStencilMask (~0);
//       glStencilFunc (GL_ALWAYS, 0, ~0);

//       draw_shadowVolume2 (i);

//       glPopAttrib ();

//       glDepthFunc (GL_EQUAL);

//       glEnable (GL_STENCIL_TEST);
//       glStencilFunc (GL_EQUAL, 0, ~0);
//       glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);

//       m_scene->drawSceneWTEX ();
      
//       reset_diffuse_light (i);
//     }
//   glDisable (GL_STENCIL_TEST);
//   for (int i = 0; i < m_nbLights /**SHADOW_SAMPLE_PER_LIGHT*/ ; i++)
//     {
//       enable_only_ambient_light (i);
//     }
//   m_scene->drawSceneWTEX ();
//   for (int i = 0; i < m_nbLights /**SHADOW_SAMPLE_PER_LIGHT*/ ; i++)
//     {
//       reset_diffuse_light (i);
//     }
//   switch_on_lights ();
//   glBlendFunc (GL_ZERO, GL_SRC_COLOR);
//   m_scene->drawScene ();
// }

void GLFlameCanvas::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);
    
    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
    int w, h;
    GetClientSize(&w, &h);
#ifndef __WXMOTIF__
    if (GetContext())
#endif
    {
        SetCurrent();
        glViewport(0, 0, (GLint) w, (GLint) h);
    }
}

void GLFlameCanvas::castShadows ()
{
  glEnable (GL_LIGHTING);
  for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
       flamesIterator != m_flames.end (); flamesIterator++)
    (*flamesIterator)->switchOff ();
  m_scene->drawSceneWT ();
  
  if(m_currentConfig->lightingMode == LIGHTING_STANDARD)
    for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
	 flamesIterator != m_flames.end (); flamesIterator++)
      (*flamesIterator)->switchOn ();
  
  glPushAttrib (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_POLYGON_BIT);
  
  glColorMask (0, 0, 0, 0);
  glDepthMask (0);
  
  glDisable (GL_CULL_FACE);
  glEnable (GL_STENCIL_TEST);
  glEnable (GL_STENCIL_TEST_TWO_SIDE_EXT);
  
  glActiveStencilFaceEXT (GL_BACK);
  glStencilOp (GL_KEEP,	// stencil test fail
	       GL_DECR_WRAP_EXT,	// depth test fail
	       GL_KEEP);	// depth test pass
  glStencilMask (~0);
  glStencilFunc (GL_ALWAYS, 0, ~0);
  
  glActiveStencilFaceEXT (GL_FRONT);
  glStencilOp (GL_KEEP,	// stencil test fail
	       GL_INCR_WRAP_EXT,	// depth test fail
	       GL_KEEP);	// depth test pass
  glStencilMask (~0);
  glStencilFunc (GL_ALWAYS, 0, ~0);

  for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
       flamesIterator != m_flames.end (); flamesIterator++)
    (*flamesIterator)->drawShadowVolume ((float *)m_currentConfig->fatness, (float *)m_currentConfig->extrudeDist);
  
  glPopAttrib ();
  
  /* On teste ensuite Ã  l'endroit où il faut dessiner */
  glDepthFunc (GL_LEQUAL);
  
  glStencilFunc (GL_EQUAL, 0, ~0);
  glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
  glBlendFunc (GL_ONE, GL_ONE);
  
  /* Activation de l'éclairage ambiant uniquement */
  GLfloat val_ambiant[]={1.0,1.0,1.0,1.0};
  GLfloat null[]={0.0,0.0,0.0,1.0};
  
  glLightfv(GL_LIGHT0,GL_DIFFUSE,null);
  glLightfv(GL_LIGHT0,GL_SPECULAR,null);
  glLightfv(GL_LIGHT0,GL_AMBIENT,val_ambiant);
  glEnable(GL_LIGHT0);
  
  /* Dessin des ombres en noir & blanc */
  m_scene->drawSceneWT ();
   
  glDisable (GL_STENCIL_TEST);
  /* Affichage de la scène en couleur en multipliant avec l'affichage précédent */
  
  glBlendFunc (GL_ZERO, GL_SRC_COLOR);
  if(m_currentConfig->lightingMode == LIGHTING_STANDARD){
    for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
	 flamesIterator != m_flames.end (); flamesIterator++)
      (*flamesIterator)->switchOn ();
    
    m_scene->drawScene ();
    glDisable (GL_LIGHTING);
  }else{
    glDisable (GL_LIGHTING);
    m_photoSolid->draw(m_currentConfig->BPSEnabled);
  }
}

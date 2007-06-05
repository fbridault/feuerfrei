#include "GLFlameCanvas.hpp"

#include "flamesFrame.hpp"
#include <iostream>

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
  m_currentConfig = NULL;
  m_globalField = NULL;
  //m_framesCountForSwitch = 0;
  
  srand(clock());
}

GLFlameCanvas::~GLFlameCanvas()
{
  /* On ne détruit pas les threads, la méthode FlamesFrame::OnClose() s'en occupe */
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
  m_displayFlamesBoundingVolumes = 0;
}

void GLFlameCanvas::InitGL()
{
  m_width = m_currentConfig->width; m_height = m_currentConfig->height;
  
  glClearColor (0.0, 0.0, 0.0, 0.0);
  /* Restriction de la zone d'affichage */
  glViewport (0, 0, m_width, m_height);
  
  glEnable (GL_DEPTH_TEST);
  glDisable (GL_BLEND);
  
  glShadeModel (GL_SMOOTH);
  glEnable (GL_LINE_SMOOTH);
  
  glEnable (GL_CULL_FACE);
  glCullFace (GL_BACK);
  
  glEnable (GL_AUTO_NORMAL);
  glEnable (GL_NORMALIZE);
  
  glPolygonMode(GL_FRONT,GL_FILL);
  
  glDisable (GL_LIGHTING);
  
  m_SVShader = new GLSLVertexShader();
  m_SVProgram = new GLSLProgram();
  m_SVShader->load ("shadowVolume.vp");
  m_SVProgram->attachShader(*m_SVShader);
  m_SVProgram->link();
  
  m_gammaEngine = new GammaEngine (m_width, m_height);
  setGammaCorrection( m_currentConfig->gammaCorrection );
  
}

void GLFlameCanvas::InitLuminaries(void)
{
  for(uint i=0; i < m_currentConfig->nbLuminaries; i++)
    m_luminaries.push_back( new Luminary( m_currentConfig->luminaries[i], m_solvers, m_flames, m_scene,
					  m_SVProgram, m_currentConfig->luminaries[i].fileName.fn_str(), i) );
  
//   m_currentConfig->nbFires = m_flames.size(); m_currentConfig->nbFields = m_solvers.size(); 
  prevNbFlames = m_flames.size(); prevNbSolvers = m_solvers.size();
  
  if( m_intensities ) delete [] m_intensities;
  m_intensities = new double[m_flames.size()];
}

void GLFlameCanvas::InitThreads()
{
  /* Création d'un thread par champ de vélocité. */
  for (vector <Field3D *>::iterator solversIterator = m_solvers.begin ();
       solversIterator != m_solvers.end (); solversIterator++)    
    m_threads.push_back(new FieldFiresThread(*solversIterator, m_scheduler));
  
  if(m_currentConfig->useGlobalField){
    m_globalField = new GlobalField(m_threads, m_scene, m_currentConfig->globalField.type,
				    m_currentConfig->globalField.resx, m_currentConfig->globalField.timeStep,
				    m_currentConfig->globalField.omegaDiff, m_currentConfig->globalField.omegaProj,
				    m_currentConfig->globalField.epsilon);
    /* Ajout du thread du solveur global le cas échéant */
    m_threads.push_back(new GlobalFieldThread(m_globalField, m_scheduler));
  }
  m_scheduler->Init(m_threads);
  
  if ( m_scheduler->Create() != wxTHREAD_NO_ERROR )
    {
      wxLogError(_("Can't create scheduler thread!"));
    }
  
  for (list <FieldThread *>::iterator threadsIterator = m_threads.begin ();
       threadsIterator != m_threads.end (); threadsIterator++)
    /* Création proprement dite du thread, il ne reste plus qu'à le lancer avec Run() */
    if ( (*threadsIterator)->Create() != wxTHREAD_NO_ERROR )
      {
	wxLogError(_("Can't create thread!"));
      }
  
  /* Lancement de l'ordonnanceur */
  m_scheduler->Run();
  /* Lancement des threads */
  for (list < FieldThread* >::iterator threadsIterator = m_threads.begin ();
       threadsIterator != m_threads.end (); threadsIterator++)
    (*threadsIterator)->Run();
}

void GLFlameCanvas::InitScene()
{  
  uint glowScales[2] = { 1, 4 };
  
  m_scene = new Scene (m_currentConfig->sceneName.fn_str(), &m_luminaries, &m_flames);
  
  InitLuminaries();  
  
  /** Il faut initialiser l'ordonnanceur avant que d'éventuels threads de CandleSets soit instanciés */
  m_scheduler = new FieldThreadsScheduler();
  
  m_photoSolid = new PhotometricSolidsRenderer(m_scene, &m_flames);
  
  m_scene->createVBOs();
  
  m_camera = new Camera (m_width, m_height, m_currentConfig->clipping, m_scene);
  
  m_glowEngine  = new GlowEngine (m_width, m_height, glowScales);
  m_depthPeelingEngine = new DepthPeelingEngine(m_width, m_height, DEPTH_PEELING_LAYERS_MAX);
  
  m_swatch = new wxStopWatch();
  m_swatch->Pause();
  
  InitThreads();
}

void GLFlameCanvas::Init (FlameAppConfig *config)
{  
  m_currentConfig = config;
  
  InitUISettings();
  SetCurrent();
  InitGL();
  
  InitScene();
  
  m_swatch->Start();
  
  m_init = true;
  
  cerr << "Initialization over" << endl;
}

void GLFlameCanvas::PauseThreads()
{
  m_scheduler->Pause();
  for (list < FieldThread* >::iterator threadsIterator = m_threads.begin ();
       threadsIterator != m_threads.end (); threadsIterator++){
    (*threadsIterator)->Pause();
  }
}

void GLFlameCanvas::ResumeThreads()
{
  m_scheduler->Resume();
  for (list < FieldThread* >::iterator threadsIterator = m_threads.begin ();
       threadsIterator != m_threads.end (); threadsIterator++){
    (*threadsIterator)->Resume();
  }
}

void GLFlameCanvas::DeleteThreads()
{
  /* On sort les threads de leur pause si nécessaire, car sinon il serait impossible de les arrêter ! */
  if( !IsRunning() ) setRunningState(true);
  m_scheduler->Stop();
  
  for (list < FieldThread* >::iterator threadsIterator = m_threads.begin ();
       threadsIterator != m_threads.end (); threadsIterator++)
    (*threadsIterator)->Stop();

  m_scheduler->forceEnd();
  m_scheduler->unblock();
  m_scheduler->Wait();
  
  for (list < FieldThread* >::iterator threadsIterator = m_threads.begin ();
       threadsIterator != m_threads.end (); threadsIterator++)
    (*threadsIterator)->Wait();
  
  /* Tous les threads sont terminés, on peut tout supprimer sans risque */
  delete m_scheduler;
  for (list < FieldThread* >::iterator threadsIterator = m_threads.begin ();
       threadsIterator != m_threads.end (); threadsIterator++)
    delete (*threadsIterator);
  m_threads.clear();
}

void GLFlameCanvas::Restart (void)
{
  Disable();
  m_init = false;
  DeleteThreads();
  DestroyScene();
  
  m_width = m_currentConfig->width; m_height = m_currentConfig->height;
  glViewport (0, 0, m_width, m_height);
  
  setNbDepthPeelingLayers(m_currentConfig->nbDepthPeelingLayers);
  
  InitScene();
  m_swatch->Start();
  m_run = true;
  m_init = true;
  cerr << "Initialization over" << endl;
  Enable();
}

void GLFlameCanvas::ReloadSolversAndFlames (void)
{
  Disable();
  m_init = false;
  DeleteThreads();
  
  delete m_photoSolid;
  for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
       flamesIterator != m_flames.end (); flamesIterator++)
    delete (*flamesIterator);
  m_flames.clear();
  
  for (vector < Field3D* >::iterator solversIterator = m_solvers.begin ();
       solversIterator != m_solvers.end (); solversIterator++)
    delete (*solversIterator);
  m_solvers.clear();
  
  for (vector < Luminary* >::iterator luminariesIterator = m_luminaries.begin ();
       luminariesIterator != m_luminaries.end (); luminariesIterator++)
    delete (*luminariesIterator);
  m_luminaries.clear();
  
  if(m_globalField){
    delete m_globalField;
    m_globalField = NULL;
  }
  
  InitLuminaries();
  /** Il faut initialiser l'ordonnanceur avant que d'éventuels threads de CandleSets soit instanciés */
  m_scheduler = new FieldThreadsScheduler();
  
  m_photoSolid = new PhotometricSolidsRenderer(m_scene, &m_flames);
  
  InitThreads();
  m_swatch->Start();
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
  
  for (vector < Luminary* >::iterator luminariesIterator = m_luminaries.begin ();
       luminariesIterator != m_luminaries.end (); luminariesIterator++)
    delete (*luminariesIterator);
  m_luminaries.clear();
}

void GLFlameCanvas::OnIdle(wxIdleEvent& event)
{
  /* Force à redessiner */
  this->Refresh();
//   thisApp->Yield();
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
    case WXK_UP: m_camera->moveOnFrontOrBehind(-step); break;
    case WXK_DOWN: m_camera->moveOnFrontOrBehind(step); break;
    case WXK_HOME: m_camera->moveUpOrDown(-step); break;
    case WXK_END: m_camera->moveUpOrDown(step); break;
    case 'l':
      for (vector < Field3D* >::iterator solversIterator = m_solvers.begin ();
	   solversIterator != m_solvers.end (); solversIterator++)
	(*solversIterator)->decreaseRes ();
      break;
    case 'L': 
      for (vector < Field3D* >::iterator solversIterator = m_solvers.begin ();
	   solversIterator != m_solvers.end (); solversIterator++)
	(*solversIterator)->increaseRes ();
      break;
    case WXK_SPACE : setRunningState(!m_run); ; break;
    }
  event.Skip();
}

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
    
  if(m_run){
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
	m_depthPeelingEngine->makePeels(this, m_scene);
	
	m_glowEngine->activate();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	/* On affiche la superposition des calques que l'on vient de décortiquer */
	m_depthPeelingEngine->render(this);
      }else{
	m_glowEngine->activate();
	
	/* Dessin de la scène dans le depth buffer sans les textures pour avoir les occlusions sur les flammes */
	glClear(GL_DEPTH_BUFFER_BIT);
 	glDrawBuffer(GL_NONE);
 	glReadBuffer(GL_NONE);
 	m_scene->drawSceneWT ();
 	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
 	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	drawFlames();
	glDisable(GL_BLEND);
      }
      m_glowEngine->blur(this);
      
      m_glowEngine->deactivate();
    }else
      if(m_currentConfig->depthPeelingEnabled)
	/* On effectue l'épluchage avant d'activer le gamma car tous les deux utilisent un FBO */
	m_depthPeelingEngine->makePeels(this, m_scene);
  }
  if(m_gammaCorrection)
    m_gammaEngine->enableGamma();
  if(!m_glowOnly){
    drawScene();
    /********************* DESSINS DES FLAMMES SANS GLOW **********************************/
    if((m_visibility || m_displayParticles) && !m_currentConfig->glowEnabled )
      if(m_currentConfig->depthPeelingEnabled)
	m_depthPeelingEngine->render(this);
      else
	drawFlames();
  }
  if((m_visibility || m_displayParticles) && m_currentConfig->glowEnabled )
    m_glowEngine->drawBlur(this);
  if(m_gammaCorrection)
    m_gammaEngine->disableGamma();
  
  /* Permet d'attendre que toutes les commandes OpenGL soient bien effectuées avant de faire le rendu */
  /* La différence n'est pas forcément visible en pratique, cela supprime surtout quelques petits bugs d'affichage */
  /* potentiels, par contre le framerate en prend un sacré coup !! */
  /* Dans l'idéal, je pense donc qu'il serait utile de placer du code CPU ici ! */
  //glFinish();
  SwapBuffers ();
  
  //event.Skip();
  
  /******************** CALCUL DU FRAMERATE *************************************/
  m_framesCount++;
  m_globalFramesCount++;
  
  m_t = m_swatch->Time();
  if (m_t >= 2000){    
    ((FlamesFrame *)GetParent())->SetFPS( m_framesCount / (m_t/1000), m_scheduler->getNbSolved() / (m_t/1000) );
    m_swatch->Start();
    m_framesCount = 0;
    m_scheduler->resetNbSolved();
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
  m_scheduler->unblock();
}

void GLFlameCanvas::drawScene()
{
  Point position, scale;
    
  if (m_currentConfig->shadowsEnabled)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  else
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /******************* AFFICHAGE DE LA SCENE *******************************/
  for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
       flamesIterator != m_flames.end (); flamesIterator++)
    (*flamesIterator)->drawWick (m_displayWickBoxes);
  
  /**** Affichage de la scène ****/  
  if (m_drawShadowVolumes){
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (vector < FireSource* >::iterator flamesIterator = m_flames.begin ();
	 flamesIterator != m_flames.end (); flamesIterator++)
      (*flamesIterator)->drawShadowVolume ((float *)m_currentConfig->fatness, (float *)m_currentConfig->extrudeDist);
    glDisable(GL_BLEND);
  }
   
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
  glEnable (GL_BLEND);
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
  glDisable (GL_BLEND);
}

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
  glEnable(GL_BLEND);
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
  glDisable(GL_BLEND);
}

#include "GLFlameCanvas.hpp"

#include "flamesFrame.hpp"
#include <iostream>

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

CGcontext *contextCopy;

/** Fonction appelée en cas d'erreur provoquée par Cg */
void cgErrorCallback(void)
{
  CGerror LastError = cgGetError();
  
  if(LastError){
    const char *Listing = cgGetLastListing(*contextCopy);
    cerr << "\n---------------------------------------------------\n" << endl;
    cerr << cgGetErrorString(LastError) << endl << endl;
    if(Listing != NULL) cerr << Listing << endl;
    cerr << "---------------------------------------------------\n" << endl;
    cerr << "Cg error, exiting...\n" << endl << flush;
  }
}

GLFlameCanvas::GLFlameCanvas(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int* attribList, 
		       long style, const wxString& name, const wxPalette& palette)
  : wxGLCanvas(parent, id, pos, size, style, name, attribList, palette)
{
  m_init = false;
  m_run = 0;
  m_pixels = new u_char[size.GetWidth()*size.GetHeight()*3];
  m_framesCount = 0;
  /* Pour éviter de faire un calcul pour ajouter des 0... */
  /* Un jour je ferais mieux, promis... */
  m_globalFramesCount = 1000000;
  intensities = NULL;
  m_switch = false;
  
  srand(clock());
}

GLFlameCanvas::~GLFlameCanvas()
{
  DestroyScene();
  delete [] m_pixels;
  delete m_SVShader;
  if (m_context)
    cgDestroyContext (m_context);
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
}

void GLFlameCanvas::InitGL(bool recompileShaders)
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
  
  // Création du contexte CG
  m_context = cgCreateContext();
  
  contextCopy = &m_context;
  cgSetErrorCallback(cgErrorCallback);
  
  m_SVShader = new CgSVShader (_("ShadowVolumeExtrusion.cg"), _("SVExtrude"), &m_context, 
			       m_currentConfig->fatness, m_currentConfig->extrudeDist, recompileShaders);
}

void GLFlameCanvas::InitFlames(void)
{
  for(uint i=0 ; i < m_currentConfig->nbFlames; i++){
    switch(m_currentConfig->flames[i].type){
    case CANDLE :
      m_flames[i] = new Candle (&m_currentConfig->flames[i], m_solvers[m_currentConfig->flames[i].solverIndex],
				m_scene, "scenes/bougie.obj", i, m_SVShader, 
				1/ 7.0);
      break;
    case FIRMALAMPE :
      m_flames[i] = new Firmalampe(&m_currentConfig->flames[i], m_solvers[m_currentConfig->flames[i].solverIndex],
				   m_scene, "scenes/firmalampe.obj", i, m_SVShader, m_currentConfig->flames[i].wickName.fn_str());
      break;
    case TORCH :
      m_flames[i] = new Torch(&m_currentConfig->flames[i], m_solvers[m_currentConfig->flames[i].solverIndex], 
			      m_scene, m_currentConfig->flames[i].wickName.fn_str(), i, m_SVShader);
      break;
    case CAMPFIRE :
      m_flames[i] = new CampFire(&m_currentConfig->flames[i], m_solvers[m_currentConfig->flames[i].solverIndex],
				 m_scene, m_currentConfig->flames[i].wickName.fn_str(), i, m_SVShader);
      break;
    case CANDLESTICK :
      m_flames[i] = new CandleStick (&m_currentConfig->flames[i], m_solvers[m_currentConfig->flames[i].solverIndex],
				     m_scene, "scenes/bougie.obj", i, m_SVShader, 
				     1/ 7.0);
      break;
    default :
      cerr << "Unknown flame type : " << (int)m_currentConfig->flames[i].type << endl;
      ::wxExit();
    }
  }
  prevNbFlames = m_currentConfig->nbFlames;
  
  if( intensities ) delete intensities;
  intensities = new double[m_currentConfig->nbFlames];
  //  ToggleDepthPeeling();
}

void GLFlameCanvas::InitSolvers(void)
{
  m_solvers = new Field3D *[m_currentConfig->nbSolvers];
  for(uint i=0 ; i < m_currentConfig->nbSolvers; i++){
    switch(m_currentConfig->solvers[i].type){
    case GS_SOLVER :
      m_solvers[i] = new GSSolver3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
				    m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
				    m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].scale, 
				    m_currentConfig->solvers[i].timeStep, m_currentConfig->solvers[i].buoyancy);
      break;
    case GCSSOR_SOLVER :
      m_solvers[i] = new GCSSORSolver3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
					m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
					m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].scale,
					m_currentConfig->solvers[i].timeStep, m_currentConfig->solvers[i].buoyancy,
					m_currentConfig->solvers[i].omegaDiff, m_currentConfig->solvers[i].omegaProj,
					m_currentConfig->solvers[i].epsilon);
      break;
    case HYBRID_SOLVER :
      m_solvers[i] = new HybridSolver3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
					m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
					m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].scale,
					m_currentConfig->solvers[i].timeStep,
					m_currentConfig->solvers[i].buoyancy, m_currentConfig->solvers[i].omegaDiff, 
					m_currentConfig->solvers[i].omegaProj, m_currentConfig->solvers[i].epsilon);
      break;
    case LOD_HYBRID_SOLVER :
      m_solvers[i] = new LODHybridSolver3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
					   m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
					   m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].scale,
					   m_currentConfig->solvers[i].timeStep,
					   m_currentConfig->solvers[i].buoyancy, m_currentConfig->solvers[i].omegaDiff, 
					   m_currentConfig->solvers[i].omegaProj, m_currentConfig->solvers[i].epsilon);
      break;
    case SIMPLE_FIELD :
      m_solvers[i] = new RealField3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
				     m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
				     m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].scale,
				     m_currentConfig->solvers[i].timeStep, m_currentConfig->solvers[i].buoyancy);
      break;
    case FAKE_FIELD :
      m_solvers[i] = new FakeField3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
				     m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
				     m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].scale,
				     m_currentConfig->solvers[i].timeStep, m_currentConfig->solvers[i].buoyancy);
      break;
    default :
      cerr << "Unknown solver type : " << (int)m_currentConfig->solvers[i].type << endl;
      ::wxExit();
    }
  }
  prevNbSolvers = m_currentConfig->nbSolvers;
}

void GLFlameCanvas::InitScene(bool recompileShaders)
{  
  uint glowScales[2] = { 1, 4 };
  InitSolvers();
  
  m_flames = new FireSource *[m_currentConfig->nbFlames];
  
  m_scene = new Scene (m_currentConfig->sceneName.fn_str(), m_flames, m_currentConfig->nbFlames);
  
  InitFlames();
  
  m_photoSolid = new PhotometricSolidsRenderer(m_scene, m_flames, m_currentConfig->nbFlames, &m_context, recompileShaders);
  
  m_scene->createDisplayLists();
  
  m_camera = new Camera (m_width, m_height, m_currentConfig->clipping);
  
  m_glowEngine  = new GlowEngine (m_width, m_height, glowScales, recompileShaders, &m_context);
  m_depthPeelingEngine = new DepthPeelingEngine(m_width, m_height, DEPTH_PEELING_LAYERS_MAX, m_scene, m_flames, m_currentConfig->nbFlames, &m_context);
}

void GLFlameCanvas::Init (FlameAppConfig *config, bool recompileShaders)
{  
  m_currentConfig = config;
  
  InitUISettings();
  SetCurrent();
  InitGL(recompileShaders);
  
  InitScene(recompileShaders);
  
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
  InitScene(false);
  setNbDepthPeelingLayers(m_currentConfig->nbDepthPeelingLayers);
  ::wxStartTimer();
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
  for (uint f = 0; f < prevNbFlames; f++)
    delete m_flames[f];
  delete[]m_flames;
   for (uint s = 0; s < prevNbSolvers; s++)
    delete m_solvers[s];
  delete[]m_solvers;
}

void GLFlameCanvas::OnIdle(wxIdleEvent& event)
{
  if(m_run){
    for(uint i=0 ; i < m_currentConfig->nbSolvers; i++)
      m_solvers[i]->cleanSources ();
    for (uint i = 0; i < m_currentConfig->nbFlames; i++)
      m_flames[i]->addForces ();
    
    for(uint i=0 ; i < m_currentConfig->nbSolvers; i++)
      m_solvers[i]->iterate ();
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
    case WXK_UP: m_camera->moveOnFrontOrBehind(-step); break;
    case WXK_DOWN: m_camera->moveOnFrontOrBehind(step); break;
    case WXK_HOME: m_camera->moveUpOrDown(-step); break;
    case WXK_END: m_camera->moveUpOrDown(step); break;
    case 'l':
      m_framesCountForSwitch = 1;
      m_switch = true;
      for(uint i=0 ; i < m_currentConfig->nbSolvers; i++)
	m_solvers[i]->decreaseRes ();
      break;

    case 'L': 
      m_framesCountForSwitch = 1;
      m_switch = true;
      for(uint i=0 ; i < m_currentConfig->nbSolvers; i++)
	m_solvers[i]->increaseRes ();
      break;
    case WXK_SPACE : m_run = !m_run; break;
    }
  event.Skip();
}

/** Fonction de dessin global */
void GLFlameCanvas::OnPaint (wxPaintEvent& event)
{
  uint f;
  
  if(!m_init)
    return;
  
  wxPaintDC dc(this);
  
  if(!GetContext())
    return;
  
  SetCurrent();
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  /* Déplacement du camera */
  m_camera->setView();
  
  //m_camera->recalcModelView();
  
  /********** CONSTRUCTION DES FLAMMES *******************************/
  // SDL_mutexP (lock);
  if(m_framesCountForSwitch){
    if(m_framesCountForSwitch == 6){
      m_switch = false;
      m_framesCountForSwitch = 0;
    }else
      m_framesCountForSwitch++;
  }
  if(m_run && !m_switch)
    for (f = 0; f < m_currentConfig->nbFlames; f++)
      m_flames[f]->build();
  // SDL_mutexV (lock);
  
  /********** RENDU DES ZONES DE GLOW + BLUR *******************************/
  if(m_currentConfig->glowEnabled && m_displayFlame || m_displayParticles){
    GLdouble m[4][4];
    double dist, sigma;
    
    /* Adaptation du flou en fonction de la distance */
    /* On module la largeur de la gaussienne */    
    glGetDoublev (GL_MODELVIEW_MATRIX, &m[0][0]);
    
    Point position(m[3][0], m[3][1], m[3][2]);
    Vector direction = position;
    dist = direction.length();
    
    /* Définition de la largeur de la gaussienne en fonction de la distance */
    /* A définir de manière plus précise par la suite */
    //     sigma = dist > 0.1 ? -log(4*dist)+6 : 6.0;
    //sigma = dist > 0.1 ? -log(dist)+6 : 6.0;
    sigma = 1.5;
    
    if(m_currentConfig->depthPeelingEnabled){
      /* On décortique dans les calques */
      m_depthPeelingEngine->makePeels(m_displayFlame, m_displayParticles);
      
      m_glowEngine->activate();
      m_glowEngine->setGaussSigma(sigma);
      
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      /* On affiche la superposition des calques que l'on vient de décortiquer */
      m_depthPeelingEngine->render();
    }else{
      m_glowEngine->activate();
      m_glowEngine->setGaussSigma(sigma);
      
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      /* Dessin de la scène sans les textures pour avoir les occlusions sur les flammes */
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      m_scene->drawSceneWT ();
      glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
      glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
      /* Dessin de la flamme */
      for (f = 0; f < m_currentConfig->nbFlames; f++)
	m_flames[f]->drawFlame (m_displayFlame, m_displayParticles);
    }
    m_glowEngine->blur();
    
    m_glowEngine->deactivate();
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  if(!m_glowOnly){
    drawScene();
    /********************* Dessin de la flamme **********************************/
    if(!m_currentConfig->glowEnabled && m_displayFlame || m_displayParticles)
      if(m_currentConfig->depthPeelingEnabled)
	{
	  m_depthPeelingEngine->makePeels(m_displayFlame, m_displayParticles);  	  
	  m_depthPeelingEngine->render();
	}
      else{
	for (f = 0; f < m_currentConfig->nbFlames; f++)
	  m_flames[f]->drawFlame (m_displayFlame, m_displayParticles);
      }
  }
  /********************* PLACAGE DU GLOW ****************************************/
  if(m_currentConfig->glowEnabled)
    m_glowEngine->drawBlur(1.0);
  
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
  uint f,s;
  
  if(m_currentConfig->lightingMode == LIGHTING_PHOTOMETRIC)
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  else
    glBlendFunc (GL_ONE, GL_ZERO);
  
  /******************* AFFICHAGE DE LA SCENE *******************************/
  for (f = 0; f < m_currentConfig->nbFlames; f++)
    m_flames[f]->drawWick (m_displayWickBoxes);
  
  /**** Affichage de la scène ****/
  glPushAttrib (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
    glEnable (GL_LIGHTING);
  if(m_currentConfig->lightingMode == LIGHTING_STANDARD){
    if (!m_currentConfig->shadowsEnabled)
      for (f = 0; f < m_currentConfig->nbFlames; f++)
	m_flames[f]->switchOn ();
  }
  
  if (m_drawShadowVolumes)
    for (f = 0; f < m_currentConfig->nbFlames; f++)
      m_flames[f]->drawShadowVolume ();
  
  if (m_currentConfig->shadowsEnabled)
    castShadows();  
  else
    if(m_currentConfig->lightingMode == LIGHTING_PHOTOMETRIC)
      m_photoSolid->draw(m_currentConfig->BPSEnabled);
    else{
      m_scene->drawScene();
      glDisable (GL_LIGHTING);
    }
  
  glPopAttrib ();  
  
  /************ Affichage des outils d'aide à la visu (grille, etc...) *********/
  if(m_currentConfig->lightingMode == LIGHTING_STANDARD)
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  for (s = 0; s < m_currentConfig->nbSolvers; s++)
    {
      Point position(m_solvers[s]->getPosition ());
      Point scale(m_solvers[s]->getScale ());
      
      glPushMatrix ();
      glTranslatef (position.x, position.y, position.z);
      glScalef (scale.x, scale.y, scale.z);
      if (m_displayBase)
	m_solvers[s]->displayBase();
      if (m_displayGrid)
	m_solvers[s]->displayGrid();
      if (m_displayVelocity)
	m_solvers[s]->displayVelocityField();
      
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
  uint f;
  
  for (f = 0; f < m_currentConfig->nbFlames; f++)
    m_flames[f]->switchOff ();
  m_scene->drawSceneWT ();
  
  if(m_currentConfig->lightingMode == LIGHTING_STANDARD)
    for (f = 0; f < m_currentConfig->nbFlames; f++){
      m_flames[f]->switchOn ();
    }
  
  glPushAttrib (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
		GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT);
  
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
  
  for (f = 0; f < m_currentConfig->nbFlames; f++)
    m_flames[f]->drawShadowVolume ();
  
  glPopAttrib ();
  
  /* On teste ensuite Ã  l'endroit où il faut dessiner */
  glDepthFunc (GL_LEQUAL);

  glEnable (GL_STENCIL_TEST);
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
    for (f = 0; f < m_currentConfig->nbFlames; f++)
      m_flames[f]->switchOn ();
    
    m_scene->drawScene ();
  }else
    m_photoSolid->draw(m_currentConfig->BPSEnabled);
}

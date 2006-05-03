#include "GLFlameCanvas.hpp"

#include "interface/mainFrame.hpp"
#include <iostream>

BEGIN_EVENT_TABLE(GLFlameCanvas, wxGLCanvas)
  EVT_PAINT(GLFlameCanvas::OnPaint)
  EVT_IDLE(GLFlameCanvas::OnIdle)
  EVT_MOTION(GLFlameCanvas::OnMouseMotion)
  EVT_MIDDLE_DOWN(GLFlameCanvas::OnMouseClick)
  EVT_MIDDLE_UP(GLFlameCanvas::OnMouseClick)
  EVT_RIGHT_DOWN(GLFlameCanvas::OnMouseClick)
  EVT_RIGHT_UP(GLFlameCanvas::OnMouseClick)
  EVT_MOUSEWHEEL(GLFlameCanvas::OnMouseWheel)
  EVT_KEY_DOWN(GLFlameCanvas::OnKeyPressed)
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
  m_pixels = new unsigned char[size.GetWidth()*size.GetHeight()*3];
  m_framesCount = 0;
  /* Pour éviter de faire un calcul pour ajouter des 0... */
  /* Un jour je ferais mieux, promis... */
  m_globalFramesCount = 1000000;
  intensities = NULL;

  srand(45542);
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
  
  glClearColor (0.0, 0.0, 0.0, 1.0);
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
  for(int i=0 ; i < m_currentConfig->nbFlames; i++){
    switch(m_currentConfig->flames[i].type){
    case CANDLE :
      m_flames[i] = new Candle (m_solvers[m_currentConfig->flames[i].solverIndex], m_currentConfig->flames[i].position, 
				m_scene, m_currentConfig->flames[i].innerForce, m_currentConfig->flames[i].samplingTolerance, 
				"bougie.obj", i, m_SVShader, m_solvers[m_currentConfig->flames[i].solverIndex]->getDimX()/ 7.0,
				m_currentConfig->flames[i].skeletonsNumber);
      break;
    case FIRMALAMPE :
      m_flames[i] = new Firmalampe(m_solvers[m_currentConfig->flames[i].solverIndex],m_currentConfig->flames[i].position,
				   m_scene, m_currentConfig->flames[i].innerForce, m_currentConfig->flames[i].samplingTolerance,
				   "firmalampe.obj", i, m_SVShader, m_currentConfig->flames[i].skeletonsNumber,
				   m_currentConfig->flames[i].wickName.fn_str());
      break;
    case TORCH :
      m_flames[i] = new Torch(m_solvers[m_currentConfig->flames[i].solverIndex],m_currentConfig->flames[i].position,
			      m_scene, m_currentConfig->flames[i].innerForce, m_currentConfig->flames[i].samplingTolerance,
			      m_currentConfig->flames[i].wickName.fn_str(), i, m_SVShader, m_currentConfig->flames[i].skeletonsNumber);
      break;
    case CAMPFIRE :
      m_flames[i] = new CampFire(m_solvers[m_currentConfig->flames[i].solverIndex],m_currentConfig->flames[i].position,
				 m_scene, m_currentConfig->flames[i].innerForce, m_currentConfig->flames[i].samplingTolerance,
				 m_currentConfig->flames[i].wickName.fn_str(), i, m_SVShader, m_currentConfig->flames[i].skeletonsNumber);
      break;
    default :
      cerr << "Unknown flame type : " << (int)m_currentConfig->flames[i].type << endl;
      ::wxExit();
    }
  }
  prevNbFlames = m_currentConfig->nbFlames;
  
  if( intensities ) delete intensities;
  intensities = new double[m_currentConfig->nbFlames];
}

void GLFlameCanvas::InitSolvers(void)
{
  m_solvers = new Solver *[m_currentConfig->nbSolvers];
  for(int i=0 ; i < m_currentConfig->nbSolvers; i++){
    switch(m_currentConfig->solvers[i].type){
    case GS_SOLVER :
      m_solvers[i] = new GSsolver(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
				  m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
				  m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].timeStep, 
				  m_currentConfig->solvers[i].buoyancy);
      break;
    case GCSSOR_SOLVER :
      m_solvers[i] = new GCSSORsolver(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
				      m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
				      m_currentConfig->solvers[i].dim,  m_currentConfig->solvers[i].timeStep,
				      m_currentConfig->solvers[i].buoyancy, m_currentConfig->solvers[i].omegaDiff, 
				      m_currentConfig->solvers[i].omegaProj, m_currentConfig->solvers[i].epsilon);
      break;
    case HYBRID_SOLVER :
      m_solvers[i] = new HybridSolver(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
				      m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
				      m_currentConfig->solvers[i].dim,  m_currentConfig->solvers[i].timeStep,
				      m_currentConfig->solvers[i].buoyancy, m_currentConfig->solvers[i].omegaDiff, 
				      m_currentConfig->solvers[i].omegaProj, m_currentConfig->solvers[i].epsilon);
      break;
    case LOGRES_SOLVER :
      m_solvers[i] = new LogResSolver(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
				      m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
				      m_currentConfig->solvers[i].dim,  m_currentConfig->solvers[i].timeStep,
				      m_currentConfig->solvers[i].buoyancy, m_currentConfig->solvers[i].nbMaxIter, 
				      m_currentConfig->solvers[i].omegaDiff, m_currentConfig->solvers[i].omegaProj, 
				      m_currentConfig->solvers[i].epsilon);
      break;
    case LOGRESAVG_SOLVER :
      m_solvers[i] = new LogResAvgSolver(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
					 m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
					 m_currentConfig->solvers[i].dim,  m_currentConfig->solvers[i].timeStep,
					 m_currentConfig->solvers[i].buoyancy, m_currentConfig->solvers[i].nbMaxIter, 
					 m_currentConfig->solvers[i].omegaDiff, m_currentConfig->solvers[i].omegaProj,
					 m_currentConfig->solvers[i].epsilon);
      break;
    case LOGRESAVGTIME_SOLVER :
      m_solvers[i] = new LogResAvgTimeSolver(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
					     m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
					     m_currentConfig->solvers[i].dim,  m_currentConfig->solvers[i].timeStep,
					     m_currentConfig->solvers[i].buoyancy, m_currentConfig->solvers[i].nbMaxIter, 
					     m_currentConfig->solvers[i].omegaDiff, m_currentConfig->solvers[i].omegaProj, 
					     m_currentConfig->solvers[i].epsilon);
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
  int glowScales[2] = { 1, 4 };
  InitSolvers();
  
  m_flames = new FireSource *[m_currentConfig->nbFlames];
  
  m_scene = new Scene (m_currentConfig->sceneName.fn_str(), m_flames, m_currentConfig->nbFlames);
  
  /* Changement de répertoire pour les textures */
  //AS_ERROR(chdir("textures"),"chdir textures");
  m_photoSolid = new SolidePhotometrique(m_scene, &m_context, recompileShaders);
  
  InitFlames();
  //AS_ERROR(chdir(".."),"chdir ..");
  m_scene->createDisplayLists();
  
  m_camera = new Camera (m_width, m_height, m_currentConfig->clipping);
  
  m_glowEngine  = new GlowEngine (m_width, m_height, glowScales, recompileShaders, &m_context);
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
  
  cerr << "Initialisation terminée" << endl;
}

void GLFlameCanvas::Restart (void)
{
  Disable();
  m_init = false;
  DestroyScene();
  InitUISettings();
  InitScene(false);
  ::wxStartTimer();
  m_init = true;
  cerr << "Réinitialisation terminée" << endl;
  Enable();
}


void GLFlameCanvas::DestroyScene(void)
{ 
  delete m_glowEngine;
  delete m_glowEngine2;
  delete m_camera;
  delete m_scene;
  delete m_photoSolid;
  for (int f = 0; f < prevNbFlames; f++)
    delete m_flames[f];
  delete[]m_flames;
   for (int s = 0; s < prevNbSolvers; s++)
    delete m_solvers[s];
  delete[]m_solvers;
}

void GLFlameCanvas::OnIdle(wxIdleEvent& event)
{
  if(m_run){
    for (int i = 0; i < m_currentConfig->nbFlames; i++)
      m_flames[i]->addForces (m_currentConfig->flames[i].flickering, m_currentConfig->flames[i].fdf);
    
    for(int i=0 ; i < m_currentConfig->nbSolvers; i++)
      m_solvers[i]->iterate ();
  }
  
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
  m_camera->moveOnFrontOrBehind(-event.GetWheelRotation()/100);
}

void GLFlameCanvas::OnKeyPressed(wxKeyEvent& event)
{
  double step=0.3;
  switch(event.GetKeyCode())
    {      
    case WXK_LEFT: m_camera->moveOnSides(step); break;
    case WXK_RIGHT: m_camera->moveOnSides(-step); break;
    case WXK_UP: m_camera->moveOnFrontOrBehind(-step); break;
    case WXK_DOWN: m_camera->moveOnFrontOrBehind(step); break;
    case WXK_HOME: m_camera->moveUpOrDown(-step); break;
    case WXK_END: m_camera->moveUpOrDown(step); break;
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* Déplacement du camera */
  m_camera->setView();
  
  //m_camera->recalcModelView();
  
  /********** CONSTRUCTION DES FLAMMES *******************************/
  // SDL_mutexP (lock);
  if(m_run)
    for (int f = 0; f < m_currentConfig->nbFlames; f++)
      m_flames[f]->build();
  // SDL_mutexV (lock);
  
  /********** RENDU DES ZONES DE GLOW + BLUR *******************************/
  if(m_currentConfig->glowEnabled){
    GLdouble m[4][4];
    double dist, sigma;
    
    /* Adaptation du flou en fonction de la distance */
    /* On module la largeur de la gaussienne */    
    glGetDoublev (GL_MODELVIEW_MATRIX, &m[0][0]);
    
    //Point position(m[3][0], m[3][1], m[3][2]);
    // Vector direction = position:
    //dist = direction.length();

    /* Définition de la largeur de la gaussienne en fonction de la distance */
    /* A définir de manière plus précise par la suite */
//     sigma = dist > 0.1 ? -log(4*dist)+6 : 6.0;
    //sigma = dist > 0.1 ? -log(dist)+6 : 6.0;
    sigma = 1.2;
    m_glowEngine->activate();
    m_glowEngine->setGaussSigma(sigma);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBlendFunc (GL_SRC_ALPHA, GL_ZERO);
    
    m_scene->draw_scene ();
    
    /* Dessin de la flamme */
    if(m_displayFlame)
      for (int f = 0; f < m_currentConfig->nbFlames; f++)
	m_flames[f]->drawFlame (m_displayParticles);
    
    m_glowEngine->blur();
    
    m_glowEngine->deactivate();
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  if(!m_glowOnly){    
    glBlendFunc (GL_ONE, GL_ZERO);
    
    /******************* AFFICHAGE DE LA SCENE *******************************/
    for (int f = 0; f < m_currentConfig->nbFlames; f++)
      m_flames[f]->drawWick (m_displayWickBoxes);
    
    if(m_currentConfig->lightingMode == LIGHTING_PHOTOMETRIC){
      /* Le 3e paramètre doit être la hauteur du solveur de la flamme */
      Point pos(m_flames[0]->getPosition());
      m_photoSolid->calculerFluctuationIntensiteCentreEtOrientation(m_flames[0]->getMainDirection(), pos, 1.0);
    }

    /**** Affichage de la scène ****/
    glPushAttrib (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glEnable (GL_LIGHTING);
    
    for (int f = 0; f < m_currentConfig->nbFlames; f++)
      {
	/* Définition de l'intensité lumineuse de chaque flamme en fonction de la hauteur de celle-ci */
	intensities[f] = m_flames[f]->getMainDirection().length() * 1.5;
	if (m_drawShadowVolumes)
	  m_flames[f]->drawShadowVolume ();
	if (!m_currentConfig->shadowsEnabled)
	  m_flames[f]->switchOn (intensities[f]);
      }
    
    if (m_currentConfig->shadowsEnabled)
      cast_shadows_double();
    else
      if(m_currentConfig->lightingMode == LIGHTING_PHOTOMETRIC)
	m_photoSolid->draw(m_currentConfig->BPSEnabled,m_currentConfig->IPSEnabled);
      else
	m_scene->draw_scene ();
    
    glPopAttrib ();
    
    glDisable (GL_LIGHTING);      
    
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /************ Affichage des outils d'aide à la visu (grille, etc...) *********/
    for (int s = 0; s < m_currentConfig->nbSolvers; s++)
      {
	Point position(m_solvers[s]->getPosition ());
	
	glPushMatrix ();
	glTranslatef (position.x, position.y, position.z);
	if (m_displayBase)
	  m_solvers[s]->displayBase();
	if (m_displayGrid)
	  m_solvers[s]->displayGrid();
	if (m_displayVelocity)
	  m_solvers[s]->displayVelocityField();
	
	glPopMatrix ();
      }
    /********************* Dessin de la flamme **********************************/
    if(!m_currentConfig->glowEnabled)
      if(m_displayFlame)
	for (int f = 0; f < m_currentConfig->nbFlames; f++)
	  m_flames[f]->drawFlame (m_displayParticles);
  }
  /********************* PLACAGE DU GLOW ****************************************/
  if(m_currentConfig->glowEnabled){
    //    glBlendColor(0.0,0.0,0.0,1.0);
    //glBlendFunc (GL_ONE, GL_CONSTANT_ALPHA);
    //     glBlendFunc (GL_ZERO,  GL_ONE_MINUS_SRC_COLOR);
    //     //m_glowEngine2->drawBlur(0.5);
    
    //     m_glowEngine->drawBlur(1.0);
    
    //     glBlendFunc (GL_SRC_ALPHA, GL_ONE);
    //     m_glowEngine2->drawBlur(0.5);
    //     glBlendFunc (GL_ONE, GL_ONE);
    //     m_glowEngine->drawBlur(1.0);
    
    m_glowEngine->drawBlur(1.0);
  }
  /******** A VERIFIER *******/
  glFlush();
  //glFinish();
  /***************************/
  SwapBuffers ();
  
  //event.Skip();
  
  /******************** CALCUL DU FRAMERATE *************************************/
  m_framesCount++;
  m_globalFramesCount++;
  
  m_t = ::wxGetElapsedTime (false);
  if (m_t >= 2000){    
    ((MainFrame *)GetParent())->SetFPS( m_framesCount / (m_t/1000) );
    ::wxStartTimer();
    m_framesCount = 0;
  }

  if(m_saveImages){
    wxString filename;
    wxString zeros;
    
    glReadPixels (0, 0, m_width-4, m_height-4, GL_RGB, GL_UNSIGNED_BYTE, m_pixels);
    
    filename << _("captures/capture") << m_globalFramesCount << _(".png");
    /* Création d'une image, le dernier paramètre précise que wxImage ne doit pas détruire */
    /* le tableau de données dans son destructeur */
    wxImage image(m_width-4,m_height-4,m_pixels,true),image2;
    image2 = image.Mirror(false);
    if(!image2.SaveFile(filename,wxBITMAP_TYPE_PNG))
      cerr << "Image saving error !!" << endl;
  }
}

// void
// GLFlameCanvas::cast_shadows_double_multiple ()
// {
//   switch_off_lights ();
//   m_scene->draw_sceneWTEX ();

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

//       m_scene->draw_sceneWTEX ();
      
//       reset_diffuse_light (i);
//     }
//   glDisable (GL_STENCIL_TEST);
//   for (int i = 0; i < m_nbLights /**SHADOW_SAMPLE_PER_LIGHT*/ ; i++)
//     {
//       enable_only_ambient_light (i);
//     }
//   m_scene->draw_sceneWTEX ();
//   for (int i = 0; i < m_nbLights /**SHADOW_SAMPLE_PER_LIGHT*/ ; i++)
//     {
//       reset_diffuse_light (i);
//     }
//   switch_on_lights ();
//   glBlendFunc (GL_ZERO, GL_SRC_COLOR);
//   m_scene->draw_scene ();
// }

void
GLFlameCanvas::cast_shadows_double ()
{
  Point pos(m_flames[0]->getPosition());
  m_photoSolid->calculerFluctuationIntensiteCentreEtOrientation(m_flames[0]->getMainDirection(), pos, 1.0);

  for (int f = 0; f < m_currentConfig->nbFlames; f++)
    m_flames[f]->switchOff ();
  m_scene->draw_sceneWT ();

  if(m_currentConfig->lightingMode == LIGHTING_STANDARD)
    for (int f = 0; f < m_currentConfig->nbFlames; f++){
      m_flames[f]->switchOn (intensities[f]);
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
  
  for (int f = 0; f < m_currentConfig->nbFlames; f++)
    m_flames[f]->drawShadowVolume ();

  glPopAttrib ();

  /* On teste ensuite Ã  l'endroit où il faut dessiner */
  glDepthFunc (GL_EQUAL);

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
  m_scene->draw_sceneWT ();
  
  glDisable (GL_STENCIL_TEST);
  /* Affichage de la scène en couleur en multipliant avec l'affichage précédent */

  glBlendFunc (GL_ZERO, GL_SRC_COLOR);
  if(m_currentConfig->lightingMode == LIGHTING_STANDARD){
    for (int f = 0; f < m_currentConfig->nbFlames; f++)
      m_flames[f]->resetDiffuseLight ();
    
    for (int f = 0; f < m_currentConfig->nbFlames; f++)
      m_flames[f]->switchOn (intensities[f]);
    
    m_scene->draw_scene ();
  }else
    m_photoSolid->draw(m_currentConfig->BPSEnabled,m_currentConfig->IPSEnabled);
  
}

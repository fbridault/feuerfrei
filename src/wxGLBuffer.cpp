#include "wxGLBuffer.hpp"

#include "main.hpp"
#include <iostream>

BEGIN_EVENT_TABLE(wxGLBuffer, wxGLCanvas)
  EVT_PAINT(wxGLBuffer::OnPaint)
  EVT_IDLE(wxGLBuffer::OnIdle)
  EVT_MOTION(wxGLBuffer::OnMouseMotion)
  EVT_MIDDLE_DOWN(wxGLBuffer::OnMouseClick)
  EVT_MIDDLE_UP(wxGLBuffer::OnMouseClick)
  EVT_RIGHT_DOWN(wxGLBuffer::OnMouseClick)
  EVT_RIGHT_UP(wxGLBuffer::OnMouseClick)
  EVT_MOUSEWHEEL(wxGLBuffer::OnMouseWheel)
  EVT_KEY_DOWN(wxGLBuffer::OnKeyPressed)
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

wxGLBuffer::wxGLBuffer(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int* attribList, 
		       long style, const wxString& name, const wxPalette& palette)
  : wxGLCanvas(parent, id, pos, size, style, name, attribList, palette)
{
  m_init = false;
  m_run = 0;
}

wxGLBuffer::~wxGLBuffer()
{
  DestroyScene();
  delete m_SVShader;
  if (m_context)
    cgDestroyContext (m_context);
}

void wxGLBuffer::InitUISettings(void)
{
  /* Pour l'affichage */
  m_run = true; 
  m_flickering = m_glowOnly = false;
  m_displayBase = m_displayVelocity = m_displayParticles = m_displayGrid = false;
  m_displayFlame = true;
  m_shadowsEnabled = false; m_shadowVolumesEnabled = false;
}

void wxGLBuffer::InitGL(bool recompileShaders)
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

  //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  // Création du contexte CG
  m_context = cgCreateContext();
  
  contextCopy = &m_context;
  cgSetErrorCallback(cgErrorCallback);
  
  m_SVShader = new CgSVShader (_("ShadowVolumeExtrusion.cg"), _("SVExtrude"), &m_context, recompileShaders);
}

void wxGLBuffer::InitFlames(void)
{
  int nbSkeletons;
    
  for(int i=0 ; i < m_currentConfig->nbFlames; i++){
    switch(m_currentConfig->flames[i].type){
    case BOUGIE :
      nbSkeletons = 4;
      m_flames[i] = new Bougie (m_solvers[m_currentConfig->flames[i].solverIndex], nbSkeletons,
				m_currentConfig->flames[i].position, 
				m_solvers[m_currentConfig->flames[i].solverIndex]->getDimX()/ 7.0,
				m_currentConfig->flames[i].fieldForces,m_currentConfig->flames[i].innerForce,
				m_SVShader,"bougie.obj",m_scene, &m_context);
      break;
    case FIRMALAMPE :
      nbSkeletons = 5;
      m_flames[i] = new Firmalampe(m_solvers[m_currentConfig->flames[i].solverIndex],nbSkeletons,
				   m_currentConfig->flames[i].position, m_currentConfig->flames[i].fieldForces,
				   m_currentConfig->flames[i].innerForce, m_SVShader, 
				   m_currentConfig->flames[i].wickName.fn_str(),"firmalampe.obj",m_scene);
      break;
    default :
      cerr << "Unknown flame type : " << (int)m_currentConfig->flames[i].type << endl;
      ::wxExit();
    }
  }
  prevNbFlames = m_currentConfig->nbFlames;
}

void wxGLBuffer::InitSolvers(void)
{
  m_solvers = new Solver *[m_currentConfig->nbSolvers];
  for(int i=0 ; i < m_currentConfig->nbSolvers; i++){
    switch(m_currentConfig->solvers[i].type){
    case GS_SOLVER :
      m_solvers[i] = new GSsolver(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
				  m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
				  m_currentConfig->solvers[i].dim,  m_currentConfig->solvers[i].timeStep);
      break;
    case GCSSOR_SOLVER :
      m_solvers[i] = new GCSSORsolver(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
				      m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
				      m_currentConfig->solvers[i].dim,  m_currentConfig->solvers[i].timeStep);
      break;
    default :
      cerr << "Unknown solver type : " << (int)m_currentConfig->solvers[i].type << endl;
      ::wxExit();
    }
  }
  prevNbSolvers = m_currentConfig->nbSolvers;
}

void wxGLBuffer::InitScene(bool recompileShaders)
{  
  InitSolvers();

  m_flames = new Flame *[m_currentConfig->nbFlames];
  
  m_scene = new CScene (m_currentConfig->sceneName.fn_str(), m_flames, m_currentConfig->nbFlames);
  
  /* Changement de répertoire pour les textures */
  //AS_ERROR(chdir("textures"),"chdir textures");
  m_photoSolid = new SolidePhotometrique(m_scene, &m_context, recompileShaders);
  
  InitFlames();
  //AS_ERROR(chdir(".."),"chdir ..");
  m_scene->createDisplayLists();
  
  m_camera = new Camera (m_width, m_height, m_currentConfig->clipping);
  
  m_glowEngine  = new GlowEngine (m_width, m_height, 4, recompileShaders, &m_context);
  m_glowEngine2 = new GlowEngine (m_width, m_height, 1, recompileShaders, &m_context);
}

void wxGLBuffer::Init (FlameAppConfig *config, bool recompileShaders)
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

void wxGLBuffer::Restart (void)
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


void wxGLBuffer::DestroyScene(void)
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

void wxGLBuffer::OnIdle(wxIdleEvent& event)
{
  if(m_run){
    for (int i = 0; i < m_currentConfig->nbFlames; i++)
      m_flames[i]->add_forces (m_flickering);
    
    for(int i=0 ; i < m_currentConfig->nbSolvers; i++)
      m_solvers[i]->iterate (m_flickering);
  }
  
  this->Refresh();
  
  /*  draw();*/
  //event.RequestMore();
}
  
void wxGLBuffer::OnMouseMotion(wxMouseEvent& event)
{
  m_camera->OnMouseMotion(event);
}
  
void wxGLBuffer::OnMouseClick(wxMouseEvent& event)
{
  m_camera->OnMouseClick(event);
}

void wxGLBuffer::OnMouseWheel(wxMouseEvent& event)
{
  m_camera->moveOnFrontOrBehind(-event.GetWheelRotation()/100);
}

void wxGLBuffer::OnKeyPressed(wxKeyEvent& event)
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
void wxGLBuffer::OnPaint (wxPaintEvent& event)
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
    
    CVector direction(m[3][0], m[3][1], m[3][2]);
    
    dist = direction.length();

    sigma = dist > 0.1 ? -log(6*dist)+6 : 6.0;
    
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
    
    m_glowEngine2->activate();
    m_glowEngine2->setGaussSigma(sigma);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBlendFunc (GL_SRC_ALPHA, GL_ZERO);
    
    m_scene->draw_scene ();
    
    /* Dessin de la flamme */
    if(m_displayFlame)
      for (int f = 0; f < m_currentConfig->nbFlames; f++)
	m_flames[f]->drawFlame (m_displayParticles);
    
    m_glowEngine2->blur();
    
    m_glowEngine2->deactivate();
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  if(!m_glowOnly){    
    glBlendFunc (GL_ONE, GL_ZERO);
    
    /******************* AFFICHAGE DE LA SCENE *******************************/
    /* !!!!!! Ceci n'est PAS CORRECT, dans le cas de PLUSIEURS flammes !!!!! */
    for (int f = 0; f < m_currentConfig->nbFlames; f++)
      m_flames[f]->drawWick ();
    
    if(m_currentConfig->PSEnabled){
      /* 3e paramètre doit être la hauteur du solveur de la flamme */
      CPoint pos(m_flames[0]->getPosition());
      m_photoSolid->calculerFluctuationIntensiteCentreEtOrientation(m_flames[0]->get_main_direction(),
								    pos,
								    1.0);
      m_photoSolid->draw(m_currentConfig->BPSEnabled,m_currentConfig->IPSEnabled);
    }else{
      /**** Affichage de la scène ****/
      glPushAttrib (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      
      glEnable (GL_LIGHTING);
      
      for (int f = 0; f < m_currentConfig->nbFlames; f++)
	{
	  if (m_shadowVolumesEnabled)
	    m_flames[f]->draw_shadowVolumes ();
	  if (m_shadowsEnabled)
	    m_flames[f]->cast_shadows_double ();
	  else{
	    m_flames[f]->switch_on_lights ();
	  }
	}
      m_scene->draw_scene ();
      
      glPopAttrib ();
      
      glDisable (GL_LIGHTING);
    }
    
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /************ Affichage des outils d'aide à la visu (grille, etc...) *********/
    for (int s = 0; s < m_currentConfig->nbSolvers; s++)
      {
	CPoint position(m_solvers[s]->getPosition ());
	
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
    glDisable (GL_DEPTH_TEST);
 //    glBlendColor(0.0,0.0,0.0,1.0);
    //glBlendFunc (GL_ONE, GL_CONSTANT_ALPHA);
//     glBlendFunc (GL_ZERO,  GL_ONE_MINUS_SRC_COLOR);
//     //m_glowEngine2->drawBlur(0.5);
    
//     m_glowEngine->drawBlur(1.0);
    
//     glBlendFunc (GL_SRC_ALPHA, GL_ONE);
//     m_glowEngine2->drawBlur(0.5);
//     glBlendFunc (GL_ONE, GL_ONE);
//     m_glowEngine->drawBlur(1.0);
    
    glBlendFunc (GL_ONE, GL_ONE);
    m_glowEngine2->drawBlur(1.0);
    m_glowEngine->drawBlur(1.0);

    glEnable (GL_DEPTH_TEST);
  }
  /******** A VERIFIER *******/
  glFlush();
  //glFinish();
  /***************************/
  SwapBuffers ();
  
  //event.Skip();
  
  /******************** CALCUL DU FRAMERATE *************************************/
  m_framesCount++;
  
  m_t = ::wxGetElapsedTime (false);
  if (m_t >= 2000){    
    ((MainFrame *)GetParent())->SetFPS( m_framesCount / (m_t/1000) );
    ::wxStartTimer();
    m_framesCount = 0;
  } 
}

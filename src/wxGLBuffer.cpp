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
  m_nbFlames = 1;
  m_run = 0;
}

wxGLBuffer::~wxGLBuffer()
{
  cerr << "pouetdel" << endl;
  DestroyScene();
  cerr << "pouetdel12" << endl;
  delete m_SVShader;
  cerr << "pouetdel14" << endl;
  if (m_context)
    cgDestroyContext (m_context);
  cerr << "pouetdel2" << endl;
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

void wxGLBuffer::InitGL(void)
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

  m_SVShader = new CgSVShader (_("ShadowVolumeExtrusion.cg"), _("SVExtrude"), &m_context);
}

void wxGLBuffer::InitScene(void)
{
  int nb_squelettes_flammes;
  cout << "pouet1" << endl;
  m_solver = new Solver(m_currentConfig->solvx, m_currentConfig->solvy, m_currentConfig->solvz, 1.0, m_currentConfig->timeStep);
  //m_solver = new BenchSolver (solvx, solvy, solvz, 1.0, timeStep);
  cout << "pouet2" << endl;
  /* Changement de répertoire pour les textures */
  //AS_ERROR(chdir("textures"),"chdir textures");
  m_flames = new Flame *[m_nbFlames];
    cout << "pouet3" << endl;
  m_photoSolid = new SolidePhotometrique(m_scene, &m_context);
  cout << "pouet4" << endl;
#ifdef BOUGIE
  CPoint pt (0.0, 0.0, 0.0), pos (0.0, 0.0, 0.0);
  nb_squelettes_flammes = 4;
  m_flames[0] = new Bougie (m_solver, nb_squelettes_flammes, &pt, &pos,
			   m_solver->getDimX()/ 7.0, m_SVShader,"bougie.obj",m_scene, &m_context);
#else
  CPoint pt (0.0, 0.0, 0.0), pos (1.5, -1.8, 0.0);
  //CPoint pt (0.0, 0.0, 0.0), pos (0.0, -0.5, 0.0);
  nb_squelettes_flammes = 5;
  m_flames[0] = new Firmalampe(m_solver,nb_squelettes_flammes,&pt,&pos,m_SVShader,m_currentConfig->mecheName.fn_str(),"firmalampe.obj",m_scene);
#endif 
cout << "pouet5" << endl;
  //AS_ERROR(chdir(".."),"chdir ..");
  m_solver->setFlames ((Flame **) m_flames, m_nbFlames);
  cout << "pouet6" << endl;
  m_scene = new CScene (m_currentConfig->sceneName.fn_str(),m_flames, m_nbFlames);
cout << "pouet7" << endl;
  m_eyeball = new Eyeball (m_width, m_height, m_currentConfig->clipping);
  
  m_glowEngine  = new GlowEngine (m_scene, m_eyeball, &m_context, m_width, m_height, 4);
  m_glowEngine2 = new GlowEngine (m_scene, m_eyeball, &m_context, m_width, m_height, 1);
}

/** Initialisation de l'interface */
void wxGLBuffer::Init (flameAppConfig *config)
{  
  m_currentConfig = config;

  InitUISettings();
  SetCurrent();
  InitGL();
  
  InitScene();
  
  ::wxStartTimer();

  m_init = true;
  
  cout << "Initialisation terminée" << endl;
}

void wxGLBuffer::Restart (void)
{
  Disable();
  m_init = false;
  DestroyScene();
  InitUISettings();
  InitScene();
  ::wxStartTimer();
  m_init = true;
  Enable();
}


void wxGLBuffer::DestroyScene(void)
{ 
  cerr << "pouetdel00" << endl;
  delete m_glowEngine;
  delete m_glowEngine2;
  cerr << "pouetdel01" << endl;
  delete m_eyeball;
  cerr << "pouetdel02" << endl;
  delete m_scene;
  cerr << "pouetdel03" << endl;
  delete m_photoSolid;
  cerr << "pouetdel03.5" << endl;
  for (int f = 0; f < m_nbFlames; f++)
    delete m_flames[f];
  cerr << "pouetdel04" << endl;
  delete[]m_flames;
  cerr << "pouetdel05" << endl;
  delete m_solver;
  cerr << "pouetdel06" << endl;
}

void wxGLBuffer::OnIdle(wxIdleEvent& event)
{
  if(m_run)
    m_solver->iterate (m_flickering);

  this->Refresh();
  
  /*  draw();*/
  //event.RequestMore();
}
  
void wxGLBuffer::OnMouseMotion(wxMouseEvent& event)
{
  m_eyeball->OnMouseMotion(event);
}
  
void wxGLBuffer::OnMouseClick(wxMouseEvent& event)
{
  m_eyeball->OnMouseClick(event);
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
  /* Déplacement du eyeball */
  m_eyeball->recalcModelView();
  
  /********** CONSTRUCTION DES FLAMMES *******************************/
  // SDL_mutexP (lock);
  if(m_run)
    for (int f = 0; f < m_nbFlames; f++)
      m_flames[f]->build();
  // SDL_mutexV (lock);
  
  /********** RENDU DES ZONES DE GLOW + BLUR *******************************/
  if(m_currentConfig->glowEnabled){
    GLfloat m[4][4];
    CVector direction;
    float dist, sigma;
    
    /* Adaptation du flou en fonction de la distance */
    /* On module la largeur de la gaussienne */
    glGetFloatv (GL_MODELVIEW_MATRIX, &m[0][0]);
    
    direction.setX (m[3][0]);
    direction.setY (m[3][1]);
    direction.setZ (m[3][2]);
    
    dist = direction.length();
    sigma = dist > 0.1 ? -log(6*dist)+6 : 6.0;
    
    m_glowEngine->activate();
    m_glowEngine->setGaussSigma(sigma);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBlendFunc (GL_SRC_ALPHA, GL_ZERO);
    
    m_scene->draw_scene ();
    
    /* Dessin de la flamme */
    if(m_displayFlame)
      for (int f = 0; f < m_nbFlames; f++)
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
      for (int f = 0; f < m_nbFlames; f++)
	m_flames[f]->drawFlame (m_displayParticles);
    
    m_glowEngine2->blur();
    
    m_glowEngine2->deactivate();
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  if(!m_glowOnly){    
    glBlendFunc (GL_ONE, GL_ZERO);
    
    /******************* AFFICHAGE DE LA SCENE *******************************/
    /* !!!!!! Ceci n'est PAS CORRECT, dans le cas de PLUSIEURS flammes !!!!! */
    for (int f = 0; f < m_nbFlames; f++) 
      m_flames[f]->drawWick ();
    
    if(m_currentConfig->PSEnabled){
      m_photoSolid->calculerFluctuationIntensiteCentreEtOrientation(m_flames[0]->get_main_direction(),
								   m_flames[0]->getPosition(),
								   m_solver->getDimY());
      m_photoSolid->draw(m_currentConfig->BPSEnabled,m_currentConfig->IPSEnabled);
    }else{
      /**** Affichage de la scène ****/
      glPushAttrib (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      
      glEnable (GL_LIGHTING);
      
      for (int f = 0; f < m_nbFlames; f++)
	{
	  if (m_shadowVolumesEnabled)
	    m_flames[f]->draw_shadowVolumes (SCENE_OBJECTS_WSV_WT);
	  if (m_shadowsEnabled)
	    m_flames[f]->cast_shadows_double (SCENE_OBJECTS_WSV_WT);
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
    for (int f = 0; f < m_nbFlames; f++)
      {
	CPoint position (*(m_flames[f]->getPosition ()));
	
	glPushMatrix ();
	glTranslatef (position.getX (), position.getY (), position.getZ ());
	if (m_displayBase)
	  m_solver->displayBase();
	if (m_displayGrid)
	  m_solver->displayGrid();
	if (m_displayVelocity)
	  m_solver->displayVelocityField();
	
	glPopMatrix ();
      }
    /********************* Dessin de la flamme **********************************/
    if(!m_currentConfig->glowEnabled)
      if(m_displayFlame)
	for (int f = 0; f < m_nbFlames; f++)
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

#include "GLFluidsCanvas.hpp"

#include "fluidsFrame.hpp"
#include <iostream>

BEGIN_EVENT_TABLE(GLFluidsCanvas, wxGLCanvas)
  EVT_SIZE(GLFluidsCanvas::OnSize)
  EVT_PAINT(GLFluidsCanvas::OnPaint)
  EVT_IDLE(GLFluidsCanvas::OnIdle)
  EVT_MOTION(GLFluidsCanvas::OnMouseMotion)
  EVT_MIDDLE_DOWN(GLFluidsCanvas::OnMouseClick)
  EVT_MIDDLE_UP(GLFluidsCanvas::OnMouseClick)
  EVT_RIGHT_DOWN(GLFluidsCanvas::OnMouseClick)
  EVT_RIGHT_UP(GLFluidsCanvas::OnMouseClick)
  EVT_MOUSEWHEEL(GLFluidsCanvas::OnMouseWheel)
  EVT_CHAR(GLFluidsCanvas::OnKeyPressed)
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

GLFluidsCanvas::GLFluidsCanvas(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int* attribList, 
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
  m_switch = false;
}

GLFluidsCanvas::~GLFluidsCanvas()
{
  DestroyScene();
  delete [] m_pixels;
  if (m_context)
    cgDestroyContext (m_context);
}

void GLFluidsCanvas::InitUISettings(void)
{
  /* Pour l'affichage */
  m_run = true;
  m_saveImages = false;
  m_displayGrid = false;
  m_displayBase = m_displayVelocity = m_displayDensity = true;
}

void GLFluidsCanvas::InitGL(bool recompileShaders)
{
  m_width = m_currentConfig->width; m_height = m_currentConfig->height;
  
  glClearColor (0.0, 0.0, 0.0, 0.0);
  /* Restriction de la zone d'affichage */
  glViewport (0, 0, m_width, m_height);
  
  glEnable (GL_DEPTH_TEST);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
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
}

void GLFluidsCanvas::InitSolvers(void)
{
  m_solvers = new Field *[m_currentConfig->nbSolvers];
  for(uint i=0 ; i < m_currentConfig->nbSolvers; i++){
    switch(m_currentConfig->solvers[i].type){
    case GS_SOLVER :
      m_solvers[i] = new GSSolver3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
				  m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
				  m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].timeStep, 
				  m_currentConfig->solvers[i].buoyancy);
      break;
    case GCSSOR_SOLVER :
      m_solvers[i] = new GCSSORSolver3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
				      m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
				      m_currentConfig->solvers[i].dim,  m_currentConfig->solvers[i].timeStep,
				      m_currentConfig->solvers[i].buoyancy, m_currentConfig->solvers[i].omegaDiff, 
				      m_currentConfig->solvers[i].omegaProj, m_currentConfig->solvers[i].epsilon);
      break;
    case HYBRID_SOLVER :
      m_solvers[i] = new HybridSolver3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
				      m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
				      m_currentConfig->solvers[i].dim,  m_currentConfig->solvers[i].timeStep,
				      m_currentConfig->solvers[i].buoyancy, m_currentConfig->solvers[i].omegaDiff, 
				      m_currentConfig->solvers[i].omegaProj, m_currentConfig->solvers[i].epsilon);
      break; 
    case LOD_HYBRID_SOLVER :
      m_solvers[i] = new LODHybridSolver3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
					 m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
					 m_currentConfig->solvers[i].dim,  m_currentConfig->solvers[i].timeStep,
					 m_currentConfig->solvers[i].buoyancy, m_currentConfig->solvers[i].omegaDiff, 
					 m_currentConfig->solvers[i].omegaProj, m_currentConfig->solvers[i].epsilon);
      break;
    case LOGRES_SOLVER :
      m_solvers[i] = new LogResSolver3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
				      m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
				      m_currentConfig->solvers[i].dim,  m_currentConfig->solvers[i].timeStep,
				      m_currentConfig->solvers[i].nbMaxIter, m_currentConfig->solvers[i].buoyancy, 
				      m_currentConfig->solvers[i].omegaDiff, m_currentConfig->solvers[i].omegaProj, 
				      m_currentConfig->solvers[i].epsilon);
      break;
    case LOGRESAVG_SOLVER :
      m_solvers[i] = new LogResAvgSolver3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
					 m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
					 m_currentConfig->solvers[i].dim,  m_currentConfig->solvers[i].timeStep,
					  m_currentConfig->solvers[i].nbMaxIter, m_currentConfig->solvers[i].buoyancy,
					 m_currentConfig->solvers[i].omegaDiff, m_currentConfig->solvers[i].omegaProj,
					 m_currentConfig->solvers[i].epsilon);
      break;
    case LOGRESAVGTIME_SOLVER :
      m_solvers[i] = new LogResAvgTimeSolver3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
					     m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
					     m_currentConfig->solvers[i].dim,  m_currentConfig->solvers[i].timeStep,
					      m_currentConfig->solvers[i].nbMaxIter, m_currentConfig->solvers[i].buoyancy,
					     m_currentConfig->solvers[i].omegaDiff, m_currentConfig->solvers[i].omegaProj, 
					     m_currentConfig->solvers[i].epsilon);
      break;
    case GS_SOLVER2D :
      m_solvers[i] = new GSSolver2D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
				  m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].dim,
				  m_currentConfig->solvers[i].timeStep, m_currentConfig->solvers[i].buoyancy);
      break;
    case SIMPLE_FIELD :
      m_solvers[i] = new Field3D(m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx, 
				  m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, 
				  m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].timeStep, 
				  m_currentConfig->solvers[i].buoyancy);
      break;
    default :
      cerr << "Unknown solver type : " << (int)m_currentConfig->solvers[i].type << endl;
      ::wxExit();
    }
  }
  prevNbSolvers = m_currentConfig->nbSolvers;
}

void GLFluidsCanvas::Init (FluidsAppConfig *config, bool recompileShaders)
{  
  m_currentConfig = config;

  InitUISettings();
  SetCurrent();
  InitGL(recompileShaders);
  
  m_camera = new Camera (m_width, m_height, m_currentConfig->clipping);
  InitSolvers();  
  
  ::wxStartTimer();
  
  m_init = true;
  
  cerr << "Initialisation terminée" << endl;
}

void GLFluidsCanvas::Restart (void)
{
  Disable();
  m_init = false;
  DestroyScene();

  m_width = m_currentConfig->width; m_height = m_currentConfig->height;
  glViewport (0, 0, m_width, m_height);
  m_camera = new Camera (m_width, m_height, m_currentConfig->clipping);
  
  InitUISettings();
  InitSolvers();
  ::wxStartTimer();
  m_init = true;
  cerr << "Réinitialisation terminée" << endl;
  Enable();
}

void GLFluidsCanvas::DestroyScene(void)
{ 
  delete m_camera;
   for (uint s = 0; s < prevNbSolvers; s++)
    delete m_solvers[s];
  delete[]m_solvers;
}

void GLFluidsCanvas::OnIdle(wxIdleEvent& event)
{
  if(m_run){    
    for(uint i=0 ; i < m_currentConfig->nbSolvers; i++)
      m_solvers[i]->iterate ();
  }
  
  this->Refresh();
  /*  draw();*/
  //event.RequestMore();
}

void GLFluidsCanvas::OnMouseMotion(wxMouseEvent& event)
{
  m_camera->OnMouseMotion(event);
}

void GLFluidsCanvas::OnMouseClick(wxMouseEvent& event)
{
  m_camera->OnMouseClick(event);
}

void GLFluidsCanvas::OnMouseWheel(wxMouseEvent& event)
{
  m_camera->moveOnFrontOrBehind(-event.GetWheelRotation()/500.0);
}

void GLFluidsCanvas::OnKeyPressed(wxKeyEvent& event)
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
void GLFluidsCanvas::OnPaint (wxPaintEvent& event)
{
  uint s;
  
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
  glDisable(GL_LIGHTING);
  /************ Affichage des outils d'aide à la visu (grille, etc...) *********/
  for (s = 0; s < m_currentConfig->nbSolvers; s++)
    {
      Point position(m_solvers[s]->getPosition ());
	
      glPushMatrix ();
      glTranslatef (position.x, position.y, position.z);
      if (m_displayBase)
	m_solvers[s]->displayBase();
      if (m_displayGrid)
	m_solvers[s]->displayGrid();
      if (m_displayDensity)
	m_solvers[s]->displayDensityField();
      if (m_displayVelocity)
	m_solvers[s]->displayVelocityField();	
      glPopMatrix ();
    }

  SwapBuffers ();
  
  //event.Skip();
  
  /******************** CALCUL DU FRAMERATE *************************************/
  m_framesCount++;
  m_globalFramesCount++;
  
  m_t = ::wxGetElapsedTime (false);
  if (m_t >= 2000){
    ((FluidsFrame *)GetParent())->SetFPS( m_framesCount / (m_t/1000) );
    
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

void GLFluidsCanvas::OnSize(wxSizeEvent& event)
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

#include "GLFluidsCanvas.hpp"

#include "fluidsFrame.hpp"
#include <iostream>

#include "../solvers/GSSolver2D.hpp"
#include "../solvers/GSSolver3D.hpp"
#include "../solvers/GCSSORSolver3D.hpp"
#include "../solvers/GCSSORSolver3D-SSE.hpp"
#include "../solvers/HybridSolver3D.hpp"
#include "../solvers/logResSolver3D.hpp"
#include "../solvers/logResAvgSolver3D.hpp"
#include "../solvers/compResAvgSolver3D.hpp"
#include "../solvers/logResAvgTimeSolver3D.hpp"
#include "../solvers/fakeField3D.hpp"
#include "../solvers/LODField3D.hpp"

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


GLFluidsCanvas::GLFluidsCanvas(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int* attribList, 
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
  m_switch = false;
  m_currentConfig = NULL;
  m_benchTime = false;
}

GLFluidsCanvas::~GLFluidsCanvas()
{
  DestroyScene();
  delete [] m_pixels;
}

void GLFluidsCanvas::InitUISettings(void)
{
  m_run = true;
  m_saveImages = false;
  m_displayGrid = false;
  m_displayBase = m_displayVelocity = m_displayDensity = true;
}

void GLFluidsCanvas::InitGL()
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
}
#define ARGS2D m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx,\
    m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].dim, \
    m_currentConfig->solvers[i].timeStep, m_currentConfig->solvers[i].buoyancy
#define ARGS m_currentConfig->solvers[i].position, m_currentConfig->solvers[i].resx,\
    m_currentConfig->solvers[i].resy, m_currentConfig->solvers[i].resz, \
    m_currentConfig->solvers[i].dim, m_currentConfig->solvers[i].scale,	\
    m_currentConfig->solvers[i].timeStep, m_currentConfig->solvers[i].buoyancy
#define ARGS_SLV ARGS, m_currentConfig->solvers[i].vorticityConfinement
#define ARGS_GC ARGS_SLV, m_currentConfig->solvers[i].omegaDiff, m_currentConfig->solvers[i].omegaProj,\
    m_currentConfig->solvers[i].epsilon
#define ARGS_LOG ARGS_GC, m_currentConfig->solvers[i].nbMaxIter

void GLFluidsCanvas::InitSolvers(void)
{
  m_solvers = new Field *[m_currentConfig->nbSolvers];
  for(uint i=0 ; i < m_currentConfig->nbSolvers; i++){
    switch(m_currentConfig->solvers[i].type){
    case GS_SOLVER :
      m_solvers[i] = new GSSolver3D(ARGS_SLV);
      break;
    case GCSSOR_SOLVER :
      m_solvers[i] = new GCSSORSolver3D(ARGS_GC);
      break;
    case GCSSOR_SOLVER_SSE :
      m_solvers[i] = new GCSSORSolver3D_SSE(ARGS_GC);
      break;
    case HYBRID_SOLVER :
      m_solvers[i] = new HybridSolver3D(ARGS_GC);
      break; 
    case LOD_HYBRID_SOLVER :
      m_solvers[i] = new LODSolver3D(ARGS_GC);
      break;
    case LOGRES_SOLVER :
      m_solvers[i] = new LogResSolver3D(ARGS_LOG);
      break;
    case LOGRESAVG_SOLVER :
      m_solvers[i] = new LogResAvgSolver3D(ARGS_LOG);
      break;
    case LOGRESAVGTIME_SOLVER :
      m_solvers[i] = new LogResAvgTimeSolver3D(ARGS_LOG);
      m_benchTime = true;
      break;
    case COMPRESAVG_SOLVER :
      m_solvers[i] = new CompResAvgSolver3D(ARGS_LOG);
      break;
    case GS_SOLVER2D :
      m_solvers[i] = new GSSolver2D(ARGS2D, m_currentConfig->solvers[i].vorticityConfinement);
      break;
    case SIMPLE_FIELD :
      m_solvers[i] = new RealField3D(ARGS);
      break;
    case FAKE_FIELD :
      m_solvers[i] = new FakeField3D(ARGS);
      break;
    case LOD_FIELD :
      m_solvers[i] = new LODField3D(ARGS_GC);
      break;
    case LOD_HYBRID_FIELD :
      m_solvers[i] = new LODSmoothField(ARGS_GC);
      break;
    default :
      cerr << "Unknown solver type : " << (int)m_currentConfig->solvers[i].type << endl;
      ::wxExit();
    }
  }
  prevNbSolvers = m_currentConfig->nbSolvers;
}

void GLFluidsCanvas::Init (FluidsAppConfig *config)
{  
  m_currentConfig = config;

  InitUISettings();
  SetCurrent();
  InitGL();
  
  m_camera = new Camera (m_width, m_height, m_currentConfig->clipping);
  InitSolvers();  
  
  m_swatch = new wxStopWatch();
  
  m_init = true;
  m_run = true;
  
  cerr << "Initialisation terminée" << endl;
}

void GLFluidsCanvas::Restart (void)
{
  Disable();
  m_run = false;
  m_init = false;
  DestroyScene();

  m_width = m_currentConfig->width; m_height = m_currentConfig->height;
  glViewport (0, 0, m_width, m_height);
  m_camera = new Camera (m_width, m_height, m_currentConfig->clipping);
  
  InitUISettings();
  InitSolvers();
  m_swatch->Start();
  m_init = true;
  m_run = true;
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
  if(m_run)
    for(uint i=0 ; i < m_currentConfig->nbSolvers; i++){
      m_solvers[i]->iterate ();
      m_solvers[i]->cleanSources();
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
  float step=0.3;
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
  
  if(m_benchTime)
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
  
  m_t = m_swatch->Time();
  if (m_t >= 2000){
    ((FluidsFrame *)GetParent())->SetFPS( m_framesCount / (m_t/1000) );
    m_swatch->Start();
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

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
    cerr << Listing << endl;
    cerr << "---------------------------------------------------\n" << endl;
    cerr << "Cg error, exiting...\n" << endl;
  }
}


wxGLBuffer::wxGLBuffer(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int* attribList, 
		       long style, const wxString& name, const wxPalette& palette)
  : wxGLCanvas(parent, id, pos, size, style, name, attribList, palette)
{
//   parent->Show(true);
//   SetCurrent();
  
  done = init = false;
  nb_flammes = 1;
}

wxGLBuffer::~wxGLBuffer()
{
  delete solveur;
  delete[]flammes;
  delete eyeball;
  delete SVShader;
  delete solidePhoto;
	
  if (context)
    cgDestroyContext (context);	
}

void wxGLBuffer::OnIdle(wxIdleEvent& event)
{
  if(animate)
    solveur->iterate (flickering);

  this->Refresh();
  
  /*  draw();*/
  event.RequestMore();
}
  
void wxGLBuffer::OnMouseMotion(wxMouseEvent& event)
{
  eyeball->OnMouseMotion(event);
}
  
void wxGLBuffer::OnMouseClick(wxMouseEvent& event)
{
  eyeball->OnMouseClick(event);
}

/** Fonction de dessin global */
void wxGLBuffer::OnPaint (wxPaintEvent& event)
{
  if(!init)
    return;

  wxPaintDC dc(this);
  
  if(!GetContext())
    return;
  
  SetCurrent();
  
  /* Déplacement du eyeball */
  eyeball->recalcModelView();

  glColor3f(1.0,1.0,0.0);
  GraphicsFn::SolidSphere(3,20,20);
  /********** CONSTRUCTION DES FLAMMES *******************************/
  // SDL_mutexP (lock);
  if(animate)
    for (int f = 0; f < nb_flammes; f++)
      flammes[f]->build();
  // SDL_mutexV (lock);
  
  /********** RENDU DES ZONES DE GLOW + BLUR *******************************/
  if(glowEnabled){
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
    
    glowEngine->activate();
    glowEngine->setGaussSigma(sigma);
    /* raz de la fenetre avec la couleur de fond */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBlendFunc (GL_SRC_ALPHA, GL_ZERO);
    
    scene->draw_scene ();
    
    /* Dessin de la flamme */
    if(affiche_flamme)
      for (int f = 0; f < nb_flammes; f++)
	flammes[f]->drawFlame (affiche_particules);
    
    glowEngine->blur();
    
    glowEngine->deactivate();
    
    glowEngine2->activate();
    glowEngine2->setGaussSigma(sigma);
    /* raz de la fenetre avec la couleur de fond */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBlendFunc (GL_SRC_ALPHA, GL_ZERO);
    
    scene->draw_scene ();
    
    /* Dessin de la flamme */
    if(affiche_flamme)
      for (int f = 0; f < nb_flammes; f++)
	flammes[f]->drawFlame (affiche_particules);
    
    glowEngine2->blur();
    
    glowEngine2->deactivate();
  }
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  if(!glowOnly){    
    glBlendFunc (GL_ONE, GL_ZERO);
    
    /******************* AFFICHAGE DE LA SCENE *******************************/
    /* !!!!!! Ceci n'est PAS CORRECT, dans le cas de PLUSIEURS flammes !!!!! */
    for (int f = 0; f < nb_flammes; f++) 
      flammes[f]->drawWick ();
    
    if(solidePhotoEnabled){
      solidePhoto->calculerFluctuationIntensiteCentreEtOrientation(flammes[0]->get_main_direction(),
								   flammes[0]->getPosition(),
								   solveur->getDimY());
      solidePhoto->draw(couleurOBJ,interpolationSP);
    }else{
      /**** Affichage de la scène ****/
      glPushAttrib (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      
      glEnable (GL_LIGHTING);
      
      for (int f = 0; f < nb_flammes; f++)
	{
	  if (shadowVolumesEnabled)
	    flammes[f]->draw_shadowVolumes (SCENE_OBJECTS_WSV_WT);
	  if (shadowsEnabled)
	    flammes[f]->cast_shadows_double (SCENE_OBJECTS_WSV_WT);
	  else{
	    flammes[f]->switch_on_lights ();
	  }
	}
      scene->draw_scene ();
      
      glPopAttrib ();
      
      glDisable (GL_LIGHTING);
    }
    
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /************ Affichage des outils d'aide à la visu (grille, etc...) *********/
    for (int f = 0; f < nb_flammes; f++)
      {
	CPoint position (*(flammes[f]->getPosition ()));
	
	glPushMatrix ();
	glTranslatef (position.getX (), position.getY (), position.getZ ());
	if (affiche_repere)
	  solveur->displayBase();
	if (affiche_grille)
	  solveur->displayGrid();
	if (affiche_velocite)
	  solveur->displayVelocityField();
	
	glPopMatrix ();
      }
    //cout << "bi8" << endl;
    /********************* Dessin de la flamme **********************************/
    if(!glowEnabled)
      if(affiche_flamme)
	for (int f = 0; f < nb_flammes; f++)
	  flammes[f]->drawFlame (affiche_particules);    
  }
  //cout << "b8" << endl;
  /********************* PLACAGE DU GLOW ****************************************/
  if(glowEnabled){
    glDisable (GL_DEPTH_TEST);
    glBlendColor(0.0,0.0,0.0,1.0);
    //glBlendFunc (GL_ONE, GL_CONSTANT_ALPHA);
    glBlendFunc (GL_ZERO,  GL_ONE_MINUS_SRC_COLOR);
    //glowEngine2->drawBlur(0.5);
    
    glowEngine->drawBlur(1.0);
    
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);
    glowEngine2->drawBlur(0.5);
    glBlendFunc (GL_ONE, GL_ONE);
    glowEngine->drawBlur(1.0);

    glEnable (GL_DEPTH_TEST);
  }
  
  /******** A VERIFIER *******/
  //glFlush();
  /***************************/
  SwapBuffers ();
  //event.Skip();


  /******************** CALCUL DU FRAMERATE *************************************/
  Frames++;
  
  t = ::wxGetElapsedTime (false);
  if (t >= 2000){    
    ((MainFrame *)GetParent())->SetFPS( Frames / (t/1000) );
    ::wxStartTimer();
    Frames = 0;
  } 
}

void wxGLBuffer::InitUISettings()
{
  /* Pour l'affichage */
  animate = true; 
  flickering = true; glowOnly = false;
  affiche_repere = affiche_velocite = affiche_particules = affiche_grille = false;
  affiche_fps = affiche_flamme = true;
  shadowsEnabled = shadowVolumesEnabled = false;
  solidePhotoEnabled = glowEnabled = true;
  interpolationSP = 1;
  couleurOBJ = 2;
}

/** Initialisation de l'interface */
void wxGLBuffer::Init (int l, int h, int solvx, int solvy, int solvz, double timeStep, 
		       char *scene_name, char *meche_name, double clipping)
{
  int nb_squelettes_flammes;
  
  InitUISettings();
  
  largeur = l; hauteur = h;

  SetCurrent();

  glClearColor (0.0, 0.0, 0.0, 1.0);
  /* Restriction de la zone d'affichage */
  glViewport (0, 0, largeur, hauteur);

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
  
  solveur = new Solver(solvx, solvy, solvz, 1.0, timeStep);
  //solveur = new BenchSolver (solvx, solvy, solvz, 1.0, timeStep);

  /* Changement de répertoire pour les textures */
  //AS_ERROR(chdir("textures"),"chdir textures");
  flammes = new Flame *[nb_flammes];
  
  // Création du contexte CG
  context = cgCreateContext();

  contextCopy = &context;
  // Fonction a executer si erreur CG
  cgSetErrorCallback(cgErrorCallback);

  solidePhoto = new SolidePhotometrique(scene, &context);
  /***************************** CG Init *******************************/
  GLfloat fatnessVec[] = { -0.001, -0.001, -0.001, 0.0 };
  GLfloat shadowExtrudeDistVec[] = { 5.0, 5.0, 5.0, 0.0 };
  SVShader = new CgSVShader (_("ShadowVolumeExtrusion.cg"), _("main"), &context);
  SVShader->setFatness (fatnessVec);
  SVShader->setshadowExtrudeDist (shadowExtrudeDistVec);
  /*********************************************************************/
  
#ifdef BOUGIE
  CPoint pt (0.0, 0.0, 0.0), pos (0.0, 0.0, 0.0);
  nb_squelettes_flammes = 4;
  flammes[0] = new Bougie (solveur, nb_squelettes_flammes, &pt, &pos,
			   solveur->getDimX()/ 7.0, SVShader,"bougie.obj",scene, &context);
#else
  CPoint pt (0.0, 0.0, 0.0), pos (1.5, -1.8, 0.0);
  //CPoint pt (0.0, 0.0, 0.0), pos (0.0, -0.5, 0.0);
  nb_squelettes_flammes = 5;
  flammes[0] = new Firmalampe(solveur,nb_squelettes_flammes,&pt,&pos,SVShader,meche_name,"firmalampe.obj",scene);
#endif	
  //AS_ERROR(chdir(".."),"chdir ..");
  solveur->setFlames ((Flame **) flammes, nb_flammes);
    
  scene = new CScene (scene_name,flammes, nb_flammes);
  eyeball = new Eyeball (largeur, hauteur, clipping);
  
  glowEngine  = new GlowEngine (scene, eyeball, &context, largeur, hauteur, 4);
  glowEngine2 = new GlowEngine (scene, eyeball, &context, largeur, hauteur, 1);
  cout << "Initialisation terminée" << endl;
  /*GraphicsFn::makeRasterFont ();
    sprintf (strfps, "%d", 0);*/
  init = true;

  ::wxStartTimer();
}

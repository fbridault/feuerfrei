#include "wxGLBuffer.hpp"

#include <iostream>

BEGIN_EVENT_TABLE(wxGLBuffer, wxGLCanvas)
  EVT_PAINT(wxGLBuffer::OnPaint)
  EVT_IDLE(wxGLBuffer::OnIdle)
END_EVENT_TABLE();


wxGLBuffer::wxGLBuffer(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int* attribList, 
		       long style, const wxString& name, const wxPalette& palette)
  : wxGLCanvas(parent, id, pos, size, style, name, attribList, palette), rotate_(0), init(true)
{
  /* Pour l'affichage */
  animate = true; 
  brintage = glowOnly = false;
  affiche_repere = affiche_velocite = affiche_particules = affiche_grille = false;
  affiche_fps = affiche_flamme = true;
  shadowsEnabled = shadowVolumesEnabled = false;
  solidePhotoEnabled = glowEnabled = true;
  interpolationSP = 1;
  couleurOBJ = 2;
  
  /* Pour le solveur */
  double dim = 1.0;
  
  if (solvx > solvy){
    if (solvx > solvz){
      dim_x = dim;
      dim_y = dim_x * solvy / solvx;
      dim_z = dim_x * solvz / solvx;
    }else{
      dim_z = dim;
      dim_x = dim_z * solvx / solvz;
      dim_y = dim_z * solvy / solvz;
    }
  }else{
    if (solvy > solvz){
      dim_y = dim;
      dim_x = dim_y * solvx / solvy;
      dim_z = dim_y * solvz / solvy;
    }else{
      dim_z = dim;
      dim_x = dim_z * solvx / solvz;
      dim_y = dim_z * solvy / solvz;
    }
  }
  /* Construction des display lists */
  GraphicsFn::repere (solvx, solvy, solvz);
  GraphicsFn::grille (solvx, solvy, solvz);
  
  solveur = new Solver(solvx, solvy, solvz, pas_de_temps);
  //solveur = new BenchSolver (solvx, solvy, solvz, pas_de_temps);

  /* Changement de répertoire pour les textures */
  //AS_ERROR(chdir("textures"),"chdir textures");
  flammes = new Flame *[nb_flammes];

}

wxGLBuffer::wxGLBuffer()
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
    solveur->iterate (brintage);

  this->Refresh();
  
  /*  draw();
      event.RequestMore();*/
}

/** Affiche le framerate à l'écran */
void wxGLBuffer::WriteFPS ()
{
  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();
  glLoadIdentity ();
  gluOrtho2D (0.0, (GLfloat) largeur, 0.0, (GLfloat) hauteur);
  glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();
  glLoadIdentity ();
  GraphicsFn::printString (strfps, largeur - 80, hauteur - 20);
  glPopMatrix ();
  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();
  glMatrixMode (GL_MODELVIEW);
}

/** Fonction de dessin de la vélocité */
void wxGLBuffer::DrawVelocity (void)
{
  double inc_x = dim_x / (double) solvx;
  double inc_y = dim_y / (double) solvy;
  double inc_z = dim_z / (double) solvz;
  
  for (int i = 1; i <= solvx; i++)
    {
      for (int j = 1; j <= solvy; j++)
	{
	  for (int k = 1; k <= solvz; k++)
	    {
	      CVector vect;
	      /* Affichage du champ de vélocité */
	      glPushMatrix ();
	      glTranslatef (inc_x * i - inc_x / 2.0 - dim_x / 2.0,
			    inc_y * j - inc_y / 2.0, 
			    inc_z * k - inc_z / 2.0 -  dim_z / 2.0);
	      //    printf("vélocité %d %d %d %f %f %f\n",i,j,k,getU(i,j,k)],getV(i,j,k),getW(i,j,k));
	      SDL_mutexP (lock);
	      vect.setX (solveur->getU (i, j, k));
	      vect.setY (solveur->getV (i, j, k));
	      vect.setZ (solveur->getW (i, j, k));
	      SDL_mutexV (lock);
	      GraphicsFn::fleche (&vect,solvx,solvy,solvz);
	      glPopMatrix ();
	    }
	}
    }
}

/** Fonction de dessin global */
void wxGLBuffer::OnPaint (wxPaintEvent& event)
{
  wxPaintDC dc(this);
  
  if(!GetContext())
    return;
  
  SetCurrent();
  
  if(init){
    InitUI();
    init=false;
  }
  
  /* Déplacement du eyeball */
  //eyeball->recalcModelView();
  
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
								   flammes[0]->getPosition());
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
	  glCallList (REPERE);
	if (affiche_grille)
	  glCallList (GRILLE);
	if (affiche_velocite)
	  draw_velocity();
	
	glPopMatrix ();
      }
    
    /********************* Dessin de la flamme **********************************/
    if(!glowEnabled)
      if(affiche_flamme)
	for (int f = 0; f < nb_flammes; f++)
	  flammes[f]->drawFlame (affiche_particules);
    
    if (affiche_fps)
      write_fps ();
  }
  
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
  glFlush();
  /***************************/
  SwapBuffers ();
  //event.Skip();

  /******************** CALCUL DU FRAMERATE *************************************/
  // Frames++;
  
//   GLint t = SDL_GetTicks ();
//   if (t - T0 >= 2000){
//     GLfloat seconds = (t - T0) / 1000.0;
//     GLfloat fps = Frames / seconds;
    
//     sprintf (strfps, "%d", (int) fps);
    
//     //printf("%d frames in %g seconds = %g FPS %s\n", Frames, seconds, fps, strfps);
//     T0 = t;
//     Frames = 0;
//   } 
}

/** Initialisation de l'interface */
void wxGLBuffer::InitUI ()
{
  int nb_squelettes_flammes;

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
  
  // Création du contexte CG
  context = cgCreateContext();
  // Fonction a executer si erreur CG
  cgSetErrorCallback(cgErrorCallback);
  
  solidePhoto = new SolidePhotometrique(scene, &context);
  
  /***************************** CG Init *******************************/
  GLfloat fatnessVec[] = { -0.001, -0.001, -0.001, 0.0 };
  GLfloat shadowExtrudeDistVec[] = { 5.0, 5.0, 5.0, 0.0 };
  SVShader = new CgSVShader ("ShadowVolumeExtrusion.cg", "main", &context);
  SVShader->setFatness (fatnessVec);
  SVShader->setshadowExtrudeDist (shadowExtrudeDistVec);
  /*********************************************************************/
#ifdef BOUGIE
  CPoint pt (0.0, 0.0, 0.0), pos (0.0, 0.0, 0.0);
  nb_squelettes_flammes = 4;
  flammes[0] = new Bougie (solveur, nb_squelettes_flammes, &pt, &pos,
			   dim_x / 7.0, SVShader,"bougie.obj",scene, &context);
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
  
  glowEngine = new GlowEngine (scene, eyeball, &context, largeur, hauteur, 4);
  glowEngine2 = new GlowEngine (scene, eyeball, &context, largeur, hauteur, 1);
  
  /*GraphicsFn::makeRasterFont ();
  sprintf (strfps, "%d", 0);*/
}

/** Fonction appelée en cas d'erreur provoquée par Cg */
void cgErrorCallback(void)
{
  CGerror LastError = cgGetError();
  
  if(LastError){
    const char *Listing = cgGetLastListing(context);
    printf("\n---------------------------------------------------\n");
    printf("%s\n\n", cgGetErrorString(LastError));
    printf("%s\n", Listing);
    printf("---------------------------------------------------\n");
    printf("Cg error, exiting...\n");
  }
}

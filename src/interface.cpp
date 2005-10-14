#include <time.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_thread.h"

#include "header.h"
#include "CgSVShader.hpp"
#include "eyeball.hpp"

#include "scene.hpp"
#include "solidePhoto.hpp"
#include "graphicsFn.hpp"
#include "bougie.hpp"
#include "firmalampe.hpp"
#include "solver.hpp"
#include "benchsolver.hpp"
#include "glowengine.hpp"

/********* Variables relatives au contrôle de l'affichage **************/
static bool animate, affiche_velocite, affiche_repere, affiche_grille,
  affiche_flamme, affiche_fps, brintage, shadowsEnabled,
  shadowVolumesEnabled, affiche_particules, glowEnabled, glowOnly;
int done=0;

/********* Variables relatives au solveur ******************************/
static Solver *solveur;
static float pas_de_temps;
int solvx, solvy, solvz;
double dim_x, dim_y, dim_z;
SDL_mutex *lock;

/********* Variables relatives à la fenêtre d'affichage ****************/
static int largeur, hauteur;
static double clipping;
CGcontext context;
static Eyeball *eyeball;
/* Pour le compte des frames */
static GLint Frames;
static GLint T0 = 0;
static char strfps[4];

/********* Variables relatives à la simulation *************************/
static char scene_name[255], meche_name[255];
static Flame **flammes;
static int nb_flammes = 1;
static CScene *scene;
static CgSVShader *SVShader;

/********* Variables relatives aux solides photométriques **************/
static SolidePhotometrique *solidePhoto;
static bool solidePhotoEnabled, rotation,modePano,pixelBFC;
/* interpolationSP = 0 ou 1; couleurOBJ = 0 ou 2 */
static unsigned char interpolationSP, couleurOBJ;

/********* Variables relatives au glow *********************************/
static GlowEngine *glowEngine;
static GlowEngine *glowEngine2;

/**************************** FONCTIONS GENERALES DE L'INTERFACE GRAPHIQUE **************************/

/** Fonction de redimensionnement du ViewPort */
static void
reshape (int width, int height)
{
  if (width > height)
    glViewport ((width - height) / 2, 0, height, height);
  else
    glViewport (0, (height - width) / 2, width, width);

  largeur = width;
  hauteur = height;

  return;
}

/** Affiche le framerate à l'écran */
void
write_fps ()
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
static void
draw_velocity (void)
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
static void
draw (void)
{
  /* Déplacement du eyeball */
  eyeball->recalcModelView();
  
  /********** CONSTRUCTION DES FLAMMES *******************************/
  SDL_mutexP (lock);
  if(animate)
    for (int f = 0; f < nb_flammes; f++)
      flammes[f]->build();
  SDL_mutexV (lock);
  
  /********** RENDU DES ZONES DE GLOW + BLUR *******************************/
  if(glowEnabled){
    GLfloat m[4][4];
    CVector direction;
    float dist, sigma;

    glGetFloatv (GL_MODELVIEW_MATRIX, &m[0][0]);

    direction.setX (m[3][0]);
    direction.setY (m[3][1]);
    direction.setZ (m[3][2]);

    dist = direction.length();
    sigma = dist > 0.1 ? -log(5*dist)+6 : 6.0;
    
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
    
    glBlendFunc (GL_ONE, GL_ONE);
    glowEngine2->drawBlur();    
    
    glowEngine->drawBlur();
    glEnable (GL_DEPTH_TEST);
  }

  SDL_GL_SwapBuffers ();
  
  /******************** CALCUL DU FRAMERATE *************************************/
  Frames++;
  
  GLint t = SDL_GetTicks ();
  if (t - T0 >= 2000){
    GLfloat seconds = (t - T0) / 1000.0;
    GLfloat fps = Frames / seconds;
    
    sprintf (strfps, "%d", (int) fps);
    
    //printf("%d frames in %g seconds = %g FPS %s\n", Frames, seconds, fps, strfps);
    T0 = t;
    Frames = 0;
  } 
}

static void printHelp()
{
  printf("Test Solide Photometrique\n");
  printf("s : active/desactive les shadows volumes\n");
  printf("d : active/desactive le Solide Photometrique\n");
  printf("c : active/desactive les couleurs et textures du fichier OBJ\n");
  printf("i : active/desactive l'interpolation en arc de spirale\n");
  printf("espace : defile entre les fichiers IES precharges\n");
  printf("m : bascule entre mode de visu panorama et eyeball\n");
  printf("f : affichage du nombre d'images par seconde\n");
  printf("h : cet aide\n");
  printf("ESC : quitter\n");
}

/** Fonction pour les interactions avec le clavier */
static void
keypressed (SDLKey touche)
{
  int i, j;

  switch (touche){
  case SDLK_ESCAPE:
    done = 1;
    break;
  case SDLK_e:
    glowEnabled = !glowEnabled;
    break;
case SDLK_z:
    glowOnly = !glowOnly;
    break;
  case SDLK_d :
    solidePhotoEnabled = !solidePhotoEnabled;
    break;
  case SDLK_m :
    modePano = !modePano;
    break;
  case SDLK_c :
    couleurOBJ = 2-couleurOBJ;
    break;
  case SDLK_i :
    interpolationSP = 1-interpolationSP;
    break;
  case SDLK_n :
    pixelBFC = !pixelBFC;
    break;
  case SDLK_SPACE : // faire défiler les fichiers IES precharges
    solidePhoto->swap();
    break;
  case SDLK_h :
    printHelp();
    break;
  case SDLK_KP4:
    for (i = -solvz / 4 - 1; i <= solvz / 4 + 1; i++)
      for (j = -2 * solvy / 4; j < -solvy / 4; j++)
	solveur->addUsrc (2,
			  ((int) (ceil (solvy / 2.0))) + j,
			  ((int) (ceil (solvz / 2.0))) + i,
			  .3);
    break;
  case SDLK_KP8:
    for (i = -solvx / 4 - 1; i <= solvx / 4 + 1; i++)
      for (j = -solvy / 2; j < solvy / 2; j++)
	solveur->
	  addWsrc (((int) (ceil (solvx / 2.0))) + i,
		   ((int) (ceil (solvy / 2.0))) + j, 
		   1,
		   .2);
    break;
  case SDLK_KP6:
    for (i = -solvz / 4 - 1; i <= solvz / 4 + 1; i++)
      for (j = -2 * solvy / 4; j < -solvy / 4; j++)
	solveur->addUsrc (solvx - 1,
			  ((int) (ceil (solvy / 2.0))) + j,
			  ((int) (ceil (solvz / 2.0))) + i,
			  -.3);
    break;
  case SDLK_KP2:
    for (i = -solvx / 4 - 1; i <= solvx / 4 + 1; i++)
      for (j = -solvy / 2; j < solvy / 2; j++)
	solveur->
	  addWsrc (((int) (ceil (solvx / 2.0))) + i,
		   ((int) (ceil (solvy / 2.0))) + j, 
		   solvz, 
		   -.2);
    break;
  case SDLK_r:
    affiche_repere = !affiche_repere;
    break;
  case SDLK_v:
    affiche_velocite = !affiche_velocite;
    break;
  case SDLK_g:
    affiche_grille = !affiche_grille;
    break;
  case SDLK_p:
    affiche_particules = !affiche_particules;
    break;
  case SDLK_f:
    affiche_fps = !affiche_fps;
    break;
  case SDLK_b:
    brintage = !brintage;
    break;
  case SDLK_KP0:
    animate = !animate;
    break;
  case SDLK_KP_PLUS:
    affiche_flamme = !affiche_flamme;
    break;
  case SDLK_KP_MINUS:
    for (int f = 0; f < nb_flammes; f++)
      flammes[f]->toggleSmoothShading ();
    break;
  case SDLK_KP_ENTER:
    solveur->cleanSources ();
    break;
  case SDLK_s:
    shadowsEnabled = !shadowsEnabled;
    break;
  case SDLK_q:
    shadowVolumesEnabled = !shadowVolumesEnabled;
  default:
    break;
  }
}

/** Fonction de parcours du fichier d'initialisation.
 * Il comprend pour l'instant : la taille de la grille de résolution,
 * le pas de temps utilisé, le nom du fichier contenant la scÃ¨ne, la taille de la fenÃªtre
 * d'affichage, et la profondeur du plan de clipping (selon la scÃ¨ne chargée Ã§a peut Ãªtre pratique ;-) )
 */
static int
parse_init_file (char *name)
{
  FILE *f;
  char buffer[255];

  if ((f = fopen (name, "r")) == NULL)
    {
      cout << "ProblÃ¨me d'ouverture du fichier de paramétrage" <<
	endl;
      return 2;
    }

  do
    {
      fscanf (f, "%s", buffer);

      if (!strcmp (buffer, "GRID_SIZE"))
	fscanf (f, "%d %d %d", &solvx, &solvy, &solvz);
      else if (!strcmp (buffer, "TIME_STEP"))
	fscanf (f, "%f", &pas_de_temps);
      else if (!strcmp (buffer, "SCENE"))
	fscanf (f, "%s", scene_name);
      else if (!strcmp (buffer, "WINDOW_SIZE"))
	fscanf (f, "%d %d", &largeur, &hauteur);
      else if (!strcmp (buffer, "CLIPPING"))
	fscanf (f, "%lf", &clipping);
      else if (!strcmp (buffer, "WICK"))
	fscanf (f, "%s", meche_name);

    }
  while (!feof (f));

  cout << "Grille de " << solvx << "x" << solvy << "x" << solvz << endl;
  cout << "Pas de temps :" << pas_de_temps << endl;
  cout << "Nom de la scÃ¨ne :" << scene_name << endl;

  fclose (f);

  return 0;
}

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

/** Initialisation de l'interface */
static void
init_ui ()
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

  /* Pour l'affichage */
  animate = true; 
  brintage = glowOnly = false;
  affiche_repere = affiche_velocite = affiche_particules = affiche_grille = false;
  affiche_fps = affiche_flamme = true;
  shadowsEnabled = shadowVolumesEnabled = false;
  solidePhotoEnabled = glowEnabled = true;
  interpolationSP = 1;
  couleurOBJ = 2;
  
  GraphicsFn::makeRasterFont ();
  sprintf (strfps, "%d", 0);
}

/** Sauvegarde de la simulation sous forme d'images BMP.
 * Prévoir la sauvegarde dans un autre format pourrait être bien pratique !
 */
static void
SaveScreen (const char *filename, SDL_Surface * screen)
{
  SDL_Surface *image;
  SDL_Surface *temp;
  int idx;

  int w = screen->w;
  int h = screen->h;

  cout << w << " " << h << endl;

  image = SDL_CreateRGBSurface (SDL_SWSURFACE, w, h, 32, 0x000000FF,
				0x0000FF00, 0x00FF0000, 0xFF000000);
  temp = SDL_CreateRGBSurface (SDL_SWSURFACE, w, h, 32, 0x000000FF,
			       0x0000FF00, 0x00FF0000, 0xFF000000);
  glReadPixels (0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
  for (idx = 0; idx < h; idx++)
    {
      memcpy ((unsigned char *) (temp->pixels) + 4 * w * idx,
	      (unsigned char *) (image->pixels) + 4 * w * (h - idx),
	      4 * w);
    }
  memcpy (image->pixels, temp->pixels, w * h * 4);
  SDL_SaveBMP (image, filename);
  SDL_FreeSurface (image);
  SDL_FreeSurface (temp);
}

/** Boucle du solveur principal dans le cas du multithread */
static int
solveur_loop (void *)
{
  GLint t0 = 0;
  GLint t, ecart = 0;
  while (!done)
    {
      if (animate)
	{
	  SDL_mutexP (lock);
	  solveur->iterate (brintage);
	  SDL_mutexV (lock);

	  //ecart++;
	  t = SDL_GetTicks ();

	  /*       if (t - t0 >= 5000) { */
	  /*         GLfloat seconds = (t - t0) / 1000.0; */
	  /*         GLfloat fps = Frames / seconds; */
	  /*         printf("%d res in %g seconds = %g res/s\n", ecart, seconds, fps); */
	  /*         t0 = t; */
	  /*         ecart = 0; */
	  /*      } */
	  ecart = t - t0;
	  //printf("time %d ms\n",ecart);
	  if (ecart < 40)
	    {
	      //printf("Delayed by %d ms\n",40-ecart);
	      SDL_Delay (40 - ecart);
	    }
	  t0 = t;
	}
    }
  return 0;
}

/** Boucle de la simulation, dans le cas oÃ¹ l'on sauvegarde la simulation 
 * sous la forme d'images bmp
 */
static int
sdl_loop_with_capture (void *arg)
{
  SDL_Surface *screen = (SDL_Surface *) arg;

  done = 0;
  int i = 100000;
  char file[15], num[7];

  while (!done)
    {
      SDL_Event event;

      solveur->iterate (brintage);
      while (SDL_PollEvent (&event))
	{
	  switch (event.type)
	    {
	    case SDL_VIDEORESIZE:
	      screen = SDL_SetVideoMode (event.resize.w,
					 event.resize.h, 16,
					 SDL_OPENGL |
					 SDL_RESIZABLE);
	      if (screen)
		{
		  reshape (screen->w, screen->h);
		}
	      else
		{
		  /* Uh oh, we couldn't set the new video mode?? */
		  ;
		}
	      break;

	    case SDL_QUIT:
	      done = 1;
	      break;

	    case SDL_KEYDOWN:
	      keypressed (event.key.keysym.sym);
	      break;
	    case SDL_MOUSEBUTTONUP:
	    case SDL_MOUSEBUTTONDOWN:
	      eyeball->mouseButton (&event.button);
	      break;
	    case SDL_MOUSEMOTION:
	      eyeball->mouseMotion (&event.motion);
	      break;
	    }
	}
      draw ();
      sprintf (num, "%d", i);

      sprintf (file, "capture/img-%s.bmp", num);
      cout << file << endl;
      SaveScreen (file, screen);
      i++;
    }
  return 0;
}

/** Boucle de la simulation, en simple processus donc */
static int
sdl_loop_single_thread (void *arg)
{
  SDL_Surface *screen = (SDL_Surface *) arg;

  done = 0;

  while (!done)
    {
      SDL_Event event;
      //~ time_t start;
      //~ double t;
		
      //~ start=clock();
   
      solveur->iterate (brintage);
      //~ t = (clock() - start)/((double)CLOCKS_PER_SEC);
      //~ printf("temps d'une itération : %g sec  \r", t);
      //~ fflush(stdout);
		
      while (SDL_PollEvent (&event))
	{
	  switch (event.type)
	    {
	    case SDL_VIDEORESIZE:
	      screen = SDL_SetVideoMode (event.resize.w,
					 event.resize.h, 16,
					 SDL_OPENGL |
					 SDL_RESIZABLE);
	      if (screen)
		{
		  reshape (screen->w, screen->h);
		}
	      else
		{
		  /* Uh oh, we couldn't set the new video mode?? */
		  ;
		}
	      break;

	    case SDL_QUIT:
	      done = 1;
	      break;

	    case SDL_KEYDOWN:
	      keypressed (event.key.keysym.sym);
	      break;
	    case SDL_MOUSEBUTTONUP:
	    case SDL_MOUSEBUTTONDOWN:
	      eyeball->mouseButton (&event.button);
	      break;
	    case SDL_MOUSEMOTION:
	      eyeball->mouseMotion (&event.motion);
	      break;
	    }
	}
      draw ();
    }
  return 0;
}

/** Boucle d'affichage dans le cas du multithread */
static int
sdl_loop (void *arg)
{
  SDL_Surface *screen = (SDL_Surface *) arg;

  done = 0;

  while (!done)
    {
      SDL_Event event;

      while (SDL_PollEvent (&event))
	{
	  switch (event.type)
	    {
	    case SDL_VIDEORESIZE:
	      screen = SDL_SetVideoMode (event.resize.w,
					 event.resize.h, 16,
					 SDL_OPENGL |
					 SDL_RESIZABLE);
	      if (screen)
		{
		  reshape (screen->w, screen->h);
		}
	      else
		{
		  /* Uh oh, we couldn't set the new video mode?? */
		  ;
		}
	      break;

	    case SDL_QUIT:
	      done = 1;
	      break;

	    case SDL_KEYDOWN:
	      keypressed (event.key.keysym.sym);
	      break;
	    case SDL_MOUSEBUTTONUP:
	    case SDL_MOUSEBUTTONDOWN:
	      eyeball->mouseButton (&event.button);
	      break;
	    case SDL_MOUSEMOTION:
	      eyeball->mouseMotion (&event.motion);
	      break;
	    }
	}
      draw ();
    }
  return 0;
}

/** Programme principal */
int
main (int argc, char *argv[])
{
  SDL_Surface *screen;
  SDL_Thread *threadSolver;
  bool capture = false, single = false;

  switch (argc)
    {
    case 1:
      break;
    case 2:
      if (!strcmp (argv[1], "capture"))
	capture = true;
      if (!strcmp (argv[1], "single"))
	single = true;
      break;
    default:
      cout << "Bad parameters number or wrong parameters" << endl;
    }
#ifdef BOUGIE
  if (parse_init_file ("param.ini"))
    exit (2);
#else
  if (parse_init_file ("param2.ini"))
    exit (2);
#endif
  SDL_Init (SDL_INIT_VIDEO);
  
  /* A ne pas oublier !!! */
  /* N'était pas nécessaire précedemment */
  SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 1 );
  
  screen = SDL_SetVideoMode (largeur, hauteur, 16,
			     SDL_OPENGL | SDL_RESIZABLE);
  
  if (!screen)
    {
      cerr << "Couldn't set " << largeur << "x" << hauteur <<
	" GL video mode: " << SDL_GetError ();
      SDL_Quit ();
      exit (2);
    }
  SDL_WM_SetCaption ("Bougie", "bougie");

  init_ui ();
  reshape (screen->w, screen->h);

  if (capture)
    {
      cout << "Lancement du programme en simple processus, avec sortie .bmp" << endl;
      sdl_loop_with_capture (screen);
    }
  else if (single)
    {
      cout << "Lancement du programme en simple processus" << endl;
      sdl_loop_single_thread (screen);
    }
  else
    {
      cout << "Lancement du programme en multi-processus" << endl;
      lock = SDL_CreateMutex ();
      if ((threadSolver =
	   SDL_CreateThread (solveur_loop, NULL)) == NULL)
	{
	  cerr << "Unable to create thread: " <<
	    SDL_GetError ();
	  SDL_Quit ();
	  return 0;
	}
      sdl_loop (screen);

      SDL_WaitThread (threadSolver, NULL);

      SDL_DestroyMutex (lock);
    }

  delete solveur;
  delete[]flammes;
  delete eyeball;
  delete SVShader;
  delete solidePhoto;
	
  if (context)
    cgDestroyContext (context);	
  
  return 0;
}

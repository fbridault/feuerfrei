#include "glowengine.hp"


GlowEngine::GlowEngine(CGcontext *cgcontext) : pbuffer("rgb"), blurVertexShaderX("glowShaders.cg","vertGlowX",  context),
						 blurVertexShaderY("glowShaders.cg","vertGlowY",  context),
						 blurFragmentShader("glowShaders.cg","fragGlow",  context, tex)
{
  // Initialiser le pbuffer maintenant que nous avons un contexte valide
  // a utiliser pour la creation du pbuffer
  pbuffer.Initialize(width, height, true, true);
  // Initialisations pour le contexte de rendu du pbuffer
  // Activer le pbuffer
  pbuffer.Activate();
  // Couleur pour l'effacement
  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  
  // Parametre de visualisation
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  context = cgcontext;
  
  glGenTextures(1, &texTemp);
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, texTemp);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

  pbuffer.Deactivate();
}

GlowEngine::~GlowEngine()
{
}

GlowEngine::render()
{
  pbuffer.Activate();
  /* raz de la fenetre avec la couleur de fond */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  /* Déplacement du eyeball */
  //eyeball->recalcModelView();
    
  /****************************************/
  //  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  /* Rendu de la scène */
  scene->draw_scene ();
  
  /* Dessin de la flamme */
  /* Voir pour créer une display list pour la 2e passe */
  SDL_mutexP (lock);
  for (int f = 0; f < nb_flammes; f++)
    flammes[f]->dessine (animate, affiche_flamme, affiche_particules);
  SDL_mutexV (lock);
  
  /* On récupère le rendu des zones de glow dans la texture */
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, texblur);
  glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, width, height);

  /* Premier blur */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  blurVertexShaderX->setOffsetsArray();
  blurVertexShaderX->setModelViewProjectionMatrix();
  blurFragmentShader->setWeightsArray();
  blurFragmentShader->setTexture(texblur);
  blurVertexShaderX->enableShader();
  blurFragmentShader->enableShader();
  
  glColor3f(1.0,1.0,1.0);
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,0);
  glVertex3f(-1.0,1.0,0.0);
  
  glTexCoord2f(width,0);
  glVertex3f(1.0,1.0,0.0);
  
  glTexCoord2f(width,height);
  glVertex3f(1.0,-1.0,0.0);
  
  glTexCoord2f(0,height);
  glVertex3f(-1.0,-1.0,0.0);

  glEnd();

  blurVertexShaderX->disableProfile();
  blurFragmentShader->disableProfile();
 
  /* On récupère le premier blur dans la texture */
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, texblur);
  glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, width, height);
  
  /* Deuxième blur */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  blurVertexShaderY->setOffsetsArray();
  blurVertexShaderY->setModelViewProjectionMatrix();
  blurFragmentShader->setWeightsArray();
   blurFragmentShader->setTexture(texblur);
  blurVertexShaderY->enableShader();
  blurFragmentShader->enableShader();
    
  glColor3f(1.0,1.0,1.0);
  
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,0);
  glVertex3f(-1.0,1.0,0.0);
  
  glTexCoord2f(width,0);
  glVertex3f(1.0,1.0,0.0);
  
  glTexCoord2f(width,height);
  glVertex3f(1.0,-1.0,0.0);
  
  glTexCoord2f(0,height);
  glVertex3f(-1.0,-1.0,0.0);
    
  glEnd();
   
  blurVertexShaderY->disableProfile();
  blurFragmentShader->disableProfile();

  /* On récupère le deuxième blur dans la texture */
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, texblur);
  glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, width, height);
  
  pbuffer.Deactivate();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_TEXTURE_RECTANGLE_NV);
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, texblur);
  
  glColor3f(1.0,1.0,1.0);
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,0);
  glVertex3f(-1.0,1.0,0.0);
  
  glTexCoord2f(width,0);
  glVertex3f(1.0,1.0,0.0);
  
  glTexCoord2f(width,height);
  glVertex3f(1.0,-1.0,0.0);
  
  glTexCoord2f(0,height);
  glVertex3f(-1.0,-1.0,0.0);

  glEnd();

  glDisable(GL_TEXTURE_RECTANGLE_NV);
}

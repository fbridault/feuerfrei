#include "glowengine.hp"


GlowEngine::GlowEngine(CGcontext *cgcontext) : pbuffer("rgb"), blurVertexShaderX("glowShaders.cg","vertGlowX",  context),
						 blurVertexShaderY("glowShaders.cg","vertGlowY",  context),
						 blurFragmentShader("glowShaders.cg","fragGlow",  context, tex)
{
  // Initialiser le pbuffer maintenant que nous avons un contexte valide
  // a utiliser pour la creation du pbuffer
  pbuffer.Initialize(LARGEUR, HAUTEUR, true, true);
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
  glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGB, 800, 800, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

  pbuffer.Deactivate();
}

GlowEngine::~GlowEngine()
{
}

GlowEngine::render()
{
  
}

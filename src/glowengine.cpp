#include "glowengine.hpp"


GlowEngine::GlowEngine(CScene *s, Camera *e, CGcontext *cgcontext, int w, int h, int sc) : 
  pbuffer("rgb"), blurVertexShaderX(_("glowShaders.cg"),_("vertGlowX"),  cgcontext),
  blurVertexShaderY(_("glowShaders.cg"),_("vertGlowY"),  cgcontext),
  blurFragmentShader(_("glowShaders.cg"),_("fragGlow"),  cgcontext)
{
  scene = s;
  context = cgcontext;
  camera = e;
  scaleFactor=sc;
  width=w/scaleFactor;
  height=h/scaleFactor;

  // Initialiser le pbuffer
  pbuffer.Initialize(width, height, true, true);
  
  glGenTextures(1, &texblur);
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, texblur);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  
  //textest = new Texture("texture.jpg",GL_TEXTURE_RECTANGLE_NV);
}

GlowEngine::~GlowEngine()
{
}

void GlowEngine::activate()
{
  pbuffer.Activate();
  glViewport (0, 0, width, height);
}

void GlowEngine::blur()
{
  // Parametre de visualisation
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(-1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  /* On récupère le rendu des zones de glow dans la texture */
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, texblur);
  glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, width, height);

  /* Premier blur */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  blurVertexShaderX.setOffsetsArray();
  blurVertexShaderX.setModelViewProjectionMatrix();
  blurFragmentShader.setWeightsArray();
  blurFragmentShader.setTexture(texblur);
  blurVertexShaderX.enableShader();
  blurFragmentShader.enableShader();
  
  glColor3f(1.0,1.0,1.0);
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,0);
  glVertex3d(-1.0,1.0,0.0);
  
  glTexCoord2f(0,height);
  glVertex3d(-1.0,-1.0,0.0);

  glTexCoord2f(width,height);
  glVertex3d(1.0,-1.0,0.0);

  glTexCoord2f(width,0);
  glVertex3d(1.0,1.0,0.0);
  
  glEnd();

  blurVertexShaderX.disableProfile();
  blurFragmentShader.disableProfile();
 
  /* On récupère le premier blur dans la texture */
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, texblur);
  glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, width, height);
  
  /* Deuxième blur */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  blurVertexShaderY.setOffsetsArray();
  blurVertexShaderY.setModelViewProjectionMatrix();
  blurFragmentShader.setWeightsArray();
  blurFragmentShader.setTexture(texblur);
  blurVertexShaderY.enableShader();
  blurFragmentShader.enableShader();
  
  glColor3f(1.0,1.0,1.0);
  
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,0);
  glVertex3d(-1.0,1.0,0.0);
  
  glTexCoord2f(0,height);
  glVertex3d(-1.0,-1.0,0.0);
  
  glTexCoord2f(width,height);
  glVertex3d(1.0,-1.0,0.0);
  
  glTexCoord2f(width,0);
  glVertex3d(1.0,1.0,0.0);
  
  glEnd();
  
  blurVertexShaderY.disableProfile();
  blurFragmentShader.disableProfile();
  
  /* On récupère le deuxième blur dans la texture */
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, texblur);
  glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void GlowEngine::deactivate()
{
  pbuffer.Deactivate();
  glViewport (0, 0, width*scaleFactor, height*scaleFactor);
}

void GlowEngine::drawBlur(double alpha)
{
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(-1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glEnable(GL_TEXTURE_RECTANGLE_NV);
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, texblur);
  
  glColor4f(1.0,1.0,1.0, alpha);
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,height);
  glVertex3d(-1.0,1.0,0.0);
  
  glTexCoord2f(0,0);
  glVertex3d(-1.0,-1.0,0.0);

  glTexCoord2f(width,0);
  glVertex3d(1.0,-1.0,0.0);

  glTexCoord2f(width,height);
  glVertex3d(1.0,1.0,0.0);
  
  glEnd();

  glDisable(GL_TEXTURE_RECTANGLE_NV);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

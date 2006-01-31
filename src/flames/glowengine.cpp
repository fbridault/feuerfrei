#include "glowengine.hpp"


GlowEngine::GlowEngine(int w, int h, int scaleFactor, bool recompileShaders, CGcontext *cgcontext) : 
  m_pbuffer("rgb"), 
  m_blurVertexShaderX(_("glowShaders.cg"),_("vertGlowX"),  cgcontext, recompileShaders),
  m_blurVertexShaderY(_("glowShaders.cg"),_("vertGlowY"),  cgcontext, recompileShaders),
  m_blurFragmentShader(_("glowShaders.cg"),_("fragGlow"),  cgcontext, recompileShaders)
{
  m_scaleFactor=scaleFactor;
  m_width=w/m_scaleFactor;
  m_height=h/m_scaleFactor;

  // Initialiser le pbuffer
  m_pbuffer.Initialize(m_width, m_height, true, true);
  
  glGenTextures(1, &m_texblur);
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_texblur);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  
  //textest = new Texture("texture.jpg",GL_TEXTURE_RECTANGLE_NV);
}

GlowEngine::~GlowEngine()
{
}

void GlowEngine::activate()
{
  m_pbuffer.Activate();
  glViewport (0, 0, m_width, m_height);
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
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_texblur);
  glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, m_width, m_height);

  /* Premier blur */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_blurVertexShaderX.setOffsetsArray();
  m_blurVertexShaderX.setModelViewProjectionMatrix();
  m_blurFragmentShader.setWeightsArray();
  m_blurFragmentShader.setTexture(m_texblur);
  m_blurVertexShaderX.enableShader();
  m_blurFragmentShader.enableShader();
  
  glColor3f(1.0,1.0,1.0);
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,0);
  glVertex3d(-1.0,1.0,0.0);
  
  glTexCoord2f(0,m_height);
  glVertex3d(-1.0,-1.0,0.0);

  glTexCoord2f(m_width,m_height);
  glVertex3d(1.0,-1.0,0.0);

  glTexCoord2f(m_width,0);
  glVertex3d(1.0,1.0,0.0);
  
  glEnd();

  m_blurVertexShaderX.disableProfile();
  m_blurFragmentShader.disableProfile();
 
  /* On récupère le premier blur dans la texture */
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_texblur);
  glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, m_width, m_height);
  
  /* Deuxième blur */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  m_blurVertexShaderY.setOffsetsArray();
  m_blurVertexShaderY.setModelViewProjectionMatrix();
  m_blurFragmentShader.setWeightsArray();
  m_blurFragmentShader.setTexture(m_texblur);
  m_blurVertexShaderY.enableShader();
  m_blurFragmentShader.enableShader();
  
  glColor3f(1.0,1.0,1.0);
  
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,0);
  glVertex3d(-1.0,1.0,0.0);
  
  glTexCoord2f(0,m_height);
  glVertex3d(-1.0,-1.0,0.0);
  
  glTexCoord2f(m_width,m_height);
  glVertex3d(1.0,-1.0,0.0);
  
  glTexCoord2f(m_width,0);
  glVertex3d(1.0,1.0,0.0);
  
  glEnd();
  
  m_blurVertexShaderY.disableProfile();
  m_blurFragmentShader.disableProfile();
  
  /* On récupère le deuxième blur dans la texture */
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_texblur);
  glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, m_width, m_height);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void GlowEngine::deactivate()
{
  m_pbuffer.Deactivate();
  glViewport (0, 0, m_width*m_scaleFactor, m_height*m_scaleFactor);
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
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_texblur);
  
  glColor4f(1.0,1.0,1.0, alpha);
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,m_height);
  glVertex3d(-1.0,1.0,0.0);
  
  glTexCoord2f(0,0);
  glVertex3d(-1.0,-1.0,0.0);

  glTexCoord2f(m_width,0);
  glVertex3d(1.0,-1.0,0.0);

  glTexCoord2f(m_width,m_height);
  glVertex3d(1.0,1.0,0.0);
  
  glEnd();

  glDisable(GL_TEXTURE_RECTANGLE_NV);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

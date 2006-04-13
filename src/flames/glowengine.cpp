#include "glowengine.hpp"


GlowEngine::GlowEngine(unsigned int w, unsigned int h, int scaleFactor[GLOW_LEVELS], bool recompileShaders, CGcontext *cgcontext) : 
  m_blurVertexShaderX(_("glowShaders.cg"),_("vertGlowX"),  cgcontext, recompileShaders),
  m_blurVertexShaderY(_("glowShaders.cg"),_("vertGlowY"),  cgcontext, recompileShaders),
  m_blurFragmentShader(_("glowShaders.cg"),_("fragGlow"),  cgcontext, recompileShaders)
{
  m_initialWidth = w;
  m_initialHeight = h;
  
  for(int i=0; i < GLOW_LEVELS; i++){
    m_scaleFactor[i] = scaleFactor[i];
    m_width[i] = w/m_scaleFactor[i];
    m_height[i] = h/m_scaleFactor[i];
    
    m_firstPassFBOs[i].Initialize(m_width[i], m_height[i]);
    m_secondPassFBOs[i].Initialize(m_width[i], m_height[i]);
    m_firstPassTex[i] = new Texture(GL_TEXTURE_RECTANGLE_NV, m_width[i], m_height[i]);
    m_secondPassTex[i] = new Texture(GL_TEXTURE_RECTANGLE_NV, m_width[i], m_height[i]);
    m_firstPassFBOs[i].Attach(m_firstPassTex[i]->getTexture(), 0);
    m_secondPassFBOs[i].Attach(m_secondPassTex[i]->getTexture(), 0);
  }
}

GlowEngine::~GlowEngine()
{
}

void GlowEngine::activate()
{
   m_firstPassFBOs[0].Activate();
  /* On prend la résolution la plus grande */
  glViewport (0, 0, m_width[0], m_height[0]);
}

void GlowEngine::blur()
{
//   glDisable(GL_DEPTH_TEST);
  
  // Parametre de visualisation
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(-1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
      
  m_blurVertexShaderX.enableShader();
    
  m_blurVertexShaderX.setOffsetsArray();
  m_blurVertexShaderX.setModelViewProjectionMatrix();
  m_blurFragmentShader.enableShader();
  m_blurFragmentShader.setWeightsArray();
    
  m_blurFragmentShader.setTexture(m_firstPassTex[0]->getTexture());
    
  for(int i=0; i < GLOW_LEVELS; i++){
    /* Premier blur */
    m_secondPassFBOs[i].Activate();
    glViewport (0, 0, m_width[i], m_height[i]);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_QUADS);
    
    glTexCoord2f(0,0);
    glVertex3d(-1.0,1.0,0.0);
    
    glTexCoord2f(0,m_height[0]);
    glVertex3d(-1.0,-1.0,0.0);
    
    glTexCoord2f(m_width[0],m_height[0]);
    glVertex3d(1.0,-1.0,0.0);
    
    glTexCoord2f(m_width[0],0);
    glVertex3d(1.0,1.0,0.0);
    
    glEnd();  
  }
  m_blurVertexShaderX.disableProfile();
  
  m_blurVertexShaderY.setOffsetsArray();
  m_blurVertexShaderY.setModelViewProjectionMatrix();
  m_blurVertexShaderY.enableShader();
 
  for(int i=0; i < GLOW_LEVELS; i++){
   
    /* Deuxième blur */
    m_firstPassFBOs[i].Activate();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glViewport (0, 0, m_width[i], m_height[i]);
   
    m_blurFragmentShader.setTexture(m_secondPassTex[i]->getTexture());
 
    glColor3f(1.0,1.0,1.0);
   
    glBegin(GL_QUADS);
   
    glTexCoord2f(0,0);
    glVertex3d(-1.0,1.0,0.0);
   
    glTexCoord2f(0,m_height[i]);
    glVertex3d(-1.0,-1.0,0.0);
   
    glTexCoord2f(m_width[i],m_height[i]);
    glVertex3d(1.0,-1.0,0.0);
   
    glTexCoord2f(m_width[i],0);
    glVertex3d(1.0,1.0,0.0);
   
    glEnd();
  }
  m_blurVertexShaderY.disableProfile();
  m_blurFragmentShader.disableProfile();
  
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  
//   glEnable(GL_DEPTH_TEST);
}

void GlowEngine::deactivate()
{
  m_firstPassFBOs[GLOW_LEVELS-1].Deactivate();
   glViewport (0, 0, m_initialWidth, m_initialHeight);
}

void GlowEngine::drawBlur(double alpha)
{
  glDisable (GL_DEPTH_TEST);

  glBlendFunc (GL_ONE, GL_ONE);
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(-1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glEnable(GL_TEXTURE_RECTANGLE_NV);
  
  for(int i=0; i < GLOW_LEVELS; i++){
    m_firstPassTex[i]->bind();
    
    glColor4f(1.0,1.0,1.0,alpha);
    glBegin(GL_QUADS);
    
    glTexCoord2f(0,m_height[i]);
    glVertex3d(-1.0,1.0,0.0);
    
    glTexCoord2f(0,0);
    glVertex3d(-1.0,-1.0,0.0);
    
    glTexCoord2f(m_width[i],0);
    glVertex3d(1.0,-1.0,0.0);
    
    glTexCoord2f(m_width[i],m_height[i]);
    glVertex3d(1.0,1.0,0.0);
    
    glEnd();    
  }
  glDisable(GL_TEXTURE_RECTANGLE_NV);
  
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  
  glEnable (GL_DEPTH_TEST);
}

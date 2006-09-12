#include "DPengine.hpp"

DepthPeelingEngine::DepthPeelingEngine(uint width, uint height, uint nbLayers, Scene *scene, FireSource **flames, 
				       uint nbFlames, CGcontext *cgcontext )
{
  m_width = width;
  m_height = height;
  m_nbLayers = m_nbLayersMax = nbLayers;
  m_scene = scene;
  m_flames = flames;
  m_nbFlames = nbFlames;
  
  m_fbo.Initialize(m_width,m_height);
  
  char _fp_peel[] = 
    "!!ARBfp1.0\n"
    "OPTION ARB_fragment_program_shadow;\n"
    "PARAM c[5] = { program.local[0..4] };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "TEMP R2;\n"
    "TEX R0, fragment.texcoord[0], texture[0], 2D\n;"
    "TEX R1.x, fragment.position, texture[1], SHADOWRECT;\n"
    "TEX R2.x, fragment.position, texture[2], SHADOWRECT;\n"
    // KIL supprime le fragment si une des composantes du vecteur est nulle
    // On place donc le résultat de {0,1} en {-.5;.5}
    "ADD R1.x, R1.x, -0.5;\n"
    "KIL R1.x;\n"
    "ADD R2.x, R2.x, -0.5;\n"
    "KIL R2.x;\n"
//     "SUB R2.a, 1, R0.a;\n"
//     "MUL R1, R2.a, fragment.color;\n"
   "MUL R0, R0.a, R0;\n"
//     "ADD result.color, R0, fragment.color;\n"
//     "MUL R1, R0, c[1];\n"
//     "ADD R0, R0, fragment.color;\n"
    "MOV result.color, R0;\n"
    "END\n";
  
  m_peelProgram.load(_fp_peel);
  
  m_colorTexMain = new Texture(GL_TEXTURE_RECTANGLE_ARB,m_width,m_height);
  m_colorTexSec = new Texture(GL_TEXTURE_RECTANGLE_ARB,m_width,m_height);
  m_depthTex[0] = new Texture(m_width,m_height,GL_GREATER,true);
  m_depthTex[1] = new Texture(m_width,m_height,GL_GREATER,true);
  m_depthTex[2] = new Texture(m_width,m_height,GL_ALWAYS,true);
  m_sceneDepthTex = new Texture(m_width,m_height,GL_LESS,true);
  
  m_flamesDisplayList = glGenLists(1);
}

DepthPeelingEngine::~DepthPeelingEngine()
{
  delete m_colorTexMain;
  delete m_colorTexSec;
  delete m_depthTex[0];
  delete m_depthTex[1];
  delete m_depthTex[2];
  delete m_sceneDepthTex;
}
  
void DepthPeelingEngine::makePeels(bool displayParticles)
{
  uint f,l;
  
  m_fbo.Activate();
  
  /* On désactive l'écriture dans le color buffer */
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  /* On stocke la profondeur de la scène dans une texture qui servira */
  /* comme deuxième test de profondeur pour le depth peeling */
  /* Il y a donc en tout trois tests de profondeur */
  m_fbo.DepthAttach(m_sceneDepthTex->getTexture());
  
  glClear(GL_DEPTH_BUFFER_BIT);
  
  m_scene->drawSceneWT();
  
  glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
  glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
  
  m_curDepthTex = 0;
    
  glActiveTextureARB(GL_TEXTURE1_ARB);
  m_sceneDepthTex->bind();
  m_peelProgram.enableShader();
    
  for(l=0; l <= m_nbLayers; l++){
//     glDisable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ZERO);
    
    // On effectue le rendu dans le FBO
    m_fbo.DepthAttach(m_depthTex[m_curDepthTex]->getTexture());    
    
    glClear(GL_DEPTH_BUFFER_BIT);
    /* Pour le premier layer, on construit la display list */
    /* et le premier test de profondeur est toujours vrai */
    if(!l){
      m_fbo.ColorAttach(m_colorTexMain->getTexture(),0);
      glClear(GL_COLOR_BUFFER_BIT);
      /* Dessin de la flamme */
      glActiveTextureARB(GL_TEXTURE2_ARB);
      m_depthTex[2]->bind();
      
      glNewList(m_flamesDisplayList,GL_COMPILE_AND_EXECUTE);
      for (f = 0; f < m_nbFlames; f++)
	m_flames[f]->drawFlame (displayParticles);
      glEndList();
    }else{
      m_fbo.ColorAttach(m_colorTexSec->getTexture(),0);
      glClear(GL_COLOR_BUFFER_BIT);
//       m_peelProgram.setParameter4d(0.5);
      /* Pour les layers > 0, le premier test de profondeur est effectué avec */
      /* la profondeur de la passe précédente */
      glActiveTextureARB(GL_TEXTURE2_ARB);
      m_depthTex[1-m_curDepthTex]->bind();
      glCallList(m_flamesDisplayList);
      
      m_peelProgram.disableShader();
      
      m_fbo.ColorAttach(m_colorTexMain->getTexture(),0);
      glBlendFunc(GL_ONE,GL_ONE);
      
      glDisable (GL_DEPTH_TEST);
  
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      gluOrtho2D(-1, 1, -1, 1);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      glEnable(GL_TEXTURE_RECTANGLE_ARB);
      
      m_colorTexSec->bind();  
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
      
      glDisable(GL_TEXTURE_RECTANGLE_ARB);
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      
      glEnable (GL_DEPTH_TEST);
      m_peelProgram.enableShader();
    }
    m_curDepthTex = 1 - m_curDepthTex;
  }
  m_peelProgram.disableShader();
  m_fbo.Deactivate();
  
  glDeleteLists(m_flamesDisplayList,1);
}

void DepthPeelingEngine::render()
{
  glDisable (GL_DEPTH_TEST);
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(-1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glEnable(GL_TEXTURE_RECTANGLE_ARB);

  m_colorTexMain->bind();
  
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
  
  glDisable(GL_TEXTURE_RECTANGLE_ARB);
  
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  
  glEnable (GL_DEPTH_TEST);
}

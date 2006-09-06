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
    // Pose problème si dessin de la scène
    //     "MUL R0, R0, fragment.color;\n"
    "MUL result.color, R0, c[1];\n"
    //     "MOV result.color, fragment.color;\n"
    "END\n";
  
  m_peelProgram.load(_fp_peel);
  m_colorTex = new Texture(GL_TEXTURE_RECTANGLE_ARB,m_width,m_height);
  m_depthTex[0] = new Texture(m_width,m_height,GL_GREATER,true);
  m_depthTex[1] = new Texture(m_width,m_height,GL_GREATER,true);
  m_depthTex[2] = new Texture(m_width,m_height,GL_ALWAYS,true);
  m_sceneDepthTex = new Texture(m_width,m_height,GL_LESS,true);
  
  m_flamesDisplayList = glGenLists(1);
}

DepthPeelingEngine::~DepthPeelingEngine()
{
  delete m_colorTex;
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
  
  glBlendFunc (GL_ONE, GL_ZERO);

  m_fbo.ColorAttach(m_colorTex->getTexture(),0);
  
  glActiveTextureARB(GL_TEXTURE2_ARB);
  m_sceneDepthTex->bind();
  m_peelProgram.enableShader();
  
  for(l=0; l <= m_nbLayers; l++){
    // On effectue le rendu dans le FBO
    m_fbo.DepthAttach(m_depthTex[m_curDepthTex]->getTexture());
    
    glClear(GL_DEPTH_BUFFER_BIT);
    
    /* Pour le premier layer, on construit la display list */
    /* et le premier test de profondeur est toujours vrai */
    glActiveTextureARB(GL_TEXTURE1_ARB);
    if(!l){
      glClear(GL_COLOR_BUFFER_BIT);
      m_depthTex[2]->bind();
      /* Dessin de la flamme */
      m_peelProgram.setParameter4d(.9);
      glNewList(m_flamesDisplayList,GL_COMPILE_AND_EXECUTE);
      for (f = 0; f < m_nbFlames; f++)
	m_flames[f]->drawFlame (displayParticles);
      glEndList();
    }else{
      glBlendFunc (GL_SRC_ALPHA, GL_ONE);
      m_peelProgram.setParameter4d(0.5);
      /* Pour les layers > 0, le premier test de profondeur est effectué avec */
      /* la profondeur de la passe précédente */
      m_depthTex[1-m_curDepthTex]->bind();
      glCallList(m_flamesDisplayList);
    }
    m_curDepthTex = 1 - m_curDepthTex;
  }
  m_peelProgram.disableShader();
  m_fbo.Deactivate();
  
  glDeleteLists(m_flamesDisplayList,1);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void DepthPeelingEngine::render()
{
  glDisable (GL_DEPTH_TEST);
  
  glBlendFunc (GL_SRC_ALPHA, GL_ONE);
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(-1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glEnable(GL_TEXTURE_RECTANGLE_ARB);
    
  m_colorTex->bind();
  
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

#include "DPengine.hpp"

DepthPeelingEngine::DepthPeelingEngine(uint width, uint height, uint nbLayers, const Scene* const scene, FireSource** const flames, 
				       uint nbFlames )
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
    "MOV result.color, R0;\n"
    "END\n";
  
  m_peelProgram.load(_fp_peel);
  m_colorTex = new Texture*[m_nbLayers+1];
  for(uint i=0; i <= m_nbLayers; i++)
    m_colorTex[i] = new Texture(GL_TEXTURE_RECTANGLE_ARB, GL_NEAREST, m_width, m_height);
  m_depthTex[0] = new Texture(m_width, m_height, GL_GREATER, true);
  m_depthTex[1] = new Texture(m_width, m_height, GL_GREATER, true);
  m_depthTex[2] = new Texture(m_width, m_height, GL_ALWAYS, true);
  m_sceneDepthTex = new Texture(m_width, m_height, GL_LESS, true);
  
  m_flamesDisplayList = glGenLists(1);
}

DepthPeelingEngine::~DepthPeelingEngine()
{
  for(uint i=0; i <= m_nbLayers; i++)
    delete m_colorTex[i];
  delete [] m_colorTex;
  delete m_depthTex[0];
  delete m_depthTex[1];
  delete m_depthTex[2];
  delete m_sceneDepthTex;
}
  
void DepthPeelingEngine::makePeels(bool displayFlames, bool displayParticles, bool displayBoundingSphere)
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
  
  glBlendFunc (GL_SRC_ALPHA, GL_ZERO);
  for(l=0; l <= m_nbLayers; l++){
    // On effectue le rendu dans le FBO
    m_fbo.ColorAttach(m_colorTex[l]->getTexture(),0);
    m_fbo.DepthAttach(m_depthTex[m_curDepthTex]->getTexture());
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glActiveTextureARB(GL_TEXTURE2_ARB);
    m_sceneDepthTex->bind();
    m_peelProgram.enableShader();
    
    /* Pour le premier layer, on construit la displayt list */
    /* et le premier test de profondeur est toujours vrai */
    glActiveTextureARB(GL_TEXTURE1_ARB);
    if(l == 0){
      m_depthTex[2]->bind();
      /* Dessin de la flamme */
      glNewList(m_flamesDisplayList,GL_COMPILE_AND_EXECUTE);
      for (f = 0; f < m_nbFlames; f++)
	m_flames[f]->drawFlame (displayFlames, displayParticles, displayBoundingSphere);
      glEndList();
    }else{
      /* Pour les layers > 0, le premier test de profondeur est effectué avec */
      /* la profondeur de la passe précédente */
      m_depthTex[1-m_curDepthTex]->bind();
      glCallList(m_flamesDisplayList);
    }
    m_peelProgram.disableShader();
    
    m_curDepthTex = 1 - m_curDepthTex;
  }
  m_fbo.Deactivate();
  
  glDeleteLists(m_flamesDisplayList,1);
}

void DepthPeelingEngine::render()
{   
  glShadeModel (GL_FLAT);
  glDisable (GL_DEPTH_TEST);
  
  glBlendFunc (GL_ONE, GL_ONE);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(-1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glEnable(GL_TEXTURE_RECTANGLE_ARB);
  
//   for(int l=1; l <= m_nbLayers ; l--){
  for(int l=m_nbLayers; l >= 0 ; l--){
    
    m_colorTex[l]->drawOnScreen(m_width,m_height);
    
//     if(l==0)
//     glColor4f(1.0,1.0,1.0,1.0);
//     else      
//      glColor4f(1.0,1.0,1.0,0.6);
//     glColor4f(1.0,1.0,1.0,0.6);
  }
  glDisable(GL_TEXTURE_RECTANGLE_ARB);
  
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  
  glEnable (GL_DEPTH_TEST);
  glShadeModel (GL_SMOOTH);
}

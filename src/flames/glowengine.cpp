#include "glowengine.hpp"


GlowEngine::GlowEngine(uint w, uint h, uint scaleFactor[GLOW_LEVELS], bool recompileShaders)
{
  m_initialWidth = w;
  m_initialHeight = h;

  m_blurVertexShaderX8.load ("glowShaderX.vp", recompileShaders);
  m_blurVertexShaderY8.load ("glowShaderY.vp", recompileShaders);
  m_blurFragmentShader8.load("glowShader.fp", recompileShaders);

  m_programX.attachShader(m_blurVertexShaderX8);
  m_programX.attachShader(m_blurFragmentShader8);
  m_programY.attachShader(m_blurVertexShaderY8);
  m_programY.attachShader(m_blurFragmentShader8);
  
  m_programX.link();  
  m_programY.link();
//   m_blurVertexShaderX[0] = &m_blurVertexShaderX8;
//   m_blurVertexShaderY[0] = &m_blurVertexShaderY8;
//   m_blurFragmentShader[0] = &m_blurFragmentShader8;
//   m_blurVertexShaderX[1] = &m_blurVertexShaderX16;
//   m_blurVertexShaderY[1] = &m_blurVertexShaderY16;
//   m_blurFragmentShader[1] = &m_blurFragmentShader16;

  for(int j=0; j < FILTER_SIZE; j++)
    m_offsets[0][j] = j-FILTER_SIZE/2+1;
  
  for(int j=0; j < FILTER_SIZE; j++)
    m_offsets[1][j] = j-FILTER_SIZE*2+1;
  
  for(int j=0; j < FILTER_SIZE; j++)
    m_offsets[2][j] = j-FILTER_SIZE+1;
  
  for(int j=0; j < FILTER_SIZE; j++)
    m_offsets[3][j] = j;

  for(int j=0; j < FILTER_SIZE; j++)
    m_offsets[4][j] = j+FILTER_SIZE;
  
  setGaussSigma(0,2);
  setGaussSigma(1,10);
  setGaussSigma(2,10);
  setGaussSigma(3,10);
  setGaussSigma(4,10);

  for(int i=0; i < GLOW_LEVELS; i++){
    m_scaleFactor[i] = scaleFactor[i];
    m_width[i] = w/( !i ? m_scaleFactor[i] : m_scaleFactor[i]*2);
    m_height[i] = h/m_scaleFactor[i];
    
    m_firstPassFBOs[i].Initialize(m_width[i], m_height[i]);
    m_firstPassTex[i] = new Texture(GL_TEXTURE_RECTANGLE_ARB, GL_LINEAR, m_width[i], m_height[i]);
    m_firstPassFBOs[i].Activate();
    m_firstPassFBOs[i].ColorAttach(m_firstPassTex[i]->getTexture(), 0);
    m_firstPassFBOs[i].RenderBufferAttach();
    
    m_secondPassFBOs[i].Initialize(m_width[i], m_height[i]);
    m_secondPassTex[i] = new Texture(GL_TEXTURE_RECTANGLE_ARB, GL_LINEAR, m_width[i], m_height[i]);
    m_secondPassFBOs[i].Activate();
    m_secondPassFBOs[i].ColorAttach(m_secondPassTex[i]->getTexture(), 0);
    m_secondPassFBOs[i].RenderBufferAttach();
  }
//   m_visibilityFBO.Initialize(m_width[0], m_height[0]);
//   m_visibilityTex = new Texture(GL_TEXTURE_RECTANGLE_ARB, GL_LINEAR, m_width[0], m_height[0], true);
//   m_visibilityFBO.Activate();
//   m_visibilityFBO.ColorAttach(m_visibilityTex->getTexture(), 0);
//   m_visibilityFBO.RenderBufferAttach();
  m_secondPassFBOs[GLOW_LEVELS-1].Deactivate();
}

GlowEngine::~GlowEngine()
{
  for(int i=0; i < GLOW_LEVELS; i++){
    delete m_firstPassTex[i];
    delete m_secondPassTex[i];
  }
//   delete m_visibilityTex;
}

void GlowEngine::computeWeights(uint index, double sigma)
{
  int offset;
  //coef = 1/sqrt(2*PI*sigma);
  m_divide[index] = 0.0;
  
    /* Calcul des poids */
  switch(index){
  case 0:
    offset = FILTER_SIZE/2;
    for(int x=-offset+1 ; x<=offset-1 ; x++){
      m_weights[index][x+offset-1] = expf(-(x*x)/(sigma*sigma));
      m_divide[index] += m_weights[index][x+offset-1];
    }
    break;
  case 1:
    offset = FILTER_SIZE*2-1;
    for(int x=-FILTER_SIZE*2+1 ; x<= -FILTER_SIZE; x++){
      m_weights[index][x+offset] = expf(-((x/10.0)*(x/10.0))/(sigma*sigma));
      m_divide[index] += m_weights[index][x+offset];
//        cerr << x << " " << x+offset << " " << m_weights[index][x+offset] << endl;
    }
//     cerr << m_divide[index] << endl;
    break;
  case 2:
    offset = FILTER_SIZE-1;
    for(int x=-FILTER_SIZE+1 ; x<= 0; x++){
      m_weights[index][x+offset] = expf(-((x/10.0)*(x/10.0))/(sigma*sigma));
      m_divide[index] += m_weights[index][x+offset];
//        cerr << x << " " << x+offset << " " << m_weights[index][x+offset] << endl;
    }
//     cerr << m_divide[index] << endl;
    break;
  case 3:
    for(int x=0 ; x< FILTER_SIZE; x++){
      m_weights[index][x] = expf(-((x/10.0)*(x/10.0))/(sigma*sigma));
      m_divide[index] += m_weights[index][x];
//        cerr << x << " " << m_weights[index][x] << endl;
    }
//     cerr << m_divide[index] << endl;
    break;
  case 4:
    offset = FILTER_SIZE;
    for(int x= FILTER_SIZE ; x< FILTER_SIZE*2; x++){
      m_weights[index][x-offset] = expf(-((x/10.0)*(x/10.0))/(sigma*sigma));
      m_divide[index] += m_weights[index][x-offset];
//        cerr << x << " " << m_weights[index][x] << endl;
    }
//     cerr << m_divide[index] << endl;
    break;
  }
}

void GlowEngine::blur()
{
  uint shaderIndex;
  
  glDisable(GL_DEPTH_TEST);
//   glGetBooleanv(GL_LIGHTING,&params);
//   cerr << (params == GL_FALSE) << endl;
  glBlendFunc (GL_ONE, GL_ZERO);
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(-1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  /* Blur à la résolution de l'écran */
  m_programX.enable();
  m_programX.setUniform1fv("weights",(float *)m_weights[0],FILTER_SIZE);
  m_programX.setUniform1f("divide",m_divide[0]);
  m_programX.setUniform1fv("offsets",(float *)m_offsets[0],FILTER_SIZE);
  
  m_secondPassFBOs[0].Activate();
  glViewport (0, 0, m_width[0], m_height[0]);
  m_firstPassTex[0]->drawOnScreen(m_width[0], m_height[0]);
  
  m_programY.enable();
  m_programY.setUniform1fv("weights",(float *)m_weights[0],FILTER_SIZE);
  m_programY.setUniform1f("divide",m_divide[0]);
  m_programY.setUniform1fv("offsets",(float *)m_offsets[0],FILTER_SIZE);
  
  m_firstPassFBOs[0].Activate();
  glViewport (0, 0, m_width[0], m_height[0]);    
  m_secondPassTex[0]->drawOnScreen(m_width[0], m_height[0]);
  
  /* Blur à une résolution inférieure */  
  m_secondPassFBOs[1].Activate();
  m_programX.enable();
  glBlendColor(0.3,0.3,0.3,1.0);
  glBlendFunc (GL_CONSTANT_COLOR, GL_ONE);
  
  glViewport (0, 0, m_width[1], m_height[1]);
  glClear(GL_COLOR_BUFFER_BIT);
  
  /* Partie X [-bandwidth/2;-bandwidth/4] du filtre */
  m_programX.setUniform1fv("offsets",(float *)m_offsets[1],FILTER_SIZE);
  m_programX.setUniform1fv("weights",(float *)m_weights[1],FILTER_SIZE);
  m_programX.setUniform1f("divide",m_divide[1]);
//   drawTexOnScreen(m_width[0], m_height[0],m_firstPassTex[0]);
  m_firstPassTex[0]->drawOnScreen(m_width[0], m_height[0]);
  
  /* Partie X [-bandwidth/4;0] du filtre */
  m_programX.setUniform1fv("offsets",(float *)m_offsets[2],FILTER_SIZE);
  m_programX.setUniform1fv("weights",(float *)m_weights[2],FILTER_SIZE);
  m_programX.setUniform1f("divide",m_divide[2]);
//   drawTexOnScreen(m_width[0], m_height[0],m_firstPassTex[0]);
  m_firstPassTex[0]->drawOnScreen(m_width[0], m_height[0]);
  
  /* Partie X [0;bandwidth/4] du filtre */
  m_programX.setUniform1fv("offsets",(float *)m_offsets[3],FILTER_SIZE);
  m_programX.setUniform1fv("weights",(float *)m_weights[3],FILTER_SIZE);
  m_programX.setUniform1f("divide",m_divide[3]);
//   drawTexOnScreen(m_width[0], m_height[0],m_firstPassTex[0]);
  m_firstPassTex[0]->drawOnScreen(m_width[0], m_height[0]);
  
  /* Partie X [bandwidth/4;bandwidth/2] du filtre */
  m_programX.setUniform1fv("offsets",(float *)m_offsets[4],FILTER_SIZE);
  m_programX.setUniform1fv("weights",(float *)m_weights[4],FILTER_SIZE);
  m_programX.setUniform1f("divide",m_divide[4]);
  //   drawTexOnScreen(m_width[0], m_height[0],m_firstPassTex[0]);
  m_firstPassTex[0]->drawOnScreen(m_width[0], m_height[0]);
  
  glBlendFunc (GL_ONE, GL_ZERO);
  m_programY.enable();
  m_programY.setUniform1fv("offsets",(float *)m_offsets[0],FILTER_SIZE);
  m_programY.setUniform1fv("weights",(float *)m_weights[0],FILTER_SIZE);
  m_programY.setUniform1f("divide",m_divide[0]);
  
  m_firstPassFBOs[1].Activate();
  //glClear(GL_COLOR_BUFFER_BIT);
  
  //drawTexOnScreen(m_width[1], m_height[1],m_secondPassTex[1]);
  m_secondPassTex[1]->drawOnScreen(m_width[1], m_height[1]);
  
  m_programY.disable();
  
  //  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  m_firstPassFBOs[1].Deactivate();
  
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  
  glEnable(GL_DEPTH_TEST);
}

// void GlowEngine::blur()
// {
//   uint shaderIndex;
  
//   glDisable(GL_DEPTH_TEST);
  
//   glBlendFunc (GL_ONE, GL_ZERO);
  
//   glMatrixMode(GL_PROJECTION);
//   glPushMatrix();
//   glLoadIdentity();
//   gluOrtho2D(-1, 1, -1, 1);
//   glMatrixMode(GL_MODELVIEW);
//   glPushMatrix();
//   glLoadIdentity();
  
//   m_blurVertexShaderX8.enableShader();
//   m_blurFragmentShader8.enableShader();
//   /* Blurs en X */
//   for(int i=0; i < GLOW_LEVELS; i++){
//     /* On dessine dans le FBO #2,i avec comme source la texture résultante du FBO #1 */
//     m_blurVertexShaderX8.setM_OffsetsArray(m_offsets[i]);
//     m_blurFragmentShader8.setWeightsArray(m_weights[i],divide[i]);
    
//     m_secondPassFBOs[i].Activate();
//     glViewport (0, 0, m_width[i], m_height[i]);    
//     glClear(GL_COLOR_BUFFER_BIT);
    
//     m_firstPassTex[0]->drawTexOnScreen(m_width[0], m_height[0]);
//   }
//   m_blurVertexShaderX8.disableProfile();  
//   m_blurVertexShaderY8.enableShader();
//   for(int i=0; i < GLOW_LEVELS; i++){
//     /* Deuxième blur */
//     /* On dessine dans le FBO #1 avec comme source la texture résultante du FBO #2 */
//     m_blurVertexShaderY8.setOffsetsArray(m_offsets[i]);
//     m_blurFragmentShader8.setWeightsArray(m_weights[i],divide[i]);
    
//     m_firstPassFBOs[i].Activate();
//     glViewport (0, 0, m_width[i], m_height[i]);
//     glClear(GL_COLOR_BUFFER_BIT);
    
//     m_secondPassTex[i]->drawTexOnScreen(m_width[i], m_height[i]);
//   }
//   m_blurVertexShaderY8.disableProfile();
//   m_blurFragmentShader8.disableProfile();
  
//   glMatrixMode(GL_PROJECTION);
//   glPopMatrix();
//   glMatrixMode(GL_MODELVIEW);
//   glPopMatrix();
  
//   glEnable(GL_DEPTH_TEST);
// }

void GlowEngine::drawBlur()
{
  GLboolean params;
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
  
  for(int i=0; i < GLOW_LEVELS; i++)
    m_firstPassTex[i]->drawOnScreen(m_width[i], m_height[i]);
  //  m_firstPassTex[1]->drawTexOnScreen(m_width[1], m_height[1]);
  //m_visibilityTex->drawOnScreen(m_width[0], m_height[0]);
  glDisable(GL_TEXTURE_RECTANGLE_ARB);
  
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  
  glEnable (GL_DEPTH_TEST);
}

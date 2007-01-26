#include "glowengine.hpp"


GlowEngine::GlowEngine(uint w, uint h, uint scaleFactor[GLOW_LEVELS], bool recompileShaders, CGcontext *cgcontext) : 
  m_blurVertexShaderX8   (_("glowShaders.cg"),_("vertGlowX"),  cgcontext, recompileShaders),
  m_blurVertexShaderY8   (_("glowShaders.cg"),_("vertGlowY"),  cgcontext, recompileShaders),
  m_blurFragmentShader8  (_("glowShaders.cg"),_("fragGlow"),  cgcontext, recompileShaders)
//   m_blurVertexShaderX16  (_("glowShaders.cg"),_("vertGlowX16"),  cgcontext, recompileShaders),
//   m_blurVertexShaderY16  (_("glowShaders.cg"),_("vertGlowY16"),  cgcontext, recompileShaders),
//   m_blurFragmentShader16 (_("glowShaders.cg"),_("fragGlow16"),  cgcontext, recompileShaders)
{
  m_initialWidth = w;
  m_initialHeight = h;

//   m_blurVertexShaderX[0] = &m_blurVertexShaderX8;
//   m_blurVertexShaderY[0] = &m_blurVertexShaderY8;
//   m_blurFragmentShader[0] = &m_blurFragmentShader8;
//   m_blurVertexShaderX[1] = &m_blurVertexShaderX16;
//   m_blurVertexShaderY[1] = &m_blurVertexShaderY16;
//   m_blurFragmentShader[1] = &m_blurFragmentShader16;

  for(int j=0; j < FILTER_SIZE; j++)
    offsets[0][j] = j-FILTER_SIZE/2+1;
  
  for(int j=0; j < FILTER_SIZE; j++)
    offsets[1][j] = j-FILTER_SIZE*2+1;
  
  for(int j=0; j < FILTER_SIZE; j++)
    offsets[2][j] = j-FILTER_SIZE+1;
  
  for(int j=0; j < FILTER_SIZE; j++)
    offsets[3][j] = j;

  for(int j=0; j < FILTER_SIZE; j++)
    offsets[4][j] = j+FILTER_SIZE;
  
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
}

GlowEngine::~GlowEngine()
{
}

void GlowEngine::computeWeights(uint index, double sigma)
{
  int offset;
  //coef = 1/sqrt(2*PI*sigma);
  divide[index] = 0.0;
  
    /* Calcul des poids */
  switch(index){
  case 0:
    offset = FILTER_SIZE/2;
    for(int x=-offset+1 ; x<=offset-1 ; x++){
      weights[index][x+offset-1] = expf(-(x*x)/(sigma*sigma));
      divide[index] += weights[index][x+offset-1];
    }
    break;
  case 1:
    offset = FILTER_SIZE*2-1;
    for(int x=-FILTER_SIZE*2+1 ; x<= -FILTER_SIZE; x++){
      weights[index][x+offset] = expf(-((x/10.0)*(x/10.0))/(sigma*sigma));
      divide[index] += weights[index][x+offset];
//        cerr << x << " " << x+offset << " " << weights[index][x+offset] << endl;
    }
//     cerr << divide[index] << endl;
    break;
  case 2:
    offset = FILTER_SIZE-1;
    for(int x=-FILTER_SIZE+1 ; x<= 0; x++){
      weights[index][x+offset] = expf(-((x/10.0)*(x/10.0))/(sigma*sigma));
      divide[index] += weights[index][x+offset];
//        cerr << x << " " << x+offset << " " << weights[index][x+offset] << endl;
    }
//     cerr << divide[index] << endl;
    break;
  case 3:
    for(int x=0 ; x< FILTER_SIZE; x++){
      weights[index][x] = expf(-((x/10.0)*(x/10.0))/(sigma*sigma));
      divide[index] += weights[index][x];
//        cerr << x << " " << weights[index][x] << endl;
    }
//     cerr << divide[index] << endl;
    break;
  case 4:
    offset = FILTER_SIZE;
    for(int x= FILTER_SIZE ; x< FILTER_SIZE*2; x++){
      weights[index][x-offset] = expf(-((x/10.0)*(x/10.0))/(sigma*sigma));
      divide[index] += weights[index][x-offset];
//        cerr << x << " " << weights[index][x] << endl;
    }
//     cerr << divide[index] << endl;
    break;
  }
}

void GlowEngine::activate()
{
  /* On dessine dans le FBO #1 */
  m_firstPassFBOs[0].Activate();
  /* On prend la résolution la plus grande */
  glViewport (0, 0, m_width[0], m_height[0]);
}

void GlowEngine::blur(uint i, uint isrc, CgBlurVertexShader& blurVertexShader, FBO& fbo, Texture* srcTex)
{
  blurVertexShader.enableShader();

  /* Premier blur */
  /* On dessine dans le FBO #2 avec comme source la texture résultante du FBO #1 */
  blurVertexShader.setOffsetsArray(offsets[i]);
  
  fbo.Activate();
  glViewport (0, 0, m_width[i], m_height[i]);    
  glClear(GL_COLOR_BUFFER_BIT);
  
  srcTex->drawOnScreen(m_width[isrc], m_height[isrc]);
  blurVertexShader.disableProfile();
}

void GlowEngine::blur()
{
  uint shaderIndex;
  
  glDisable(GL_DEPTH_TEST);
  
  glBlendFunc (GL_ONE, GL_ZERO);
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(-1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  m_blurFragmentShader8.enableShader();
  
  /* Blur à la résolution de l'écran */
  m_blurFragmentShader8.setWeightsArray(weights[0],divide[0]);
  blur(0,0,m_blurVertexShaderX8,m_secondPassFBOs[0],m_firstPassTex[0]);
  blur(0,0,m_blurVertexShaderY8,m_firstPassFBOs[0],m_secondPassTex[0]);
  
  /* Blur à une résolution inférieure */
  m_blurVertexShaderX8.enableShader();
  m_blurFragmentShader8.setWeightsArray(weights[1],divide[1]);
  m_blurVertexShaderX8.setOffsetsArray(offsets[1]);
  
  m_secondPassFBOs[1].Activate();
  glBlendColor(0.0,0.0,0.0,0.3);
  glBlendFunc (GL_CONSTANT_ALPHA, GL_ONE);
  
  glViewport (0, 0, m_width[1], m_height[1]);
  glClear(GL_COLOR_BUFFER_BIT);
  
  m_firstPassTex[0]->drawOnScreen(m_width[0], m_height[0]);
  
  m_blurFragmentShader8.setWeightsArray(weights[2],divide[2]);
  m_blurVertexShaderX8.setOffsetsArray(offsets[2]);
  m_firstPassTex[0]->drawOnScreen(m_width[0], m_height[0]);

  m_blurFragmentShader8.setWeightsArray(weights[3],divide[3]);
  m_blurVertexShaderX8.setOffsetsArray(offsets[3]);
  m_firstPassTex[0]->drawOnScreen(m_width[0], m_height[0]);

  m_blurFragmentShader8.setWeightsArray(weights[4],divide[4]);
  m_blurVertexShaderX8.setOffsetsArray(offsets[4]);
  m_firstPassTex[0]->drawOnScreen(m_width[0], m_height[0]);
  
  m_blurVertexShaderX8.disableProfile();
  
  glBlendFunc (GL_ONE, GL_ZERO);
  m_blurVertexShaderY8.enableShader();
  m_blurVertexShaderY8.setOffsetsArray(offsets[0]);
  m_blurFragmentShader8.setWeightsArray(weights[0],divide[0]);
  
  m_firstPassFBOs[1].Activate();
  glViewport (0, 0, m_width[1], m_height[1]);
  glClear(GL_COLOR_BUFFER_BIT);
  
  //m_secondPassTex[0]->drawOnScreen(m_width[1], m_height[1]);
  m_secondPassTex[1]->drawOnScreen(m_width[1], m_height[1]);

  m_blurVertexShaderY8.disableProfile();  
  m_blurFragmentShader8.disableProfile();
  
  //  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
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
//     m_blurVertexShaderX8.setOffsetsArray(offsets[i]);
//     m_blurFragmentShader8.setWeightsArray(weights[i],divide[i]);
    
//     m_secondPassFBOs[i].Activate();
//     glViewport (0, 0, m_width[i], m_height[i]);    
//     glClear(GL_COLOR_BUFFER_BIT);
    
//     m_firstPassTex[0]->drawOnScreen(m_width[0], m_height[0]);
//   }
//   m_blurVertexShaderX8.disableProfile();  
//   m_blurVertexShaderY8.enableShader();
//   for(int i=0; i < GLOW_LEVELS; i++){
//     /* Deuxième blur */
//     /* On dessine dans le FBO #1 avec comme source la texture résultante du FBO #2 */
//     m_blurVertexShaderY8.setOffsetsArray(offsets[i]);
//     m_blurFragmentShader8.setWeightsArray(weights[i],divide[i]);
    
//     m_firstPassFBOs[i].Activate();
//     glViewport (0, 0, m_width[i], m_height[i]);
//     glClear(GL_COLOR_BUFFER_BIT);
    
//     m_secondPassTex[i]->drawOnScreen(m_width[i], m_height[i]);
//   }
//   m_blurVertexShaderY8.disableProfile();
//   m_blurFragmentShader8.disableProfile();
  
//   glMatrixMode(GL_PROJECTION);
//   glPopMatrix();
//   glMatrixMode(GL_MODELVIEW);
//   glPopMatrix();
  
//   glEnable(GL_DEPTH_TEST);
// }

void GlowEngine::deactivate()
{
  m_firstPassFBOs[GLOW_LEVELS].Deactivate();
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
  
  glEnable(GL_TEXTURE_RECTANGLE_ARB);
  
  for(int i=0; i < GLOW_LEVELS; i++)
    m_firstPassTex[i]->drawOnScreen(m_width[i], m_height[i]);
  //  m_firstPassTex[1]->drawOnScreen(m_width[1], m_height[1]);
  
  glDisable(GL_TEXTURE_RECTANGLE_ARB);
  
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  
  glEnable (GL_DEPTH_TEST);
}

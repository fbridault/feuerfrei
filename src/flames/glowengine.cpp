#include "glowengine.hpp"
#include "../interface/GLFlameCanvas.hpp"

GlowEngine::GlowEngine(uint w, uint h, uint scaleFactor[GLOW_LEVELS])
{
  m_initialWidth = w;
  m_initialHeight = h;

  m_blurFragmentShader8X.load("glowShaderX.fp");
  m_blurFragmentShader8Y.load("glowShaderY.fp");
  m_programX.attachShader(m_blurFragmentShader8X);
  m_programY.attachShader(m_blurFragmentShader8Y);  
  m_programX.link();
  m_programY.link();

  m_blurRendererShader.load("viewportSizedScaledTex.fp");
  m_blurRendererProgram.attachShader(m_blurRendererShader);
  m_blurRendererProgram.link();

  computeWeights(0,3);
  computeWeights(1,10);
  computeWeights(2,10);
  
  for(int i=0; i < GLOW_LEVELS; i++){
    m_scaleFactor[i] = scaleFactor[i];
    m_width[i] = w/m_scaleFactor[i];
    m_height[i] = h/m_scaleFactor[i];
    m_firstPassFBOs[i].Initialize();    
    m_secondPassFBOs[i].Initialize();
  }
  
  generateTex();
  
  /* Offsets centrés pour taille texture en entrée = taille texture en sortie */
  for(int j=0; j < FILTER_SIZE; j++)
    m_offsets[0][j] = j-FILTER_SIZE/2+1;
  /* Offsets centrés pour taille texture en entrée > taille texture en sortie */
  for(int j=0; j < FILTER_SIZE; j++){
    m_offsets[1][j] = (j-FILTER_SIZE/2+1)*(int)(m_scaleFactor[1]);
  }  
  m_secondPassFBOs[GLOW_LEVELS-1].Deactivate();
}

GlowEngine::~GlowEngine()
{
  deleteTex();
}

void GlowEngine::deleteTex()
{
  for(int i=0; i < GLOW_LEVELS; i++)
    {
      delete m_firstPassTex[i];
      delete m_secondPassTex[i];
    }
}

void GlowEngine::generateTex()
{  
  for(int i=0; i < GLOW_LEVELS; i++){
    m_width[i] = m_initialWidth/m_scaleFactor[i];
    m_height[i] = m_initialHeight/m_scaleFactor[i];
    
    m_firstPassFBOs[i].setSize(m_width[i], m_height[i]);
    m_firstPassTex[i] = new Texture(GL_TEXTURE_RECTANGLE_ARB, GL_LINEAR, m_width[i], m_height[i]);
    m_firstPassFBOs[i].Activate();
    m_firstPassFBOs[i].ColorAttach(m_firstPassTex[i]->getTexture(), 0);
    m_firstPassFBOs[i].RenderBufferAttach();
    
    m_secondPassFBOs[i].setSize(m_width[i], m_height[i]);
    m_secondPassTex[i] = new Texture(GL_TEXTURE_RECTANGLE_ARB, GL_LINEAR, m_width[i], m_height[i]);
    m_secondPassFBOs[i].Activate();
    m_secondPassFBOs[i].ColorAttach(m_secondPassTex[i]->getTexture(), 0);
    m_secondPassFBOs[i].RenderBufferAttach();
  }
}

void GlowEngine::computeWeights(uint index, float sigma)
{
  int offset;
  //coef = 1/sqrt(2*PI*sigma);
  m_divide[index] = 0.0f;
  
    /* Calcul des poids */
  switch(index){
  case 0:
    offset = FILTER_SIZE/2;
    for(int x=-offset+1 ; x<=offset-1 ; x++){
      m_weights[index][x+offset-1] = expf(-(x*x)/(sigma*sigma));
      m_divide[index] += m_weights[index][x+offset-1];
//       cerr << x << " " << x+offset << " " << m_weights[index][x+offset-1] << endl;
    }
//     cerr << m_divide[index] << endl;
    m_divide[index] = 1/m_divide[index];
    break;
  case 1:
    offset = FILTER_SIZE/2;
    for(int x=-offset+1 ; x<=offset-1 ; x++){
      m_weights[index][x+offset-1] = expf(-(x*x)/(sigma*sigma));
      m_divide[index] += m_weights[index][x+offset-1];
//       cerr << x << " " << x+offset << " " << m_weights[index][x+offset-1] << endl;
    }
//     cerr << m_divide[index] << endl;
    m_divide[index] = 1/m_divide[index];
    break;
//   case 2:
//     /* Pour la partie gauche d'un filtre */
//     offset = FILTER_SIZE-1;
//     for(int x=-FILTER_SIZE+1 ; x<= 0; x++){
//       m_weights[index][x+offset] = expf(-((x/10.0f)*(x/10.0f))/(sigma*sigma));
//       m_divide[index] += m_weights[index][x+offset];
// //        cerr << x << " " << x+offset << " " << m_weights[index][x+offset] << endl;
//     }
//     m_divide[index] = 1/m_divide[index];
// //     cerr << m_divide[index] << endl;
//     break;
//   case 3:
//     /* Pour la partie droite d'un filtre */
//       for(int x=0 ; x< FILTER_SIZE; x++){
//       m_weights[index][x] = expf(-((x/10.0f)*(x/10.0f))/(sigma*sigma));
//       m_divide[index] += m_weights[index][x];
// //        cerr << x << " " << m_weights[index][x] << endl;
//     }
//     m_divide[index] = 1/m_divide[index];
// //     cerr << m_divide[index] << endl;
//     break;
  }
}

void GlowEngine::blur(GLFlameCanvas* const glBuffer)
{
  glDepthFunc (GL_LEQUAL);
  
  /* Blur à la résolution de l'écran */
  m_programX.enable();
  m_programX.setUniform1fv("weights",m_weights[0],FILTER_SIZE);
  m_programX.setUniform1f("divide",m_divide[0]);
  m_programX.setUniform1f("scale",m_scaleFactor[0]);
  m_programX.setUniform1fv("offsets",m_offsets[0],FILTER_SIZE);
  
  m_secondPassFBOs[0].Activate();
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  m_firstPassTex[0]->bind();
  
  /** On dessine seulement les englobants des flammes pour indiquer à quel endroit effectuer le blur */
  glBuffer->drawFlamesBoundingBoxes();
  
  m_programY.enable();
  m_programY.setUniform1fv("weights",m_weights[0],FILTER_SIZE);
  m_programY.setUniform1f("divide",m_divide[0]);
  m_programY.setUniform1f("scale",m_scaleFactor[0]);
  m_programY.setUniform1fv("offsets",m_offsets[0],FILTER_SIZE);
  
  m_firstPassFBOs[0].Activate();
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  m_secondPassTex[0]->bind();
  glBuffer->drawFlamesBoundingBoxes();
  
  /* Blur à une résolution inférieure */
  m_secondPassFBOs[1].Activate();
  m_programX.enable();
  glBlendColor(0.8f,0.8f,0.8f,1.0f);
  glEnable (GL_BLEND);
  glBlendFunc (GL_CONSTANT_COLOR, GL_ONE);
  
  glViewport (0, 0, m_width[1], m_height[1]);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  
  /* Partie X du filtre */
  /* Attention, il faut prendre les offsets correspondants à la texture à la résolution normale */
  m_programX.setUniform1fv("offsets",m_offsets[1],FILTER_SIZE);
  m_programX.setUniform1fv("weights",m_weights[1],FILTER_SIZE);
  m_programX.setUniform1f("divide",m_divide[0]);
  m_programX.setUniform1f("scale",m_scaleFactor[1]);
  m_firstPassTex[0]->bind();
  glBuffer->drawFlamesBoundingBoxes();
  
  /* Partie Y du filtre */
  m_programY.enable();
  m_programY.setUniform1fv("offsets",m_offsets[0],FILTER_SIZE);
  m_programY.setUniform1fv("weights",m_weights[1],FILTER_SIZE);
  m_programY.setUniform1f("divide",m_divide[0]);
  m_programY.setUniform1f("scale",m_scaleFactor[0]);
  
  m_firstPassFBOs[1].Activate();
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  
  m_secondPassTex[1]->bind();
  glBuffer->drawFlamesBoundingBoxes();
  
  m_secondPassTex[1]->bind();
  glBuffer->drawFlamesBoundingBoxes();
  
  m_programY.disable();
  glDisable(GL_BLEND);
  
  m_firstPassFBOs[1].Deactivate();
  
  glDepthFunc (GL_LESS);
}

void GlowEngine::drawBlur(GLFlameCanvas* const glBuffer, bool glowOnly)
{
  glDepthFunc (GL_LEQUAL);
  glEnable (GL_BLEND);
  glBlendFunc (GL_ONE, GL_ONE);
  
  glActiveTextureARB(GL_TEXTURE0);
  glEnable(GL_TEXTURE_RECTANGLE_ARB);
  m_blurRendererProgram.enable();
  m_blurRendererProgram.setUniform1i("text",0);
  
  for(int i=glowOnly ? 1 : 0; i < GLOW_LEVELS; i++){
    m_blurRendererProgram.setUniform1f("scale",1/(float)m_scaleFactor[i]);
    m_firstPassTex[i]->bind();
    glBuffer->drawFlamesBoundingBoxes();
  }

  m_blurRendererProgram.disable();
  glDisable(GL_TEXTURE_RECTANGLE_ARB);
  glDepthFunc (GL_LESS);
  glDisable (GL_BLEND);
}

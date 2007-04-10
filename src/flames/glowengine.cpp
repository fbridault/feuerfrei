#include "glowengine.hpp"


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

  setGaussSigma(0,2);
  setGaussSigma(1,10);
  setGaussSigma(2,10);
  
  for(int i=0; i < GLOW_LEVELS; i++){
    m_scaleFactor[i] = scaleFactor[i];
    m_width[i] = w/m_scaleFactor[i];
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
  for(int j=0; j < FILTER_SIZE; j++)
    m_offsets[0][j] = j-FILTER_SIZE/2+1;
  
  for(int j=0; j < FILTER_SIZE; j++){
    m_offsets[1][j] = (j-FILTER_SIZE+1)*(int)(m_scaleFactor[1]);
//     cerr << m_offsets[1][j] << endl;
  }
  
  for(int j=0; j < FILTER_SIZE; j++){
    m_offsets[2][j] = j*(int)(m_scaleFactor[1]);
//     cerr << m_offsets[2][j] << endl;
  }
  
  for(int j=0; j < FILTER_SIZE; j++){
    m_offsets[3][j] = (j-FILTER_SIZE+1);
//     cerr << m_offsets[3][j] << endl;
  }
  
  for(int j=0; j < FILTER_SIZE; j++){
    m_offsets[4][j] = j;
//     cerr << m_offsets[4][j] << endl;
  }
  
  m_secondPassFBOs[GLOW_LEVELS-1].Deactivate();
}

GlowEngine::~GlowEngine()
{
  for(int i=0; i < GLOW_LEVELS; i++){
    delete m_firstPassTex[i];
    delete m_secondPassTex[i];
  }
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
//       cerr << x << " " << x+offset << " " << m_weights[index][x+offset-1] << endl;
    }
//     cerr << m_divide[index] << endl;
    m_divide[index] = 1/m_divide[index];
    break;
  case 1:
    offset = FILTER_SIZE-1;
    for(int x=-FILTER_SIZE+1 ; x<= 0; x++){
      m_weights[index][x+offset] = expf(-((x/10.0)*(x/10.0))/(sigma*sigma));
      m_divide[index] += m_weights[index][x+offset];
//        cerr << x << " " << x+offset << " " << m_weights[index][x+offset] << endl;
    }
    m_divide[index] = 1/m_divide[index];
//     cerr << m_divide[index] << endl;
    break;
  case 2:
    for(int x=0 ; x< FILTER_SIZE; x++){
      m_weights[index][x] = expf(-((x/10.0)*(x/10.0))/(sigma*sigma));
      m_divide[index] += m_weights[index][x];
//        cerr << x << " " << m_weights[index][x] << endl;
    }
    m_divide[index] = 1/m_divide[index];
//     cerr << m_divide[index] << endl;
    break;
  }
}

void GlowEngine::blur(vector <FireSource *>& flames)
{
  uint shaderIndex;
  
  glDepthFunc (GL_LEQUAL);
//   glGetBooleanv(GL_LIGHTING,&params);
//   cerr << (params == GL_FALSE) << endl;
  glBlendFunc (GL_ONE, GL_ZERO);
  
  /* Blur à la résolution de l'écran */
  m_programX.enable();
  m_programX.setUniform1fv("weights",m_weights[0],FILTER_SIZE);
  m_programX.setUniform1f("divide",m_divide[0]);
  m_programX.setUniform1f("scale",m_scaleFactor[0]);
  m_programX.setUniform1fv("offsets",m_offsets[0],FILTER_SIZE);
  
  m_secondPassFBOs[0].Activate();
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  /** On dessine seulement les englobants des flammes pour indiquer à quel endroit effectuer le blur */
  m_firstPassTex[0]->bind();
  
  for (vector < FireSource* >::iterator flamesIterator = flames.begin ();
       flamesIterator != flames.end (); flamesIterator++)
    (*flamesIterator)->drawFlame (true, false, true);
  
  m_programY.enable();
  m_programY.setUniform1fv("weights",m_weights[0],FILTER_SIZE);
  m_programY.setUniform1f("divide",m_divide[0]);
  m_programY.setUniform1f("scale",m_scaleFactor[0]);
  m_programY.setUniform1fv("offsets",m_offsets[0],FILTER_SIZE);
  
  m_firstPassFBOs[0].Activate();
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  m_secondPassTex[0]->bind();
  for (vector < FireSource* >::iterator flamesIterator = flames.begin ();
       flamesIterator != flames.end (); flamesIterator++)
    (*flamesIterator)->drawFlame (true, false, true);
  
  /* Blur à une résolution inférieure */
  m_secondPassFBOs[1].Activate();
  m_programX.enable();
  glBlendColor(0.6,0.6,0.6,1.0);
  glBlendFunc (GL_CONSTANT_COLOR, GL_ONE);
  
  glViewport (0, 0, m_width[1], m_height[1]);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  
  /* Partie X [-bandwidth/4;0] du filtre */
  m_programX.setUniform1fv("offsets",m_offsets[1],FILTER_SIZE);
  m_programX.setUniform1fv("weights",m_weights[1],FILTER_SIZE);
  m_programX.setUniform1f("divide",m_divide[1]);
  m_programX.setUniform1f("scale",m_scaleFactor[1]);
  //   drawTexOnScreen(m_width[0], m_height[0],m_firstPassTex[0]);
  m_firstPassTex[0]->bind();
  for (vector < FireSource* >::iterator flamesIterator = flames.begin ();
       flamesIterator != flames.end (); flamesIterator++)
    (*flamesIterator)->drawFlame (true, false, true);
  
  /* Partie X [0;bandwidth/4] du filtre */
  m_programX.setUniform1fv("offsets",m_offsets[2],FILTER_SIZE);
  m_programX.setUniform1fv("weights",m_weights[2],FILTER_SIZE);
  m_programX.setUniform1f("divide",m_divide[2]);
  m_programX.setUniform1f("scale",m_scaleFactor[1]);
  //   drawTexOnScreen(m_width[0], m_height[0],m_firstPassTex[0]);
  m_firstPassTex[0]->bind();
  for (vector < FireSource* >::iterator flamesIterator = flames.begin ();
       flamesIterator != flames.end (); flamesIterator++)
    (*flamesIterator)->drawFlame (true, false, true);
    
  /* Partie Y [-bandwidth/4;0] du filtre */
  m_programY.enable();
  m_programY.setUniform1fv("offsets",m_offsets[3],FILTER_SIZE);
  m_programY.setUniform1fv("weights",m_weights[1],FILTER_SIZE);
  m_programY.setUniform1f("divide",m_divide[1]);
  m_programY.setUniform1f("scale",m_scaleFactor[0]);
  
  m_firstPassFBOs[1].Activate();
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  
  //drawTexOnScreen(m_width[1], m_height[1],m_secondPassTex[1]);
  m_secondPassTex[1]->bind();
  for (vector < FireSource* >::iterator flamesIterator = flames.begin ();
       flamesIterator != flames.end (); flamesIterator++)
    (*flamesIterator)->drawFlame (true, false, true);
  
  /* Partie Y [0;bandwidth/4] du filtre */
  m_programY.setUniform1fv("offsets",m_offsets[4],FILTER_SIZE);
  m_programY.setUniform1fv("weights",m_weights[2],FILTER_SIZE);
  m_programY.setUniform1f("divide",m_divide[2]);
  m_programY.setUniform1f("scale",m_scaleFactor[0]);
  
  //drawTexOnScreen(m_width[1], m_height[1],m_secondPassTex[1]);
  m_secondPassTex[1]->bind();
  for (vector < FireSource* >::iterator flamesIterator = flames.begin ();
       flamesIterator != flames.end (); flamesIterator++)
    (*flamesIterator)->drawFlame (true, false, true);
  
  m_programY.disable();
  
  glBlendFunc (GL_ONE, GL_ZERO);
  //  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  m_firstPassFBOs[1].Deactivate();
  
//   glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LESS);
}

void GlowEngine::drawBlur(vector <FireSource *>& flames)
{
  glDepthFunc (GL_LEQUAL);
  glBlendFunc (GL_ONE, GL_ONE);
    
  glActiveTextureARB(GL_TEXTURE0);
  glEnable(GL_TEXTURE_RECTANGLE_ARB);
  m_blurRendererProgram.enable();
  m_blurRendererProgram.setUniform1i("text",0);
  
  for(int i=0; i < GLOW_LEVELS; i++){
    m_blurRendererProgram.setUniform1f("scale",1/(double)m_scaleFactor[i]);
    m_firstPassTex[i]->bind();
    for (vector < FireSource* >::iterator flamesIterator = flames.begin ();
	 flamesIterator != flames.end (); flamesIterator++)
      (*flamesIterator)->drawFlame (true, false, true);
  }
  //    m_firstPassTex[i]->drawOnScreen(m_width[i], m_height[i]);

  m_blurRendererProgram.disable();
  glDisable(GL_TEXTURE_RECTANGLE_ARB);
  glDepthFunc (GL_LESS);
}

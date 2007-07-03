#include "solidePhoto.hpp"

PhotometricSolidsRenderer::PhotometricSolidsRenderer(const Scene* const s, const vector <FireSource *> *flames)
{
  char macro[25];

  m_scene = s;
  m_flames = flames;

  sprintf(macro,"#define NBSOURCES %d\n",(int)m_flames->size());
  
  m_centers = new GLfloat[m_flames->size()*3];
  m_intensities = new GLfloat[m_flames->size()];
  m_lazimuth_lzenith = new GLfloat[m_flames->size()*2];
  
  m_SPVertexShaderTex.load("photoSolid.vp");
  m_SPFragmentShader[0].load("photoSolidOnly.fp", true,macro);
  m_SPFragmentShader[1].load("photoSolid.fp", true,macro);
  
  m_SPProgram1.attachShader(m_SPVertexShaderTex);
  m_SPProgram1.attachShader(m_SPFragmentShader[0]);
  m_SPProgram2.attachShader(m_SPVertexShaderTex);
  m_SPProgram2.attachShader(m_SPFragmentShader[1]);
  
  m_SPProgram1.link();  
  m_SPProgram2.link();
  
  generateTexture();
}

PhotometricSolidsRenderer::~PhotometricSolidsRenderer()
{
  delete [] m_centers;
  delete [] m_intensities;
  delete [] m_lazimuth_lzenith;
  delete m_photometricSolidsTex;
}

void PhotometricSolidsRenderer::generateTexture(void)
{
  uint tmp, sav, p;
  uint sizex, sizey;
  GLfloat *tex3DValues, *ptrTex;
  
  m_tex2DSize[0] = m_tex2DSize[1] = 0;
  /* Calcul de la taille maximale */
  for (vector < FireSource* >::const_iterator flamesIterator = m_flames->begin ();
       flamesIterator != m_flames->end (); flamesIterator++){
    if((*flamesIterator)->getIESAzimuthSize() > m_tex2DSize[0])
      m_tex2DSize[0] = (*flamesIterator)->getIESAzimuthSize();
    if((*flamesIterator)->getIESZenithSize() > m_tex2DSize[1])
      m_tex2DSize[1] = (*flamesIterator)->getIESZenithSize();
  }
  
  /* On prend la puissance de deux supérieure */
  sav = tmp = m_tex2DSize[0];
  p = 0;
  while(tmp > 0){
    tmp=tmp>>1; p++;  
  }
 m_tex2DSize[0] = (uint)pow(2,p-1);
  if(sav != m_tex2DSize[0])
    m_tex2DSize[0] = m_tex2DSize[0] << 1;
  
  sav = tmp = m_tex2DSize[1];
  p = 0;
  while(tmp > 0){
    tmp=tmp>>1; p++;  
  }
  m_tex2DSize[1] = (uint)pow(2,p-1);
  if(sav != m_tex2DSize[1])
    m_tex2DSize[1] = m_tex2DSize[1] << 1;
  
  cerr << "Texture 3D size : " << m_tex2DSize[0] << " " << m_tex2DSize[1] << endl;
  
  tex3DValues = new GLfloat[m_tex2DSize[0]*m_tex2DSize[1]*m_flames->size()];
  ptrTex = tex3DValues;
    
  for (vector < FireSource* >::const_iterator flamesIterator = m_flames->begin ();
       flamesIterator != m_flames->end (); flamesIterator++){
    GLfloat *values = (*flamesIterator)->getIntensities();
    
    sizex = (*flamesIterator)->getIESAzimuthSize();
    sizey = (*flamesIterator)->getIESZenithSize();
    
    /* On crée la texture 3D en remplissant de 0 là où on n'a pas de valeur */
    for(uint j=0; j < sizey; j++){
      
      for(uint i=0; i < sizex; i++)
	*ptrTex++ = *values++;
      for(uint i=sizex; i < m_tex2DSize[0]; i++)
	*ptrTex++ = 0;
      
    }
    
    for(uint j=sizey; j < m_tex2DSize[1]; j++)
      for(uint i=0; i < m_tex2DSize[0]; i++)
	*ptrTex++ = 0;      
  }
  
//    ptrTex = tex3DValues;
//    for(uint k=0; k < m_flames->size(); k++){
//      for(uint j=0; j < m_tex2DSize[1]; j++){
//        for(uint i=0; i < m_tex2DSize[0]; i++){
//  	cerr << *ptrTex++ << " ";
//        }
//        cerr << endl;
//      }
//      cerr << "Fin 2D" << endl;
//    }
  
  m_photometricSolidsTex = new Texture((GLsizei)m_tex2DSize[0], (GLsizei)m_tex2DSize[1], (GLsizei)m_flames->size(), tex3DValues);
  
  uint k=0;
  for (vector < FireSource* >::const_iterator flamesIterator = m_flames->begin ();
       flamesIterator != m_flames->end (); flamesIterator++, k++){
    /* On prend l'inverse pour éviter une division dans le shader et on divise par la taille */
    /* pour avoir sur l'intervalle [O:1] dans l'espace de la texture */
    /* Le calcul final d'un uv dans le shader est le suivant : */
    /* phi / m_lazimuth_lzenith[i].x / m_tex2Dsize[0] */
    /* Ce qui est simplifié grâce au traitement effectué ici en : */
    /* phi * m_lazimuth_lzenith[i].x */
    m_lazimuth_lzenith[k*2] = (1/(*flamesIterator)->getLazimut())/m_tex2DSize[0];
    m_lazimuth_lzenith[k*2+1] = (1/(*flamesIterator)->getLzenith())/m_tex2DSize[1];
//     cerr << m_lazimuth_lzenith[k*2] << " " << m_lazimuth_lzenith[k*2+1] << endl;
  }
  
  delete [] tex3DValues;
}

void PhotometricSolidsRenderer::draw(u_char color)
{
  uint k=0;
  /* Récupération des propriétés des flammes */
  for (vector < FireSource* >::const_iterator flamesIterator = m_flames->begin ();
       flamesIterator != m_flames->end (); flamesIterator++, k++)
    {
      (*flamesIterator)->computeIntensityPositionAndDirection();
      
      m_intensities[k] = (*flamesIterator)->getIntensity();
      (*flamesIterator)->getCenterSP(m_centers[k*3], m_centers[k*3+1], m_centers[k*3+2]);
    }
  
  /* Affichage du solide seul */
  if(!color){
    m_SPProgram1.enable();
    m_SPProgram1.setUniform3fv("centreSP", m_centers, m_flames->size());
    m_SPProgram1.setUniform1fv("fluctuationIntensite", m_intensities, m_flames->size());
    m_SPProgram1.setUniform2fv("angles",m_lazimuth_lzenith, m_flames->size());
    m_SPProgram1.setUniform1f("incr", m_flames->size() > 1 ? 1/(m_flames->size()-1) : 0.0f);
    
    glActiveTexture(GL_TEXTURE0);
    m_photometricSolidsTex->bind();
    m_SPProgram1.setUniform1i("textureSP",0);
    /* Dessin en enlevant toutes les textures si nécessaire */
    m_scene->drawSceneWT();
    m_SPProgram1.disable();
  }else{
    /* Affichage du solide modulé avec la couleur des objets  */
    m_SPProgram2.enable();
    m_SPProgram2.setUniform3fv("centreSP", m_centers, m_flames->size());
    m_SPProgram2.setUniform1fv("fluctuationIntensite", m_intensities, m_flames->size());
    m_SPProgram2.setUniform2fv("angles", m_lazimuth_lzenith, m_flames->size());
    m_SPProgram2.setUniform1f("incr", m_flames->size() > 1 ? 1/(m_flames->size()-1) : 0.0f);    
    m_SPProgram2.setUniform1i("isTextured",1);
    
    glActiveTexture(GL_TEXTURE1);
    m_photometricSolidsTex->bind();
    m_SPProgram2.setUniform1i("textureSP",1);
    m_SPProgram2.setUniform1i("textureObjet",0);
    m_scene->drawSceneTEX();
    m_SPProgram2.setUniform1i("isTextured",0);
    m_scene->drawSceneWTEX();
    m_SPProgram2.disable();
  }
}

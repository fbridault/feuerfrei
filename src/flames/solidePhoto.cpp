#include "solidePhoto.hpp"

PhotometricSolidsRenderer::PhotometricSolidsRenderer(Scene *s, FireSource **flames, uint nbFlames, CGcontext *context, bool recompileShaders) :
  m_SPVertexShaderTex(_("SolidePhotometriqueVP.cg"),_("vpSPTEX"),context,recompileShaders)
{
  m_scene = s;
  m_flames = flames;
  m_nbFlames = nbFlames;
  
  m_centers = new double[m_nbFlames*3];
  m_intensities = new double[m_nbFlames];
  m_lazimuth_lzenith = new double[m_nbFlames*2];
  
  m_SPFragmentShader[0] = new CgSPFragmentShader(_("SolidePhotometriqueFP.cg"),_("fpSPSeul"), m_nbFlames, context, 0, true);
  m_SPFragmentShader[1] = new CgSPFragmentShader(_("SolidePhotometriqueFP.cg"),_("fpSPTEX"), m_nbFlames, context, 2, true);
  
  generateTexture();
}

PhotometricSolidsRenderer::~PhotometricSolidsRenderer()
{
  for(ushort i=0; i < m_NBSHADER; i++)
    delete m_SPFragmentShader[i];
  delete [] m_centers;
  delete [] m_intensities;
  delete [] m_lazimuth_lzenith;
  delete m_photometricSolidsTex;
}

void PhotometricSolidsRenderer::generateTexture(void)
{
  uint tmp, sav, p;
  uint sizex, sizey;
  float *tex3DValues, *ptrTex;
  
  m_tex2DSize[0] = m_tex2DSize[1] = 0;
  /* Calcul de la taille maximale */
  for(uint k=0; k < m_nbFlames; k++){
    if(m_flames[k]->getIESAzimuthSize() > m_tex2DSize[0])
      m_tex2DSize[0] = m_flames[k]->getIESAzimuthSize();
    if(m_flames[k]->getIESZenithSize() > m_tex2DSize[1])
      m_tex2DSize[1] = m_flames[k]->getIESZenithSize();
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
  
  tex3DValues = new float[m_tex2DSize[0]*m_tex2DSize[1]*m_nbFlames];
  ptrTex = tex3DValues;
  
  for(uint k=0; k < m_nbFlames; k++){
    float *values = m_flames[k]->getIntensities();
    
    sizex = m_flames[k]->getIESAzimuthSize();
    sizey = m_flames[k]->getIESZenithSize();
    
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
  
//   ptrTex = tex3DValues;
//   for(uint k=0; k < m_nbFlames; k++){
//     for(uint j=0; j < m_tex2DSize[1]; j++){
//       for(uint i=0; i < m_tex2DSize[0]; i++){
// 	cerr << *ptrTex++ << " ";
//       }
//       cerr << endl;
//     }
//     cerr << "Fin 2D" << endl;
//   }
  
  m_photometricSolidsTex = new Texture((GLsizei)m_tex2DSize[0], (GLsizei)m_tex2DSize[1], (GLsizei)m_nbFlames, tex3DValues);
  
  m_SPFragmentShader[0]->SetTexture(m_photometricSolidsTex->getTexture());
  m_SPFragmentShader[1]->SetTexture(m_photometricSolidsTex->getTexture());
  
  for(uint k=0; k < m_nbFlames; k++){
    /* On prend l'inverse pour éviter une division et on divise par la taille */
    /* pour avoir sur l'intervalle [O:1] dans l'espace de la texture */
    /* Le calcul final d'un uv dans le shader est le suivant : */
    /* phi / m_lazimuth_lzenith[i].x / m_tex2Dsize[0] */
    /* Ce qui est simplifié grâce au traitement effectué ici en : */
    /* phi * m_lazimuth_lzenith[i].x */
    m_lazimuth_lzenith[k*2] = (1/m_flames[k]->getLazimut())/m_tex2DSize[0];
    m_lazimuth_lzenith[k*2+1] = (1/m_flames[k]->getLzenith())/m_tex2DSize[1];
//     cerr << m_lazimuth_lzenith[k*2] << " " << m_lazimuth_lzenith[k*2+1] << endl;
  }
  
  m_SPFragmentShader[0]->SetInitialParameters(m_lazimuth_lzenith);
  m_SPFragmentShader[1]->SetInitialParameters(m_lazimuth_lzenith);

  delete [] tex3DValues;
}

void PhotometricSolidsRenderer::draw(u_char color)
{
  /* Récupération des propriétés des flammes */
  for(uint k=0; k < m_nbFlames; k++){
    m_flames[k]->computeIntensityPositionAndDirection();
    
    m_intensities[k] = m_flames[k]->getIntensity();
    m_flames[k]->getCenterSP(m_centers[k*3], m_centers[k*3+1], m_centers[k*3+2]);
  }
  
  /* Affichage des objets sans couleur */
  if(color == 0){
    m_SPVertexShaderTex.setModelViewProjectionMatrix();
    m_SPVertexShaderTex.enableShader();
    m_SPFragmentShader[color]->enableShader(m_centers,m_intensities);
    m_scene->drawScene(m_SPVertexShaderTex);
    m_SPVertexShaderTex.disableProfile();
    m_SPFragmentShader[color]->disableProfile();
  }else{
    /* Affichage des objets avec textures */
    m_SPVertexShaderTex.setModelViewProjectionMatrix();
    m_SPVertexShaderTex.enableShader();
    m_SPFragmentShader[color]->enableShader(m_centers,m_intensities);
    m_SPFragmentShader[color]->setIsTextured(1);
    m_scene->drawSceneTEX();
    m_SPFragmentShader[color]->setIsTextured(0);
    m_scene->drawSceneWTEX(m_SPVertexShaderTex);
    m_SPVertexShaderTex.disableProfile();
    m_SPFragmentShader[color]->disableProfile();
  }
}

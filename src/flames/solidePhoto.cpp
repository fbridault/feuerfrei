#include "solidePhoto.hpp"

PixelLightingRenderer::PixelLightingRenderer(const Scene* const s, const vector <FireSource *> *flames)
{
  char macro[25];

  m_scene = s;
  m_flames = flames;

  sprintf(macro,"#define NBSOURCES %d\n",(int)m_flames->size());

  m_centers = new GLfloat[m_flames->size()*3];
  m_intensities = new GLfloat[m_flames->size()];
  m_lazimuth_lzenith = new GLfloat[m_flames->size()*2];

  m_SPVertexShaderTex.load("pixelLighting.vp");
  m_SPFragmentShader.load("pixelLighting.fp", true, macro);

  m_SPProgram.attachShader(m_SPVertexShaderTex);
  m_SPProgram.attachShader(m_SPFragmentShader);

  m_SPProgram.link();
}

PixelLightingRenderer::PixelLightingRenderer(const Scene* const s, const vector <FireSource *> *flames, bool dummy)
{
  m_scene = s;
  m_flames = flames;

  m_centers = new GLfloat[m_flames->size()*3];
  m_intensities = new GLfloat[m_flames->size()];
  m_lazimuth_lzenith = new GLfloat[m_flames->size()*2];

  m_SPVertexShaderTex.load("pixelLighting.vp");
}

PixelLightingRenderer::~PixelLightingRenderer()
{
  delete [] m_centers;
  delete [] m_intensities;
  delete [] m_lazimuth_lzenith;
}

void PixelLightingRenderer::draw(bool color)
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

  /* Affichage du solide modulé avec la couleur des objets  */
  m_SPProgram.enable();
  m_SPProgram.setUniform3fv("centreSP", m_centers, m_flames->size());
  m_SPProgram.setUniform1fv("fluctuationIntensite", m_intensities, m_flames->size());
  m_SPProgram.setUniform1i("isTextured",1);
  m_SPProgram.setUniform3f("lumTr", 0.0f, 0.0f, 0.0f);
  m_SPProgram.setUniform4f("scale", 1.0f, 1.0f, 1.0f, 1.0f);

  m_SPProgram.setUniform1i("textureObjet",0);
  m_scene->drawSceneTEX();
  m_SPProgram.setUniform1i("isTextured",0);
  m_scene->drawSceneWTEX(&m_SPProgram);
  m_SPProgram.disable();
}

/***************************************************************************************************************/
PhotometricSolidsRenderer::PhotometricSolidsRenderer(const Scene* const s, const vector <FireSource *> *flames) :
  PixelLightingRenderer(s, flames, true)
{
  char macro[25];

  sprintf(macro,"#define NBSOURCES %d\n",(int)m_flames->size());

  m_SPOnlyFragmentShader.load("photoSolidOnly.fp", true, macro);
  m_SPFragmentShader.load("photoSolid.fp", true, macro);

  m_SPProgram.attachShader(m_SPVertexShaderTex);
  m_SPProgram.attachShader(m_SPFragmentShader);
  m_SPOnlyProgram.attachShader(m_SPVertexShaderTex);
  m_SPOnlyProgram.attachShader(m_SPOnlyFragmentShader);

  m_SPProgram.link();
  m_SPOnlyProgram.link();

  generateTexture();
}

PhotometricSolidsRenderer::~PhotometricSolidsRenderer()
{
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

  cout << "Texture 3D size : " << m_tex2DSize[0] << " " << m_tex2DSize[1] << endl;

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

  m_photometricSolidsTex = new Texture3D((GLsizei)m_tex2DSize[0], (GLsizei)m_tex2DSize[1], (GLsizei)m_flames->size(), tex3DValues);

  uint k=0;
  for (vector < FireSource* >::const_iterator flamesIterator = m_flames->begin ();
       flamesIterator != m_flames->end (); flamesIterator++, k++){
    /* On prend l'inverse pour éviter une division dans le shader et on divise par la taille */
    /* pour avoir sur l'intervalle [O:1] dans l'espace de la texture */
    /* Le calcul final d'un uv dans le shader est le suivant : */
    /* phi / m_lazimuth_lzenith[i].x / m_tex2Dsize[0] */
    /* Ce qui est simplifié grâce au traitement effectué ici en : */
    /* phi * m_lazimuth_lzenith[i].x */
    m_lazimuth_lzenith[k*2] = (1/(*flamesIterator)->getLazimutTEX())/m_tex2DSize[0];
    m_lazimuth_lzenith[k*2+1] = (1/(*flamesIterator)->getLzenithTEX())/m_tex2DSize[1];
//     cerr << m_lazimuth_lzenith[k*2] << " " << m_lazimuth_lzenith[k*2+1] << endl;
  }

  delete [] tex3DValues;
}

void PhotometricSolidsRenderer::draw(bool color)
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
    m_SPOnlyProgram.enable();
    m_SPOnlyProgram.setUniform3fv("centreSP", m_centers, m_flames->size());
    m_SPOnlyProgram.setUniform1fv("fluctuationIntensite", m_intensities, m_flames->size());
    m_SPOnlyProgram.setUniform2fv("angles",m_lazimuth_lzenith, m_flames->size());
    m_SPOnlyProgram.setUniform1f("incr", m_flames->size() > 1 ? 1/(m_flames->size()-1) : 0.0f);
    m_SPOnlyProgram.setUniform3f("lumTr", 0.0f, 0.0f, 0.0f);
    m_SPOnlyProgram.setUniform3f("scale", 1.0f, 1.0f, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    m_photometricSolidsTex->bind();
    m_SPOnlyProgram.setUniform1i("textureSP",0);
    /* Dessin en enlevant toutes les textures si nécessaire */
    m_scene->drawSceneWT(&m_SPOnlyProgram);
    m_SPOnlyProgram.disable();
  }else{
    /* Affichage du solide modulé avec la couleur des objets  */
    m_SPProgram.enable();
    m_SPProgram.setUniform3fv("centreSP", m_centers, m_flames->size());
    m_SPProgram.setUniform1fv("fluctuationIntensite", m_intensities, m_flames->size());
    m_SPProgram.setUniform2fv("angles", m_lazimuth_lzenith, m_flames->size());
    m_SPProgram.setUniform1f("incr", m_flames->size() > 1 ? 1/(m_flames->size()-1) : 0.0f);
    m_SPProgram.setUniform1i("isTextured",1);
    m_SPProgram.setUniform3f("lumTr", 0.0f, 0.0f, 0.0f);
    m_SPProgram.setUniform4f("scale", 1.0f, 1.0f, 1.0f, 1.0f);

    glActiveTexture(GL_TEXTURE1);
    m_photometricSolidsTex->bind();
    m_SPProgram.setUniform1i("textureSP",1);
    m_SPProgram.setUniform1i("textureObjet",0);
    m_scene->drawSceneTEX();
    m_SPProgram.setUniform1i("isTextured",0);
    m_scene->drawSceneWTEX(&m_SPProgram);
    m_SPProgram.disable();
  }
}

#include "solidePhoto.hpp"

SolidePhotometrique::SolidePhotometrique(Scene *s, CGcontext *context, bool recompileShaders) :
  m_SPVertexShaderTex(_("SolidePhotometriqueVP.cg"),_("vpSPTEX"),context,recompileShaders),
  m_SPVertexShaderWTex(_("SolidePhotometriqueVP.cg"),_("vpSPWTEX"),context,recompileShaders)
{
  m_scene = s;
  m_orientationSPtheta = 0.0;
  
  m_ieslist.addIESFile("IES/test.ies");
  m_ieslist.addIESFile("IES/out111621PN.IES");
  m_ieslist.addIESFile("IES/out2518T1EF.IES");
  m_ieslist.addIESFile("IES/out8013H1EN.IES");
  
  m_SPFragmentShader[0] = new CgSPFragmentShader(_("SolidePhotometriqueFP.cg"),_("fpSPSeul"),
						 context,&m_ieslist,0,recompileShaders);
  m_SPFragmentShader[1] = new CgSPFragmentShader(_("SolidePhotometriqueFP.cg"),_("fpSPSeulInterpole"),
						 context,&m_ieslist,1,recompileShaders);
  m_SPFragmentShader[2] = new CgSPFragmentShader(_("SolidePhotometriqueFP.cg"),_("fpSPTEX"),
						 context,&m_ieslist,0,recompileShaders);
  m_SPFragmentShader[3] = new CgSPFragmentShader(_("SolidePhotometriqueFP.cg"),_("fpSPTestTEX"),
						 context,&m_ieslist,1,recompileShaders);
  m_SPFragmentShader[4] = new CgSPFragmentShader(_("SolidePhotometriqueFP.cg"),_("fpSPWTEX"),
						 context,&m_ieslist,0,recompileShaders);
  m_SPFragmentShader[5] = new CgSPFragmentShader(_("SolidePhotometriqueFP.cg"),_("fpSPTestWTEX"),
						 context,&m_ieslist,1,recompileShaders);
}

SolidePhotometrique::~SolidePhotometrique()
{
  for(int i=0; i < m_NBSHADER; i++)
    delete m_SPFragmentShader[i];
}

void 
SolidePhotometrique::draw(unsigned char color, unsigned char interpolation)
{
  unsigned char fragmentShaderIndex = color+interpolation;
  
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glLoadIdentity();
  glRotatef(m_orientationSPtheta,m_axeRotation.x,m_axeRotation.y,m_axeRotation.z);
  glMatrixMode(GL_MODELVIEW);
  /* Affichage des objets sans couleur */
  if(fragmentShaderIndex < 2){
    m_SPVertexShaderWTex.setModelViewProjectionMatrix();
    m_SPVertexShaderWTex.enableShader();
    m_SPFragmentShader[fragmentShaderIndex]->enableShader(&m_centreSP,m_fluctuationIntensite);
    m_scene->draw_scene(m_SPVertexShaderWTex);
    m_SPVertexShaderWTex.disableProfile();
    m_SPFragmentShader[fragmentShaderIndex]->disableProfile();
  }else{
    /* Affichage des objets avec textures */
    m_SPVertexShaderTex.setModelViewProjectionMatrix();
    m_SPVertexShaderTex.enableShader();
    m_SPFragmentShader[fragmentShaderIndex]->enableShader(&m_centreSP,m_fluctuationIntensite);
    m_scene->draw_sceneTEX();
    m_SPVertexShaderTex.disableProfile();
    m_SPFragmentShader[fragmentShaderIndex]->disableProfile();
    
    /* Affichage des objets sans texture */
    m_SPVertexShaderWTex.setModelViewProjectionMatrix();
    m_SPVertexShaderWTex.enableShader();
    m_SPFragmentShader[fragmentShaderIndex+2]->enableShader(&m_centreSP,m_fluctuationIntensite);
    m_scene->draw_sceneWTEX(m_SPVertexShaderWTex);
    m_SPVertexShaderWTex.disableProfile();
    m_SPFragmentShader[fragmentShaderIndex+2]->disableProfile();
  }
  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

void 
SolidePhotometrique::calculerFluctuationIntensiteCentreEtOrientation(Vector o, Point& p, double dim_y)
{
  double r,y;

  // l'intensité est calculée à partir du rapport de la longeur de la flamme (o)
  // et de la taille en y de la grille fois un coeff correcteur
  m_fluctuationIntensite=o.length()/dim_y * 5.0;
  
  // le centre du SP est la position de la flamme + la moitié du vecteur orientation
  // (orientation = vecteur position vers dernière particule)
  m_centreSP= p+(o/2.0);
  
  // l'axe de rotation est dans le plan x0z perpendiculaire aux coordonnées
  // de o projeté perpendiculairement dans ce plan
  m_axeRotation.set(-o.z,0.0,o.x);
  
  // l'angle de rotation theta est la coordonnée sphérique correspondante
  y=o.y;
  r = (double)o.length();
  if(r - fabs(y) < EPSILON)
    m_orientationSPtheta = 0.0;
  else
    m_orientationSPtheta=acos(y / r)*180.0/M_PI;
  
//   printf("theta : %g ",orientationSPtheta);
//  printf("fluctint : %g ",fluctuationIntensite);
//   p->afficher();
//   cout << " ";
//   o.afficher();
//   cout << " ";
//   centreSP->afficher();
//  cout << "\r";
//   axeRotation->afficher();
//  fflush(stdout);
}

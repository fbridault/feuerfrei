#include "solidePhoto.hpp"

SolidePhotometrique::SolidePhotometrique(CScene *s, CGcontext *context) :
  SPVertexShaderTex(_("SolidePhotometriqueVP.cg"),_("vpSPTEX"),context),
  SPVertexShaderWTex(_("SolidePhotometriqueVP.cg"),_("vpSPWTEX"),context)
{
  scene = s;
  orientationSPtheta = 0.0;
  
  ieslist.addIESFile("IES/test.ies");
  ieslist.addIESFile("IES/out111621PN.IES");
  ieslist.addIESFile("IES/out2518T1EF.IES");
  ieslist.addIESFile("IES/out8013H1EN.IES");
  
  SPFragmentShader[0] = new CgSPFragmentShader(_("SolidePhotometriqueFP.cg"),_("fpSPSeul"),context,&ieslist,0);
  SPFragmentShader[1] = new CgSPFragmentShader(_("SolidePhotometriqueFP.cg"),_("fpSPSeulInterpole"),context,&ieslist,1);
  SPFragmentShader[2] = new CgSPFragmentShader(_("SolidePhotometriqueFP.cg"),_("fpSPTEX"),context,&ieslist,0);
  SPFragmentShader[3] = new CgSPFragmentShader(_("SolidePhotometriqueFP.cg"),_("fpSPTestTEX"),context,&ieslist,1);
  SPFragmentShader[4] = new CgSPFragmentShader(_("SolidePhotometriqueFP.cg"),_("fpSPWTEX"),context,&ieslist,0);
  SPFragmentShader[5] = new CgSPFragmentShader(_("SolidePhotometriqueFP.cg"),_("fpSPTestWTEX"),context,&ieslist,1);
}

SolidePhotometrique::~SolidePhotometrique()
{
  delete [] SPFragmentShader;
}

void 
SolidePhotometrique::draw(unsigned char color, unsigned char interpolation)
{
  unsigned char fragmentShaderIndex = color+interpolation;
  
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glLoadIdentity();
  glRotatef(orientationSPtheta,axeRotation.getX(),axeRotation.getY(),axeRotation.getZ());
  
  /* Affichage des objets sans couleur */
  if(fragmentShaderIndex < 2){
    SPVertexShaderWTex.setModelViewProjectionMatrix();
    SPVertexShaderWTex.enableShader();
    SPFragmentShader[fragmentShaderIndex]->enableShader(&centreSP,fluctuationIntensite);
    scene->draw_scene();
    SPVertexShaderWTex.disableProfile();
    SPFragmentShader[fragmentShaderIndex]->disableProfile();
  }else{
    /* Affichage des objets avec textures */
    SPVertexShaderTex.setModelViewProjectionMatrix();
    SPVertexShaderTex.enableShader();
    SPFragmentShader[fragmentShaderIndex]->enableShader(&centreSP,fluctuationIntensite);
    scene->draw_sceneTEX();    
    SPVertexShaderTex.disableProfile();
    SPFragmentShader[fragmentShaderIndex]->disableProfile();
    
    /* Affichage des objets sans texture */
    SPVertexShaderWTex.setModelViewProjectionMatrix();
    SPVertexShaderWTex.enableShader();    
    SPFragmentShader[fragmentShaderIndex+2]->enableShader(&centreSP,fluctuationIntensite);    
    scene->draw_sceneWTEX();    
    SPVertexShaderWTex.disableProfile();
    SPFragmentShader[fragmentShaderIndex+2]->disableProfile();
  }
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

void
SolidePhotometrique::calculerFluctuationIntensiteCentreEtOrientation(CVector o,CPoint* p, double dim_y)
{
  float r,y;

  // l'intensité est calculée à partir du rapport de la longeur de la flamme (o)
  // et de la taille en y de la grille fois un coeff correcteur
  fluctuationIntensite=o.length()/dim_y * 3.0;
  
  // le centre du SP est la position de la flamme + la moitié du vecteur orientation
  // (orientation = vecteur position vers dernière particule)
  centreSP= *p+(o/2.0);

  // l'axe de rotation est dans le plan x0z perpendiculaire aux coordonnées
  // de o projeté perpendiculairement dans ce plan
  axeRotation.set(-o.getZ(),0.0,o.getX());
  
  // l'angle de rotation theta est la coordonnée sphérique correspondante
  y=o.getY();
  r = (float)o.length();
  if(r - fabs(y) < EPSILON)
    orientationSPtheta = 0.0;
  else
    orientationSPtheta=acos(y / r)*180.0/M_PI;

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

#ifndef SOLIDEPHOTO
#define SOLIDEPHOTO

class solidePhotometrique;

#include "header.h"
#include "ies.hpp"
#include "CgSPVertexShader.hpp"
#include "CgSPFragmentShader.hpp"
#include "scene.hpp"

/** Abstraction d'un solide photométrique.<br>
 * Cette classe permet de charger des fichiers IES et de les utiliser ensuite pour éclairer
 * une scène OpenGL en temps-réel. Elle propose d'afficher les données brutes du fichier en 
 * noir et blanc, ces mêmes données interpolées en arc de spirale, ou encore mélangées avec
 * la couleur des matériaux des objets
 */
class SolidePhotometrique
{
public:
  /** Constructeur par défaut.
   * @param s Pointeur vers la scène 3D
   * @param context Pointeur vers le contexte Cg
   */
  SolidePhotometrique(CScene *s, CGcontext *context);
  virtual ~SolidePhotometrique();

  /** Méthode de dessin de la scène en utilisant l'éclairage d'un solide photométrique
   * @param interpolation 0 ou 1 pour utiliser l'interpolation
   * @param color 0 ou 2 pour mélanger la luminance du solide avec la couleur des matériaux
   */
  void draw(unsigned char color, unsigned char interpolation);
  
  /** Calcul de l'intensité du centre et de l'orientation du solide photométrique
   * @param o Orientation du solide
   * @param p Centre du solide
   */
  void calculerFluctuationIntensiteCentreEtOrientation(CVector o,CPoint* p, double dim_y);

  /** Passe au fichier suivant dans la liste des fichiers IES */
  void swap()
  {
    ieslist.swap();
    for(int i=0; i < NBSHADER; i++)
      SPFragmentShader[i]->setTexture();
  };
private:
  /** Pointeur vers la scène 3D */
  CScene *scene;
  /** Centre du solide photométrique dans l'espace */
  CPoint centreSP;
  /** Orientation du solide photométrique, utilisée pour la rotation */
  double orientationSPtheta;
  /** Axe de rotation */
  CVector axeRotation;
  /** Valeur de l'intensité du solide */
  double fluctuationIntensite;
  /** Nombre de fragments shaders dans le tableau SPFragmentShader */
  const static int NBSHADER=6;
  /** Liste des fichiers IES */
  IESList ieslist;
  
  /** Vertex Shader pour les objets texturés */
  CgSPVertexShader SPVertexShaderTex;
  /** Vertex Shader pour les objets non texturés */
  CgSPVertexShader SPVertexShaderWTex;

  /** 
   * [0] : SP non interpolé sans couleur des objets fpSPSeul<br>
   * [1] : SP interpolé sans couleur des objets fpSPSeulInterpole<br>
   * [2] : SP non interpolé avec couleur pour les objets texturés fpSPTEX<br>
   * [3] : SP interpolé avec couleur pour les objets texturés fpSPTestTEX<br>
   * [4] : SP non interpolé avec couleur pour les objets non texturés fpSPWTEX<br>
   * [5] : SP interpolé avec couleur pour les objets non texturés fpSPTestWTEX
   */
  CgSPFragmentShader *SPFragmentShader[NBSHADER];
  
};

#endif

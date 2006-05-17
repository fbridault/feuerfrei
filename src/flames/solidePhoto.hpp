#ifndef SOLIDEPHOTO_H
#define SOLIDEPHOTO_H

class solidePhotometrique;

#include "flames.hpp"
#include "ies.hpp"
#include "../shaders/CgSPFragmentShader.hpp"
#include "../scene/scene.hpp"

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
  SolidePhotometrique(Scene *s, CGcontext *context, bool recompileShaders);
  virtual ~SolidePhotometrique();

  /** Méthode de dessin de la scène en utilisant l'éclairage d'un solide photométrique
   * @param interpolation 0 ou 1 pour utiliser l'interpolation
   * @param color 0 ou 2 pour mélanger la luminance du solide avec la couleur des matériaux
   */
  void draw(u_char color, u_char interpolation);
  
  /** Calcul de l'intensité du centre et de l'orientation du solide photométrique
   * @param o Orientation du solide
   * @param p Centre du solide
   */
  void calculerFluctuationIntensiteCentreEtOrientation(Vector o,Point& p, double dim_y);

  /** Passe au fichier suivant dans la liste des fichiers IES */
  void swap()
  {
    m_ieslist.swap();
    for(int i=0; i < m_NBSHADER; i++)
      m_SPFragmentShader[i]->setTexture();
  };
  
private:
  /** Pointeur vers la scène 3D */
  Scene *m_scene;
  /** Centre du solide photométrique dans l'espace */
  Point m_centreSP;
  /** Orientation du solide photométrique, utilisée pour la rotation */
  double m_orientationSPtheta;
  /** Axe de rotation */
  Vector m_axeRotation;
  /** Valeur de l'intensité du solide */
  double m_fluctuationIntensite;
  /** Nombre de fragments shaders dans le tableau SPFragmentShader */
  const static int m_NBSHADER=6;
  /** Liste des fichiers IES */
  IESList m_ieslist;
  
  /** Vertex Shader pour les objets texturés */
  CgBasicVertexShader m_SPVertexShaderTex;
  /** Vertex Shader pour les objets non texturés */
  CgBasicVertexShader m_SPVertexShaderWTex;
  
  /** 
   * [0] : SP non interpolé sans couleur des objets fpSPSeul<br>
   * [1] : SP interpolé sans couleur des objets fpSPSeulInterpole<br>
   * [2] : SP non interpolé avec couleur pour les objets texturés fpSPTEX<br>
   * [3] : SP interpolé avec couleur pour les objets texturés fpSPTestTEX<br>
   * [4] : SP non interpolé avec couleur pour les objets non texturés fpSPWTEX<br>
   * [5] : SP interpolé avec couleur pour les objets non texturés fpSPTestWTEX
   */
  CgSPFragmentShader *m_SPFragmentShader[m_NBSHADER];  
};

#endif

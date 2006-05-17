#ifndef SOLIDEPHOTO_H
#define SOLIDEPHOTO_H

class solidePhotometrique;

#include "flames.hpp"
#include "ies.hpp"
#include "../shaders/CgSPFragmentShader.hpp"
#include "../scene/scene.hpp"

/** Abstraction d'un solide photom�trique.<br>
 * Cette classe permet de charger des fichiers IES et de les utiliser ensuite pour �clairer
 * une sc�ne OpenGL en temps-r�el. Elle propose d'afficher les donn�es brutes du fichier en 
 * noir et blanc, ces m�mes donn�es interpol�es en arc de spirale, ou encore m�lang�es avec
 * la couleur des mat�riaux des objets
 */
class SolidePhotometrique
{
public:
  /** Constructeur par d�faut.
   * @param s Pointeur vers la sc�ne 3D
   * @param context Pointeur vers le contexte Cg
   */
  SolidePhotometrique(Scene *s, CGcontext *context, bool recompileShaders);
  virtual ~SolidePhotometrique();

  /** M�thode de dessin de la sc�ne en utilisant l'�clairage d'un solide photom�trique
   * @param interpolation 0 ou 1 pour utiliser l'interpolation
   * @param color 0 ou 2 pour m�langer la luminance du solide avec la couleur des mat�riaux
   */
  void draw(u_char color, u_char interpolation);
  
  /** Calcul de l'intensit� du centre et de l'orientation du solide photom�trique
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
  /** Pointeur vers la sc�ne 3D */
  Scene *m_scene;
  /** Centre du solide photom�trique dans l'espace */
  Point m_centreSP;
  /** Orientation du solide photom�trique, utilis�e pour la rotation */
  double m_orientationSPtheta;
  /** Axe de rotation */
  Vector m_axeRotation;
  /** Valeur de l'intensit� du solide */
  double m_fluctuationIntensite;
  /** Nombre de fragments shaders dans le tableau SPFragmentShader */
  const static int m_NBSHADER=6;
  /** Liste des fichiers IES */
  IESList m_ieslist;
  
  /** Vertex Shader pour les objets textur�s */
  CgBasicVertexShader m_SPVertexShaderTex;
  /** Vertex Shader pour les objets non textur�s */
  CgBasicVertexShader m_SPVertexShaderWTex;
  
  /** 
   * [0] : SP non interpol� sans couleur des objets fpSPSeul<br>
   * [1] : SP interpol� sans couleur des objets fpSPSeulInterpole<br>
   * [2] : SP non interpol� avec couleur pour les objets textur�s fpSPTEX<br>
   * [3] : SP interpol� avec couleur pour les objets textur�s fpSPTestTEX<br>
   * [4] : SP non interpol� avec couleur pour les objets non textur�s fpSPWTEX<br>
   * [5] : SP interpol� avec couleur pour les objets non textur�s fpSPTestWTEX
   */
  CgSPFragmentShader *m_SPFragmentShader[m_NBSHADER];  
};

#endif

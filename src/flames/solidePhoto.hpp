#ifndef SOLIDEPHOTO_H
#define SOLIDEPHOTO_H

class PhotometricSolidsRenderer;

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
class PhotometricSolidsRenderer
{
public:
  /** Constructeur par défaut.
   * @param s Pointeur vers la scène 3D.
   * @param flames Tableau contenant les flammes.
   * @param nbFlames Nombre de flammes dans le tableau.
   * @param context Pointeur vers le contexte Cg.
   * @param recompileShaders Booléne indiquant s'il faut recompiler ou non les shaders Cg.
   */
  PhotometricSolidsRenderer(Scene *s, FireSource **flames, uint nbFlames, CGcontext *context, bool recompileShaders);
  /** Destructeur. */
  virtual ~PhotometricSolidsRenderer();
  
  /** Méthode de dessin de la scène en utilisant l'éclairage d'un solide photométrique.
   * @param color 0 ou 2 pour mélanger la luminance du solide avec la couleur des matériaux.
   */
  void draw(u_char color);
  
  /** Efface la texture. */
  void deleteTexture(void){ delete m_photometricSolidsTex; };
  /** Générer une nouvelle texture pour le solide photométrique. */
  void generateTexture(void);

private:  
  /** Pointeur vers la scène 3D. */
  Scene *m_scene;
  /** Pointeur vers les flammes. */
  FireSource **m_flames;
  
  /** Nombre de flammes dans le tableau. */
  uint m_nbFlames;
  /** Nombre de fragments shaders dans le tableau SPFragmentShader. */
  const static int m_NBSHADER=2;
  
  /** Vertex Shader pour les objets texturés. */
  CgBasicVertexShader m_SPVertexShaderTex;
  
  /** Tableau contenant les fragment programs
   * [0] : SP non interpolé sans couleur des objets - fpSPSeul<br>
   * [1] : SP interpolé sans couleur des objets - fpSPSeulInterpole<br>
   * [2] : SP non interpolé avec couleur des objets - fpSPTEX<br>
   * [3] : SP interpolé avec couleur des objets - fpSPTestTEX<br>
   */
  CgSPFragmentShader *m_SPFragmentShader[m_NBSHADER];

  /** Texture 3D contenant les valeurs de luminance de tous les flammes. */
  Texture *m_photometricSolidsTex;

  /** Tableau contenant les centres de tous les solides. */
  double *m_centers;
  /** Tableau contenant les intensités de tous les solides. */
  double *m_intensities;
  /** Tableau contenant les valeurs de tous les solides. */
  double *m_lazimuth_lzenith;
  /** Tableau contenant la taille des textures 2D. */
  uint m_tex2DSize[2];
};

#endif

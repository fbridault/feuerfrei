#ifndef SOLIDEPHOTO_H
#define SOLIDEPHOTO_H

class PhotometricSolidsRenderer;

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
class PhotometricSolidsRenderer
{
public:
  /** Constructeur par d�faut.
   * @param s Pointeur vers la sc�ne 3D.
   * @param flames Tableau contenant les flammes.
   * @param nbFlames Nombre de flammes dans le tableau.
   * @param context Pointeur vers le contexte Cg.
   * @param recompileShaders Bool�ne indiquant s'il faut recompiler ou non les shaders Cg.
   */
  PhotometricSolidsRenderer(Scene *s, FireSource **flames, uint nbFlames, CGcontext *context, bool recompileShaders);
  /** Destructeur. */
  virtual ~PhotometricSolidsRenderer();
  
  /** M�thode de dessin de la sc�ne en utilisant l'�clairage d'un solide photom�trique.
   * @param color 0 ou 2 pour m�langer la luminance du solide avec la couleur des mat�riaux.
   */
  void draw(u_char color);
  
  /** Efface la texture. */
  void deleteTexture(void){ delete m_photometricSolidsTex; };
  /** G�n�rer une nouvelle texture pour le solide photom�trique. */
  void generateTexture(void);

private:  
  /** Pointeur vers la sc�ne 3D. */
  Scene *m_scene;
  /** Pointeur vers les flammes. */
  FireSource **m_flames;
  
  /** Nombre de flammes dans le tableau. */
  uint m_nbFlames;
  /** Nombre de fragments shaders dans le tableau SPFragmentShader. */
  const static int m_NBSHADER=2;
  
  /** Vertex Shader pour les objets textur�s. */
  CgBasicVertexShader m_SPVertexShaderTex;
  
  /** Tableau contenant les fragment programs
   * [0] : SP non interpol� sans couleur des objets - fpSPSeul<br>
   * [1] : SP interpol� sans couleur des objets - fpSPSeulInterpole<br>
   * [2] : SP non interpol� avec couleur des objets - fpSPTEX<br>
   * [3] : SP interpol� avec couleur des objets - fpSPTestTEX<br>
   */
  CgSPFragmentShader *m_SPFragmentShader[m_NBSHADER];

  /** Texture 3D contenant les valeurs de luminance de tous les flammes. */
  Texture *m_photometricSolidsTex;

  /** Tableau contenant les centres de tous les solides. */
  double *m_centers;
  /** Tableau contenant les intensit�s de tous les solides. */
  double *m_intensities;
  /** Tableau contenant les valeurs de tous les solides. */
  double *m_lazimuth_lzenith;
  /** Tableau contenant la taille des textures 2D. */
  uint m_tex2DSize[2];
};

#endif

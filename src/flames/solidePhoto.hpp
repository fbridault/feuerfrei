#ifndef SOLIDEPHOTO_H
#define SOLIDEPHOTO_H

class PixelLightingRenderer;
class PhotometricSolidsRenderer;

#include "flames.hpp"
#include "ies.hpp"
#include "../shaders/glsl.hpp"
#include "../scene/scene.hpp"

/** Abstraction d'un solide photométrique.<br>
 * Cette classe permet de charger des fichiers IES et de les utiliser ensuite pour éclairer
 * une scène OpenGL en temps-réel. Elle propose d'afficher les données brutes du fichier en 
 * noir et blanc, ces mêmes données interpolées en arc de spirale, ou encore mélangées avec
 * la couleur des matériaux des objets
 */
class PixelLightingRenderer
{
public:
  /** Constructeur par défaut.
   * @param s Pointeur vers la scène 3D.
   * @param flames Tableau contenant les flammes.
   * @param nbFlames Nombre de flammes dans le tableau.
   * @param context Pointeur vers le contexte Cg.
   * @param  Booléne indiquant s'il faut recompiler ou non les shaders Cg.
   */
  PixelLightingRenderer(const Scene* const s, const vector <FireSource *> *flames);
  PixelLightingRenderer(const Scene* const s, const vector <FireSource *> *flames, bool dummy);
  /** Destructeur. */
  virtual ~PixelLightingRenderer();
  
  /** Méthode de dessin de la scène en utilisant l'éclairage d'un solide photométrique.
   * @param color 0 ou 2 pour mélanger la luminance du solide avec la couleur des matériaux.
   */
  virtual void draw(bool color);
  
protected:  
  /** Pointeur vers la scène 3D. */
  const Scene* m_scene;
  /** Pointeur vers les flammes. */
  const vector <FireSource *> *m_flames;
  
  GLSLProgram m_SPProgram;
  /** Vertex Shader pour les objets texturés. */
  GLSLVertexShader m_SPVertexShaderTex;
  
  /** Tableau contenant le fragment program pour SP interpolé avec couleur des objets */
  GLSLFragmentShader m_SPFragmentShader;

  /** Tableau contenant les centres de tous les solides. */
  GLfloat *m_centers;
  /** Tableau contenant les intensités de tous les solides. */
  GLfloat *m_intensities;
  /** Tableau contenant les valeurs de tous les solides. */
  GLfloat *m_lazimuth_lzenith;
};

/** Abstraction d'un solide photométrique.<br>
 * Cette classe permet de charger des fichiers IES et de les utiliser ensuite pour éclairer
 * une scène OpenGL en temps-réel. Elle propose d'afficher les données brutes du fichier en 
 * noir et blanc, ces mêmes données interpolées en arc de spirale, ou encore mélangées avec
 * la couleur des matériaux des objets
 */
class PhotometricSolidsRenderer : public PixelLightingRenderer
{
public:
  /** Constructeur par défaut.
   * @param s Pointeur vers la scène 3D.
   * @param flames Tableau contenant les flammes.
   * @param nbFlames Nombre de flammes dans le tableau.
   * @param context Pointeur vers le contexte Cg.
   * @param  Booléne indiquant s'il faut recompiler ou non les shaders Cg.
   */
  PhotometricSolidsRenderer(const Scene* const s, const vector <FireSource *> *flames);
  /** Destructeur. */
  virtual ~PhotometricSolidsRenderer();
  
  /** Méthode de dessin de la scène en utilisant l'éclairage d'un solide photométrique.
   * @param color 0 ou 2 pour mélanger la luminance du solide avec la couleur des matériaux.
   */
  void draw(bool color);
  
  /** Efface la texture. */
  void deleteTexture(void){ delete m_photometricSolidsTex; };
  /** Générer une nouvelle texture pour le solide photométrique. */
  void generateTexture(void);
  
private:  
  /** Texture 3D contenant les valeurs de luminance de tous les flammes. */
  Texture *m_photometricSolidsTex;
  
  /** Tableau contenant le fragment program pour SP interpolé sans couleur des objets */
  GLSLFragmentShader m_SPOnlyFragmentShader;
  
  /** Tableau contenant la taille des textures 2D. */
  uint m_tex2DSize[2];
  
  GLSLProgram m_SPOnlyProgram;
};


#endif

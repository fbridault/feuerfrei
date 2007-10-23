#ifndef SOLIDEPHOTO_H
#define SOLIDEPHOTO_H

class PixelLightingRenderer;
class PhotometricSolidsRenderer;

#include "flames.hpp"
#include "ies.hpp"
#include "../shaders/glsl.hpp"
#include "../scene/scene.hpp"

/** Abstraction d'un solide photom�trique.<br>
 * Cette classe permet de charger des fichiers IES et de les utiliser ensuite pour �clairer
 * une sc�ne OpenGL en temps-r�el. Elle propose d'afficher les donn�es brutes du fichier en 
 * noir et blanc, ces m�mes donn�es interpol�es en arc de spirale, ou encore m�lang�es avec
 * la couleur des mat�riaux des objets
 */
class PixelLightingRenderer
{
public:
  /** Constructeur par d�faut.
   * @param s Pointeur vers la sc�ne 3D.
   * @param flames Tableau contenant les flammes.
   * @param nbFlames Nombre de flammes dans le tableau.
   * @param context Pointeur vers le contexte Cg.
   * @param  Bool�ne indiquant s'il faut recompiler ou non les shaders Cg.
   */
  PixelLightingRenderer(const Scene* const s, const vector <FireSource *> *flames);
  PixelLightingRenderer(const Scene* const s, const vector <FireSource *> *flames, bool dummy);
  /** Destructeur. */
  virtual ~PixelLightingRenderer();
  
  /** M�thode de dessin de la sc�ne en utilisant l'�clairage d'un solide photom�trique.
   * @param color 0 ou 2 pour m�langer la luminance du solide avec la couleur des mat�riaux.
   */
  virtual void draw(bool color);
  
protected:  
  /** Pointeur vers la sc�ne 3D. */
  const Scene* m_scene;
  /** Pointeur vers les flammes. */
  const vector <FireSource *> *m_flames;
  
  GLSLProgram m_SPProgram;
  /** Vertex Shader pour les objets textur�s. */
  GLSLVertexShader m_SPVertexShaderTex;
  
  /** Tableau contenant le fragment program pour SP interpol� avec couleur des objets */
  GLSLFragmentShader m_SPFragmentShader;

  /** Tableau contenant les centres de tous les solides. */
  GLfloat *m_centers;
  /** Tableau contenant les intensit�s de tous les solides. */
  GLfloat *m_intensities;
  /** Tableau contenant les valeurs de tous les solides. */
  GLfloat *m_lazimuth_lzenith;
};

/** Abstraction d'un solide photom�trique.<br>
 * Cette classe permet de charger des fichiers IES et de les utiliser ensuite pour �clairer
 * une sc�ne OpenGL en temps-r�el. Elle propose d'afficher les donn�es brutes du fichier en 
 * noir et blanc, ces m�mes donn�es interpol�es en arc de spirale, ou encore m�lang�es avec
 * la couleur des mat�riaux des objets
 */
class PhotometricSolidsRenderer : public PixelLightingRenderer
{
public:
  /** Constructeur par d�faut.
   * @param s Pointeur vers la sc�ne 3D.
   * @param flames Tableau contenant les flammes.
   * @param nbFlames Nombre de flammes dans le tableau.
   * @param context Pointeur vers le contexte Cg.
   * @param  Bool�ne indiquant s'il faut recompiler ou non les shaders Cg.
   */
  PhotometricSolidsRenderer(const Scene* const s, const vector <FireSource *> *flames);
  /** Destructeur. */
  virtual ~PhotometricSolidsRenderer();
  
  /** M�thode de dessin de la sc�ne en utilisant l'�clairage d'un solide photom�trique.
   * @param color 0 ou 2 pour m�langer la luminance du solide avec la couleur des mat�riaux.
   */
  void draw(bool color);
  
  /** Efface la texture. */
  void deleteTexture(void){ delete m_photometricSolidsTex; };
  /** G�n�rer une nouvelle texture pour le solide photom�trique. */
  void generateTexture(void);
  
private:  
  /** Texture 3D contenant les valeurs de luminance de tous les flammes. */
  Texture *m_photometricSolidsTex;
  
  /** Tableau contenant le fragment program pour SP interpol� sans couleur des objets */
  GLSLFragmentShader m_SPOnlyFragmentShader;
  
  /** Tableau contenant la taille des textures 2D. */
  uint m_tex2DSize[2];
  
  GLSLProgram m_SPOnlyProgram;
};


#endif

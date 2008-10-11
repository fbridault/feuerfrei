#if !defined(TEXTURE_H)
#define TEXTURE_H

#include <GL/gl.h>
#include <string>

using namespace std;

/** La classe texture peut être utilisée pour sous-classer différents types de textures, comme des textures
 * bitmaps à partir d'un fichier image, ou des textures de profondeur.
 * Lors du texturage, il suffit d'appeler la fonction glBindTexture() avec comme paramètre
 * Texture::getTexture().
 * A noter que cette classe peut facilement être réutilisée pour une autre application.
 *
 * @author	Flavien Bridault
 */
class Texture
{
protected:
  Texture(): m_type(GL_TEXTURE_2D) {};
  /** Constructeur protégé uniquement à destination des sous-classes */
  Texture(GLenum type);

public:
  virtual ~Texture();

  /** Donne l'identifiant de la texture à utiliser avec glBindTexture().
   * @return Identifiant de la texture.
   */
  const GLuint getTexture() const {
    return m_texName;
  };

  /** Donne le type de la texture.
   * @return Type de la texture.
   */
  const GLuint getTextureType() const {
    return m_type;
  };

  /** Active la texture pour l'objet courant avec glBindTexture().
   */
  const void bind() const
  {
    glBindTexture(m_type, m_texName);
  };

  /** Active la texture pour l'objet courant avec glBindTexture().
   * @param unité de texture
   */
  const void bind(uint uiTexUnit) const
  {
    glActiveTexture(GL_TEXTURE0+uiTexUnit);
    glBindTexture(m_type, m_texName);
  };

  void drawOnScreen(uint width, uint height) const
  {
    glBindTexture(m_type, m_texName);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-1.0f,-1.0f,0.0f);
    glTexCoord2f(width,0.0f);
    glVertex3f(1.0f,-1.0f,0.0f);
    glTexCoord2f(width,height);
    glVertex3f(1.0f,1.0f,0.0f);
    glTexCoord2f(0.0f,height);
    glVertex3f(-1.0f,1.0f,0.0f);

    glEnd();
  }

protected:
  /** Identifiant OpenGL de la texture */
  GLuint m_texName;

  /** Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB, ... */
  GLenum m_type;
};

/************************************************************************************/
/********************************* Class BitmapTexture ******************************/
/************************************************************************************/
class BitmapTexture : public Texture
{
public:
  /** Construit une texture RGB de type 2D à partir d'un fichier image.
   * @param filename Nom du fichier image à charger.
   */
  BitmapTexture(const string& filename);

  /** Construit une texture RGB à partir d'un fichier image.
   * @param filename Nom du fichier image à charger.
   * @param type Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB,...
   */
  BitmapTexture(const string& filename, GLenum type);

  /** Construit une texture RGBA de type 2D à partir d'un fichier image.
   * @param filename Nom du fichier image à charger.
   * @param wrap_s Paramètre de répétition de la texture dans la direction s {GL_WRAP,GL_REPEAT}.
   * @param wrap_t Paramètre de répétition de la texture dans la direction t {GL_WRAP,GL_REPEAT}.
   */
  BitmapTexture(const string& filename, GLint wrap_s, GLint wrap_t);

  virtual ~BitmapTexture();

  const bool hasAlpha() const {
    return m_hasAlpha;
  };

  const string& getName() const {
    return m_filename;
  };

private:
  void load(const string& filename);

private:
  string m_filename;
  bool m_hasAlpha;
};

/************************************************************************************/
/******************************* Class RenderTexture ********************************/
/************************************************************************************/

class RenderTexture : public Texture
{
  friend class RenderTarget;
private:
  /** Construit une texture RGBA vide.
   * @param   type  Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB,...
   * @param  width  Largeur de la texture.
   * @param height  Hauteur de la texture.
   * @param format  Format de la texture (à revoir).
   */
  RenderTexture(GLenum type, GLenum filter, uint width, uint height, char format);

  /** Construit une texture RGBA vide.
   * @param   type  Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB,...
   * @param  width  Largeur de la texture.
   * @param height  Hauteur de la texture.
   */
  RenderTexture(GLenum type, GLenum filter, uint width, uint height);

  virtual ~RenderTexture(){};
};
/************************************************************************************/
/******************************** Class DepthTexture ********************************/
/************************************************************************************/

class DepthTexture : public Texture
{
public:
  /** Construit une texture de profondeur vide, utilisable pour faire du render-to-texture.
  * @param   type  Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB,...
  * @param           width  Largeur de la texture.
  * @param          height  Hauteur de la texture.
  * @param depthComparison  Indique que la texture est une shadow map ou non
  * (c.à.d., si elle retourne un résultat de comparaison ou une profondeur)
  */
  DepthTexture(GLenum type, uint width, uint height, GLenum filter, bool depthComparison);

  /** Construit une texture de profondeur vide, utilisable pour faire du render-to-texture.
  * @param   type  Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB,...
  * @param  width  Largeur de la texture.
  * @param height  Hauteur de la texture.
  * @param   func  Fonction de comparaison (GL_LESS, GL_EQUAL, GL_GREATER, etc...)
  */
  DepthTexture(GLenum type, uint width, uint height, GLenum filter, GLenum func);

  virtual ~DepthTexture(){};
};
/************************************************************************************/
/********************************* Class CubeTexture ********************************/
/************************************************************************************/

class CubeTexture : public Texture
{
  friend class RenderTarget;
private:
  /** Constructeur d'une cubemap vide, utilisable pour faire du render-to-texture.
  * @param  width  Largeur de la texture.
  * @param height  Hauteur de la texture.
  */
  CubeTexture(uint width, uint height);
public:
  /** Constructeur d'une cubemap à partir de six fichiers images.
  * @param filenames  Tableau comprenant les nom des six fichiers images.
  */
  CubeTexture(const string filenames[6]);

  virtual ~CubeTexture(){};

  /** Liste des targets pour la création des textures de CubeMap */
  static const GLenum s_cubeMapTarget[6];
};

/************************************************************************************/
/********************************* Class Texture3D **********************************/
/************************************************************************************/

class Texture3D : public Texture
{
public:
  /** Constructeur d'une cubemap à partir de six fichiers images.
  * @param filenames  Tableau comprenant les nom des six fichiers images.
  */
  /** Construit une texture 3D en GL_LUMINANCE à partir d'un tableau de réels, de la largeur
  * et de la hauteur de la texture. Utilisé seulement pour construire la texture des
  * solides photom<E9>triques.
  * @param x Largeur de la texture
  * @param y Hauteur de la texture
  * @param z Profondeur de la texture
  */
  Texture3D(GLsizei x, GLsizei y, GLsizei z, const GLfloat* const texels);

  virtual ~Texture3D(){};
};

#endif

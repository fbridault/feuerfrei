#if !defined(TEXTURE_H)
#define TEXTURE_H

#include <wx/string.h>
#include <wx/image.h>
#include <GL/gl.h>

/** La classe texture peut être utilisée pour définir une texture à partir d'un fichier image. 
 * Il suffit simplement de définir un objet Texture en lui donnant en paramètre le nom de la texture. 
 * Ensuite, lors du texturage, il suffit d'appeler la fonction glBindTexture() avec comme paramètre 
 * Texture::getTexture(). Pour l'instant, la classe est limitée aux textures 2D.
 * A noter que cette classe peut facilement être réutilisée pour une autre application.
 *
 * @author	Flavien Bridault
 */
class Texture
{
public:
  /** Construit une texture vide.
   * @param type type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB,...
   * @param width largeur de la texture
   * @param height hauteur de la texture
   */
  Texture(GLenum type, uint width, uint height);
  
  /** Construit une texture vide.
   * @param type type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB,...
   * @param width largeur de la texture
   * @param height hauteur de la texture
   */
  Texture(uint width, uint height, GLenum func, bool dummy);

  /** Construit une texture à partir d'un fichier image.
   * @param filename Nom du fichier image à charger
   */
  Texture(const wxString& filename);

  Texture(const wxString& filename, GLenum type);
  virtual ~Texture();
  
    /** Construit une texture à partir d'un fichier image.
   * @param filename Nom du fichier image à charger
   * @param wrap_s paramètre de répétition de la texture dans la direction s {GL_WRAP,GL_REPEAT}
   * @param wrap_t paramètre de répétition de la texture dans la direction t {GL_WRAP,GL_REPEAT}
   */
  Texture(const wxString& filename, GLint wrap_s, GLint wrap_t);

  /** Construit une texture 3D en GL_LUMINANCE à partir d'un tableau de réels, de la largeur
    et de la hauteur de la texture. Utilisé seulement pour construire la texture des
    solides photmétriques.
   * @param x : largeur de la texture
   * @param y : hauteur de la texture
   * @param z : profondeur de la texture
   * @param texels : pointeur vers le tableau de réels
   */
  Texture(GLsizei x, GLsizei y, GLsizei z, const GLfloat *texels);
  
  /** Donne l'identifiant de la texture à utiliser avec glBindTexture(). */
  const GLuint getTexture() const {return m_texName;};

  /** Donne l'identifiant de la texture à utiliser avec glBindTexture(). */
  const void bind() const { glBindTexture(m_type, m_texName); };
  
private:
  /** Identifiant OpenGL de la texture */
  GLuint m_texName;

  GLenum m_type;
  /** Image */
  wxImage *m_wxtex;
};

#endif

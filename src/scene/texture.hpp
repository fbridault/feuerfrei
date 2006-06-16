#if !defined(TEXTURE_H)
#define TEXTURE_H

#include <wx/string.h>
#include <wx/image.h>
#include <GL/gl.h>

/** La classe texture peut �tre utilis�e pour d�finir une texture � partir d'un fichier image. 
 * Il suffit simplement de d�finir un objet Texture en lui donnant en param�tre le nom de la texture. 
 * Ensuite, lors du texturage, il suffit d'appeler la fonction glBindTexture() avec comme param�tre 
 * Texture::getTexture(). Pour l'instant, la classe est limit�e aux textures 2D.
 * A noter que cette classe peut facilement �tre r�utilis�e pour une autre application.
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

  /** Construit une texture � partir d'un fichier image.
   * @param filename Nom du fichier image � charger
   */
  Texture(const wxString& filename);

  Texture(const wxString& filename, GLenum type);
  virtual ~Texture();
  
    /** Construit une texture � partir d'un fichier image.
   * @param filename Nom du fichier image � charger
   * @param wrap_s param�tre de r�p�tition de la texture dans la direction s {GL_WRAP,GL_REPEAT}
   * @param wrap_t param�tre de r�p�tition de la texture dans la direction t {GL_WRAP,GL_REPEAT}
   */
  Texture(const wxString& filename, GLint wrap_s, GLint wrap_t);

  /** Construit une texture 3D en GL_LUMINANCE � partir d'un tableau de r�els, de la largeur
    et de la hauteur de la texture. Utilis� seulement pour construire la texture des
    solides photm�triques.
   * @param x : largeur de la texture
   * @param y : hauteur de la texture
   * @param z : profondeur de la texture
   * @param texels : pointeur vers le tableau de r�els
   */
  Texture(GLsizei x, GLsizei y, GLsizei z, const GLfloat *texels);
  
  /** Donne l'identifiant de la texture � utiliser avec glBindTexture(). */
  const GLuint getTexture() const {return m_texName;};

  /** Donne l'identifiant de la texture � utiliser avec glBindTexture(). */
  const void bind() const { glBindTexture(m_type, m_texName); };
  
private:
  /** Identifiant OpenGL de la texture */
  GLuint m_texName;

  GLenum m_type;
  /** Image */
  wxImage *m_wxtex;
};

#endif

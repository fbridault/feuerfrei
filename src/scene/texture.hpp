#if !defined(TEXTURE_H)
#define TEXTURE_H

#include <wx/string.h>
#include <wx/image.h>
#include <GL/gl.h>

/** La classe texture peut être utilisée pour définir différents types de textures, comme des textures
 * bitmaps à partir d'un fichier image, ou des textures de profondeur.
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
  /** Construit une texture RGBA vide.
   * @param type Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB,...
   * @param width Largeur de la texture.
   * @param height Hauteur de la texture.
   */
  Texture(GLenum type, uint width, uint height);
  
  /** Construit une texture de profondeur vide. Cette texture pourra être utilisée ensuite
   * pour effectuer des tests de profondeur.
   * @param width Largeur de la texture.
   * @param height Hauteur de la texture.
   * @param func Fonction de comparaison parmi GL_LESS, GL_EQUAL, GL_GREATER, GL_LEQUAL, etc...
   * @param dummy Inutilisé. Il s'agit juste d'un hack tout pourri pour permettre de différencier ce
   * constructeur du précédent. Il faudra trouver mieux un jour...
   */
  Texture(uint width, uint height, GLenum func, bool dummy);

  /** Construit une texture RGB de type 2D à partir d'un fichier image.
   * @param filename Nom du fichier image à charger.
   */
  Texture(const wxString& filename);
    
  /** Construit une texture RGB à partir d'un fichier image.
   * @param filename Nom du fichier image à charger.
   * @param type Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB,...
   */
  Texture(const wxString& filename, GLenum type);
  
  /** Construit une texture RGBA de type 2D à partir d'un fichier image.
   * @param filename Nom du fichier image à charger.
   * @param wrap_s Paramètre de répétition de la texture dans la direction s {GL_WRAP,GL_REPEAT}.
   * @param wrap_t Paramètre de répétition de la texture dans la direction t {GL_WRAP,GL_REPEAT}.
   */
  Texture(const wxString& filename, GLint wrap_s, GLint wrap_t);

  /** Construit une texture 3D en GL_LUMINANCE à partir d'un tableau de réels, de la largeur
   * et de la hauteur de la texture. Utilisé seulement pour construire la texture des
   * solides photométriques.
   * @param x Largeur de la texture
   * @param y Hauteur de la texture
   * @param z Profondeur de la texture
   * @param texels Pointeur vers le tableau de réels
   */
  Texture(GLsizei x, GLsizei y, GLsizei z, const GLfloat *texels);
  
  /** Destructeur */
  virtual ~Texture();
  
  /** Donne l'identifiant de la texture à utiliser avec glBindTexture().
   * @return Identifiant de la texture.
   */
  const GLuint getTexture() const {return m_texName;};

  /** Active la texture pour l'objet courant avec glBindTexture().
   */
  const void bind() const { glBindTexture(m_type, m_texName); };
  
private:
  /** Identifiant OpenGL de la texture */
  GLuint m_texName;

  /** Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB, ... */
  GLenum m_type;
  
  /** Image */
  wxImage *m_wxtex;
};

#endif

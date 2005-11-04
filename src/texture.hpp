#if !defined(TEXTURE_H)
#define TEXTURE_H

#include "header.h"

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
  /** Construit une texture à partir d'un fichier image.
   * @param filename Nom du fichier image à charger
   */
  Texture(const wxString& filename);

  Texture(const wxString& filename, GLenum gltexture);
  virtual ~Texture();
  
    /** Construit une texture à partir d'un fichier image.
   * @param filename Nom du fichier image à charger
   * @param wrap_s paramètre de répétition de la texture dans la direction s {GL_WRAP,GL_REPEAT}
   * @param wrap_t paramètre de répétition de la texture dans la direction t {GL_WRAP,GL_REPEAT}
   */
  Texture(const wxString& filename, GLint wrap_s, GLint wrap_t);

  /** Construit une texture GL_LUMINANCE a partir d'un tableau de reels, de la largeur
    et de la hauteur de la texture.
   * @param w : largeur de la texture
   * @param h : hauteur de la texture
   * @param texels : pointeur vers le tableau de reels
   */
  Texture(GLsizei w, GLsizei h, const GLfloat *texels);
  
  /** Donne l'identifiant de la texture à utiliser avec glBindTexture(). */
  const GLuint getTexture() const {return texName;};

  const long GuessImageType(const wxString& filename);

private:
  /** Indique si la texture possède un canal alpha */
  bool hasAlpha;

  /** Identifiant OpenGL de la texture */
  GLuint texName;
  wxImage *wxtex;
};

#endif

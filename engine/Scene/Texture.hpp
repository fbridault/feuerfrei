#if !defined(TEXTURE_H)
#define TEXTURE_H

#include <GL/gl.h>
#include <string>
#include "../Common.hpp"

using namespace std;

/** La classe texture peut être utilisée pour sous-classer différents types de textures, comme des textures
 * bitmaps à partir d'un fichier image, ou des textures de profondeur.
 * Lors du texturage, il suffit d'appeler la fonction glBindTexture() avec comme paramètre
 * ITexture::getTexture().
 * A noter que cette classe peut facilement être réutilisée pour une autre application.
 *
 * @author	Flavien Bridault
 */
class ITexture
{
protected:
	ITexture(): m_type(GL_TEXTURE_2D) {};
	/** Constructeur protégé uniquement à destination des sous-classes */
	ITexture(GLenum type);

public:
	virtual ~ITexture();

	/** Donne l'identifiant de la texture à utiliser avec glBindTexture().
	 * @return Identifiant de la texture.
	 */
	const GLuint getTexture() const
	{
		return m_texName;
	};

	/** Donne le type de la texture.
	 * @return Type de la texture.
	 */
	const GLuint getTextureType() const
	{
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
/********************************* Class CBitmapTexture ******************************/
/************************************************************************************/
class CBitmapTexture : public ITexture
{
public:
	/** Construit une texture RGB de type 2D à partir d'un fichier image.
	 * @param filename Nom du fichier image à charger.
	 */
	CBitmapTexture(CharCPtrC a_szFilename);

	/** Construit une texture RGB à partir d'un fichier image.
	 * @param filename Nom du fichier image à charger.
	 * @param type Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB,...
	 */
	CBitmapTexture(CharCPtrC a_szFilename, GLenum type);

	/** Construit une texture RGBA de type 2D à partir d'un fichier image.
	 * @param filename Nom du fichier image à charger.
	 * @param wrap_s Paramètre de répétition de la texture dans la direction s {GL_WRAP,GL_REPEAT}.
	 * @param wrap_t Paramètre de répétition de la texture dans la direction t {GL_WRAP,GL_REPEAT}.
	 */
	CBitmapTexture(CharCPtrC a_szFilename, GLint wrap_s, GLint wrap_t);

	virtual ~CBitmapTexture();

	const bool hasAlpha() const
	{
		return m_hasAlpha;
	};

	CharCPtrC getName() const
	{
		return m_szFilename;
	};

private:
	void load(CharCPtrC a_szFilename);

private:
	CharCPtr m_szFilename;
	bool m_hasAlpha;
};

/************************************************************************************/
/******************************* Class CRenderTexture ********************************/
/************************************************************************************/

class CRenderTexture : public ITexture
{
	friend class CRenderTarget;
private:
	/** Construit une texture RGBA vide.
	 * @param   type  Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB,...
	 * @param  width  Largeur de la texture.
	 * @param height  Hauteur de la texture.
	 * @param format  Format de la texture (à revoir).
	 */
	CRenderTexture(GLenum type, GLenum filter, uint width, uint height, char format);

	/** Construit une texture RGBA vide.
	 * @param   type  Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB,...
	 * @param  width  Largeur de la texture.
	 * @param height  Hauteur de la texture.
	 */
	CRenderTexture(GLenum type, GLenum filter, uint width, uint height);

	virtual ~CRenderTexture(){};
};
/************************************************************************************/
/******************************** Class CDepthTexture ********************************/
/************************************************************************************/

class CDepthTexture : public ITexture
{
public:
	/** Construit une texture de profondeur vide, utilisable pour faire du render-to-texture.
	* @param   type  Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB,...
	* @param           width  Largeur de la texture.
	* @param          height  Hauteur de la texture.
	* @param depthComparison  Indique que la texture est une shadow map ou non
	* (c.à.d., si elle retourne un résultat de comparaison ou une profondeur)
	*/
	CDepthTexture(GLenum type, uint width, uint height, GLenum filter, bool depthComparison);

	/** Construit une texture de profondeur vide, utilisable pour faire du render-to-texture.
	* @param   type  Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB,...
	* @param  width  Largeur de la texture.
	* @param height  Hauteur de la texture.
	* @param   func  Fonction de comparaison (GL_LESS, GL_EQUAL, GL_GREATER, etc...)
	*/
	CDepthTexture(GLenum type, uint width, uint height, GLenum filter, GLenum func);

	virtual ~CDepthTexture(){};
};
/************************************************************************************/
/********************************* Class CCubeTexture ********************************/
/************************************************************************************/

class CCubeTexture : public ITexture
{
	friend class CRenderTarget;
private:
	/** Constructeur d'une cubemap vide, utilisable pour faire du render-to-texture.
	* @param  width  Largeur de la texture.
	* @param height  Hauteur de la texture.
	*/
	CCubeTexture(uint width, uint height);
public:
	/** Constructeur d'une cubemap à partir de six fichiers images.
	* @param filenames  Tableau comprenant les nom des six fichiers images.
	*/
	CCubeTexture(const string filenames[6]);

	virtual ~CCubeTexture(){};

	/** Liste des targets pour la création des textures de CubeMap */
	static const GLenum s_cubeMapTarget[6];
};

/************************************************************************************/
/********************************* Class CTexture3D **********************************/
/************************************************************************************/

class CTexture3D : public ITexture
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
	CTexture3D(GLsizei x, GLsizei y, GLsizei z, const GLfloat* const texels);

	virtual ~CTexture3D(){};
};

#endif

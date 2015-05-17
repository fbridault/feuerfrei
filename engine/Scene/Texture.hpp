#if !defined(TEXTURE_H)
#define TEXTURE_H

#include <GL/gl.h>
#include <string>
#include "../Common.hpp"

using namespace std;

/** La classe texture peut être utilisée pour sous-classer différents types de textures, comme des textures
 * bitmaps à partir d'un fichier image, ou des textures de profondeur.
 * Lors du texturage, il suffit d'appeler la fonction glBindTexture() avec comme param�tre
 * ITexture::GetTexture().
 * A noter que cette classe peut facilement �tre r�utilis�e pour une autre application.
 *
 * @author	Flavien Bridault
 */
class ITexture
{
protected:
	ITexture(): m_eType(GL_TEXTURE_2D) {};
	/** Constructeur prot�g� uniquement � destination des sous-classes */
	ITexture(GLenum type);

public:
	virtual ~ITexture();

	/** Donne l'identifiant de la texture � utiliser avec glBindTexture().
	 * @return Identifiant de la texture.
	 */
	GLuint GetTexture() const
	{
		return m_uiTexId;
	};

	/** Donne le type de la texture.
	 * @return Type de la texture.
	 */
	GLuint GetTextureType() const
	{
		return m_eType;
	};

	/** Active la texture pour l'objet courant avec glBindTexture(). */
	void bind() const
	{
		glBindTexture(m_eType, m_uiTexId);
	};

	/** Active la texture pour l'objet courant avec glBindTexture().
	 * @param unit� de texture
	 */
	void bind(uint uiTexUnit) const
	{
		glActiveTexture(GL_TEXTURE0+uiTexUnit);
		glBindTexture(m_eType, m_uiTexId);
	};

	void drawOnScreen(uint width, uint height) const
	{
		glBindTexture(m_eType, m_uiTexId);
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
	GLuint m_uiTexId;

	/** Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB, ... */
	GLenum m_eType;
};

/************************************************************************************/
/********************************* Class CBitmapTexture ******************************/
/************************************************************************************/
class CBitmapTexture : public ITexture
{
public:
	/** Construit une texture RGB de type 2D � partir d'un fichier image.
	 * @param filename Nom du fichier image � charger.
	 */
	CBitmapTexture(string const& a_strFilename);

	/** Construit une texture RGB � partir d'un fichier image.
	 * @param filename Nom du fichier image � charger.
	 * @param type Type de la texture parmi GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB,...
	 */
	CBitmapTexture(string const& a_strFilename, GLenum type);

	/** Construit une texture RGBA de type 2D � partir d'un fichier image.
	 * @param filename Nom du fichier image � charger.
	 * @param wrap_s Param�tre de r�p�tition de la texture dans la direction s {GL_WRAP,GL_REPEAT}.
	 * @param wrap_t Param�tre de r�p�tition de la texture dans la direction t {GL_WRAP,GL_REPEAT}.
	 */
	CBitmapTexture(string const& a_strFilename, GLint wrap_s, GLint wrap_t);

	virtual ~CBitmapTexture();

	bool hasAlpha() const
	{
		return m_hasAlpha;
	};

	string const& GetName() const
	{
		return m_strFilename;
	};

private:
	void load(string const& a_strFilename);

private:
	string m_strFilename;
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
	 * @param format  Format de la texture (� revoir).
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
	* (c.�.d., si elle retourne un r�sultat de comparaison ou une profondeur)
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
	/** Constructeur d'une cubemap � partir de six fichiers images.
	* @param filenames  Tableau comprenant les nom des six fichiers images.
	*/
	CCubeTexture(const string filenames[6]);

	virtual ~CCubeTexture(){};

	/** Liste des tarGets pour la cr�ation des textures de CubeMap */
	static const GLenum s_cubeMapTarget[6];
};

/************************************************************************************/
/********************************* Class CTexture3D **********************************/
/************************************************************************************/

class CTexture3D : public ITexture
{
public:
	/** Constructeur d'une cubemap � partir de six fichiers images.
	* @param filenames  Tableau comprenant les nom des six fichiers images.
	*/
	/** Construit une texture 3D en GL_LUMINANCE � partir d'un tableau de r�els, de la largeur
	* et de la hauteur de la texture. Utilis� seulement pour construire la texture des
	* solides photom<E9>triques.
	* @param x Largeur de la texture
	* @param y Hauteur de la texture
	* @param z Profondeur de la texture
	*/
	CTexture3D(GLsizei x, GLsizei y, GLsizei z, const GLfloat* const texels);

	virtual ~CTexture3D(){};
};

#endif

#include "Texture.hpp"
#include <ilut.h>
#include <iostream>

using namespace std;

/** Constructeur protégé pour les sous-classes */

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
ITexture::ITexture(GLenum type)
{
	m_eType = type;

	glGenTextures(1, &m_uiTexId);
	glBindTexture(m_eType, m_uiTexId);
	/** Les paramètres des textures sont ensuite déterminées dans le constructeur de la sous-classe */
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
ITexture::~ITexture()
{
	glDeleteTextures(1, &m_uiTexId);
}

/************************************************************************************/
/******************************* Class CBitmapTexture ******************************/
/************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CBitmapTexture::CBitmapTexture(string const& a_strFilename) :
		ITexture(GL_TEXTURE_2D)
{
	load(a_strFilename);
	cout << "Loading scene texture : " << a_strFilename << "......" << endl;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CBitmapTexture::CBitmapTexture(string const& a_strFilename, GLenum type) :
		ITexture(type)
{
	glTexParameteri(m_eType,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(m_eType,GL_TEXTURE_WRAP_T,GL_CLAMP);

	cout << "Loading scene texture : " << a_strFilename << "......" << endl;
	load(a_strFilename);
	glTexParameteri(m_eType,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(m_eType,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CBitmapTexture::CBitmapTexture(string const& a_strFilename, GLint wrap_s, GLint wrap_t) :
		ITexture(GL_TEXTURE_2D)
{
	glTexParameteri(m_eType,GL_TEXTURE_WRAP_S,wrap_s);
	glTexParameteri(m_eType,GL_TEXTURE_WRAP_T,wrap_t);

	cout << "Loading texture : " << a_strFilename << "......" << endl;
	load(a_strFilename);
	glTexParameteri(m_eType,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(m_eType,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CBitmapTexture::~CBitmapTexture()
{
}

#define isPowerOfTwo(x) ( ((x&(x-1)) == 0) && (x!=0) )

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
void CBitmapTexture::load(string const& a_strFilename)
{
	uint w,h;

	ILuint ImgId; // image name for DevIL

	ilGenImages(1, &ImgId); // Generate an image name to use.
	ilBindImage(ImgId); // Bind this image name

	if ( !ilLoadImage((char *)a_strFilename.c_str()) )
		cerr << "Error loading texture " << a_strFilename << endl;

	w=ilGetInteger(IL_IMAGE_WIDTH);
	h=ilGetInteger(IL_IMAGE_HEIGHT);

	if (m_eType == GL_TEXTURE_2D && (!isPowerOfTwo(w) || !isPowerOfTwo(h)))
		cerr << "(WW) Bitmap texture " << a_strFilename << " size is not a power of 2" << endl;

	m_uiTexId = ilutGLBindMipmaps();

	m_strFilename = a_strFilename;
	m_hasAlpha = false;

	ilDeleteImages(1, &ImgId);
}

/************************************************************************************/
/******************************* Class CRenderTexture ********************************/
/************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CRenderTexture::CRenderTexture(GLenum type, GLenum filter, uint width, uint height, char format) :
		ITexture(type)
{
	glTexParameteri(m_eType,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(m_eType,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameteri(m_eType,GL_TEXTURE_MAG_FILTER,filter);
	glTexParameteri(m_eType,GL_TEXTURE_MIN_FILTER,filter);

	switch (format)
	{
		case 0 :
			glTexImage2D(m_eType, 0, GL_RGB16F_ARB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
			break;
		case 1 :
			glTexImage2D(m_eType, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			break;
//    case 0 : glTexImage2D(m_eType, 0, GL_RGB32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, NULL); break;
//    case 1 : glTexImage2D(m_eType, 0, GL_FLOAT_RG16_NV, width, height, 0, GL_FLOAT_RG16_NV, GL_UNSIGNED_BYTE, NULL); break;
//    case 2 : glTexImage2D(m_eType, 0, GL_RGB10_A2, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); break;
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CRenderTexture::CRenderTexture(GLenum type, GLenum filter, uint width, uint height) :
		ITexture(type)
{
	glTexParameteri(m_eType,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(m_eType,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameteri(m_eType,GL_TEXTURE_MAG_FILTER,filter);
	glTexParameteri(m_eType,GL_TEXTURE_MIN_FILTER,filter);

	glTexImage2D(m_eType, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}

/************************************************************************************/
/******************************** Class CDepthTexture ********************************/
/************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CDepthTexture::CDepthTexture(GLenum type, uint width, uint height, GLenum filter, bool depthComparison) :
		ITexture( type)
{
	glTexParameteri(m_eType, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(m_eType, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(m_eType, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(m_eType, GL_TEXTURE_WRAP_T, GL_CLAMP);
	if (depthComparison)
	{
		glTexParameteri(m_eType,GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexParameteri(m_eType,GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	}
	glTexImage2D( m_eType, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CDepthTexture::CDepthTexture(GLenum type, uint width, uint height, GLenum filter, GLenum func) :
		ITexture( type)
{
	glTexParameteri(m_eType,GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(m_eType,GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(m_eType,GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(m_eType,GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(m_eType,GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(m_eType,GL_TEXTURE_COMPARE_FUNC, func);
//   glTexParameteri(m_eType, GL_DEPTH_TEXTURE_MODE_ARB, GL_ALPHA);

	glTexImage2D(m_eType, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

/************************************************************************************/
/********************************* Class CCubeTexture ********************************/
/************************************************************************************/

const GLenum CCubeTexture::s_cubeMapTarget[6] =
{
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CCubeTexture::CCubeTexture(uint width, uint height) :
		ITexture(GL_TEXTURE_CUBE_MAP)
{
	glTexParameteri(m_eType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(m_eType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_eType, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(m_eType, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(m_eType, GL_TEXTURE_WRAP_R, GL_CLAMP);

	for (int i = 0; i < 6; i++)
//    glTexImage2D( s_cubeMapTarget[i], 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//    glTexImage2D( s_cubeMapTarget[i], 0, GL_FLOAT_R16_NV, width, height, 0, GL_RED, GL_FLOAT, NULL);
//    glTexImage2D( s_cubeMapTarget[i], 0, GL_FLOAT_R_NV, width, height, 0, GL_RED, GL_FLOAT, NULL);
		glTexImage2D( s_cubeMapTarget[i], 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CCubeTexture::CCubeTexture(const string filenames[6]) :
	ITexture(GL_TEXTURE_CUBE_MAP)
{
	glTexParameteri(m_eType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(m_eType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_eType, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(m_eType, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(m_eType, GL_TEXTURE_WRAP_R, GL_CLAMP);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);

	for (int i = 0; i < 6; i++)
	{
		ILuint ImgId; // image name for DevIL

		ilGenImages(1, &ImgId); // Generate an image name to use.
		ilBindImage(ImgId); // Bind this image name
		if ( !ilLoadImage((char *)filenames[i].c_str()) )
			cerr << "Error loading texture " << filenames[i] << endl;

		ILubyte *pixels = ilGetData();

		glTexImage2D( s_cubeMapTarget[i], 0, ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL), ilGetInteger(IL_IMAGE_WIDTH),
		              ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), pixels);
	}
}

/************************************************************************************/
/********************************* Class CTexture3D ********************************/
/************************************************************************************/

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
CTexture3D::CTexture3D(GLsizei x, GLsizei y, GLsizei z, const GLfloat* const texels) :
		ITexture(GL_TEXTURE_3D)
{
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, x, y, z, 0, GL_LUMINANCE, GL_FLOAT, texels);
}

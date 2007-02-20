#include "texture.hpp"
#include "GL/glu.h"
#include <iostream>

using namespace std;

Texture::Texture(GLenum type, GLenum filter, uint width, uint height)
{  
  m_type = type;
  m_hasAlpha = true;
  
  glGenTextures(1, &m_texName);
  glBindTexture(m_type, m_texName); 
  
  glTexParameteri(m_type,GL_TEXTURE_WRAP_S,GL_CLAMP);
  glTexParameteri(m_type,GL_TEXTURE_WRAP_T,GL_CLAMP);
  glTexParameteri(m_type,GL_TEXTURE_MAG_FILTER,filter);
  glTexParameteri(m_type,GL_TEXTURE_MIN_FILTER,filter);
  
  glTexImage2D(m_type, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
  m_wxtex = NULL;
}


Texture::Texture(uint width, uint height, GLenum func, bool dummy)
{
  m_hasAlpha = false;
  m_type = GL_TEXTURE_RECTANGLE_ARB;
  
  glGenTextures(1, &m_texName);
  glBindTexture(m_type, m_texName);
  
  glTexParameteri(m_type,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(m_type,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(m_type,GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(m_type,GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(m_type,GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
  glTexParameteri(m_type,GL_TEXTURE_COMPARE_FUNC_ARB, func);
//   glTexParameteri(m_type, GL_DEPTH_TEXTURE_MODE_ARB, GL_ALPHA);

  glTexImage2D(m_type, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
  m_wxtex = NULL;
}

Texture::Texture(const wxString& filename) : m_fileName(filename)
{
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  
  m_type = GL_TEXTURE_2D;
  
  glGenTextures(1, &m_texName);
  glBindTexture(GL_TEXTURE_2D, m_texName);
 
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_MODULATE );
 
  cout << "Chargement texture scene : " << filename.fn_str() << "......";
  m_wxtex = new wxImage (filename);

  if(!m_wxtex) {
    cout << "Error ";
  }else{
    cout << "OK" << endl;
    if( m_wxtex->HasAlpha() )
      loadWithAlphaChannel();
    else{
      glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, m_wxtex->GetWidth(), m_wxtex->GetHeight(), 0, 
		    GL_RGB, GL_UNSIGNED_BYTE, m_wxtex->GetData());
      m_hasAlpha = false;
    }
  }
  m_wxtex->Destroy();
}

Texture::Texture(const wxString& filename, GLenum type) : m_fileName(filename)
{  
  m_type = type;

  glGenTextures(1, &m_texName);
  glBindTexture(m_type, m_texName); 
  
  glTexParameteri(m_type,GL_TEXTURE_WRAP_S,GL_CLAMP);
  glTexParameteri(m_type,GL_TEXTURE_WRAP_T,GL_CLAMP);
  glTexParameteri(m_type,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(m_type,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  cout << "Chargement texture : " << filename.fn_str() << "......";
  m_wxtex = new wxImage (filename);

  if(!m_wxtex) {
    cout << "Error";
  }else{
    cout << "OK" << endl;
    if( m_wxtex->HasAlpha() )
      loadWithAlphaChannel();
    else{
      glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, m_wxtex->GetWidth(), m_wxtex->GetHeight(), 0, 
		    GL_RGB, GL_UNSIGNED_BYTE, m_wxtex->GetData());
      m_hasAlpha = false;
    }
  }
  m_wxtex->Destroy();  
}

Texture::Texture(const wxString& filename, GLint wrap_s, GLint wrap_t) : m_fileName(filename)
{
  m_type = GL_TEXTURE_2D;

  glGenTextures(1, &m_texName);
  glBindTexture(GL_TEXTURE_2D, m_texName);
 
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,wrap_s);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,wrap_t);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
 
  cout << "Chargement texture : " << filename.fn_str() << "......";
  m_wxtex = new wxImage (filename);
  if(!m_wxtex) {
    cout << "Error";
  }else{
    cout << "OK" << endl;    
    if( m_wxtex->HasAlpha() )
      loadWithAlphaChannel();
    else{
      m_hasAlpha = false;
      glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, m_wxtex->GetWidth(), m_wxtex->GetHeight(), 0, 
		    GL_RGB, GL_UNSIGNED_BYTE, m_wxtex->GetData());
    }
  }
  /* Semble nécessaire pour éviter un plantage lors de la libération de la wxImage  */
  /* Toutefois cette fonction plante si on la met dans le destructeur, je la laisse */
  /* donc ici pour le moment */
  m_wxtex->Destroy();
}

Texture::Texture(GLsizei x, GLsizei y, GLsizei z, const GLfloat* const texels)
{  
  m_wxtex = NULL;
  m_type = GL_TEXTURE_3D;
  m_hasAlpha = false;
  
  glGenTextures(1, &m_texName);
  glBindTexture(GL_TEXTURE_3D, m_texName);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, x, y, z, 0, GL_LUMINANCE, GL_FLOAT, texels);
  
//   GLenum err=glGetError();
//   cerr << "gl : " << gluErrorString(err) << endl;
}

Texture::~Texture()
{
  glDeleteTextures(1, &m_texName);
  if(m_wxtex)
    delete m_wxtex;
}

void Texture::loadWithAlphaChannel()
{
  cout << "Found alpha channel..." << endl;
  m_hasAlpha = true;
  u_char *imgcpy,*tmp;
  tmp = imgcpy = new u_char[m_wxtex->GetWidth()*m_wxtex->GetHeight()*4];
  
  for(int j=0; j < m_wxtex->GetHeight(); j++)
    for(int i=0; i < m_wxtex->GetWidth(); i++){
      *(tmp++) = m_wxtex->GetRed(i,j);
      *(tmp++) = m_wxtex->GetGreen(i,j);
      *(tmp++) = m_wxtex->GetBlue(i,j);
      *(tmp++) = m_wxtex->GetAlpha(i,j);
    }
  
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, m_wxtex->GetWidth(), m_wxtex->GetHeight(), 0, 
		GL_RGBA, GL_UNSIGNED_BYTE, imgcpy);
  delete [] imgcpy;
}

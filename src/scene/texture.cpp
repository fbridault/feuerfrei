#include "texture.hpp"

#include <iostream>

using namespace std;

Texture::Texture(GLenum type, int width, int height)
{  
  m_type = type;
  
  glGenTextures(1, &m_texName);
  glBindTexture(m_type, m_texName); 
  
  glTexParameteri(m_type,GL_TEXTURE_WRAP_S,GL_CLAMP);
  glTexParameteri(m_type,GL_TEXTURE_WRAP_T,GL_CLAMP);
  glTexParameteri(m_type,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(m_type,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  //  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_REPLACE );
  
  glTexImage2D(m_type, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}

Texture::Texture(const wxString& filename)
{
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  
  m_type = GL_TEXTURE_2D;
  
  glGenTextures(1, &m_texName);
  glBindTexture(GL_TEXTURE_2D, m_texName);
 
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
 
  cout << "Chargement texture : " << filename.fn_str() << "......";
  m_wxtex = new wxImage (filename);

  if(!m_wxtex) {
    cout << "Error ";
  }else{
    cout << "OK" << endl;
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB8, m_wxtex->GetWidth(), m_wxtex->GetHeight(), 0, 
		  GL_RGB, GL_UNSIGNED_BYTE, m_wxtex->GetData());
  }
}

Texture::Texture(const wxString& filename, GLenum type)
{  
  m_type = type;

  glGenTextures(1, &m_texName);
  glBindTexture(m_type, m_texName); 
  
  glTexParameteri(m_type,GL_TEXTURE_WRAP_S,GL_CLAMP);
  glTexParameteri(m_type,GL_TEXTURE_WRAP_T,GL_CLAMP);
  glTexParameteri(m_type,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(m_type,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
//  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_REPLACE );
  cout << "Chargement texture : " << filename.fn_str() << "......";
  m_wxtex = new wxImage (filename);

  if(!m_wxtex) {
    cout << "Error";
  }else{
    cout << "OK" << endl;
    glTexImage2D (m_type, 0, GL_RGB, m_wxtex->GetWidth(), m_wxtex->GetHeight(), 0, 
		  GL_RGB, GL_UNSIGNED_BYTE, m_wxtex->GetData());
  }
  
}

Texture::Texture(const wxString& filename, GLint wrap_s, GLint wrap_t)
{
  m_type = GL_TEXTURE_2D;

  glGenTextures(1, &m_texName);
  glBindTexture(GL_TEXTURE_2D, m_texName);
 
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,wrap_s);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,wrap_t);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
 
  cout << "Chargement texture : " << filename.fn_str() << "......";
  m_wxtex = new wxImage (filename);
  if(!m_wxtex) {
    cout << "Error";
  }else{
    cout << "OK" << endl;
    
    if( m_wxtex->HasAlpha() ){
      unsigned char *imgcpy,*tmp;
      tmp = imgcpy = new unsigned char[m_wxtex->GetWidth()*m_wxtex->GetHeight()*4];
            
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
    else
      glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, m_wxtex->GetWidth(), m_wxtex->GetHeight(), 0, 
		    GL_RGB, GL_UNSIGNED_BYTE, m_wxtex->GetData());
  }
  /* Semble nécessaire pour éviter un plantage lors de la libération de la wxImage  */
  /* Toutefois cette fonction plante si on la met dans le destructeur, je la laisse */
  /* donc ici pour le moment */
  m_wxtex->Destroy();
}

Texture::Texture(GLsizei w, GLsizei h, const GLfloat *texels)
{  
  m_wxtex = NULL;
  m_type = GL_TEXTURE_RECTANGLE_NV;
  
  glGenTextures(1, &m_texName);
  
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_texName);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_R32_NV, w, h, 0, GL_LUMINANCE, GL_FLOAT, texels);
}

Texture::~Texture()
{
  if(m_wxtex)
    delete m_wxtex;
}

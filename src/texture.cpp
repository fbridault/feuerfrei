#include "texture.hpp"

#include <stdlib.h>

Texture::Texture(const wxString& filename)
{
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  
  glGenTextures(1, &texName);
  glBindTexture(GL_TEXTURE_2D, texName);
 
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
 
  cout << "Chargement texture : " << filename.fn_str() << "......";
  wxtex = new wxImage (filename);

  if(!wxtex) {
    cout << "Error ";
  }else{
    cout << "OK" << endl;
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB8, wxtex->GetWidth(), wxtex->GetHeight(), 0, 
		  GL_RGB, GL_UNSIGNED_BYTE, wxtex->GetData());
  }
}

Texture::Texture(const wxString& filename, GLenum gltexture)
{  
  glGenTextures(1, &texName);
  glBindTexture(gltexture, texName);
 
  glTexParameteri(gltexture,GL_TEXTURE_WRAP_S,GL_CLAMP);
  glTexParameteri(gltexture,GL_TEXTURE_WRAP_T,GL_CLAMP);
  glTexParameteri(gltexture,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(gltexture,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
//  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_REPLACE );
  cout << "Chargement texture : " << filename.fn_str() << "......";
  wxtex = new wxImage (filename);

  if(!wxtex) {
    cout << "Error";
  }else{
    cout << "OK" << endl;
    glTexImage2D (gltexture, 0, GL_RGB, wxtex->GetWidth(), wxtex->GetHeight(), 0, 
		  GL_RGB, GL_UNSIGNED_BYTE, wxtex->GetData());
  }
  
}

Texture::Texture(const wxString& filename, GLint wrap_s, GLint wrap_t)
{

  glGenTextures(1, &texName);
  glBindTexture(GL_TEXTURE_2D, texName);
 
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,wrap_s);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,wrap_t);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
 
  cout << "Chargement texture : " << filename.fn_str() << "......";
  wxtex = new wxImage (filename);
  if(!wxtex) {
    cout << "Error";
  }else{
    cout << "OK" << endl;
    
    if( wxtex->HasAlpha() ){
      unsigned char *imgcpy,*tmp;
      tmp = imgcpy = new unsigned char[wxtex->GetWidth()*wxtex->GetHeight()*4];
            
      for(int j=0; j < wxtex->GetHeight(); j++)
	for(int i=0; i < wxtex->GetWidth(); i++){
	  *(tmp++) = wxtex->GetRed(i,j);
	  *(tmp++) = wxtex->GetGreen(i,j);
	  *(tmp++) = wxtex->GetBlue(i,j);
	  *(tmp++) = wxtex->GetAlpha(i,j);
      }
      
      cout << "Canal alpha présent" << endl;
      glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, wxtex->GetWidth(), wxtex->GetHeight(), 0, 
		  GL_RGBA, GL_UNSIGNED_BYTE, imgcpy);
      delete [] imgcpy;
    }
    else
      glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, wxtex->GetWidth(), wxtex->GetHeight(), 0, 
		    GL_RGB, GL_UNSIGNED_BYTE, wxtex->GetData());
  }
  /* Semble nécessaire pour éviter un plantage lors de la libération de la wxImage  */
  /* Toutefois cette fonction plante si on la met dans le destructeur, je la laisse */
  /* donc ici pour le moment */
  wxtex->Destroy();
  //  if(hasAlpha)
  //     glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,texture);
  //   else
  //     glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,texture);
}

Texture::Texture(GLsizei w, GLsizei h, const GLfloat *texels)
{  
  glGenTextures(1, &texName);
  
//    glBindTexture(GL_TEXTURE_2D, texName);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//    glTexImage2D (GL_TEXTURE_2D, 0,GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_FLOAT, texels);

//    glTexImage2D (GL_TEXTURE_2D, 0,GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, texels);

   glBindTexture(GL_TEXTURE_RECTANGLE_NV, texName);
   glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_R32_NV, w, h, 0, GL_LUMINANCE, GL_FLOAT, texels);

//   glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGB32_NV, w, h, 0, GL_RGB, GL_FLOAT, texels);
}

Texture::~Texture()
{
  delete wxtex;
}

const long Texture::GuessImageType(const wxString& filename)
{
  wxString pngSuffix = _(".png");
  wxString jpgSuffix = _(".jpg");

  if(filename.Right(4) == pngSuffix){
    cout << "image " << filename << "est de type PNG" << endl;
    return wxBITMAP_TYPE_PNG;
  }
  if(filename.Right(4) == jpgSuffix){
    cout << "image " << filename << "est de type JPG" << endl;
    return wxBITMAP_TYPE_JPEG;
  }
  return wxBITMAP_TYPE_ANY;
}

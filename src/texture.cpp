#include "texture.hpp"

#include <stdlib.h>

Texture::Texture(const char* const filename)
{
  /*unsigned char *texture;
  unsigned int width,height;

  read_png(filename,&texture,&width,&height);*/
  
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  
  glGenTextures(1, &texName);
  glBindTexture(GL_TEXTURE_2D, texName);
 
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
 
  cout << "Chargement texture : " << filename << "......";
  sdltex = IMG_Load (filename);

  if(!sdltex) {
    cout << "Error : " << IMG_GetError();
  }else{
    cout << "OK" << endl;
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB8, sdltex->w, sdltex->h, 0, 
		  GL_RGB, GL_UNSIGNED_BYTE, sdltex->pixels);
  }
}

Texture::Texture(const char* const filename, GLenum gltexture)
{  
 // glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  
  glGenTextures(1, &texName);
  glBindTexture(gltexture, texName);
 
  glTexParameteri(gltexture,GL_TEXTURE_WRAP_S,GL_CLAMP);
  glTexParameteri(gltexture,GL_TEXTURE_WRAP_T,GL_CLAMP);
  glTexParameteri(gltexture,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(gltexture,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
//  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_REPLACE );
  cout << "Chargement texture : " << filename << "......";
  sdltex = IMG_Load (filename);

  if(!sdltex) {
    cout << "Error : " << IMG_GetError();
  }else{
    cout << "OK" << endl;
    glTexImage2D (gltexture, 0, GL_RGB, sdltex->w, sdltex->h, 0, 
		GL_RGB, GL_UNSIGNED_BYTE, sdltex->pixels);
  }
  
}

Texture::Texture(const char* const filename, GLint wrap_s, GLint wrap_t)
{
  /*unsigned char *texture;
  unsigned int width,height;
  
  read_png(filename,&texture,&width,&height);*/
  
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  
  glGenTextures(1, &texName);
  glBindTexture(GL_TEXTURE_2D, texName);
 
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,wrap_s);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,wrap_t);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
 
  cout << "Chargement texture : " << filename << "......";
  sdltex = IMG_Load (filename);

  if(!sdltex) {
    cout << "Error : " << IMG_GetError();
  }else{
    cout << "OK" << endl;
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, sdltex->w, sdltex->h, 0, 
		  GL_RGBA, GL_UNSIGNED_BYTE, sdltex->pixels);
  }
  //  if(hasAlpha)
  //     glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,texture);
  //   else
  //     glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,texture);
}

Texture::Texture(GLsizei w, GLsizei h, const GLfloat *texels)
{
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  
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

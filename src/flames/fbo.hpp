#ifndef FBO_H
#define FBO_H

#include "flames.hpp"

class FBO
{
public:
  FBO();
  ~FBO();

  void Initialize(unsigned int width, unsigned int height);
  
  void Attach(GLuint tex, unsigned int colorAttachment);

  void Activate( void )
  {
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_frameBuffer );
    //glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_depthRenderBuffer );
  }

  void Deactivate( void )
  {
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
//     glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );
  }

  unsigned int GetWidth()
  { return m_width; }
  
  unsigned int GetHeight()
  { return m_height; }
  
private:
  GLuint m_frameBuffer;
  GLuint m_depthRenderBuffer;
  
  unsigned int m_width;
  unsigned int m_height;
};

#endif

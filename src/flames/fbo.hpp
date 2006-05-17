#ifndef FBO_H
#define FBO_H

#include "flames.hpp"

class FBO
{
public:
  FBO();
  ~FBO();

  void Initialize(uint width, uint height);
  
  void Attach(GLuint tex, uint colorAttachment);

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

  uint GetWidth()
  { return m_width; }
  
  uint GetHeight()
  { return m_height; }
  
private:
  GLuint m_frameBuffer;
  GLuint m_depthRenderBuffer;
  
  uint m_width;
  uint m_height;
};

#endif

#ifndef FBO_H
#define FBO_H

#include "flames.hpp"

class FBO
{
public:
  FBO();
  virtual ~FBO();

  void Initialize();
  
  void DepthAttach(GLuint tex);
  void ColorAttach(GLuint tex, int colorAttachment);

  void RenderBufferAttach(void);

  void CheckStatus(void);

  void Activate( void )
  {
//     const GLenum buffers[] =
// 	{
// 	  GL_COLOR_ATTACHMENT0_EXT,
// 	  GL_COLOR_ATTACHMENT1_EXT
// 	};
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_frameBuffer );
    //glDrawBuffers( 2, buffers );
    //glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_depthRenderBuffer );
    //CheckStatus();
  }

  void Deactivate( void )
  {
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
    //glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );
    //CheckStatus();
  }

  uint GetWidth()
  { return m_width; }
  
  uint GetHeight()
  { return m_height; }
  
  void setSize(uint width, uint height)
  { m_width = width; m_height=height; }
  
private:
  GLuint m_frameBuffer;
  GLuint m_depthRenderBuffer;
  
  uint m_width;
  uint m_height;
};

#endif

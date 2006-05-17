#include "fbo.hpp"

FBO::FBO()
{
}

void FBO::Initialize(uint width, uint height)
{
  m_width = width;
  m_height = height;
  
  glGenFramebuffersEXT( 1, &m_frameBuffer );
  glGenRenderbuffersEXT( 1, &m_depthRenderBuffer );
}

void FBO::Attach(GLuint tex, uint colorAttachment)
{
  GLenum l_colorAttachment;
  
  /* A compléter par la suite, sachant que le nombre d'attachements possibles */
  /* est dépendant du driver */
  switch(colorAttachment){
  case 0 : l_colorAttachment = GL_COLOR_ATTACHMENT0_EXT; break;
  case 1 : l_colorAttachment = GL_COLOR_ATTACHMENT1_EXT; break;
  case 2 : l_colorAttachment = GL_COLOR_ATTACHMENT2_EXT; break;
  case 3 : l_colorAttachment = GL_COLOR_ATTACHMENT3_EXT; break;
  case 4 : l_colorAttachment = GL_COLOR_ATTACHMENT4_EXT; break;
  case 5 : l_colorAttachment = GL_COLOR_ATTACHMENT5_EXT; break;
  case 6 : l_colorAttachment = GL_COLOR_ATTACHMENT6_EXT; break;
  case 7 : l_colorAttachment = GL_COLOR_ATTACHMENT7_EXT; break;
  }
  
  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_frameBuffer );
  glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, l_colorAttachment, GL_TEXTURE_RECTANGLE_ARB, tex, 0 );

  glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_depthRenderBuffer );
  glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_width, m_height );
  glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthRenderBuffer );
  
  GLenum status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
  switch( status )
    {      
    case GL_FRAMEBUFFER_COMPLETE_EXT:
      //MessageBox(NULL,"GL_FRAMEBUFFER_COMPLETE_EXT!","SUCCESS",MB_OK|MB_ICONEXCLAMATION);
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
      cerr << "Error initializing frame buffer object : FRAMEBUFFER_UNSUPPORTED_EXT" << endl;
      exit(0);
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT :
      cerr << "Error initializing frame buffer object : FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << endl;
      exit(0);
      break;  
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT :
      cerr << "Error initializing frame buffer object : FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << endl;
      exit(0);
      break;  
    case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT :
      cerr << "Error initializing frame buffer object : FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT" << endl;
      exit(0);
      break;  
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
      cerr << "Error initializing frame buffer object : FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT" << endl;
      exit(0);
      break;  
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
      cerr << "Error initializing frame buffer object : FRAMEBUFFER_INCOMPLETE_FORMATS_EXT" << endl;
      exit(0);
      break;  
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
      cerr << "Error initializing frame buffer object : FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT" << endl;
      exit(0);
      break;  
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
      cerr << "Error initializing frame buffer object : FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT" << endl;
      exit(0);
      break; 
    default:
      cerr << "Error initializing frame buffer object : " << endl;
      exit(0);
    }
}


FBO::~FBO( void )
{
  glDeleteFramebuffersEXT( 1, &m_frameBuffer );
  glDeleteRenderbuffersEXT( 1, &m_depthRenderBuffer );
}


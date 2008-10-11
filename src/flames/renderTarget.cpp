#include "renderTarget.hpp"

#include <iostream>

RenderTarget::RenderTarget(uint width, uint height)
{
  glGenFramebuffersEXT( 1, &m_frameBuffer );
  m_hasDepthRenderBuffer = m_hasDepthTexture = false;
  m_width = width;
  m_height=height;
}


RenderTarget::RenderTarget(const string& format, uint width, uint height, uint texUnit, uint attachment)
{
  glGenFramebuffersEXT( 1, &m_frameBuffer );
  m_hasDepthRenderBuffer = m_hasDepthTexture = false;
  m_width = width;
  m_height=height;
  addTarget(format, texUnit, attachment);
  if (format.find("depthbuffer") != string::npos)
    addDepthRenderBuffer();
}

RenderTarget::~RenderTarget()
{
  glDeleteFramebuffersEXT( 1, &m_frameBuffer );
  if (m_hasDepthRenderBuffer) glDeleteRenderbuffersEXT( 1, &m_depthRenderBuffer );

  for (vector<Texture *>::iterator texturesIterator = m_vRenderTextures.begin ();
       texturesIterator != m_vRenderTextures.end (); texturesIterator++)
    delete *texturesIterator;
}


void RenderTarget::addTarget(const string& format, uint texUnit, int attachment)
{
  GLenum type, filter;
  GLint n;

  assert( !format.empty() );

  if (attachment==-1)
    attachment = texUnit;

  m_vTexUnits.push_back(texUnit);

  if (format.find("rect") != string::npos)
    type = GL_TEXTURE_RECTANGLE_ARB;
  else
    if (format.find("2D") != string::npos)
      type = GL_TEXTURE_2D;
    else
      if (format.find("cube") == string::npos)
        cerr << "(EE) Render target type error" << endl;

  if (format.find("nearest") != string::npos)
    filter = GL_NEAREST;
  else
    if (format.find("linear") != string::npos)
      filter = GL_LINEAR;
    else
      filter = GL_NEAREST;

//    glTexImage2D(m_type, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
//    case 0 : glTexImage2D(m_type, 0, GL_RGB32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, NULL); break;
//    case 1 : glTexImage2D(m_type, 0, GL_FLOAT_RG16_NV, width, height, 0, GL_FLOAT_RG16_NV, GL_UNSIGNED_BYTE, NULL); break;
//    case 2 : glTexImage2D(m_type, 0, GL_RGB10_A2, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); break;

  if (format.find("color") != string::npos)
  {
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &n);
    assert( m_vTexUnits.size() <= (uint)n);

    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_frameBuffer );
    if (format.find("rgba16f") != string::npos)
    {
      Texture *tex=new RenderTexture(type,filter,m_width,m_height, 0);
      m_vRenderTextures.push_back(tex);
      glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+attachment, type, tex->getTexture(), 0 );
    }
    else
      if (format.find("rgba") != string::npos)
      {
        Texture *tex=new RenderTexture(type,filter,m_width,m_height, 1);
        m_vRenderTextures.push_back(tex);
        glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+attachment, type, tex->getTexture(), 0 );
      }
      else
        if (format.find("rgb10a2") != string::npos)
        {
          Texture *tex=new RenderTexture(type,filter,m_width,m_height, 2);
          m_vRenderTextures.push_back(tex);
          glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+attachment, type, tex->getTexture(), 0 );
        }else
          if (format.find("cube") != string::npos)
          {
            Texture *tex=new CubeTexture(m_width,m_height);
            m_vRenderTextures.push_back(tex);
            for (uint j=0; j < 6; j++)
              glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+j,
                                         CubeTexture::s_cubeMapTarget[j], tex->getTexture(), 0 );
          }
          else
            cerr << "(EE) Render target format error" << endl;

    /** Activation du rendu dans tous les attachements
     *  Pour l'instant on le fait à chaque nouvelle cible pour éviter un appel supplémentaire
     */
    if (m_vTexUnits.size() > 1)
    {
      vector<GLenum> m_vAttachements;
      for (uint i=0; i < m_vTexUnits.size(); i++)
        m_vAttachements.push_back(GL_COLOR_ATTACHMENT0_EXT+i);
      glDrawBuffers(m_vAttachements.size(), &m_vAttachements[0] );
    }
  }else

    if (format.find("depth") != string::npos )
    {
      if (m_hasDepthRenderBuffer)
        cerr << "(EE) Can't bind depth to texture, since it is always bound to a render buffer" << endl;

      assert(m_hasDepthTexture == false);

      m_hasDepthTexture = true;

      bool shadow = format.find("shadow") != string::npos;

      bool customFunc = false;
      GLenum func;
      // Format comparison
      if(format.find("greater") != string::npos)
      {
          func = GL_GREATER;
          customFunc = true;
      }else
      if(format.find("equal") != string::npos)
      {
          func = GL_EQUAL;
          customFunc = true;
      }else
      if(format.find("less") != string::npos)
      {
          func = GL_LESS;
          customFunc = true;
      }else
      if(format.find("less") != string::npos)
      {
          func = GL_LESS;
          customFunc = true;
      }

      glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_frameBuffer );

        Texture *tex;
        if(customFunc)
            tex=new DepthTexture(type, m_width,m_height, filter, func);
        else
            tex=new DepthTexture(type, m_width,m_height, filter, shadow);

        m_vRenderTextures.push_back(tex);

        glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, type, tex->getTexture(), 0 );

        /** Si le nombre de texture est égal à 1 alors on suppose qu'on a qu'une depth texture */
        if(m_vRenderTextures.size() == 1)
        {
            glDrawBuffer (GL_NONE);
            glReadBuffer (GL_NONE);
        }

    }else
      cerr << "(EE) Render target format error : should be at least a depth or color texture." << endl;

  assert( CheckStatus() );
  bindDefaultTarget();
}


void RenderTarget::addDepthRenderBuffer(void)
{
  /** Attachement d'un render buffer pour la profondeur */
  m_hasDepthRenderBuffer = true;
  glGenRenderbuffersEXT( 1, &m_depthRenderBuffer );
  glBindFramebufferEXT(         GL_FRAMEBUFFER_EXT,  m_frameBuffer );
  glBindRenderbufferEXT(        GL_RENDERBUFFER_EXT, m_depthRenderBuffer );
  glRenderbufferStorageEXT(     GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24,    m_width, m_height );
  glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT,  GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthRenderBuffer );

  bindDefaultTarget();
}


bool RenderTarget::CheckStatus(void) const
{
  GLenum status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
  switch ( status )
  {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
      cerr << "(EE) Error initializing frame buffer object : FRAMEBUFFER_UNSUPPORTED_EXT" << endl;
      return false;
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT :
      cerr << "(EE) Error initializing frame buffer object : FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << endl;
      return false;
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT :
      cerr << "(EE) Error initializing frame buffer object : FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << endl;
      return false;
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
      cerr << "(EE) Error initializing frame buffer object : FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT" << endl;
      return false;
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
      cerr << "(EE) Error initializing frame buffer object : FRAMEBUFFER_INCOMPLETE_FORMATS_EXT" << endl;
      return false;
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
      cerr << "(EE) Error initializing frame buffer object : FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT" << endl;
      return false;
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
      cerr << "(EE) Error initializing frame buffer object : FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT" << endl;
      return false;
      break;
    default:
      cerr << "(EE) Error initializing frame buffer object : " << status << endl;
      return false;
  }
  return true;
}

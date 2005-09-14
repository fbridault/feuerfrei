#ifndef __PBUFFERS_H__
#define __PBUFFERS_H__

#if defined(WIN32)
#  include <windows.h>
#  include <GL/gl.h>
#  include <GL/wglext.h>
#  pragma warning (disable : 4786)
#elif defined(UNIX)
#  include <GL/glx.h>
#  include <GL/glxext.h>
#elif defined(MACOS)
#  include <AGL/agl.h>
#endif

#include <string>
#include <vector>

// The pixel format for the pbuffer is controlled by the mode string passed
// into the PBuffer constructor. This string can have the following attributes:
//
// rgb          - 8 bit rgb pixel format
// alpha        - must have alpha channel
// depth        - must have a depth buffer
// depth=n      - must have n-bit depth buffer
// stencil      - must have a stencil buffer
// double       - must support double buffered rendering
// samples=n    - must support n-sample antialiasing (n can be 2 or 4)
// float=n      - must support n-bit per channel floating point
// 
// texture2D
// textureRECT
// textureCUBE  - must support binding pbuffer as texture to specified target
//              - binding the depth buffer is also supporting by specifying
//                '=depth' like so: texture2D=depth or textureRECT=depth
//              - the internal format of the texture will be rgba by default or
//                float if pbuffer is floating point
//      
class PBuffer
{
 public:
  // see above for documentation on strMode format
  PBuffer(const char *strMode);
  ~PBuffer();

  bool Initialize(int iWidth, int iHeight, bool bShareContexts, bool bShareObjects);
  void Activate();
  void Deactivate();

#if defined(WIN32)
  void HandleModeSwitch();
#endif

#if defined(UNIX) || defined(WIN32)
  int Bind(int iBuffer);
  int Release(int iBuffer);
#endif

  inline int GetWidth()
    { return m_iWidth; }

  inline int GetHeight()
    { return m_iHeight; }

  inline bool IsSharedContext()
    { return m_bSharedContext; }

#if defined(UNIX) || defined(WIN32)
  inline bool IsTexture()
    { return m_bIsTexture; }
#endif

 protected:
#if defined(WIN32)
  HDC         m_hDC;     ///< Handle to a device context.
    HGLRC       m_hGLRC;   ///< Handle to a GL context.
      HPBUFFERARB m_hPBuffer;///< Handle to a pbuffer.

        HGLRC       m_hOldGLRC;
        HDC         m_hOldDC;
        
        std::vector<int> m_pfAttribList;
        std::vector<int> m_pbAttribList;

#elif defined(UNIX)
        Display    *m_pDisplay;
        GLXPbuffer  m_glxPbuffer;
        GLXContext  m_glxContext;

        Display    *m_pOldDisplay;
        GLXPbuffer  m_glxOldDrawable;
        GLXContext  m_glxOldContext;
        
        std::vector<int> m_pfAttribList;
        std::vector<int> m_pbAttribList;
#elif defined(MACOS)
        AGLContext  m_context;
        WindowPtr   m_window;
        std::vector<int> m_pfAttribList;
#endif

        int m_iWidth;
        int m_iHeight;
    
        const char *m_strMode;
        bool m_bSharedContext;
        bool m_bShareObjects;

 private:
        std::string getStringValue(std::string token);
        int getIntegerValue(std::string token);
#if defined(UNIX) || defined(WIN32)        
        void parseModeString(const char *modeString, std::vector<int> *pfAttribList, std::vector<int> *pbAttribList);

        bool m_bIsBound;
        bool m_bIsActive;
	
	bool m_bIsTexture;
#endif
};

#endif

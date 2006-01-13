#include "interface/mainFrame.hpp"

class FlamesApp : public wxApp
{
    virtual bool OnInit();
};

IMPLEMENT_APP(FlamesApp)

bool FlamesApp::OnInit()
{
  bool recompileShaders=false;
  /* Déclaration des handlers pour la gestion des formats d'image */
	 wxImage::AddHandler(new wxPNGHandler);
  wxImage::AddHandler(new wxJPEGHandler);
  
  MainFrame *frame = new MainFrame( _("Real-time Animation of small Flames"), wxDefaultPosition, wxSize(1060,860) );
 
  frame->Show(TRUE);
  
  if(argc == 2){
    if(wxString(argv[1]) == _("--recompile"))
      recompileShaders = true;
  }


  SetTopWindow(frame);
 
  frame->InitGLBuffer(recompileShaders);
	
  return TRUE;
}

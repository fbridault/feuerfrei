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
    
  if(argc == 2){
    if(wxString(argv[1]) == _("--recompile"))
      recompileShaders = true;
  }
  
  MainFrame *frame = new MainFrame( _("Real-time Animation of small Flames - param.ini"), wxDefaultPosition, wxSize(1060,860), _("param.ini") );
 
  frame->Show(TRUE);

  SetTopWindow(frame);
 
  frame->InitGLBuffer(recompileShaders);
	
  return TRUE;
}

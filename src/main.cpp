#include "interface/mainFrame.hpp"

class FlamesApp : public wxApp
{
  virtual bool OnInit();
  bool areShadersCompiled();
};

IMPLEMENT_APP(FlamesApp)

bool FlamesApp::areShadersCompiled()
{
  bool ret=true;
  
  if(wxFile::Exists(_("vertGlowX.o")))
    if(wxFile::Exists(_("vertGlowY.o")))
      if(wxFile::Exists(_("fragGlow.o")))
	if(wxFile::Exists(_("SVExtrude.o")))
	  if(wxFile::Exists(_("vpSPTEX.o")))
	    ret = false;
  return ret;
}

bool FlamesApp::OnInit()
{
  bool recompileShaders;
  wxString configFileName;
  
  /* Déclaration des handlers pour la gestion des formats d'image */
  wxImage::AddHandler(new wxPNGHandler);
  wxImage::AddHandler(new wxJPEGHandler);
  
  if(argc == 2){
    configFileName = wxString(argv[1]);
  }else
    configFileName = _("param.ini");

  if( !wxFile::Exists(configFileName) ){
    cerr << "File " << configFileName.fn_str() << " doesn't exist." << endl << "Exiting..." << endl;
    return false;
  }    
  
  recompileShaders = areShadersCompiled();
  
  /** Teste s'il est nécessaire de recompiler les shaders */
  MainFrame *frame = new MainFrame( _("Real-time Animation of small Flames - ")+configFileName, wxDefaultPosition, wxDefaultSize, configFileName );
 
  frame->Show(TRUE);

  SetTopWindow(frame);
 
  frame->InitGLBuffer(recompileShaders);
	
  return true;
}

#include "interface/flamesFrame.hpp"

/** \mainpage Index page
 *
 * \section intro_sec Introduction
 *
 * This is the introduction.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Opening the box
 *  
 * etc...
 */

/** \todo commenter... */

/** Classe Principale.
 * Classe principale charg�e de l'analyse des arguments de la ligne de commande et du lancement
 * de la fen�tre principale de l'application.
*/
class FlamesApp : public wxApp
{
  /** M�thode d'initialisation de l'application 
   */
  virtual bool OnInit();
  
  /** D�termine si les shaders Cg sont d�j� compil�s
   * @return false si les shaders sont compil�s
   */
  bool areShadersCompiled();
};

IMPLEMENT_APP(FlamesApp)

bool FlamesApp::areShadersCompiled()
{
  if(wxFile::Exists(wxString::Format(_("%s%s"),SHADERS_OBJECTS_PATH, _("vertGlowX.o"))))
    if(wxFile::Exists(wxString::Format(_("%s%s"),SHADERS_OBJECTS_PATH, _("vertGlowY.o"))))
      if(wxFile::Exists(wxString::Format(_("%s%s"),SHADERS_OBJECTS_PATH, _("fragGlow.o"))))
	if(wxFile::Exists(wxString::Format(_("%s%s"),SHADERS_OBJECTS_PATH, _("SVExtrude.o"))))
	  if(wxFile::Exists(wxString::Format(_("%s%s"),SHADERS_OBJECTS_PATH, _("vpSPTEX.o"))))
	     return false;
  cerr << "Cg shaders are not compiled yet, that will be done during this run." << endl;
  return true;
}

bool FlamesApp::OnInit()
{
  bool recompileShaders;
  wxString configFileName;
  
  /* D�claration des handlers pour la gestion des formats d'image */
  wxImage::AddHandler(new wxPNGHandler);
  wxImage::AddHandler(new wxJPEGHandler);
  
  if(argc == 2){
    configFileName = wxString(argv[1]);
  }else
    configFileName = _("params/param.ini");

  if( !wxFile::Exists(configFileName) ){
    cerr << "File " << configFileName.fn_str() << " doesn't exist." << endl << "Exiting..." << endl;
    return false;
  }
  
  recompileShaders = areShadersCompiled();
  
  /* Teste s'il est n�cessaire de recompiler les shaders */
  FlamesFrame *frame = new FlamesFrame( _("Real-time Animation of small Flames - ")+configFileName, wxDefaultPosition, wxDefaultSize, configFileName );
 
  frame->Show(TRUE);

  SetTopWindow(frame);
 
  frame->InitGLBuffer(recompileShaders);
	
  return true;
}
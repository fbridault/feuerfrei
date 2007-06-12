#include "interface/fluidsFrame.hpp"

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
class FluidsApp : public wxApp
{
  /** M�thode d'initialisation de l'application 
   */
  virtual bool OnInit();
};

IMPLEMENT_APP(FluidsApp)

bool FluidsApp::OnInit()
{
  wxString configFileName;
    
  setlocale(LC_NUMERIC, "C");
  if(argc == 2){
    configFileName = wxString(argv[1]);
  }else
    configFileName = _("params/solver.slv");

  if( !wxFile::Exists(configFileName) ){
    cerr << "File " << configFileName.fn_str() << " doesn't exist." << endl << "Exiting..." << endl;
    return false;
  }
  
  FluidsFrame *frame = new FluidsFrame( _("Real-time Fluids - ")+configFileName, wxDefaultPosition, wxDefaultSize, configFileName );
  
  frame->Show(TRUE);

  SetTopWindow(frame);
  frame->InitGLBuffer();
  
  return true;
}

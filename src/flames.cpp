#include "interface/flamesFrame.hpp"

#include <wx/tooltip.h>
#include <engine/Init.hpp>

/** \mainpage Index page
 *
 * Bienvenue sur la doc de rtflames et rtfluids !
 *
 * \section Instructions
 *
 * Mise à jour de l'archive :
 * cg-update
 *
 * Compilation de rtflames:
 * make flames
 * Compilation de rtfluids:
 * make fluids
 *
 * Compilation des deux cibles:
 * make
 *
 * Compilation d'une version de déboguage :
 * make build
 * ccmake build
 * et remplir le champ CMAKE_BUILD_TYPE avec Debug
 * ou
 * cmake -i build
 * et répondre Debug pour le type de build
 *
 * Lancement :
 * rtflames <fichier de simulation> (par défaut params/param.ini)
 * rtfluids <fichier de simulation> (par défaut params/solver.ini)
 *
 * L'option --recompile n'est plus nécessaire, le programme détermine lui-même si
 * il faut recompiler ou non les shaders.
 *
 * Soumission des changements :
 * make clean
 * cg-push
 *
 * \section consignes Consignes
 *
 * Lorsque vous ajoutez ou modifiez des classes, merci de préfixer toutes les variables membres
 * par 'm_'. Ceci facilite grandement la lecture du code.
 *
 * Merci également d'essayer de faire des commits le plus petit possible de manière à faciliter les fusions.
 */

/** \todo commenter... */

/** Classe Principale.
 * Classe principale chargée de l'analyse des arguments de la ligne de commande et du lancement
 * de la fenêtre principale de l'application.
*/
wxApp *thisApp;

class FlamesApp : public wxApp
{
	/** Méthode d'initialisation de l'application
	 */
	virtual bool OnInit();
};

IMPLEMENT_APP(FlamesApp)

bool FlamesApp::OnInit()
{
	//  bool recompileShaders;
	wxString configFileName;

	setlocale(LC_NUMERIC, "C");
	/* Déclaration des handlers pour la gestion des formats d'image */
	wxImage::AddHandler(new wxPNGHandler);
	wxImage::AddHandler(new wxJPEGHandler);

	wxIdleEvent::SetMode(wxIDLE_PROCESS_SPECIFIED);

	if (argc == 2)
		configFileName = wxString(argv[1]);
	else
		configFileName = _("params/param.ini");

	if ( !wxFile::Exists(configFileName) )
	{
		cerr << "File " << configFileName << " doesn't exist." << endl << "Exiting..." << endl;
		return false;
	}

	initEngine();

	/* Teste s'il est nécessaire de recompiler les shaders */
	FlamesFrame *frame = new FlamesFrame( 	_("Real-time Animation of small Flames - ") + configFileName,
	                                       wxDefaultPosition,
	                                       wxDefaultSize,
	                                       configFileName );

	wxToolTip::Enable(true);

	frame->Show(TRUE);

	SetTopWindow(frame);
	frame->InitGLBuffer();

	return true;
}

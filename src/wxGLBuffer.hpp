#ifndef WXGLBUFFER_H
#define WXGLBUFFER_H

class wxGLBuffer;

#include "header.h"

#include <wx/glcanvas.h>

#include "CgSVShader.hpp"
#include "eyeball.hpp"

#include "scene.hpp"
#include "solidePhoto.hpp"
#include "graphicsFn.hpp"
#include "bougie.hpp"
#include "firmalampe.hpp"
#include "solver.hpp"
#include "benchsolver.hpp"
#include "glowengine.hpp"


class wxGLBuffer : public wxGLCanvas
{
public:
  wxGLBuffer() {}
  wxGLBuffer(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int* attribList = 0,  
	     long style=0, const wxString& name=_("GLCanvas"), const wxPalette& palette = wxNullPalette);
  virtual ~wxGLBuffer();
  
  void OnIdle(wxIdleEvent& event);
  void OnPaint(wxPaintEvent& event);
  
  /** Défini l'action à effectuer lorsque la souris se déplace */
  void OnMouseMotion(wxMouseEvent& event);
  /** Défini l'action à effectuer lorsqu'un bouton de la souris est enfoncé */
  void OnMouseClick(wxMouseEvent& event);
    
  /** Initialisation globale du contrôle */
  void Init(int l, int h, int solvx, int solvy, int solvz, double timeStep, 
	    char *scene_name, char *meche_name, double clipping);
  /** Initialisation des variables d'affichage de l'interface */
  void InitUISettings();

  /** Lance/arrête l'animation */
  void ToggleRun(void) { animate=!animate; };
  /** Lance/arrête l'animation */
  void ToggleFlickering(void) { flickering=!flickering; };
  /** Active/Désactive le solide photométrique */
  void ToggleSP(void) { solidePhotoEnabled=!solidePhotoEnabled; };
  /** Active/Désactive le glow */
  void ToggleGlow(void) { glowEnabled=!glowEnabled; };

private:
  void WriteFPS ();
  void DrawVelocity (void);
  
  /********* Variables relatives au contrôle de l'affichage **************/
  bool animate, affiche_velocite, affiche_repere, affiche_grille,
    affiche_flamme, affiche_fps, flickering, shadowsEnabled,
    shadowVolumesEnabled, affiche_particules, glowEnabled, glowOnly;
  bool init, done;

  /********* Variables relatives à la fenêtre d'affichage ****************/
  int largeur, hauteur;
  
  CGcontext context;
  Eyeball *eyeball;
  /* Pour le compte des frames */
  int Frames;
  int t;
  
  /********* Variables relatives aux solides photométriques **************/
  SolidePhotometrique *solidePhoto;
  bool solidePhotoEnabled, rotation,modePano,pixelBFC;
  /* interpolationSP = 0 ou 1; couleurOBJ = 0 ou 2 */
  unsigned char interpolationSP, couleurOBJ;

  /********* Variables relatives au glow *********************************/
  GlowEngine *glowEngine;
  GlowEngine *glowEngine2;

  /********* Variables relatives au solveur ******************************/
  Solver *solveur;
    
  /********* Variables relatives à la simulation *************************/
  Flame **flammes;
  int nb_flammes;
  CScene *scene;
  CgSVShader *SVShader;

  DECLARE_EVENT_TABLE()
};

#endif

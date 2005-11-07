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
  
  /** D�fini l'action � effectuer lorsque la souris se d�place */
  void OnMouseMotion(wxMouseEvent& event);
  /** D�fini l'action � effectuer lorsqu'un bouton de la souris est enfonc� */
  void OnMouseClick(wxMouseEvent& event);
    
  /** Initialisation globale du contr�le */
  void Init(flameAppConfig *config);
  /** Initialisation des variables d'affichage de l'interface */
  void InitUISettings();

  /** Lance/arr�te l'animation */
  void ToggleRun(void) { animate=!animate; };
  /** Lance/arr�te l'animation */
  void ToggleFlickering(void) { flickering=!flickering; };
  /** Active/D�sactive le solide photom�trique */
  void ToggleSP(void) { solidePhotoEnabled=!solidePhotoEnabled; };
  /** Active/D�sactive le glow */
  void ToggleGlow(void) { glowEnabled=!glowEnabled; };
  void ToggleGridDisplay(void) { affiche_grille=!affiche_grille; };
  void ToggleBaseDisplay(void) { affiche_repere=!affiche_repere; };
  void ToggleVelocityDisplay(void) { affiche_velocite=!affiche_velocite; };
  void ToggleParticlesDisplay(void) { affiche_particules=!affiche_particules; };
  void ToggleFlamesDisplay(void) { affiche_flamme=!affiche_flamme; };
  void ToggleSmoothShading(void) { 
    for (int f = 0; f < nb_flammes; f++)
    flammes[f]->toggleSmoothShading (); };
  void ToggleBlendedSP(void) { couleurOBJ = 2-couleurOBJ; };
  void ToggleInterpolationSP(void) { interpolationSP = 1-interpolationSP; };

  void Swap(void) { solidePhoto->swap(); };

private:
  void WriteFPS ();
  void DrawVelocity (void);
  
  /********* Variables relatives au contr�le de l'affichage **************/
  bool animate, affiche_velocite, affiche_repere, affiche_grille,
    affiche_flamme, affiche_fps, flickering, shadowsEnabled,
    shadowVolumesEnabled, affiche_particules, glowEnabled, glowOnly;
  bool init, done;

  /********* Variables relatives � la fen�tre d'affichage ****************/
  int largeur, hauteur;
  
  CGcontext context;
  Eyeball *eyeball;
  /* Pour le compte des frames */
  int Frames;
  int t;
  
  /********* Variables relatives aux solides photom�triques **************/
  SolidePhotometrique *solidePhoto;
  bool solidePhotoEnabled, rotation,modePano,pixelBFC;
  /* interpolationSP = 0 ou 1; couleurOBJ = 0 ou 2 */
  unsigned char interpolationSP, couleurOBJ;

  /********* Variables relatives au glow *********************************/
  GlowEngine *glowEngine;
  GlowEngine *glowEngine2;

  /********* Variables relatives au solveur ******************************/
  Solver *solveur;
    
  /********* Variables relatives � la simulation *************************/
  Flame **flammes;
  int nb_flammes;
  CScene *scene;
  CgSVShader *SVShader;

  DECLARE_EVENT_TABLE()
};

#endif

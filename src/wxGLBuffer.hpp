#include "header.hpp"
#include <wx/glcanvas.h>



class wxGLBuffer : public wxGLCanvas
{
public:
  wxGLBuffer(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int* attribList = 0,  
	     long style=0, const wxString& name=_("GLCanvas"), const wxPalette& palette = wxNullPalette);
  virtual ~wxGLBuffer();
  
  void OnIdle(wxIdleEvent& event);
  void OnPaint(wxPaintEvent& event);
  
  void InitUI();
  
  void ToggleRun(void)
  {
    animate=!animate;
  };
  
  DECLARE_EVENT_TABLE()

private:
  void WriteFPS ();
  void DrawVelocity (void);

  /********* Variables relatives au contrôle de l'affichage **************/
  bool init;
  bool animate, affiche_velocite, affiche_repere, affiche_grille,
    affiche_flamme, affiche_fps, brintage, shadowsEnabled,
    shadowVolumesEnabled, affiche_particules, glowEnabled, glowOnly;
  int done=0;

  /********* Variables relatives à la fenêtre d'affichage ****************/
  int largeur, hauteur;
  double clipping;
  CGcontext context;
  Eyeball *eyeball;
  /* Pour le compte des frames */
  GLint Frames;
  GLint T0 = 0;
  char strfps[4];

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
  float pas_de_temps;
  int solvx, solvy, solvz;
  double dim_x, dim_y, dim_z;
  
  /********* Variables relatives à la simulation *************************/
  char scene_name[255], meche_name[255];
  Flame **flammes;
  int nb_flammes = 1;
  CScene *scene;
  CgSVShader *SVShader;
};

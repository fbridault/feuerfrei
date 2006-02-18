#if !defined(INTERFACECOMMON_H)
#define INTERFACECOMMON_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "../vector.hpp"

/* Types de flammes */
#define BOUGIE 0
#define FIRMALAMPE 1

/* Types de solveurs */
#define GS_SOLVER 0
#define GCSSOR_SOLVER 1
#define HYBRID_SOLVER 2
#define LOGRES_SOLVER 3
#define LOGRESAVG_SOLVER 4
#define LOGRESAVGTIME_SOLVER 5

#define NB_MAXSOLVERS 10
#define NB_MAXFLAMMES 20

#define LIGHTING_STANDARD 0
#define LIGHTING_PHOTOMETRIC 1

class SolverConfig{
public:
  char type;
  CPoint position;
  int resx, resy, resz;
  double dim;
  double timeStep;
  double omegaDiff, omegaProj;
  double epsilon;
  int nbMaxIter;
  double buoyancy;
};
class FlameConfig{
public:
  char type;
  int solverIndex;
  /** La position de la flamme est RELATIVE par rapport au solveur,
   *  il s'agit d'une fraction de la dimension du solveur 
   */
  CPoint position;
  wxString wickName;
  int skeletonsNumber;
  double innerForce;
  /* O non, 1 vertical, 2 vers la gauche */
  char flickering;
};

class FlameAppConfig{
public:
  int width, height;
  double clipping;
  wxString sceneName;
  /* LIGHTING_STANDARD normal, LIGHTING_PHOTOMETRIC pour solides photmétriques */ 
  int lightingMode;
  bool shadowsEnabled;
  bool glowEnabled;
  /* IPSEnabled = 0 ou 1; BPSEnabled = 0 ou 2 */
  int IPSEnabled, BPSEnabled;
  int nbSolvers;
  SolverConfig *solvers;
  int nbFlames;
  FlameConfig *flames;
  double fatness[4];
  double extrudeDist[4];
};

#endif

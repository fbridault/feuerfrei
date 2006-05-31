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
#define CANDLE 0
#define FIRMALAMPE 1
#define TORCH 2
#define CAMPFIRE 3

/* Types de solveurs */
#define GS_SOLVER 0
#define GCSSOR_SOLVER 1
#define HYBRID_SOLVER 2
#define LOD_HYBRID_SOLVER 3
#define LOGRES_SOLVER 4
#define LOGRESAVG_SOLVER 5
#define LOGRESAVGTIME_SOLVER 6

#define NB_MAXSOLVERS 10
#define NB_MAXFLAMMES 20

#define LIGHTING_STANDARD 0
#define LIGHTING_PHOTOMETRIC 1

class SolverConfig{
public:
  char type;
  Point position;
  uint resx, resy, resz;
  double dim;
  double timeStep;
  double omegaDiff, omegaProj;
  double epsilon;
  uint nbMaxIter;
  double buoyancy;
};
class FlameConfig{
public:
  char type;
  int solverIndex;
  /** La position de la flamme est RELATIVE par rapport au solveur,
   *  il s'agit d'une fraction de la dimension du solveur 
   */
  Point position;
  wxString wickName;
  int skeletonsNumber;
  double innerForce;
  /* O non, 1 vertical, 2 vers la gauche */
  char flickering;
  int fdf;
  double samplingTolerance;
};

class FlameAppConfig{
public:
  uint width, height;
  double clipping;
  wxString sceneName;
  /* LIGHTING_STANDARD normal, LIGHTING_PHOTOMETRIC pour solides photmétriques */ 
  int lightingMode;
  bool shadowsEnabled;
  bool glowEnabled;
  /* IPSEnabled = 0 ou 1; BPSEnabled = 0 ou 2 */
  int IPSEnabled, BPSEnabled;
  uint nbSolvers;
  SolverConfig *solvers;
  uint nbFlames;
  FlameConfig *flames;
  double fatness[4];
  double extrudeDist[4];
};

#endif

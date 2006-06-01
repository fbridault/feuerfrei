#if !defined(COMMON_H)
#define COMMON_H

#include <iostream>

#include "vector.hpp"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

using namespace std;

#define UNDEFINED -10

#define AS_ERROR(ret,str) {if(ret==-1) perror(str);}

#define PI 3.14159265358979323846

#define FLICKERING_NONE 0
#define FLICKERING_VERTICAL 1
#define FLICKERING_RIGHT 2
#define FLICKERING_RANDOM 3

#define FDF_LINEAR 0
#define FDF_EXPONENTIAL 1
#define FDF_GAUSS 2
#define FDF_RANDOM 3

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
  uint skeletonsNumber;
  double innerForce;
  /** O non, 1 vertical, 2 vers la gauche */
  char flickering;
  int fdf;
  double samplingTolerance;
  /** Durée de vie des squelettes */
  uint leadLifeSpan, periLifeSpan;
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
  bool depthPeelingEnabled;
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

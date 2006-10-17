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

#define PARAMS_DIRECTORY _("/params")
#define SCENES_DIRECTORY _("/scenes")
#define TEXTURES_DIRECTORY _("/textures")
#define SHADERS_OBJECTS_PATH _("build/shadersObjects")

#define AS_ERROR(ret,str) {if(ret==-1) perror(str);}

#define PI 3.14159265358979323846

#define FLICKERING_NONE 0
#define FLICKERING_VERTICAL 1
#define FLICKERING_RIGHT 2
#define FLICKERING_RANDOM 3

#define FDF_LINEAR 0
#define FDF_BILINEAR 1
#define FDF_EXPONENTIAL 2
#define FDF_GAUSS 3
#define FDF_RANDOM 4

/** Configuration d'un solveur de fluide */
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

/** Configuration d'une source lumineuse de type flamme */
class FlameConfig{
public:
  char type;
  int solverIndex;
  /** La position de la flamme est RELATIVE par rapport au solveur,
   *  il s'agit d'une fraction de la dimension du solveur 
   */
  Point position;
  /** Nom du fichier représentant la ou les mèches */
  wxString wickName;
  uint skeletonsNumber;
  double innerForce;
  /** O non, 1 vertical, 2 vers la gauche */
  char flickering;
  int fdf;
  double samplingTolerance;
  /** Durée de vie des squelettes */
  uint leadLifeSpan, periLifeSpan;
  /** Chemin vers le fichier IES utilsé pour le soldie photométrique */
  wxString IESFileName;
  /** Indique si le feu peut générer des flammes détachées */
  bool breakable;
  /** Coefficient pour l'intensité lumineuse de la source */
  double intensityCoef;
};

class FlameAppConfig{
public:
  uint width, height;
  double clipping;
  wxString sceneName;
  /** LIGHTING_STANDARD normal, LIGHTING_PHOTOMETRIC pour solides photmétriques */ 
  int lightingMode;
  bool shadowsEnabled;
  /** Activation du glow */
  bool glowEnabled;
  /** Activation du Depth Peeling */
  bool depthPeelingEnabled;
  /** Nombre de calques pour le Depth Peeling, compris entre 0 et DEPTH_PEELING_LAYERS_MAX */
  uint nbDepthPeelingLayers;
  /** IPSEnabled = 0 ou 1; BPSEnabled = 0 ou 2 */
  int IPSEnabled, BPSEnabled;
  uint nbSolvers;
  SolverConfig *solvers;
  uint nbFlames;
  FlameConfig *flames;
  double fatness[4];
  double extrudeDist[4];
};

class FluidsAppConfig{
public:
  uint width, height;
  double clipping;
  uint nbSolvers;
  SolverConfig *solvers;
};

#endif

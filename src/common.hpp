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
#define SHADERS_OBJECTS_PATH _("build/shadersObjects/")

#define AS_ERROR(ret,str) {if(ret==-1) perror(str);}

#define PI 3.14159265358979323846f

#define FLICKERING_NONE 0
#define FLICKERING_VERTICAL 1
#define FLICKERING_RANDOM1 2
#define FLICKERING_RANDOM2 3
#define FLICKERING_NOISE 4

#define FDF_LINEAR 0
#define FDF_BILINEAR 1
#define FDF_EXPONENTIAL 2
#define FDF_GAUSS 3
#define FDF_RANDOM 4

/** Configuration d'un solveur de fluide */
class SolverConfig{
public:
#ifdef RTFLUIDS_BUILD
  Point position;
#endif
  char type;
  uint resx, resy, resz;
  /* Dimension r�elle du solveur, utilis�e lors de la r�solution */
  float dim;
  /* Facteur d'�chelle */
  Point scale;
  float timeStep;
  float omegaDiff, omegaProj;
  float epsilon;
  uint nbMaxIter;
  float buoyancy;
  float vorticityConfinement;
};

#ifdef RTFLAMES_BUILD

/* Propri�t�s des flammes */
class FlameConfig{
public:
  /** Type de flamme utilis� pour le luminaire. */
  char type;
  uint skeletonsNumber;
  float innerForce;
  char flickering;
  int fdf;
  float samplingTolerance;
  /** Dur�e de vie des squelettes. */
  uint leadLifeSpan, periLifeSpan;
  /** Chemin vers le fichier IES utils� pour le solide photom�trique. */
  wxString IESFileName;
};

/** Configuration d'un luminaire. Un luminaire est charg� � partir d'un fichier OBJ et contient
 * �ventuellement des m�ches. Il d�termine donc le placement des champs de v�locit� et des flammes.
 * A chaque luminaire est associ� un seul type de champ. Si le luminaire entra�ne la cr�ation de plusieurs
 * champs, ils auront tous les m�mes propri�t�s. Idem pour les flammes.
 */
class LuminaryConfig{
public:
  Point position;
  /** Nom du fichier contenant le luminaire et les m�ches. */
  wxString fileName;
  uint nbFields;
  SolverConfig *fields;
  uint nbFires;
  FlameConfig *fires;
};

class FlameAppConfig{
public:
  uint width, height;
  float clipping;
  wxString sceneName;
  /** LIGHTING_STANDARD normal, LIGHTING_PHOTOMETRIC pour solides photm�triques */ 
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
  uint nbLuminaries;
  LuminaryConfig *luminaries;
  bool useGlobalField;
  SolverConfig globalField;
  GLfloat fatness[4];
  GLfloat extrudeDist[4];
  float gammaCorrection;
};

#else

class FluidsAppConfig{
public:
  uint width, height;
  float clipping;
  uint nbSolvers;
  SolverConfig *solvers;
};

#endif

#endif

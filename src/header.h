#if !defined(HEADER_H)
#define HEADER_H

#include <stdio.h>
#include <math.h>
#include <values.h>
#include <iostream>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "mathFn.hpp"

using namespace std;

#define FALSE 0
#define TRUE 1

#define UNDEFINED -10

#define NB_PARTICULES 8

#define FPS .04

#define COEFFICIENT_ROTATION_QUATERNION 1.0

#define SWAP(x0,x) {double *tmp=x0;x0=x;x=tmp;}
#define AS_ERROR(ret,str) {if(ret==-1) perror(str);}

#define ALL      0
#define TEXTURED 1
#define FLAT    -1

#define BOUGIE 1
#define FIRMALAMPE 2

// Pour le calcul du dégradé de couleur des pyramides
#define VELOCITE_MAX .2

#define EPSILON                 1.0e-8
#define COMPOSANTES             4
#define RED                     0
#define GREEN                   1
#define BLUE                    2

#define SHADOW_SAMPLE_PER_LIGHT 3

#define BOUGIE 1
#define FIRMALAMPE 2

#include "vector.hpp"

class FlameConfig{
public:
  char type;
  CPoint position;
  wxString wickName;
};

class FlameAppConfig{
public:
  int solvx, solvy, solvz;
  double timeStep;
  int width, height;
  double clipping;
  wxString sceneName,mecheName;
  bool PSEnabled,glowEnabled;
  /* IPSEnabled = 0 ou 1; BPSEnabled = 0 ou 2 */
  int IPSEnabled, BPSEnabled;
  int nbFlames;
  FlameConfig *flames;
};

#endif

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

#define BOUGIE

#ifdef BOUGIE
#define LIFE_SPAN_AT_BIRTH 6
#else
#define LIFE_SPAN_AT_BIRTH 4
#endif

#define FPS .04

#define COEFFICIENT_ROTATION_QUATERNION 1.0

#define SWAP(x0,x) {double *tmp=x0;x0=x;x=tmp;}
#define AS_ERROR(ret,str) {if(ret==-1) perror(str);}

/************************* Display lists *********************/
#define REPERE 1
#define GRILLE 2
/* Tous les objets de la sc√®ne */
#define SCENE_OBJECTS 3
/* Objets textur√©s */
#define SCENE_OBJECTS_TEX 4
/* Objets sans textures */
#define SCENE_OBJECTS_WTEX 5
/* Objets textur√©s  qui projettent des ombres */
#define SCENE_OBJECTS_WSV_TEX 6
/* Objets sans textures qui projettent des ombres */
#define SCENE_OBJECTS_WSV_WTEX 7
/* Tous les objets qui projettent des ombres */
#define SCENE_OBJECTS_WSV 8
/* Tous les objets qui projettent des ombres, sans les textures Èventuelles */
#define SCENE_OBJECTS_WSV_WT 9
/* Tous les objets de la scËne sans les textures Èventuelles */
#define SCENE_OBJECTS_WT 10

#define CYLINDRE 12
#define MECHE 15
/*************************************************************/

// Pour le calcul du d√©grad√© de couleur des pyramides
#define VELOCITE_MAX .2

#define EPSILON                 1.0e-8
#define COMPOSANTES             4
#define RED                     0
#define GREEN                   1
#define BLUE                    2


#define SHADOW_SAMPLE_PER_LIGHT 3

class flameAppConfig{
public:
  int solvx, solvy, solvz;
  double timeStep;
  int width, height;
  double clipping;
  wxString sceneName,mecheName;
  bool PSEnabled,glowEnabled;
  /* IPSEnabled = 0 ou 1; BPSEnabled = 0 ou 2 */
  int IPSEnabled, BPSEnabled;
};

#include "vector.hpp"

#endif

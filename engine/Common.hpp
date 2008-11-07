#ifndef ENGINE_COMMON_H
#define ENGINE_COMMON_H

#include <GL/gl.h>
#include <GL/glu.h>

#include "assert.h"
#include "stdlib.h"
#include <string>

using namespace std;

typedef unsigned int uint;

#define OBJECT_TEX_UNIT     0
#define SHADOW_MAP_TEX_UNIT 3

#define RAD_TO_DEG 180.0/M_PI
#define DEG_TO_RAD M_PI/180.0

// TEMP
extern GLfloat g_modelViewMatrix[16];

#define rThis *this

typedef char * CharPtr;
typedef char const * CharCPtr;
typedef char * const CharPtrC;
typedef char const * const CharCPtrC;

#endif

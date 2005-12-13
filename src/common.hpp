#if !defined(COMMON_H)
#define COMMON_H

#include <iostream>

using namespace std;

#define UNDEFINED -10

#define AS_ERROR(ret,str) {if(ret==-1) perror(str);}

#define EPSILON                 1.0e-8
#define PI 3.14159265358979323846

#endif

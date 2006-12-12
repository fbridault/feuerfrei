#include "noise.hpp"
#include <math.h>

float Noise::Noise1(int x)
{
  x = (x<<13) ^ x;
  return ( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);    
}

float Noise::SmoothedNoise1(float x)
{
  return Noise1(x)/2  +  Noise1(x-1)/4  +  Noise1(x+1)/4;
}

float Noise::Cosine_Interpolate(float a, float b, float x)
{
  float ft = x * 3.1415927;
  float f = (1 - cos(ft)) * .5;
    
  return  a*(1-f) + b*f;
}

float Noise::InterpolatedNoise1(float x)
{
  int integer_X = (int)x;
  float fractional_X = x - integer_X;
    
  return Cosine_Interpolate(SmoothedNoise1(integer_X) , SmoothedNoise1(integer_X + 1) , fractional_X);
}

float Noise::PerlinNoise1D(float x)
{
  float total = 0;
  int p = .5;
  int n = 8;
  int i;
  float frequency, amplitude;

  for (i=0; i < n ; i++){
    frequency = powl(2,i);
    amplitude = powl(p,i);

    /* A priori il faut écrire différentes fonctions de génération de bruit */
    total = total + InterpolatedNoise1(x * frequency) * amplitude;
  }

  return total;
}

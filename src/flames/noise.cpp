#include "noise.hpp"
#include <math.h>
#include <stdlib.h>
#include <iostream>

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
PerlinNoise1D::PerlinNoise1D(float increment, float min, float max)
{
  m_increment = increment;

  /* On suppose que max est supérieur à min */
  m_coef = (max - min)/2.0;
  m_offset = min + m_coef;

  m_persistence = .5;
  m_nbOctaves = 8;
  //  m_currentX = 0;
  /* On donne un germe différent pour chaque générateur */
  m_currentX = 100* (rand()/((float)RAND_MAX));
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
float PerlinNoise1D::Noise1(int x)
{
  x = (x<<13) ^ x;
  return ( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
float PerlinNoise1D::SmoothedNoise1(float x)
{
  return Noise1((int)x)/2  +  Noise1((int)(x-1))/4  +  Noise1((int)(x+1))/4;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
float PerlinNoise1D::Cosine_Interpolate(float a, float b, float x)
{
  float ft = x * 3.1415927;
  float f = (1 - cos(ft)) * .5;

  return  a*(1-f) + b*f;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
float PerlinNoise1D::InterpolatedNoise1(float x)
{
  int integer_X = (int)x;
  float fractional_X = x - integer_X;

  return Cosine_Interpolate(SmoothedNoise1(integer_X) , SmoothedNoise1(integer_X + 1) , fractional_X);
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
float PerlinNoise1D::getNextValue()
{
  float total = 0;
  uint i;
  float frequency, amplitude;

  m_currentX += m_increment;

  for (i=0; i < m_nbOctaves ; i++){
    frequency = pow(2,i);
    amplitude = pow(m_persistence,i);

    /* A priori il faut écrire différentes fonctions de génération de bruit */
    total = total + InterpolatedNoise1(m_currentX * frequency) * amplitude;
  }

  return total*m_coef+m_offset;
}

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
float PerlinNoise1D::getNextValue(float offset)
{
  float total = 0;
  uint i;
  float frequency, amplitude;

  m_currentX += m_increment;

  for (i=0; i < m_nbOctaves ; i++){
    frequency = pow(2,i);
    amplitude = pow(m_persistence,i);

    /* A priori il faut écrire différentes fonctions de génération de bruit */
    total = total + InterpolatedNoise1((m_currentX+offset) * frequency) * amplitude;
  }

  return total*m_coef+m_offset;
}

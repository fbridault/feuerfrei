#ifndef PERLINNOISE_HPP
#define PERLINNOISE_HPP

#include <sys/types.h>

/** Classe permettant de générer un bruit de Perlin sur une dimension. */
class PerlinNoise1D
{
public:
  /** Constructeur.
   * @param increment Valeur de l'incrémentation (x)
   * @param min Valeur minimale du bruit (y)
   * @param max Valeur maximale du bruit (y)
   */
  PerlinNoise1D(float increment, float min, float max);
  /** Génération d'un bruit de Perlin pour une valeur x. */
  float getNextValue();
  float getNextValue(float offset);
  
private:
  float Noise1(int x);
  float SmoothedNoise1(float x);
  float Cosine_Interpolate(float a, float b, float x);
  float InterpolatedNoise1(float x);
  
  float m_currentX;
  float m_increment;
  float m_offset, m_coef;
  
  /** Compris entre 0 et 1. Plus elle est grande, plus le bruit est accidenté. */
  float m_persistence;
  uint m_nbOctaves;
};

#endif

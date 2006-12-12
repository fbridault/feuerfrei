#ifndef NOISE_HPP
#define NOISE_HPP

class Noise
{
public:
  static float Noise1(int x);
  static float SmoothedNoise1(float x);
  static float Cosine_Interpolate(float a, float b, float x);
  static float InterpolatedNoise1(float x);
  static float PerlinNoise1D(float x);
};

#endif

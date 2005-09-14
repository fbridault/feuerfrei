#if !defined(MATHFN_H)
#define MATHFN_H

#define PI 3.14159265358979323846
#define RAD_TO_DEG 180/PI

#include <math.h>

/** Ensemble de fonctions statiques pour les calculs mathématiques */
class MathFn
{
public:
  /** Fonction racine carr-Aée-b
   */
  static double Sqrt( const double& fValue )
  {
    return (double)sqrt(fValue);
  }
};

#endif

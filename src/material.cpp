/* Material.cpp: implementation of the Material class. */
#include "material.hpp"

CMaterial::CMaterial ( double *const ambientCoefficients,
		       double *const diffuseCoefficients,
		       double *const specularCoefficients,
		       double specularExponent, Texture *const tex)
{
  Kss = specularExponent;

  if (ambientCoefficients != NULL)
    {
      Ka = CIntensity (ambientCoefficients);
    }
  if (diffuseCoefficients != NULL)
    {
      Kd = CIntensity (diffuseCoefficients);
    }
  if (specularCoefficients != NULL)
    {
      Ks = CIntensity (specularCoefficients);
    }
  Kss = specularExponent;
  diffuseTexture = tex;
}

void CMaterial::apply () const
{
  GLfloat matDiffuse[COMPOSANTES], matSpecular[COMPOSANTES],
    matAmbient[COMPOSANTES], matShininess[1];
  
  matShininess[0] = Kss;
  
  for (int i = 0; i < COMPOSANTES - 1; i++)
    {
      matDiffuse[i] = Kd.getColor (i);
      matSpecular[i] = Ks.getColor (i);
      matAmbient[i] = 0.3;  // Ka
    } 
  /* Mis à 0 pour indiquer que l'on ne fait pas de glow */
  matDiffuse[3] = 0.0;
  matSpecular[3] = 1.0;
  matAmbient[3] = 1.0;
  
    for (int i = 0; i < COMPOSANTES-1; i++)
      cout << matDiffuse[i] << " ";
    cout << endl;

  glMaterialfv (GL_FRONT, GL_DIFFUSE, matDiffuse);
  glMaterialfv (GL_FRONT, GL_AMBIENT, matAmbient);
  glMaterialfv (GL_FRONT, GL_SPECULAR, matSpecular);
  glMaterialfv (GL_FRONT, GL_SHININESS, matShininess);
  
  glColor4fv(matDiffuse);
}

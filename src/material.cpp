/* Material.cpp: implementation of the Material class. */
#include "material.hpp"

CMaterial::CMaterial ( const string& name, 
		       double *const ambientCoefficients,
		       double *const diffuseCoefficients,
		       double *const specularCoefficients,
		       double specularExponent, Texture *const tex) : m_name(name)
{
  m_Kss = specularExponent;

  if (ambientCoefficients != NULL)
    {
      m_Ka = CIntensity (ambientCoefficients);
    }
  if (diffuseCoefficients != NULL)
    {
      m_Kd = CIntensity (diffuseCoefficients);
    }
  if (specularCoefficients != NULL)
    {
      m_Ks = CIntensity (specularCoefficients);
    }
  m_Kss = specularExponent;
  m_diffuseTexture = tex;
}

void CMaterial::apply () const
{
  GLfloat matDiffuse[COMPOSANTES], matSpecular[COMPOSANTES],
    matAmbient[COMPOSANTES], matShininess[1];
  
  matShininess[0] = m_Kss;
  
  for (int i = 0; i < COMPOSANTES - 1; i++)
    {
      matDiffuse[i] = m_Kd.getColor (i);
      matSpecular[i] = m_Ks.getColor (i);
      matAmbient[i] = 0.3;  // m_Ka
    } 
  /* Mis à 0 pour indiquer que l'on ne fait pas de glow */
  matDiffuse[3] = 0.0;
  matSpecular[3] = 1.0;
  matAmbient[3] = 1.0;
  
//     for (int i = 0; i < COMPOSANTES-1; i++)
//       cout << matDiffuse[i] << " ";
//     cout << endl;

  glMaterialfv (GL_FRONT, GL_DIFFUSE, matDiffuse);
  glMaterialfv (GL_FRONT, GL_AMBIENT, matAmbient);
  glMaterialfv (GL_FRONT, GL_SPECULAR, matSpecular);
  glMaterialfv (GL_FRONT, GL_SHININESS, matShininess);
  
  glColor4fv(matDiffuse);
}

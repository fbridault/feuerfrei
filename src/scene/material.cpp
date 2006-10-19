/* Material.cpp: implementation of the Material class. */
#include "material.hpp"

#include "scene.hpp"

Material::Material (Scene * const scene) : m_name("default")
{
  double coeff[3] = { 1.0, 1.0, 1.0 };
 
  m_scene = scene;
  
  m_Kss = 0;

  m_Ka = Intensity (coeff);
  
  m_diffuseTexture = -1;
}

Material::Material ( Scene * const scene, const string& name, 
		     double *const ambientCoefficients,
		     double *const diffuseCoefficients,
		     double *const specularCoefficients,
		     double specularExponent, int tex) : m_name(name)
{
  m_scene = scene;
  m_Kss = specularExponent;
  
  if (ambientCoefficients != NULL)
    {
      m_Ka = Intensity (ambientCoefficients);
    }
  if (diffuseCoefficients != NULL)
    {
      m_Kd = Intensity (diffuseCoefficients);
    }
  if (specularCoefficients != NULL)
    {
      m_Ks = Intensity (specularCoefficients);
    }
  m_Kss = specularExponent;
  m_diffuseTexture = tex;
}

void Material::apply () const
{
  GLfloat matDiffuse[COMPOSANTES], matSpecular[COMPOSANTES],
    matAmbient[COMPOSANTES], matShininess[1];
  
  matShininess[0] = m_Kss;
  
  for (int i = 0; i < COMPOSANTES - 1; i++)
    {
      matDiffuse[i] = m_Kd.getColor (i);
      matSpecular[i] = m_Ks.getColor (i);
      matAmbient[i] = m_Ka.getColor (i);
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

const Texture* Material::getDiffuseTexture() const
{ 
  return m_scene->getTexture(m_diffuseTexture);
};

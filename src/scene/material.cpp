/* Material.cpp: implementation of the Material class. */
#include "material.hpp"


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

const bool Material::isTransparent() const
{
  if(m_scene->getTexture(m_diffuseTexture)->hasAlpha()) cerr << m_name << " is transparent" << endl;
  return (m_scene->getTexture(m_diffuseTexture)->hasAlpha());
}

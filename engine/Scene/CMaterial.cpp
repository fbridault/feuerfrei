#include "CMaterial.hpp"


CMaterial::CMaterial (const CScene* const scene) : m_name("default")
{
  float coeff[3] = { 1.0, 1.0, 1.0 };

  m_scene = scene;

  m_Kss = 0;

  m_Ka = Energy (coeff);

  m_diffuseTexture = -1;
}

CMaterial::CMaterial ( const CScene* const scene, const string& name,
                     float *const ambientCoefficients,
                     float *const diffuseCoefficients,
                     float *const specularCoefficients,
                     float specularExponent, int tex ) : m_name(name)
{
  m_scene = scene;
  m_Kss = specularExponent;

  if (diffuseCoefficients != NULL)
    m_Kd = Energy (diffuseCoefficients);

  if (ambientCoefficients != NULL)
    m_Ka = Energy (ambientCoefficients);

  if (specularCoefficients != NULL)
    m_Ks = Energy (specularCoefficients);

  m_Kss = specularExponent;

  m_diffuseTexture = tex;
}

const bool CMaterial::isTransparent() const
{
  if (m_scene->getTexture(m_diffuseTexture)->hasAlpha()) cerr << m_name << " is transparent" << endl;
  return (m_scene->getTexture(m_diffuseTexture)->hasAlpha());
}

#include "CMaterial.hpp"


CMaterial::CMaterial (CScene const& a_rScene) :
	m_rScene(a_rScene),
	m_name("default")
{
	float coeff[3] = { 1.0, 1.0, 1.0 };

	m_Kss = 0;

	m_Ka = CEnergy (coeff);

	m_diffuseTexture = -1;
}

CMaterial::CMaterial ( 	CScene const& a_rScene,
											string const& name,
											float *const ambientCoefficients,
											float *const diffuseCoefficients,
											float *const specularCoefficients,
											float specularExponent, int tex ) :
	m_rScene(a_rScene),
	m_name(name),
	m_Kss (specularExponent),
	m_diffuseTexture(tex)
{

	if (diffuseCoefficients != NULL)
		m_Kd = CEnergy (diffuseCoefficients);

	if (ambientCoefficients != NULL)
		m_Ka = CEnergy (ambientCoefficients);

	if (specularCoefficients != NULL)
		m_Ks = CEnergy (specularCoefficients);

}

const bool CMaterial::isTransparent() const
{
	if (m_rScene.getTexture(m_diffuseTexture)->hasAlpha()) cerr << m_name << " is transparent" << endl;
	return (m_rScene.getTexture(m_diffuseTexture)->hasAlpha());
}

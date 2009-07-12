#include "CMaterial.hpp"


CMaterial::CMaterial (CScene const& a_rScene) :
	m_rScene(a_rScene),
	m_name("default"),
	m_iDiffuseTexture(-1)
{
	float coeff[3] = { 1.0, 1.0, 1.0 };

	m_Kss = 0;
	m_Ka = CEnergy (coeff);
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
	m_iDiffuseTexture(tex)
{

	if (diffuseCoefficients != NULL)
		m_Kd = CEnergy (diffuseCoefficients);

	if (ambientCoefficients != NULL)
		m_Ka = CEnergy (ambientCoefficients);

	if (specularCoefficients != NULL)
		m_Ks = CEnergy (specularCoefficients);

}

bool CMaterial::isTransparent() const
{
	if (m_rScene.GetTexture(m_iDiffuseTexture).hasAlpha()) cerr << m_name << " is transparent" << endl;
	return (m_rScene.GetTexture(m_iDiffuseTexture).hasAlpha());
}

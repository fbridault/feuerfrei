#ifndef MATERIAL_H
#define MATERIAL_H

class CMaterial;

#include "../Physics/CEnergy.hpp"
#include "Texture.hpp"
#include "CScene.hpp"

class ITexture;
class CScene;

/**
 * Classe de base repr�sentant un mat�riau.
 * Un mat�riau est d�finit par son comportement lumineux, c'est � dire la
 * mani&egrave;re dont il r�fl�chit la lumi�re.
 *
 * @author	Flavien Bridault
 * @see CEnergy
 */
class CMaterial
{
public:
	/**
	 * Constructeur par d�faut. Cr�e un mat�riau blanc ambiant.
	 */
	CMaterial(CScene const& a_rScene);
	/**
	 * Constructeur.
	 * @param name Nom donn� au mat�riau.
	 * @param ambientCoefficients	Composante de r�flexion ambiante.
	 * @param diffuseCoefficients	Composante de r�flexion diffuse.
	 * @param specularCoefficients Composante de r�flexion sp�culaire.
	 * @param specularExponent Indice de tache sp�culaire.
	 * @param tex Pointeur optionel sur la texture.
	 */
	CMaterial(CScene const& a_rScene, const string& name, float* const ambientCoefficients, float* const diffuseCoefficients, float* const specularCoefficients, float specularExponent=0.0, int tex=-1);
	/** Destructeur par d�faut. */
	~CMaterial(){};

	/** Retourne le nom du mat�riau.
	 * @return Nom du mat�riau.
	 */
	string const& GetName() const
	{
		return m_name;
	};

	/* Lecture de la composante sp�culaire.
	 * @return Une variable de type CEnergy.
	 */
	CEnergy const& GetSpecularCoefficients() const
	{
		return (m_Ks);
	};

	/** Lecture de la composante diffuse.
	 * @return Une variable de type CEnergy.
	 */
	CEnergy const& GetDiffuseCoefficients() const
	{
		return (m_Kd);
	};

	/** Lecture de la composante ambiante.
	 * @return Une variable de type CEnergy.
	 */
	CEnergy const& GetAmbientCoefficients() const
	{
		return (m_Ka);
	};

	/** Applique le mat�riau avec glMaterial(). */
	void apply () const
	{
//    glMaterialfv (GL_FRONT, GL_DIFFUSE, m_Kd.GetColors());
//    glMaterialfv (GL_FRONT, GL_AMBIENT, m_Ka.GetColors());
//    glMaterialfv (GL_FRONT, GL_SPECULAR, m_Ks.GetColors());
//    glMaterialfv (GL_FRONT, GL_SHININESS, &m_Kss);
		glColor4fv(m_Kd.GetColors());
	}

	/** Indique si le mat�riau poss�de une texture.
	 * @return True si le mat�riau a une texture.
	 */
	bool hasDiffuseTexture() const
	{
		return (m_iDiffuseTexture!=-1);
	};

	/** Retourne un pointeur sur la texture du mat�riau.
	 * @return Pointeur sur la texture
	 */
	ITexture const& GetDiffuseTexture() const
	{
		return m_rScene.GetTexture(m_iDiffuseTexture);
	};

	/** Lecture de la composante diffuse.
	 * @return Une variable de type CEnergy.
	 */
	CEnergy const& GetDiffuseReflectivity() const
	{
		if (m_iDiffuseTexture > -1)
			return m_KDiffuseTexture;
		else
			return m_Kd;
	};

	/** Indique si le mat�riau poss�de une texture.
	 * @return True si le mat�riau a une texture.
	 */
	bool isTransparent() const;

private:
	CScene const& m_rScene; /** Pointeur vers la sc�ne, utilis� pour r�cup�rer les textures. */
	string m_name;               /** Nom du mat�riau. */
	CEnergy m_Kd;                 /** Composante de r�flexion diffuse. */
	CEnergy m_Ks;                 /** Composante de r�flexion sp�culaire. */
	float m_Kss;                 /** Indice de tache sp�culaire. */
	CEnergy m_Ka;                 /** Composante de r�flexion ambiante. */
	CEnergy m_KDiffuseTexture;    /** Composante moyenne de r�flexion diffuse de la texture. */

	int m_iDiffuseTexture;  /** Indice de la texture diffuse dans la sc�ne. -1 si le mat�riau n'est pas textur�. */
};

#endif

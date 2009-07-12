#ifndef MATERIAL_H
#define MATERIAL_H

class CMaterial;

#include "../Physics/CEnergy.hpp"
#include "Texture.hpp"
#include "CScene.hpp"

class ITexture;
class CScene;

/**
 * Classe de base représentant un matériau.
 * Un matériau est définit par son comportement lumineux, c'est à dire la
 * mani&egrave;re dont il réfléchit la lumière.
 *
 * @author	Flavien Bridault
 * @see CEnergy
 */
class CMaterial
{
public:
	/**
	 * Constructeur par défaut. Crée un matériau blanc ambiant.
	 */
	CMaterial(CScene const& a_rScene);
	/**
	 * Constructeur.
	 * @param name Nom donné au matériau.
	 * @param ambientCoefficients	Composante de réflexion ambiante.
	 * @param diffuseCoefficients	Composante de réflexion diffuse.
	 * @param specularCoefficients Composante de réflexion spéculaire.
	 * @param specularExponent Indice de tache spéculaire.
	 * @param tex Pointeur optionel sur la texture.
	 */
	CMaterial(CScene const& a_rScene, const string& name, float* const ambientCoefficients, float* const diffuseCoefficients, float* const specularCoefficients, float specularExponent=0.0, int tex=-1);
	/** Destructeur par défaut. */
	~CMaterial(){};

	/** Retourne le nom du matériau.
	 * @return Nom du matériau.
	 */
	string const& GetName() const
	{
		return m_name;
	};

	/* Lecture de la composante spéculaire.
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

	/** Applique le matériau avec glMaterial(). */
	void apply () const
	{
//    glMaterialfv (GL_FRONT, GL_DIFFUSE, m_Kd.GetColors());
//    glMaterialfv (GL_FRONT, GL_AMBIENT, m_Ka.GetColors());
//    glMaterialfv (GL_FRONT, GL_SPECULAR, m_Ks.GetColors());
//    glMaterialfv (GL_FRONT, GL_SHININESS, &m_Kss);
		glColor4fv(m_Kd.GetColors());
	}

	/** Indique si le matériau possède une texture.
	 * @return True si le matériau a une texture.
	 */
	bool hasDiffuseTexture() const
	{
		return (m_iDiffuseTexture!=-1);
	};

	/** Retourne un pointeur sur la texture du matériau.
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

	/** Indique si le matériau possède une texture.
	 * @return True si le matériau a une texture.
	 */
	bool isTransparent() const;

private:
	CScene const& m_rScene; /** Pointeur vers la scène, utilisé pour récupérer les textures. */
	string m_name;               /** Nom du matériau. */
	CEnergy m_Kd;                 /** Composante de réflexion diffuse. */
	CEnergy m_Ks;                 /** Composante de réflexion spéculaire. */
	float m_Kss;                 /** Indice de tache spéculaire. */
	CEnergy m_Ka;                 /** Composante de réflexion ambiante. */
	CEnergy m_KDiffuseTexture;    /** Composante moyenne de réflexion diffuse de la texture. */

	int m_iDiffuseTexture;  /** Indice de la texture diffuse dans la scène. -1 si le matériau n'est pas texturé. */
};

#endif

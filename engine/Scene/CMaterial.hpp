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
 * @see Energy
 */
class CMaterial
{
public:
	/**
	 * Constructeur par défaut. Crée un matériau blanc ambiant.
	 */
	CMaterial(const CScene* const scene );
	/**
	 * Constructeur.
	 * @param name Nom donné au matériau.
	 * @param ambientCoefficients	Composante de réflexion ambiante.
	 * @param diffuseCoefficients	Composante de réflexion diffuse.
	 * @param specularCoefficients Composante de réflexion spéculaire.
	 * @param specularExponent Indice de tache spéculaire.
	 * @param tex Pointeur optionel sur la texture.
	 */
	CMaterial(const CScene* const scene, const string& name, float* const ambientCoefficients, float* const diffuseCoefficients, float* const specularCoefficients, float specularExponent=0.0, int tex=-1);
	/** Destructeur par défaut. */
	~CMaterial(){};

	/** Retourne le nom du matériau.
	 * @return Nom du matériau.
	 */
	const string& getName() const
	{
		return m_name;
	};

	/* Lecture de la composante spéculaire.
	 * @return Une variable de type Energy.
	 */
	const Energy& getSpecularCoefficients() const
	{
		return (m_Ks);
	};

	/** Lecture de la composante diffuse.
	 * @return Une variable de type Energy.
	 */
	const Energy& getDiffuseCoefficients() const
	{
		return (m_Kd);
	};

	/** Lecture de la composante ambiante.
	 * @return Une variable de type Energy.
	 */
	const Energy& getAmbientCoefficients() const
	{
		return (m_Ka);
	};

	/** Applique le matériau avec glMaterial(). */
	void apply () const
	{
//    glMaterialfv (GL_FRONT, GL_DIFFUSE, m_Kd.getColors());
//    glMaterialfv (GL_FRONT, GL_AMBIENT, m_Ka.getColors());
//    glMaterialfv (GL_FRONT, GL_SPECULAR, m_Ks.getColors());
//    glMaterialfv (GL_FRONT, GL_SHININESS, &m_Kss);
		glColor4fv(m_Kd.getColors());
	}

	/** Indique si le matériau possède une texture.
	 * @return True si le matériau a une texture.
	 */
	const bool hasDiffuseTexture() const
	{
		return (m_diffuseTexture!=-1);
	};

	/** Retourne un pointeur sur la texture du matériau.
	 * @return Pointeur sur la texture
	 */
	const ITexture* getDiffuseTexture() const
	{
		return m_scene->getTexture(m_diffuseTexture);
	};

	/** Lecture de la composante diffuse.
	 * @return Une variable de type Energy.
	 */
	const Energy& getDiffuseReflectivity() const
	{
		if (m_diffuseTexture > -1)
			return m_KDiffuseTexture;
		else
			return m_Kd;
	};

	/** Indique si le matériau possède une texture.
	 * @return True si le matériau a une texture.
	 */
	const bool isTransparent() const;

private:
	string m_name;               /** Nom du matériau. */
	Energy m_Kd;                 /** Composante de réflexion diffuse. */
	Energy m_Ks;                 /** Composante de réflexion spéculaire. */
	float m_Kss;                 /** Indice de tache spéculaire. */
	Energy m_Ka;                 /** Composante de réflexion ambiante. */
	Energy m_KDiffuseTexture;    /** Composante moyenne de réflexion diffuse de la texture. */

	const CScene *m_scene; /** Pointeur vers la scène, utilisé pour récupérer les textures. */
	int m_diffuseTexture;  /** Indice de la texture diffuse dans la scène. -1 si le matériau n'est pas texturé. */
};

#endif

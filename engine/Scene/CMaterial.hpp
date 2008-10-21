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
 * @see Energy
 */
class CMaterial
{
public:
	/**
	 * Constructeur par d�faut. Cr�e un mat�riau blanc ambiant.
	 */
	CMaterial(const CScene* const scene );
	/**
	 * Constructeur.
	 * @param name Nom donn� au mat�riau.
	 * @param ambientCoefficients	Composante de r�flexion ambiante.
	 * @param diffuseCoefficients	Composante de r�flexion diffuse.
	 * @param specularCoefficients Composante de r�flexion sp�culaire.
	 * @param specularExponent Indice de tache sp�culaire.
	 * @param tex Pointeur optionel sur la texture.
	 */
	CMaterial(const CScene* const scene, const string& name, float* const ambientCoefficients, float* const diffuseCoefficients, float* const specularCoefficients, float specularExponent=0.0, int tex=-1);
	/** Destructeur par d�faut. */
	~CMaterial(){};

	/** Retourne le nom du mat�riau.
	 * @return Nom du mat�riau.
	 */
	const string& getName() const
	{
		return m_name;
	};

	/* Lecture de la composante sp�culaire.
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

	/** Applique le mat�riau avec glMaterial(). */
	void apply () const
	{
//    glMaterialfv (GL_FRONT, GL_DIFFUSE, m_Kd.getColors());
//    glMaterialfv (GL_FRONT, GL_AMBIENT, m_Ka.getColors());
//    glMaterialfv (GL_FRONT, GL_SPECULAR, m_Ks.getColors());
//    glMaterialfv (GL_FRONT, GL_SHININESS, &m_Kss);
		glColor4fv(m_Kd.getColors());
	}

	/** Indique si le mat�riau poss�de une texture.
	 * @return True si le mat�riau a une texture.
	 */
	const bool hasDiffuseTexture() const
	{
		return (m_diffuseTexture!=-1);
	};

	/** Retourne un pointeur sur la texture du mat�riau.
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

	/** Indique si le mat�riau poss�de une texture.
	 * @return True si le mat�riau a une texture.
	 */
	const bool isTransparent() const;

private:
	string m_name;               /** Nom du mat�riau. */
	Energy m_Kd;                 /** Composante de r�flexion diffuse. */
	Energy m_Ks;                 /** Composante de r�flexion sp�culaire. */
	float m_Kss;                 /** Indice de tache sp�culaire. */
	Energy m_Ka;                 /** Composante de r�flexion ambiante. */
	Energy m_KDiffuseTexture;    /** Composante moyenne de r�flexion diffuse de la texture. */

	const CScene *m_scene; /** Pointeur vers la sc�ne, utilis� pour r�cup�rer les textures. */
	int m_diffuseTexture;  /** Indice de la texture diffuse dans la sc�ne. -1 si le mat�riau n'est pas textur�. */
};

#endif

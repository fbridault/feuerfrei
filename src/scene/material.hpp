#ifndef MATERIAL_H
#define MATERIAL_H

class CMaterial;

#include <string>
#include <GL/gl.h>
#include "intensity.hpp"
#include "texture.hpp"

using namespace std;

/** 
 * Classe de base repr&eacute;sentant un mat&eacute;riau. 
 * Un mat&eacute;riau est d&eacute;finit par son comportement lumineux, c'est &agrave; dire la 
 * mani&egrave;re dont il r&eacute;fl&eacute;chi la lumi&egrave;re. 
 *
 * @author	Christophe Cassagnab&egrave;re modifi&eacute; par Flavien Bridault
 * @version	%I%, %G%
 * @since	1.0
 * @see CIntensity
 */
class CMaterial 
{
private:
  string m_name;
  CIntensity m_Kd;        /**<composante de r&eacute;flexion diffuse.*/
  CIntensity m_Ks;        /**<composante de r&eacute;flexion sp&eacute;culaire.*/
  double m_Kss;       /**<indice de tache sp&eacute;culaire*/
  CIntensity m_Ka;        /**<composante de r&eacute;flexion ambiante.*/

  Texture *m_diffuseTexture;
public:
  /**
   * Constructeur par d&eacute;faut. Cr&eacute;e un mat&eacute;riau noir opaque.
   */
  CMaterial();
  /**
   * Constructeur param&eacute;trique.
   * @param ambientCoefficients	composante de r&eacute;flexion ambiante.
   * @param diffuseCoefficients	composante de r&eacute;flexion diffuse.
   * @param specularCoefficients	composante de r&eacute;flexion sp&eacute;culaire.
   * @param transmissiveCoefficients	composante de transparence.
   * @param specularExponent	indice de tache sp&eacute;culaire.
   * @param refractionIndex	indice de r&eacute;fraction.
   */
  CMaterial( const string& name, double* const ambientCoefficients, double* const diffuseCoefficients, double* const specularCoefficients, double specularExponent=0.0, Texture * const tex=NULL);
  /**
   * Destructeur par d&eacute;faut.
   */
  ~CMaterial(){if(m_diffuseTexture!=NULL) delete m_diffuseTexture;};

  const string *getName() const
  { return &m_name;};
  /*
   * Lecture de la composante sp&eacute;culaire. 
   * @return Une variable de type CIntensity.
   */
  CIntensity getSpecularCoefficients() const
  { return (m_Ks);};
  /**
   * Lecture de la composante diffuse. 
   * @return Une variable de type CIntensity.
   */ 
  CIntensity getDiffuseCoefficients() const
  { return (m_Kd);};
  /**
   * Lecture de la composante ambiante. 
   * @return Une variable de type CIntensity.
   */ 
  const CIntensity& getAmbientCoefficients() const
  { return (m_Ka); };
  
  void apply () const;
  
  const bool hasDiffuseTexture() const
  { return (m_diffuseTexture!=NULL);};

  const GLuint getDiffuseTexture() const
  { return (m_diffuseTexture->getTexture());};
};//CMaterial

#endif 

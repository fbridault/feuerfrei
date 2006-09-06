#ifndef MATERIAL_H
#define MATERIAL_H

class Material;

#include <string>
#include <GL/gl.h>
#include "intensity.hpp"
#include "texture.hpp"

using namespace std;

/** 
 * Classe de base repr�sentant un mat�riau. 
 * Un mat�riau est d�finit par son comportement lumineux, c'est � dire la 
 * mani&egrave;re dont il r�fl�chi la lumi�re. 
 *
 * @author	Christophe Cassagnab�re modifi� par Flavien Bridault
 * @see Intensity
 */
class Material 
{
private:
  string m_name;   /** Nom du mat�riau. */
  Intensity m_Kd;  /**Composante de r�flexion diffuse.*/
  Intensity m_Ks;  /**Composante de r�flexion sp�culaire.*/
  double m_Kss;    /**Indice de tache sp�culaire*/
  Intensity m_Ka;  /**Composante de r�flexion ambiante.*/
  
  Texture *m_diffuseTexture;  /** Pointeur sur la texture diffuse. NULL si le mat�riau n'est pas textur�e. */
public:
  /**
   * Constructeur par d�faut. Cr�e un mat�riau blanc ambiant.
   */
  Material();
  /**
   * Constructeur.
   * @param name Nom donn� au mat�riau.
   * @param ambientCoefficients	Composante de r�flexion ambiante.
   * @param diffuseCoefficients	Composante de r�flexion diffuse.
   * @param specularCoefficients Composante de r�flexion sp�culaire.
   * @param specularExponent Indice de tache sp�culaire.
   * @param tex Pointeur optionel sur la texture.
   */
  Material( const string& name, double* const ambientCoefficients, double* const diffuseCoefficients, double* const specularCoefficients, double specularExponent=0.0, Texture * const tex=NULL);
  /** Destructeur par d�faut. */
  ~Material(){if(m_diffuseTexture!=NULL) delete m_diffuseTexture;};
  
  /** Retourne le nom du mat�riau.
   * @return Nom du mat�riau.
   */
  const string *getName() const
  { return &m_name;};
  
  /* Lecture de la composante sp�culaire. 
   * @return Une variable de type Intensity.
   */
  Intensity getSpecularCoefficients() const
  { return (m_Ks);};
  
  /** Lecture de la composante diffuse. 
   * @return Une variable de type Intensity.
   */  
  Intensity getDiffuseCoefficients() const
  { return (m_Kd);};
  
  /** Lecture de la composante ambiante. 
   * @return Une variable de type Intensity.
   */ 
  const Intensity& getAmbientCoefficients() const
  { return (m_Ka); };
  
  /** Applique le mat�riau avec glMaterial(). */ 
  void apply () const;
  
  /** Indique si le mat�riau poss�de une texture.
   * @return True si le mat�riau a une texture.
   */
  const bool hasDiffuseTexture() const
  { return (m_diffuseTexture!=NULL);};

  /** Retourne un pointeur sur la texture du mat�riau.
   * @return Pointeur sur la texture
   */
  const Texture* getDiffuseTexture() const
  { return m_diffuseTexture;};
};

#endif

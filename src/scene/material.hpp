#ifndef MATERIAL_H
#define MATERIAL_H

class Material;

#include <string>
#include <GL/gl.h>
#include "intensity.hpp"
#include "texture.hpp"

class Texture;
class Scene;

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
  
  Scene *m_scene; /** Pointeur vers la sc�ne, utilis� pour r�cup�rer les textures. */
  int m_diffuseTexture;  /** Indice de la texture diffuse dans la sc�ne. -1 si le mat�riau n'est pas textur�. */
public:
  /**
   * Constructeur par d�faut. Cr�e un mat�riau blanc ambiant.
   */
  Material( Scene * const scene );
  /**
   * Constructeur.
   * @param name Nom donn� au mat�riau.
   * @param ambientCoefficients	Composante de r�flexion ambiante.
   * @param diffuseCoefficients	Composante de r�flexion diffuse.
   * @param specularCoefficients Composante de r�flexion sp�culaire.
   * @param specularExponent Indice de tache sp�culaire.
   * @param tex Pointeur optionel sur la texture.
   */
  Material( Scene * const scene, const string& name, double* const ambientCoefficients, double* const diffuseCoefficients, double* const specularCoefficients, double specularExponent=0.0, int tex=-1);
  /** Destructeur par d�faut. */
  ~Material(){};
  
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
  { return (m_diffuseTexture!=-1);};

  /** Retourne un pointeur sur la texture du mat�riau.
   * @return Pointeur sur la texture
   */
  const Texture* getDiffuseTexture() const;
};

#endif

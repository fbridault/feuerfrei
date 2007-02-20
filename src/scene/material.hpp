#ifndef MATERIAL_H
#define MATERIAL_H

class Material;

#include <string>
#include <GL/gl.h>
#include "intensity.hpp"
#include "texture.hpp"
#include "scene.hpp"

class Texture;
class Scene;

using namespace std;

/** 
 * Classe de base représentant un matériau. 
 * Un matériau est définit par son comportement lumineux, c'est à dire la 
 * mani&egrave;re dont il réfléchit la lumière. 
 *
 * @author	Flavien Bridault
 * @see Intensity
 */
class Material 
{
public:
  /**
   * Constructeur par défaut. Crée un matériau blanc ambiant.
   */
  Material(const Scene* const scene );
  /**
   * Constructeur.
   * @param name Nom donné au matériau.
   * @param ambientCoefficients	Composante de réflexion ambiante.
   * @param diffuseCoefficients	Composante de réflexion diffuse.
   * @param specularCoefficients Composante de réflexion spéculaire.
   * @param specularExponent Indice de tache spéculaire.
   * @param tex Pointeur optionel sur la texture.
   */
  Material(const Scene* const scene, const string& name, double* const ambientCoefficients, double* const diffuseCoefficients, double* const specularCoefficients, double specularExponent=0.0, int tex=-1);
  /** Destructeur par défaut. */
  ~Material(){};
  
  /** Retourne le nom du matériau.
   * @return Nom du matériau.
   */
  const string *getName() const
  { return &m_name;};
  
  /* Lecture de la composante spéculaire. 
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
  
  /** Applique le matériau avec glMaterial(). */ 
  void apply () const
  {
    GLfloat matDiffuse[COMPOSANTES], matSpecular[COMPOSANTES],
      matAmbient[COMPOSANTES], matShininess[1];
  
    matShininess[0] = m_Kss;
  
    for (int i = 0; i < COMPOSANTES - 1; i++)
      {
	matDiffuse[i] = m_Kd.getColor (i);
	matSpecular[i] = m_Ks.getColor (i);
	matAmbient[i] = m_Ka.getColor (i);
      }
    /* Mis à 0 de l'alpha pour indiquer que l'on ne fait pas de glow */
    matDiffuse[3] = 0.0;
    matSpecular[3] = 1.0;
    matAmbient[3] = 1.0;
  
    glMaterialfv (GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv (GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv (GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv (GL_FRONT, GL_SHININESS, matShininess);
  
    glColor4fv(matDiffuse);
  }
  
  /** Indique si le matériau possède une texture.
   * @return True si le matériau a une texture.
   */
  const bool hasDiffuseTexture() const { return (m_diffuseTexture!=-1); };

  /** Retourne un pointeur sur la texture du matériau.
   * @return Pointeur sur la texture
   */
  const Texture* getDiffuseTexture() const { return m_scene->getTexture(m_diffuseTexture); };

  /** Indique si le matériau possède une texture.
   * @return True si le matériau a une texture.
   */
  const bool isTransparent() const;

private:
  string m_name;   /** Nom du matériau. */
  Intensity m_Kd;  /** Composante de réflexion diffuse. */
  Intensity m_Ks;  /** Composante de réflexion spéculaire. */
  double m_Kss;    /** Indice de tache spéculaire. */
  Intensity m_Ka;  /** Composante de réflexion ambiante. */
  
  const Scene *m_scene; /** Pointeur vers la scène, utilisé pour récupérer les textures. */
  int m_diffuseTexture;  /** Indice de la texture diffuse dans la scène. -1 si le matériau n'est pas texturé. */
};

#endif

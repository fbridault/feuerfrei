#ifndef FIELD_HPP
#define FIELD_HPP

#define SWAP(x0,x) {float *tmp=x0;x0=x;x=tmp;}
// Pour le calcul du dégradé de couleur des pyramides
#define VELOCITE_MAX .1

#define RAD_TO_DEG 180/PI

#include <GL/gl.h>
#include <math.h>

#include "../common.hpp"

/** La Field est une interface basique pour tous les champs de vélocité, sans notion de dimension
 * qui est apportée par ses filles Field2D et Field3D.
 * 
 * @author	Flavien Bridault.
 */
class Field
{
public:
  /** Constructeur par défaut nécessaire pour l'héritage multiple */
  Field ();
  
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param timeStep Pas de temps utilisé pour la simulation.
   * @param buoyancy Intensité de la force de flottabilité dans le solveur.
   */
  Field (const Point& position, float timeStep, float buoyancy);
  /** Destructeur */
  virtual ~Field ();
  
  /** Lance une itération du solveur. */
  virtual void iterate () = 0;
  
  /** Ajout des forces externes.
   * @param x Composante à traiter.
   * @param src Composante contenant les forces engendrées par les sources externes.
   */
  virtual void add_source (float *const x, float *const src)
  {
    uint i;
    for (i = 0; i < m_nbVoxels; i++)
      x[i] += m_dt * src[i] * m_forceCoef;
  };
  
  /** Remet à zéro toutes les forces externes */
  virtual void cleanSources () = 0;
  
  /** Retourne le pas de temps.
   * @return Valeur du pas de temps en ms.
   */
  float getTimeStep() const
  {
    return m_dt;
  };

  /** Retourne l'itération en cours.
   * @return Numéro de l'itération.
   */
  float getNbIter() const
  {
    return m_nbIter;
  };
  
  /** Retourne la position du solveur dans le repère du monde 
   * @return Position dans l'espace.
   */
  virtual Point getPosition () const { return (m_position); }; 
  
  /** Affecte la position du solveur dans le repère du monde 
   * @return Position dans l'espace.
   */
  virtual void setPosition (const Point& position) { m_position = position; };
  
  /** Déplace le solveur et ajoute de façon temporaire (une itération) des
   * forces externes sur une des faces du solveur. 
   * @param forces Amplitude du déplacement en (x,y,z).
   */
  virtual void move(const Point& forces) { m_movingForces += forces; }
  
  /** Ajoute de façon temporaire (une itération) des forces externes sur une des faces du solveur. Cette méthode
   * est utilisée principalement lorsque que du vent est appliqué sur une flamme.
   * @param forces Intensité de la force en (x,y,z).
   */
  virtual void addTemporaryExternalForces(const Point& forces) { m_temporaryExternalForces = forces; }

  /** Ajoute de façon permanente des forces externes sur une des faces du solveur. Cette méthode
   * est utilisée principalement lorsque que du vent est appliqué sur une flamme.
   * @param forces Intensité de la force en (x,y,z).
   */
  virtual void addPermanentExternalForces(const Point& forces) { m_permanentExternalForces = forces; }

  virtual void addDensity(int id) {} ;
  
  /** Modifie la force de flottabilité dans le solveur
   * @param value Nouvelle valeur.
   */
  virtual void setBuoyancy(float value){ m_buoyancy=value; };
  
  /** Modifie la force de vorticité dans le solveur
   * @param value Nouvelle valeur.
   */
  virtual void setVorticity(float value){ m_vorticityConfinement=value; };
  
  /** Divise la résolution de la grille par 2 */
  virtual void divideRes () = 0;
  
  /** Multiplie la résolution de la grille par 2 */
  virtual void multiplyRes () = 0;
  
  /** Diminue la résolution de la grille de deux voxels */
  virtual void decreaseRes () = 0;
  
  /** Augmente la résolution de la grille de deux voxels */
  virtual void increaseRes () = 0;
  
  /** Fonction de dessin de la grille */
  virtual void displayGrid (){
    glCallList (m_gridDisplayList);
  };
  
  /** Fonction de dessin du repère de base */
  virtual void displayBase (){
    glCallList (m_baseDisplayList);
  };
  
  /** Fonction de dessin du champ de vélocité */
  virtual void displayVelocityField (void) = 0;
  
  /** Fonction de dessin du champ de densité */
  virtual void displayDensityField (void) = 0;
  
  /** Fonction de dessin de la vélocité d'une cellule */
  virtual void displayArrow (const Vector& direction) = 0;
  
  virtual void setRunningState(bool state) { m_run = state; } ;
protected:
  /** Ajoute de façon ponctuelle des forces externes sur une des faces du champ de vélocité. Cette méthode
   * est utilisée avant une itération du solveur pour ajouter toutes les forces externes appliquées
   * à partir de l'interface graphique. Elle peut également déplacer la grille le cas échéant.
   * @param position Intensité de la force.
   * @param move Si true, alors le solveur est en plus déplacé à la position passée en paramètre.
   */
  virtual void addExternalForces(const Point& position, bool move) = 0;
  
  /** Fonction de construction de la display list de la grille du solveur */
  virtual void buildDLGrid () = 0;
  
  /** Fonction de construction de la display list du repère du solveur */
  virtual void buildDLBase () = 0;
  
  /** Position du solveur dans l'espace */
  Point m_position;

  /** Taille totale en nombre de voxels */
  uint m_nbVoxels;
  /** Intensité de la force de flottabilité */
  float m_buoyancy;

  /** Nombre d'itérations */
  uint m_nbIter;
  /** Pas de temps. */
  float m_dt;
  
  /** Display list de la grille du solveur. */
  GLuint m_gridDisplayList;
  /** Display list de la base de la grille du solveur. */
  GLuint m_baseDisplayList;
  
  /** Force externe permanente, elle est ajoutée à chaque itération. */
  Point m_permanentExternalForces;
  /** Force externe temporaire, elle est ajoutée à la prochaine itération et remise à zéro ensuite. */
  Point m_temporaryExternalForces;
  /** Force externe consécutivé à un déplacement. Elle est ajoutée à la prochaine itération et remise à zéro ensuite. */
  Point m_movingForces;

  /** Intensité des forces de vorticité */
  float m_vorticityConfinement;
  
  float m_forceCoef;
  float m_forceRatio;

  bool m_run;
};


#endif

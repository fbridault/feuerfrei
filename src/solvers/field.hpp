#ifndef FIELD_HPP
#define FIELD_HPP

#define SWAP(x0,x) {double *tmp=x0;x0=x;x=tmp;}
// Pour le calcul du dégradé de couleur des pyramides
#define VELOCITE_MAX .1

#define RAD_TO_DEG 180/PI

#include <GL/gl.h>
#include <math.h>

#include "../common.hpp"

class Field;

/** La classe Solver propose une implémentation de la méthode stable implicite semi-lagrangienne de Stam.
 * A noter que dans le cadre de notre modèle de flamme, le champ de densité n'est pas utilisé, ce qui,
 * physiquement semble assez curieux (d'après Alexei). 
 * 
 * @author	Flavien Bridault et Michel Lebond.
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
  Field (const Point& position, double timeStep, double buoyancy);
  /** Destructeur */
  virtual ~Field ();
  
  /** Lance une itération du solveur. */
  virtual void iterate () = 0;
  
  /** Ajout des forces externes.
   * @param x Composante à traiter.
   * @param src Composante contenant les forces engendrées par les sources externes.
   */
  virtual void add_source (double *const x, double *const src)
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
  double getTimeStep()
  {
    return m_dt;
  };

  /** Retourne l'itération en cours.
   * @return Numéro de l'itération.
   */
  double getNbIter()
  {
    return m_nbIter;
  };
  
  /** Retourne la position du solveur dans le repère du monde 
   * @return Position dans l'espace.
   */
  virtual Point getPosition ()
  {
    return (m_position);
  };
  
  /** Ajoute de façon temporaire (une itération) des forces externes sur une des faces du solveur. Cette méthode
   * est utilisée principalement lorsque que du vent est appliqué sur une flamme.
   * @param forces Intensité de la force en (x,y,z).
   */
  virtual void addTemporaryExternalForces(Point& forces) { m_temporaryExternalForces = forces; }

  /** Ajoute de façon permanente des forces externes sur une des faces du solveur. Cette méthode
   * est utilisée principalement lorsque que du vent est appliqué sur une flamme.
   * @param forces Intensité de la force en (x,y,z).
   */
  virtual void addPermanentExternalForces(Point& forces) { m_permanentExternalForces = forces; }

  virtual void addDensity(int id) {} ;
  
  /** Modifie la force de flottabilité dans le solveur
   * @param value Nouvelle valeur.
   */
  virtual void setBuoyancy(double value){ m_buoyancy=value; };
  
  /** Divise la résolution de la grille par 2 */
  virtual void divideRes () = 0;

  /** Multiplie la résolution de la grille par 2 */
  virtual void multiplyRes () = 0;
  
  /** Diminue la résolution de la grille de un voxel */
  virtual void decreaseRes () = 0;
  
  /** Augmente la résolution de la grille de un voxel */
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
  /** Ajoute de façon ponctuelle des forces externes sur une des faces du solveur. Cette méthode
   * est utilisée principalement lorsque qu'une flamme est déplacée.
   * @param position Nouvelle position du solveur. Détermine l'intensité de la force.
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
  double m_buoyancy;

  /** Nombre d'itérations */
  uint m_nbIter;
  /** Pas de temps. */
  double m_dt;
  
  /** Display list de la grille du solveur. */
  GLuint m_gridDisplayList;
  /** Display list de la base de la grille du solveur. */
  GLuint m_baseDisplayList;
  
  Point m_permanentExternalForces;
  Point m_temporaryExternalForces;

  double m_forceCoef;
  double m_forceRatio;

  bool m_run;
};


#endif

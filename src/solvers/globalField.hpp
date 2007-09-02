#ifndef GLOBALFIELD_HPP
#define GLOBALFIELD_HPP

class GlobalField;

#include "field3D.hpp"
#include <vector>
#include <list>

class FieldThread;
class Field3D;
class Scene;

/** La classe GlobalField est un champ de vecteur qui englobe toute la scène. Il sert de conteneur pour tous
 * les solveurs et c'est lui qui transmet les forces globales aux solveurs locaux.
 * 
 * @author	Flavien Bridault.
 */
class GlobalField
{
public:
  /** Constructeur du solveur.
   * @param localFields Tableau contenant les champs de vecteurs locaux.
   * @param nbLocalFields Nombre de champs locaux.
   * @param scene Pointeur sur la scène
   * @param n Résolution de la grille sur la dimension maximale.
   * @param timeStep Pas de temps utilisé pour la simulation.
   * @param buoyancy Intensité de la force de flottabilité dans le solveur.
   * @param omegaDiff Paramètre omega pour la diffusion
   * @param omegaProj Paramètre omega pour la projection.
   * @param epsilon Tolérance d'erreur pour GCSSOR.
   */
  GlobalField (const list <FieldThread *> &threads, Scene *const scene, char type, uint n,
	       float timeStep, float vorticityConfinement, float omegaDiff, float omegaProj, float epsilon);
  /** Destructeur. */
  virtual ~GlobalField () { delete m_field; };

  /** Lance une itération du solveur. */
  void iterate (void) { m_field->iterate(); shareForces(); };
  void cleanSources (void) { m_field->cleanSources(); };
  
  /** Ajoute de façon ponctuelle des forces externes sur une des faces du champ. Il faut également considérer
   * l'ajout des forces dans les solveurs locaux.
   * @param position Nouvelle position du solveur. Détermine l'intensité de la force.
   * @param move Si true, alors le solveur est en plus déplacé à la position passée en paramètre.
   */
  void shareForces();

  /** Ajoute de façon permanente des forces externes sur une des faces du solveur. Cette méthode
   * est utilisée principalement lorsque que du vent est appliqué sur une flamme.
   * @param forces Intensité de la force en (x,y,z).
   */
  virtual void addPermanentExternalForces(Point& forces){ m_field->addPermanentExternalForces(forces); }

  /** Ajoute de façon temporaire (une itération) des forces externes sur une des faces du solveur. Cette méthode
   * est utilisée principalement lorsque que du vent est appliqué sur une flamme.
   * @param forces Intensité de la force en (x,y,z).
   */
  virtual void addTemporaryExternalForces(Point& forces) { m_field->addTemporaryExternalForces(forces); }

  /** Modifie la force de flottabilité dans le solveur
   * @param value Nouvelle valeur.
   */
  virtual void setBuoyancy(float value){ m_field->setBuoyancy(value); };
  /** Modifie la force de flottabilité dans le solveur
   * @param value Nouvelle valeur.
   */
  virtual void setVorticity(float value){ m_field->setVorticity(value); };
  
  /** Fonction de dessin du champ de vélocité */
  void displayVelocityField (void) { m_field->displayVelocityField(); };
  
  /** Fonction de dessin de la grille */
  void displayGrid (void){ m_field->displayGrid(); };
  
  /** Fonction de dessin du repère de base */
  void displayBase (void){ m_field->displayBase(); };
  
  /** Retourne la position du solveur dans le repère du monde 
   * @return Position dans l'espace.
   */
  Point getPosition (void) { return m_field->getPosition(); };
  
protected:
  /** Pointeur sur les solveurs contenus */
  vector <Field3D*> m_localFields;
  Field3D *m_field;
};

#endif

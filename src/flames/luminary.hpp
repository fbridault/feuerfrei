#ifndef LUMINARY_HPP
#define LUMINARY_HPP

#include "abstractFires.hpp"
#include "../solvers/field3D.hpp"

/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE LUMINARY ****************************************/
/**********************************************************************************************************************/

/** La classe Luminary
 *
 * @author	Flavien Bridault
 */
class Luminary
{
public:
  /** Constructeur d'une source de flammes. La position de la source est donnée dans le repère du solveur.
   * @param config Configuration du luminaire.
   * @param fields Vecteur contenant les solveurs de la scène.
   * @param fireSources Vecteur contenant les feux de la scène.
   * @param scene Pointeur sur la scène.
   * @param program Pointeur sur le program chargé de la construction des shadow volumes.
   * @param filename Nom du fichier contenant le luminaire.
   * @param index Indice de la flamme dans la scène (pour attribution d'une lumière OpenGL).
   */
  Luminary (const LuminaryConfig& config, vector <Field3D *> &fields, vector <FireSource *> &fireSources,
	    Scene* const scene, const GLSLProgram * const program, const char *filename, uint index);
  /** Destructeur */
  virtual ~Luminary ();
  
  Field3D* initField(const SolverConfig& fieldConfig, const Point& position);
  FireSource* initFire(const FlameConfig& flameConfig, const char *fileName, Field3D* field, Scene* const scene, 
		       uint i, const GLSLProgram* const SVProgram);
  /** Retourne la position absolue dans le repère du monde.
   * @return Position absolue dans le repère du monde.
   */
  virtual Point getPosition () const { return m_position; };
  
  /** Dessine le luminaire de la flamme. Les luminaires sont définis en (0,0,0), une translation
   * est donc effectuée pour tenir compte du placement du feu dans le monde.
   */
  void draw() const
  {
    if(m_hasLuminary){
      Point position(getPosition());
      glPushMatrix();
      glTranslatef (position.x, position.y, position.z);
      glScalef (m_scale.x, m_scale.y, m_scale.z);
      for (list < Object* >::const_iterator luminaryIterator = m_luminary.begin ();
	   luminaryIterator  != m_luminary.end (); luminaryIterator++)
	(*luminaryIterator)->draw();
      glPopMatrix();
    }
  }
  
  /** Ajuste la valeur d'échantillonnage de la NURBS.
   * @param value Valeur de sampling, généralement compris dans un intervalle [1;1000]. 
   */
  virtual void setSamplingTolerance(u_char value){ 
    for (list < FireSource* >::iterator fireIterator = m_fireSources.begin ();
	 fireIterator != m_fireSources.end (); fireIterator++)
      (*fireIterator)->setSamplingTolerance(value);
  };
  
  /** Affectation du coefficient multiplicateur de la FDF.
   * @param value Coefficient.
   */
  virtual void setInnerForce(double value){ 
    for (list < FireSource* >::iterator fireIterator = m_fireSources.begin ();
	 fireIterator != m_fireSources.end (); fireIterator++)
      (*fireIterator)->setInnerForce(value);
  };
  
  /** Affectation de la FDF.
   * @param value FDF.
   */
  virtual void setFDF(int value) { 
    for (list < FireSource* >::iterator fireIterator = m_fireSources.begin ();
	 fireIterator != m_fireSources.end (); fireIterator++)
      (*fireIterator)->setFDF(value);
  };
  
  /** Affectation de la méthode de perturbation.
   * @param value Perturbation.
   */
  virtual void setPerturbateMode(char value) {
    for (list < FireSource* >::iterator fireIterator = m_fireSources.begin ();
	 fireIterator != m_fireSources.end (); fireIterator++)
      (*fireIterator)->setPerturbateMode(value);
  };
  
  /** Affectation de la durée de vie des squelettes guides.
   * @param value Durée de vie en itérations.
   */
  virtual void setLeadLifeSpan(uint value) {
    for (list < FireSource* >::iterator fireIterator = m_fireSources.begin ();
	 fireIterator != m_fireSources.end (); fireIterator++)
      (*fireIterator)->setLeadLifeSpan(value);
  };
  
  /** Affectation de la durée de vie des squelettes périphériques.
   * @param value Durée de vie en itérations.
   */
  virtual void setPeriLifeSpan(uint value) { 
    for (list < FireSource* >::iterator fireIterator = m_fireSources.begin ();
	 fireIterator != m_fireSources.end (); fireIterator++)
      (*fireIterator)->setPeriLifeSpan(value); 
  };

  virtual void setBuoyancy(double value){
    for (list < Field3D* >::iterator fieldIterator = m_fields.begin ();
	 fieldIterator != m_fields.end (); fieldIterator++)
      (*fieldIterator)->setBuoyancy(value); 
  }
  
  /** Déplace le luminaire.
   * @param forces Déplacement en (x,y,z).
   */
  virtual void move(const Point& position) { 
    Point diff = position - m_position;
    for (list < Field3D* >::iterator fieldIterator = m_fields.begin ();
	 fieldIterator != m_fields.end (); fieldIterator++)
      (*fieldIterator)->move(diff);
    m_position = position;
  }
  
protected:
  /** Luminaire */
  list <Object *> m_luminary;
  /** Luminaire */
  list <FireSource *> m_fireSources;
  /** Luminaire */
  list <Field3D *> m_fields;
  
  /** Il se peut que le luminaire ne soit pas un objet physique, dans ce cas ce booléen est à false */
  bool m_hasLuminary;
  
  /** Position du luminaire. */
  Point m_position, m_scale;
};


#endif
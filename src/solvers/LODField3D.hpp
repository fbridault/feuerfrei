#ifndef LODFIELD3D_HPP
#define LODFIELD3D_HPP

class LODField3D;

#include "fakeField3D.hpp"
#include "HybridSolver3D.hpp"


/** @test La classe LODField3D implémente un champ de vecteur fonctionnant soit comme un solveur de classe HybridSolver 
 * ou un FakeField3D, dans le but de moduler le niveau de détail en fonction de la distance.<br>
 * Conceptuellement parlant, la classe hérite de Field3D, mais possède une relation de composition avec FakeField3D et
 * HybridSolver. Son implémentation des comportements d'un Field3D consiste donc à appeler ceux de l'un ou l'autre de
 * ses composés.
 */
class LODField3D: public Field3D
{
public:
  /** Constructeur du solveur.
   * @param position Position du solveur de la flamme.
   * @param n_x Résolution de la grille en x.
   * @param n_y Résolution de la grille en y.
   * @param n_z Résolution de la grille en z.
   * @param dim Dimension du solveur, utilisé pour l'affichage de la flamme.
   * @param timeStep Pas de temps utilisé pour la simulation.
   * @param buoyancy Intensité de la force de flottabilité dans le solveur.
   * @param omegaDiff Paramètre omega pour la diffusion.
   * @param omegaProj Paramètre omega pour la projection.
   * @param epsilon Tolérance d'erreur pour GCSSOR.
   */
  LODField3D (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale, double timeStep,
		   double buoyancy, double omegaDiff, double omegaProj, double epsilon);
  virtual ~LODField3D () {};
  
  /********************* Redéfinition des méthodes héritées *********************/
  virtual void iterate ()
  {
    return m_currentField->iterate();
  };
    
  Point getUVW (const Point& pos, double selfVelocity) const
  {
    return m_currentField->getUVW (pos, selfVelocity);
  };
  
  void addUsrc (const Point& pos, double value)
  {
    m_currentField->addUsrc (pos, value);
  };
  
  void addVsrc (const Point& pos, double value, double& selfVelocity)
  {
    m_currentField->addVsrc (pos, value, selfVelocity);
  };
  
  void addWsrc (const Point& pos, double value)
  {
    m_currentField->addWsrc (pos, value);
  };
  
  void setVsrc (const Point& pos, double value)
  {
    m_currentField->setVsrc (pos, value);
  };  
    
  void cleanSources ()
  {
    m_currentField->cleanSources ();
  };
  
  void displayVelocityField (void)
  {
    m_currentField->displayVelocityField ();
  };
  
  Point getPosition (void) const { return m_currentField->getPosition(); };
  
  void setPosition (const Point& position) { 
    m_fakeField.setPosition(position);
    m_solver.setPosition(position); 
  };
  
  void addTemporaryExternalForces(Point& forces)
  {
    m_fakeField.addTemporaryExternalForces(forces);
    m_solver.addTemporaryExternalForces(forces);
  }
    
  void addPermanentExternalForces(Point& forces)
  {
    m_fakeField.addPermanentExternalForces(forces);
    m_solver.addPermanentExternalForces(forces);
  }

  void addForcesOnFace(unsigned char face, const Point& BLStrength, const Point& TLStrength,
		       const Point& TRStrength, const Point& BRStrength)
  {
    m_fakeField.addForcesOnFace(face, BLStrength, TLStrength, TRStrength, BRStrength);
    m_solver.addForcesOnFace(face, BLStrength, TLStrength, TRStrength, BRStrength);
  }
  
  virtual void setBuoyancy(double value){ 
    m_fakeField.setBuoyancy(value);
    m_solver.setBuoyancy(value);
  }
  
  virtual void setRunningState(bool state) { 
    m_fakeField.setRunningState(state);
    m_solver.setRunningState(state);  
  }
  
  virtual bool isRealSolver () const { return (m_currentField == &m_solver); };
  virtual void switchToRealSolver () { m_currentField = &m_solver; };
  virtual void switchToFakeField () { m_currentField = &m_fakeField; };
private:
  virtual void addExternalForces(const Point& position, bool move) {};
  
  FakeField3D m_fakeField;
  HybridSolver3D m_solver;
  /** Pointeur sur le champ de vecteur utilisé. */
  Field3D *m_currentField;
};


#endif

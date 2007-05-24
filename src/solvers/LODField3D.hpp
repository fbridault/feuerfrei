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
  
  virtual Point getUVW (const Point& pos, double selfVelocity) const
  {
    return m_currentField->getUVW (pos, selfVelocity);
  };
  
  virtual void moveParticle (Particle& particle, double selfVelocity) const
  {
    m_currentField->moveParticle (particle, selfVelocity);
  };
  
  void addUsrc (const Point& pos, double value)
  {
    m_currentField->addUsrc (pos, value);
  };
  
  virtual void addVsrc (const Point& pos, double value, double& selfVelocity)
  {
    m_currentField->addVsrc (pos, value, selfVelocity);
  };
  
  void addWsrc (const Point& pos, double value)
  {
    m_currentField->addWsrc (pos, value);
  };
  
  virtual void cleanSources ()
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
  
protected:
  FakeField3D m_fakeField;
  LODHybridSolver3D m_solver;
  /** Pointeur sur le champ de vecteur utilisé. */
  Field3D *m_currentField;
private:
  virtual void addExternalForces(const Point& position, bool move) {};
};

#define NB_STEPS_TO_SWITCH 40

/** @test La classe LODHybridField implémente un champ de vecteur fonctionnant sur la base d'un LODField3D.
 * La différence intervient lors de la transition entre deux solveurs, une interpolation linéaire est effectuée.
 */
class LODHybridField: public LODField3D
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
  LODHybridField (const Point& position, uint n_x, uint n_y, uint n_z, double dim, const Point& scale, double timeStep,
		double buoyancy, double omegaDiff, double omegaProj, double epsilon);
  virtual ~LODHybridField () {};
  
  /********************* Redéfinition des méthodes héritées *********************/
  void iterate ()
  {
    /* Adaptation de la taille de la grille */
    while(m_decreaseCount > 0){
      m_solver.decreaseRes();
      m_decreaseCount--;
    }
    while(m_increaseCount > 0){
      m_solver.increaseRes();
      m_increaseCount--;
    }
    
    if(m_switch)
      {
	m_switch--;
	if(m_switch){
	  m_fieldWeight += m_fieldIncrement;
	  m_solverWeight += m_solverIncrement;
	  m_fakeField.iterate();
	  m_solver.iterate();
	}else{
	  m_currentField = m_fieldToSwitch;
	  m_currentField->iterate();
	}
      }
    else
      m_currentField->iterate();
  };
  
  Point getUVW (const Point& pos, double selfVelocity) const
  {
    if(m_switch)
      return (m_fakeField.getUVW (pos, selfVelocity)*m_fieldWeight + m_solver.getUVW (pos, selfVelocity)*m_solverWeight);
    else
      return m_currentField->getUVW (pos, selfVelocity);
  };
  
  void moveParticle (Particle& pos, double selfVelocity) const
  {
    if(m_switch)
      {
	Particle p(pos);
	m_fakeField.moveParticle (p, selfVelocity);
	m_solver.moveParticle (pos, selfVelocity);
	pos = p*m_fieldWeight + pos*m_solverWeight;
      }
    else
      m_currentField->moveParticle (pos, selfVelocity);
  };
  
  void addVsrc (const Point& pos, double value, double& selfVelocity)
  {
    //if(m_switch)
    m_solver.addVsrc (pos, value, selfVelocity);
    m_fakeField.addVsrc (pos, value, selfVelocity);
    //}else
    //m_currentField->addVsrc (pos, value, selfVelocity);
  };
  
  void cleanSources ()
  {
    //if(m_switch){
    m_fakeField.cleanSources();
    m_solver.cleanSources();
    //}else
    //m_currentField->cleanSources();
  };
  
  void displayGrid (void)
  {
    m_currentField->displayGrid ();
  };
  
  void switchToRealSolver ()
  {
    cerr << "real" << endl;
    m_switch = NB_STEPS_TO_SWITCH;
    m_fieldWeight = 1;
    m_solverWeight = 0;
    m_fieldIncrement = -1/(double)NB_STEPS_TO_SWITCH;
    m_solverIncrement = 1/(double)NB_STEPS_TO_SWITCH;
    m_fieldToSwitch = &m_solver; 
  };
  
  void switchToFakeField () 
  {
    cerr << "fake" << endl;
    m_switch = NB_STEPS_TO_SWITCH;
    m_fieldWeight = 0;
    m_solverWeight = 1;
    m_fieldIncrement =   1/(double)NB_STEPS_TO_SWITCH;
    m_solverIncrement = -1/(double)NB_STEPS_TO_SWITCH;
    m_fieldToSwitch = &m_fakeField;
//     while(m_solver.getXRes() > RESOLUTION_MIN && m_solver.getYRes() > RESOLUTION_MIN && m_solver.getZRes() > RESOLUTION_MIN )
//       m_solver.decreaseRes();
  };
  
  virtual void divideRes () { m_solver.divideRes(); };  
  virtual void multiplyRes () { m_solver.multiplyRes(); };  
  virtual void decreaseRes () { m_decreaseCount++; };  
  virtual void increaseRes () { m_increaseCount++; };
  virtual uint getNbMaxDiv () { return m_solver.getNbMaxDiv(); };
private:
  Field3D *m_fieldToSwitch;
  uint m_switch;
  double m_fieldWeight, m_solverWeight;
  double m_fieldIncrement, m_solverIncrement;
  uint m_decreaseCount, m_increaseCount;
};

#endif

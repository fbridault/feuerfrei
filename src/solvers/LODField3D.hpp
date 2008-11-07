#ifndef LODFIELD3D_HPP
#define LODFIELD3D_HPP

class LODSolver3D;
class LODField3D;

#include "fakeField3D.hpp"
#include "GCSSORSolver3D.hpp"

#define RESOLUTION_MIN 6

/** @test La classe LODSolver implémente un solveur de classe GCSSORSolver permettant d'utiliser une grille
 * adaptative. La résolution de la grille ne pourra jamais dépasser la résolution initiale passée au constructeur.
 */
class LODSolver3D: public GCSSORSolver3D
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
  LODSolver3D (const CPoint& position, uint n_x, uint n_y, uint n_z, float dim, const CPoint& scale, float timeStep,
	       float buoyancy, float vorticityConfinement, float omegaDiff, float omegaProj, float epsilon);
  virtual ~LODSolver3D ();

  void iterate ()
  {
    /* Adaptation de la taille de la grille */
    while(m_resCount < 0){
      decrementRes();
//       divideRes();
      m_resCount++;
    }
    while(m_resCount > 0){
      incrementRes();
//       multiplyRes();
      m_resCount--;
    }

    Solver3D::iterate();
  }

  virtual void decreaseRes () { m_resCount--; };
  virtual void increaseRes () { m_resCount++; };

  virtual uint getNbMaxDiv () { return m_nbMaxDiv; };

  /** Fonction de dessin de la grille */
  virtual void displayGrid ();

  /** Fonction de dessin du repère de base */
  virtual void displayBase ();

private:
  virtual void recomputeAttributes();

  virtual void divideRes ();
  virtual void multiplyRes ();
  virtual void decrementRes ();
  virtual void incrementRes ();

  float *m_tmp;

  /** Nombre de voxels initiaux en X. */
  uint m_initialNbVoxelsX;
  /** Nombre de voxels initiaux en Y. */
  uint m_initialNbVoxelsY;
  /** Nombre de voxels initiaux en Z. */
  uint m_initialNbVoxelsZ;
  /** Variable permettant de mémoriser les changements de grille, qui ne sont effectués réellement
   * qu'en début d'itération dans iterate().
   */
  int m_resCount;
  /** Niveau dans la grille. Le niveau supérieur est noté 0. */
  uint m_level;
};

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
  LODField3D (const CPoint& position, uint n_x, uint n_y, uint n_z, float dim, const CPoint& scale, float timeStep,
	      float buoyancy, float vorticityConfinement, float omegaDiff, float omegaProj, float epsilon);
  virtual ~LODField3D () {};

  /********************* Redéfinition des méthodes héritées *********************/
  virtual void iterate ()
  {
    /* Changement de type de champ de vélocité si demandé */
    if(m_currentField != m_fieldToSwitch)
      m_currentField = m_fieldToSwitch;

    m_currentField->iterate();
  };

  virtual CPoint getUVW (const CPoint& pos, float selfVelocity) const
  {
    return m_currentField->getUVW (pos, selfVelocity);
  };

  virtual void moveParticle (Particle& particle, float selfVelocity) const
  {
    m_currentField->moveParticle (particle, selfVelocity);
  };

  void addUsrc (const CPoint& pos, float value)
  {
    m_currentField->addUsrc (pos, value);
  };

  void addVsrc (const CPoint& pos, float value, float& selfVelocity)
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

  void addWsrc (const CPoint& pos, float value)
  {
    m_currentField->addWsrc (pos, value);
  };

  void displayVelocityField (void)
  {
    m_currentField->displayVelocityField ();
  };

  void displayGrid (void)
  {
    m_currentField->displayGrid ();
  };

  CPoint const& getPosition (void) const { return m_currentField->getPosition(); };

  void setPosition (const CPoint& position)
  {
    m_fakeField.setPosition(position);
    m_solver.setPosition(position);
  }

  virtual void move(const CPoint& forces)
  {
    m_fakeField.move(forces);
    m_solver.move(forces);
  }

  void addTemporaryExternalForces(const CPoint& forces)
  {
    m_fakeField.addTemporaryExternalForces(forces);
    m_solver.addTemporaryExternalForces(forces);
  }

  void addPermanentExternalForces(const CPoint& forces)
  {
    m_fakeField.addPermanentExternalForces(forces);
    m_solver.addPermanentExternalForces(forces);
  }

  void addForcesOnFace(unsigned char face, const CPoint& BLStrength, const CPoint& TLStrength,
		       const CPoint& TRStrength, const CPoint& BRStrength)
  {
    m_fakeField.addForcesOnFace(face, BLStrength, TLStrength, TRStrength, BRStrength);
    m_solver.addForcesOnFace(face, BLStrength, TLStrength, TRStrength, BRStrength);
  }

  virtual void setBuoyancy(float value)
  {
    m_fakeField.setBuoyancy(value);
    m_solver.setBuoyancy(value);
  }

  virtual void setVorticity(float value)
  {
    m_fakeField.setVorticity(value);
    m_solver.setVorticity(value);
  }

  virtual void setRunningState(bool state)
  {
    m_fakeField.setRunningState(state);
    m_solver.setRunningState(state);
  }

  virtual bool isRealSolver () const { return (m_currentField == &m_solver); };
  virtual void switchToRealSolver () { m_fieldToSwitch = &m_solver; };
  virtual void switchToFakeField () { m_fieldToSwitch = &m_fakeField; };

  virtual void decreaseRes () { m_solver.decreaseRes(); };
  virtual void increaseRes () { m_solver.increaseRes(); };
  virtual uint getNbMaxDiv () { return m_solver.getNbMaxDiv(); };
protected:
  FakeField3D m_fakeField;
  LODSolver3D m_solver;
  /** CPointeur sur le champ de vecteur utilisé. */
  Field3D *m_currentField;
  Field3D *m_fieldToSwitch;
private:
  virtual void addExternalForces(const CPoint& position, bool move) {};
  virtual void add_source (float *const x, float *const src){};
};

#define NB_STEPS_TO_SWITCH 40

/** @test La classe LODSmoothField implémente un champ de vecteur fonctionnant sur la base d'un LODField3D.
 * La différence intervient lors de la transition entre deux solveurs, une interpolation linéaire est effectuée.
 */
class LODSmoothField: public LODField3D
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
  LODSmoothField (const CPoint& position, uint n_x, uint n_y, uint n_z, float dim, const CPoint& scale, float timeStep,
		  float buoyancy, float vorticityConfinement, float omegaDiff, float omegaProj, float epsilon);
  virtual ~LODSmoothField () {};

  /********************* Redéfinition des méthodes héritées *********************/
  void iterate ()
  {
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

  CPoint getUVW (const CPoint& pos, float selfVelocity) const
  {
    if(m_switch)
      return (m_fakeField.getUVW (pos, selfVelocity)*m_fieldWeight + m_solver.getUVW (pos, selfVelocity)*m_solverWeight);
    else
      return m_currentField->getUVW (pos, selfVelocity);
  };

  void moveParticle (Particle& pos, float selfVelocity) const
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

  void switchToRealSolver ()
  {
    cerr << "real" << endl;
    m_switch = NB_STEPS_TO_SWITCH;
    m_fieldWeight = 1.0f;
    m_solverWeight = 0.0f;
    m_fieldIncrement = -1.0f/(float)NB_STEPS_TO_SWITCH;
    m_solverIncrement = 1.0f/(float)NB_STEPS_TO_SWITCH;
    m_fieldToSwitch = &m_solver;
  };

  void switchToFakeField ()
  {
    cerr << "fake" << endl;
    m_switch = NB_STEPS_TO_SWITCH;
    m_fieldWeight = 0.0f;
    m_solverWeight = 1.0f;
    m_fieldIncrement =   1.0f/(float)NB_STEPS_TO_SWITCH;
    m_solverIncrement = -1.0f/(float)NB_STEPS_TO_SWITCH;
    m_fieldToSwitch = &m_fakeField;
//     while(m_solver.getXRes() > RESOLUTION_MIN && m_solver.getYRes() > RESOLUTION_MIN && m_solver.getZRes() > RESOLUTION_MIN )
//       m_solver.decreaseRes();
  };

private:
  uint m_switch;
  float m_fieldWeight, m_solverWeight;
  float m_fieldIncrement, m_solverIncrement;
};

#endif

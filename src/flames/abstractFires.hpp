#if !defined(FIRE_H)
#define FIRE_H

class FlameLight;
class FireSource;

#include "flames.hpp"

#include "realFlames.hpp"
#include "../solvers/solver.hpp"
#include "../scene/texture.hpp"
#include "../scene/object.hpp"
#include "../shaders/CgSVShader.hpp"
#include "ies.hpp"

class RealFlame;
class Solver;
class Object;
class Scene;
class IES;


/**********************************************************************************************************************/
/************************************** DEFINITION DE LA CLASSE FLAMELIGHT ********************************************/
/**********************************************************************************************************************/

/** La classe FlameLight défini une flamme comme une source de lumière pour la scène.
 * Elle permet de spécifier les méthodes d'éclairage et d'ombrage. En revanche, la flamme
 * en elle-même n'est pas représentée ici.
 */
class FlameLight
{
public:
  /** Constructeur de flamme simple.
   * @param scene pointeur sur la scène
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL)
   * @param shader pointeur sur le shader chargé de la construction des shadow volumes
   */
  FlameLight (Scene *scene, uint index, CgSVShader* shader, const char* const IESFilename);
  virtual ~FlameLight();
  
  void setLightPosition (Point& pos)
  {
    m_lightPosition[0] = pos.x;
    m_lightPosition[1] = pos.y;
    m_lightPosition[2] = pos.z;
  }
  
  void drawShadowVolume ();  
  
  void switchOn (double coef);
  
  void switchOff ();
  
  void resetDiffuseLight ();
    
  float *getIntensities(void) const { return m_iesFile->getIntensities(); };
  
  const double getIntensity(void) const { return m_intensity; };
  
  const void getCenterSP(double& x, double& y, double& z) const { x = m_centreSP.x; y = m_centreSP.y; z = m_centreSP.z; };

  const double getLazimut() const {return m_iesFile->getLazimut();};
  
  const double getLzenith() const {return m_iesFile->getLzenith();};
  
  const double getLazimutTEX() const {return m_iesFile->getLazimutTEX();};
  
  const double getLzenithTEX() const {return m_iesFile->getLzenithTEX();};
  
  /** Retourne le nombre de valeurs en zénithal */
  const uint getIESZenithSize() const {return m_iesFile->getNbzenith();};
  
  /** Retourne le nombre de valeurs en azimuthal */
  const uint getIESAzimuthSize() const {return m_iesFile->getNbazimut();};
  
  /** Supprime le fichier IES courant et en utilise un autre */
  void useNewIESFile(const char* const IESFilename) { delete m_iesFile; m_iesFile = new IES(IESFilename); };
  
protected:
  /** Centre du solide photométrique dans l'espace */
  Point m_centreSP;
  /** Orientation du solide photométrique, utilisée pour la rotation */
  double m_orientationSPtheta;
  /** Axe de rotation */
  Vector m_axeRotation;
  /** Valeur de l'intensité du solide */
  double m_intensity;   

private:
  short m_light;
  
  /** Pointeur sur la scène */
  Scene *m_scene;

  /** Pointeur sur le shader générateur de volumes d'ombres */
  CgSVShader *m_cgShader;
  
  /** Position de la lumière ponctuelle OpenGL dans l'espace */
  GLfloat m_lightPosition[4];
  
  /** Fichier IES utilisé pour le solide photométrique de la source */
  IES *m_iesFile;
};


/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE FIRESOURCE ****************************************/
/**********************************************************************************************************************/

/** La classe FireSource désigne un objet qui produit du feu. Il se compose d'un luminaire (classe Object), de
 * flammes (classe RealFlame) et d'une source de lumière (classe FlameLight). Cette classe est abstraite.<br>
 * La position de la flamme stockée dans m_position est une position relative par rapport au solveur à laquelle
 * elle appartient. Pour obtenir la position absolue dans le repère du monde, il faut passer par la méthode
 * getPosition() qui additionne la position relative de la flamme à la position absolue du solveur dans le monde.
 * Toute la construction de la flamme est faite dans le repère (0,0,0). Lors du dessin de la flamme dans la méthode
 * drawFlame(), une translation est effectuée en récupérant la position avec getPosition() pour placer la flamme
 * dans le repère du monde.
 *
 * @author	Flavien Bridault
 */
class FireSource : public FlameLight
{
public:
  /** Constructeur d'une source de flammes. La position de la source est donnée dans le repère du solveur.
   * @param s pointeur sur le solveur de fluides
   * @param nbFlames nombre de flammes, si = 0 alors le tableau contenant les flammes n'est pas alloué
   * et ceci doit alors être réalisé par la classe fille
   * @param scene pointeur sur la scène
   * @param innerForce force intérieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param texname nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL)
   * @param shader pointeur sur le shader chargé de la construction des shadow volumes
   * @param objname nom du luminaire à charger dans le fichier filename
   */
  FireSource (FlameConfig* flameConfig, Solver * s, uint nbFlames, Scene *scene, const char *filename,
	      const wxString &texname, uint index, CgSVShader * shader, const char *objName=NULL);
  virtual ~FireSource ();

  virtual void setSamplingTolerance(double value){ 
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->setSamplingTolerance(value);
  };
  
//   virtual void setRenderMode() {  
//     for (uint i = 0; i < m_nbFlames; i++)
//       m_flames[i]->setRenderMode();
//   };

//   virtual void setTesselateMode() { 
//     for (uint i = 0; i < m_nbFlames; i++)
//       m_flames[i]->setTesselateMode();
//   };

  /** Retourne la position absolue dans le repère du monde .
   * @return position absolue dans le repère du monde
   */
  Point getPosition ()
  {
    return Point(m_position+m_solver->getPosition());
  }
  
  /** Fonction chargée de construire les flammes composant la source de feu. Elle se charge également 
   * de déterminer la position de la source de lumière.
   */
  virtual void build();
  
  /** Dessine la mèche de la flamme. Les mèches des RealFlame sont définies en (0,0,0), une translation
   * est donc effectuée pour tenir compte du placement du feu dans le monde.
   */
  virtual void drawWick(bool displayBoxes)
  {      
    Point pt(getPosition());
    glPushMatrix();
    glTranslatef (pt.x, pt.y, pt.z);
    glScalef (m_solver->getDimX(), m_solver->getDimY(), m_solver->getDimZ());
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->drawWick(displayBoxes);
    glPopMatrix();
  }
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
   * build() avec le placage de texture. La flamme d'une RealFlame est définie dans le repère du solveur,
   * donc seule une translation correspondant à la position du solveur est effectuée.
   *
   * @param displayParticle affiche ou non les particules des squelettes
   */
  virtual void drawFlame(bool displayParticle)
  {
    Point pt(m_solver->getPosition());
    glPushMatrix();
    glTranslatef (pt.x, pt.y, pt.z);
    glScalef (m_solver->getDimX(), m_solver->getDimY(), m_solver->getDimZ());
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->drawFlame(displayParticle);  
    glPopMatrix();
  }  

  virtual void drawCachedFlame()
  {
    Point pt(m_solver->getPosition());
    glPushMatrix();
    glTranslatef (pt.x, pt.y, pt.z);
    glScalef (m_solver->getDimX(), m_solver->getDimY(), m_solver->getDimZ());
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->drawCachedFlame();  
    glPopMatrix();
  } 
  /** Dessine la flamme et sa mèche
   *
   * @param displayParticle affiche ou non les particules des squelettes
   */
  void draw(bool displayParticle, bool displayBoxes)
  {
    glScalef (m_solver->getDimX(), m_solver->getDimY(), m_solver->getDimZ());
    drawWick(displayBoxes);
    drawFlame(displayParticle);
  }
  
  /** Dessine le luminaire de la flamme. Les luminaires sont définis en (0,0,0), une translation
   * est donc effectuée pour tenir compte du placement du feu dans le monde.
   */
  void drawLuminary()
  {
    if(m_hasLuminary){
      Point position(getPosition());
      glPushMatrix();
      glTranslatef (position.x, position.y, position.z);
      glScalef (m_solver->getDimX(), m_solver->getDimY(), m_solver->getDimZ());
      glCallList(m_luminaryDL);
      glPopMatrix();
    }
  }
    
  /** Dessine le luminaire de la flamme. Les luminaires sont définis en (0,0,0), une translation
   * est donc effectuée pour tenir compte du placement du feu dans le monde.
   *
   * @param shader vertex program utilisé pour le solide photométrique 
   */
  void drawLuminary(CgBasicVertexShader& shader)
  {
    if(m_hasLuminary){
      Point position(getPosition());
      glPushMatrix();
      glTranslatef (position.x, position.y, position.z);
      glScalef (m_solver->getDimX(), m_solver->getDimY(), m_solver->getDimZ());
      shader.setModelViewProjectionMatrix();
      glCallList(m_luminaryDL);
      glPopMatrix();
    }
  }
  
  /** Fonction appelée par le solveur de fluides pour ajouter l'élévation thermique de la flamme.
   * Elle demande à chacune des flammes composant le feu d'ajouter ses forces.
   *
   * @param perturbate type de perturbation dans l'élévation thermique
   */
  virtual void addForces (char perturbate, char fdf=0)
  {
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->addForces(perturbate,fdf);
  }
  
  virtual void setForces(double value)
  {
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->setForces(value);
  }
  
  virtual void toggleSmoothShading ()
  {
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->toggleSmoothShading();
  }
  
  /** Fonction permettant de récupérer l'orientation principale de la flamme
   * pour orienter le solide photométrique.
   */
  virtual Vector getMainDirection();
  
  virtual void locateInSolver(){
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->locateInSolver();
  }
  
  /** Calcul de l'intensité du centre et de l'orientation du solide photométrique
   */
  void computeIntensityPositionAndDirection(void);
  
protected:
  /** Nombre de flammes */
  uint m_nbFlames;
  /** Tableau contenant les flammes */
  RealFlame **m_flames;
  
  /** Index de la display list contenant le luminaire */
  GLuint m_luminaryDL;
  
  /** Pointeur sur le solveur de fluides */
  Solver *m_solver;

  /** Position relative de la source dans le solveur auquel elle appartient */
  Point m_position;

  /** Est-ce que la source possède un luminaire */
  bool m_hasLuminary;

  /** Indique si la source produit des flammes détachées */
  bool m_breakable;

  /** Texture utilisée pour les flammes */
  Texture m_texture;
};

/** La classe Firesource ajoute la notion de flammes détachées.
 *
 * @author	Flavien Bridault
 */
class DetachableFireSource : public FireSource
{
public:
  /** Constructeur.
   * @param posRel position du centre de la flamme dans le solveur.
   * @param scene pointeur sur la scène
   * @param s pointeur sur le solveur de fluides
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la scène (pour attribution d'une lumière OpenGL)
   * @param shader pointeur sur le shader chargé de la construction des shadow volumes
   * @param wickFileName nom du fichier contenant la mèche
   */
  DetachableFireSource (FlameConfig* flameConfig, Solver * s, uint nbFlames, Scene *scene, const char *filename,
			const wxString &texname, uint index, CgSVShader * shader, const char *objName=NULL);
  virtual ~DetachableFireSource();
  
  /** Fonction chargée de construire les flammes composant la source de feu. Elle se charge également 
   * de déterminer la position de la source de lumière.
   */
  virtual void build();
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
   * build() avec le placage de texture. La flamme d'une RealFlame est définie dans le repère du solveur,
   * donc seule une translation correspondant à la position du solveur est effectuée.
   *
   * @param displayParticle affiche ou non les particules des squelettes
   */
  virtual void drawFlame(bool displayParticle);
  
  virtual void addDetachedFlame(DetachedFlame* detachedFlame)
  {
    m_detachedFlamesList.push_back(detachedFlame);
  }
  
  virtual void removeDetachedFlame(DetachedFlame* detachedFlame)
  {
    m_detachedFlamesList.remove(detachedFlame);
  }
  
private:
  /** Liste des flammes détachées */
  list<DetachedFlame *> m_detachedFlamesList;
};

#endif

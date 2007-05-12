#if !defined(FIRE_H)
#define FIRE_H

#define NO_BOUNDING_VOLUME 0
#define BOUNDING_SPHERE 1
#define BOUNDING_BOX 2

class FlameLight;
class FireSource;

#include "flames.hpp"

#include "../scene/graphicsFn.hpp"

#include "../solvers/solver3D.hpp"
#include "../scene/object.hpp"
#include "../scene/texture.hpp"
#include "../shaders/glsl.hpp"
#include "realFlames.hpp"
#include "ies.hpp"

class RealFlame;
class Field3D;
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
  /** Constructeur de source lumineuse de type flamme.
   * @param scene pointeur sur la scène.
   * @param index Indice de la lumière pour OpenGL. Compris entre [0;7].
   * @param program pointeur sur le program chargé de la construction des shadow volumes.
   * @param IESFilename nom du fichier IES à utiliser
   */
  FlameLight (const Scene* const scene, uint index, const GLSLProgram* const program, const char* const IESFilename);
  
  /** Destructeur */
  virtual ~FlameLight();
  
  /** Déplace la lumière à la position. Ceci déplace en réalité la lumière OpenGL.
   * @param pos Position de la lumière.
   */
  void setLightPosition (const Point& pos)
  {
    m_lightPosition[0] = pos.x;
    m_lightPosition[1] = pos.y;
    m_lightPosition[2] = pos.z;
    m_centreSP = pos;
  }
  
  /** Dessine les shadow volumes à partir de la source. */
  void drawShadowVolume (GLfloat fatness[4], GLfloat extrudeDist[4]);  
  
  /** Active la source de lumière. */
  void switchOn ();  
  /** Eteint la source */
  void switchOff ();
  
  /** Récupération du tableau des intensités du solide photométrique.
   * @return Pointeur sur les intensités.
   */
  float *getIntensities(void) const { return m_iesFile->getIntensities(); };
  
  /** Retourne l'intensité globale de la source.
   * @return Coefficient de l'intensité.
   */
  const double getIntensity(void) const { return m_intensity; };
  
  /** Récupération du centre du solide photométrique.
   * @param x Coordonnée x.
   * @param y Coordonnée y.
   * @param z Coordonnée z.
   */
  void getCenterSP(GLfloat& x, GLfloat& y, GLfloat& z) const { 
    x = (GLfloat)m_centreSP.x; y = (GLfloat)m_centreSP.y; z = (GLfloat)m_centreSP.z;
  };
  
  /** Récupération du centre.
   */
  Point getCenterSP(void) const { return m_centreSP; };
  
  const double getLazimut() const {return m_iesFile->getLazimut();};
  
  const double getLzenith() const {return m_iesFile->getLzenith();};
  
  const double getLazimutTEX() const {return m_iesFile->getLazimutTEX();};
  
  const double getLzenithTEX() const {return m_iesFile->getLzenithTEX();};
  
  /** Retourne le nombre de valeurs en zénithal. */
  const uint getIESZenithSize() const {return m_iesFile->getNbzenith();};
  
  /** Retourne le nombre de valeurs en azimuthal. */
  const uint getIESAzimuthSize() const {return m_iesFile->getNbazimut();};
  
  /** Supprime le fichier IES courant et en utilise un autre. */
  void useNewIESFile(const char* const IESFilename) { delete m_iesFile; m_iesFile = new IES(IESFilename); };
  
  /** Calcul de l'intensité du centre et de l'orientation du solide photométrique. */
  virtual void computeIntensityPositionAndDirection() = 0;
  
  /** Modifie le coefficient pondérateur de l'intensité. */
  virtual void setIntensityCoef(double coef) { m_intensityCoef = coef; };
  
protected:
  /** Centre du solide photométrique dans l'espace. */
  Point m_centreSP;
  /** Orientation du solide photométrique, utilisée pour la rotation. */
  double m_orientationSPtheta;
  /** Axe de rotation. */
  Vector m_axeRotation;
  /** Valeur de l'intensité du solide. */
  double m_intensity;
  /** Coefficient pondérateur de l'intensité de la source. */
  double m_intensityCoef;
private:
  /** Indice de la lumière pour OpenGL. */
  short m_light;
  
  /** Pointeur sur la scène. */
  const Scene *m_scene;
  
  /** Pointeur sur le program générateur de volumes d'ombres. */
  const GLSLProgram *m_SVProgram;
  
  /** Position de la lumière ponctuelle OpenGL dans l'espace. */
  GLfloat m_lightPosition[4];
  
  /** Fichier IES utilisé pour le solide photométrique de la source. */
  IES *m_iesFile;
};


/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE FIRESOURCE ****************************************/
/**********************************************************************************************************************/

/** La classe FireSource désigne un objet qui produit du feu. Il se compose d'un luminaire (classe Object), de
 * flammes (classe RealFlame) et d'une source de lumière (classe FlameLight). Cette classe est abstraite.<br>
 * Pour obtenir la position absolue dans le repère du monde, il faut passer par la méthode
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
   * @param flameConfig Configuration de la flamme.
   * @param s Pointeur sur le solveur de fluides.
   * @param nbFlames Nombre de flammes, si = 0 alors le tableau contenant les flammes n'est pas alloué.
   * et ceci doit alors être réalisé par la classe fille.
   * @param scene Pointeur sur la scène.
   * @param filename Nom du fichier contenant le luminaire.
   * @param texname Nom du fichier contenant le luminaire.
   * @param index Indice de la flamme dans la scène (pour attribution d'une lumière OpenGL).
   * @param program Pointeur sur le program chargé de la construction des shadow volumes.
   * @param objName Nom du luminaire à charger dans le fichier filename.
   */
  FireSource (FlameConfig* const flameConfig, Field3D* const s, uint nbFlames, Scene* const scene, const char *filename,
	      const wxString &texname, uint index, const GLSLProgram* const program, const char *objName=NULL);
  /** Destructeur */
  virtual ~FireSource ();

  /** Ajuste la valeur d'échantillonnage de la NURBS.
   * @param value Valeur de sampling, généralement compris dans un intervalle [1;1000]. 
   */
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
  
  /** Retourne la position absolue dans le repère du monde.
   * @return Position absolue dans le repère du monde.
   */
  Point getPosition () const
  {
    return Point(m_solver->getPosition());
  }
  
  /** Fonction chargée de construire les flammes composant la source de feu. Elle se charge également 
   * de déterminer la position de la source de lumière.
   */
  virtual void build();
  
  /** Dessine la mèche de la flamme. Les mèches des RealFlame sont définies en (0,0,0), une translation
   * est donc effectuée pour tenir compte du placement du feu dans le monde.
   */
  virtual void drawWick(bool displayBoxes) const
  {      
    Point pt(getPosition());
    Point scale(m_solver->getScale());
    glPushMatrix();
    glTranslatef (pt.x, pt.y, pt.z);
    glScalef (scale.x, scale.y, scale.z);
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->drawWick(displayBoxes);
    glPopMatrix();
  }
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
   * build() avec le placage de texture. La flamme d'une RealFlame est définie dans le repère du solveur,
   * donc seule une translation correspondant à la position du solveur est effectuée.
   *
   * @param display affiche ou non la flamme
   * @param displayParticle affiche ou non les particules des squelettes
   * @parma boundingVolume Optionel, volume englobant à afficher: 0 aucun, 1 sphère, 2 boîte
   */
  virtual void drawFlame(bool display, bool displayParticle, u_char boundingVolume=0) const
  {
      switch(boundingVolume){
      case BOUNDING_SPHERE : drawBoundingSphere(); break;
      case BOUNDING_BOX : drawBoundingBox(); break;
      default : 
	if(m_visibility)
	  {
	    Point pt(m_solver->getPosition());
	    Point scale(m_solver->getScale());
	    glPushMatrix();
	    glTranslatef (pt.x, pt.y, pt.z);
	    glScalef (scale.x, scale.y, scale.z);
	    for (uint i = 0; i < m_nbFlames; i++)
	      m_flames[i]->drawFlame(display, displayParticle);
	    glPopMatrix();
	  }
	break;
      }
  }
  
  virtual void drawBoundingSphere() const { if(m_visibility) m_boundingSphere.draw(); };
  
  virtual void drawBoundingBox() const
  {
    if(m_visibility){
      Point position(getPosition());
      Point scale(m_solver->getScale());
      glPushMatrix();
      glTranslatef (position.x, position.y, position.z);
      glScalef (scale.x, scale.y, scale.z);
      GraphicsFn::SolidBox(Point(),m_solver->getDim());
      glPopMatrix();
    }
  };
  
  /** Dessine le luminaire de la flamme. Les luminaires sont définis en (0,0,0), une translation
   * est donc effectuée pour tenir compte du placement du feu dans le monde.
   */
  void drawLuminary() const
  {
    if(m_hasLuminary){
      Point position(getPosition());
      Point scale(m_solver->getScale());
      glPushMatrix();
      glTranslatef (position.x, position.y, position.z);
      glScalef (scale.x, scale.y, scale.z);
      for (list < Object* >::const_iterator luminaryIterator = m_luminary.begin ();
	   luminaryIterator  != m_luminary.end (); luminaryIterator++)
	(*luminaryIterator)->draw();
      glPopMatrix();
    }
  }
  
  /** Fonction appelée par le solveur de fluides pour ajouter l'élévation thermique de la flamme.
   */
  virtual void addForces ()
  {
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->addForces();
  }
  
  /** Affectation de la vélocité induite par la flamme.
   * @param value Vélocité de la flamme.
   */
  virtual void setForces(double value)
  {
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->setForces(value);
  }
  
  /** Modifie le type de flickering sur les flammes. */  
  void setFlickeringMode(char mode){ m_flameConfig->flickering = mode; };

  /** Modifie le type de flickering sur les flammes. */  
  char getFlickeringMode(){ return m_flameConfig->flickering; };
  
  /** Active ou désactive l'affichage texturé sur la flamme. */
  virtual void setSmoothShading (bool state)
  {
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->setSmoothShading(state);
  }
  
  /** Fonction permettant de récupérer le centre de la flamme dans le repère local */
  virtual Point getCenter() const
  {
    Point averagePt;
    
    for (uint i = 0; i < m_nbFlames; i++)
      averagePt += m_flames[i]->getCenter ();
    averagePt = averagePt/m_nbFlames;
    
    return(averagePt);
  }
  /** Fonction permettant de récupérer l'orientation principale de la flamme
   */
  virtual Vector getMainDirection() const
  {
    Vector averageVec;
    
    for (uint i = 0; i < m_nbFlames; i++)
      averageVec += m_flames[i]->getMainDirection ();
    averageVec = averageVec/m_nbFlames;
    
    return(averageVec);
  }
  
  /** Calcul de l'intensité du centre et de l'orientation du solide photométrique */
  void computeIntensityPositionAndDirection(void);
  
  /** Construction de la sphère englobante de l'objet. */
  void buildBoundingSphere ();
  
  /** Calcul de la visibilité de la source. La méthode crée d'abord une sphère englobante 
   * et teste ensuite la visibilité de celle-ci. */
  virtual void computeVisibility(const Camera &view, bool forceSpheresBuild=false);
  
  bool isVisible() { return m_visibility; };
  double getDistance() { return m_dist; };

  virtual bool operator<(const FireSource& other) const;

  /** Opérateur de comparaison des flammes basées sur leur distance par rapport
   * à la caméra. On est obligé de définir cet opérateur car on utilise des
   * vecteurs de pointeurs pour stocker les flammes. */
  static bool cmp( const FireSource* a, const FireSource* b ) {
    return a->m_dist < b->m_dist;
  }
protected:
  /** Nombre de flammes */
  uint m_nbFlames;
  /** Tableau contenant les flammes */
  RealFlame **m_flames;
  
  /** Luminaire */
  list <Object *> m_luminary;
  
  /** Index de la display list contenant le luminaire */
  GLuint m_luminaryDL;
  
  uint m_nbObjLuminary;
  
  /** Est-ce que la source possède un luminaire */
  bool m_hasLuminary;

  /** Pointeur sur le solveur de fluides */
  Field3D *m_solver;
  
  /** Indique si la source produit des flammes détachées */
  bool m_breakable;

  /** Texture utilisée pour les flammes */
  Texture m_texture;
  
  /** Sphère englobante. */
  BoundingSphere m_boundingSphere;
  /** Visibilité de la flamme. */
  bool m_visibility;
  /** Distance par rapport à la caméra */
  double m_dist;

  /** Dernière valeur du flickering connue avant que la gestion du LOD ne la modifie */
  char m_flickSave;
  
  /** Configuration de la flamme */
  FlameConfig *m_flameConfig;
};

/** La classe Firesource ajoute la notion de flammes détachées.
 *
 * @author	Flavien Bridault
 */
class DetachableFireSource : public FireSource
{
public:
  /** Constructeur.
   * @param flameConfig Configuration de la flamme.
   * @param s Pointeur sur le solveur de fluides.
   * @param filename Nom du fichier contenant le luminaire.
   * @param nbFlames Nombre de flammes.
   * @param scene Pointeur sur la scène.
   * @param texname Nom du fichier image de la texture.
   * @param index Indice de la flamme dans la scène (pour attribution d'une lumière OpenGL).
   * @param program Pointeur sur le program chargé de la construction des shadow volumes.
   * @param objName Nom du luminaire à charger dans le fichier filename.
   */
  DetachableFireSource (FlameConfig* const flameConfig, Field3D* const s, uint nbFlames, Scene* const scene, const char *filename,
			const wxString &texname, uint index, const GLSLProgram* const program, const char *objName=NULL);
  virtual ~DetachableFireSource();
  
  virtual void build();
  virtual void drawFlame(bool display, bool displayParticle, u_char boundingVolume=0) const;
  
  virtual void drawBoundingBox () const;

  /** Ajoute une flamme détachée à la source.
   * @param detachedFlame Pointeur sur la nouvelle flamme détachée à ajouter.
   */
  virtual void addDetachedFlame(DetachedFlame* detachedFlame)
  {
    m_detachedFlamesList.push_back(detachedFlame);
  }
  
  /** Supprime une flamme détachée à la source.
   * @param detachedFlame Pointeur sur la flamme détachée à enlever.
   */
  virtual void removeDetachedFlame(DetachedFlame* detachedFlame)
  {
    m_detachedFlamesList.remove(detachedFlame);
  }
  
  /** Active ou désactive l'affichage texturé sur la flamme. */
  virtual void setSmoothShading (bool state);
  
  virtual void computeVisibility(const Camera &view, bool forceSpheresBuild=false);
  
private:
  /** Liste des flammes détachées */
  list<DetachedFlame *> m_detachedFlamesList;
  /** Sphère englobante pour les flammes détachées. */
  BoundingSphere m_boundingSphereForDetachedFlames;
  double m_differenceDist;
};

#endif

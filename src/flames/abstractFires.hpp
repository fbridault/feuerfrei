#if !defined(FIRE_H)
#define FIRE_H

#define NO_BOUNDING_VOLUME 0
#define BOUNDING_SPHERE 1
#define IMPOSTOR 2

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

#ifdef COUNT_NURBS_POLYGONS
extern uint g_count;
#endif


/**********************************************************************************************************************/
/************************************** DEFINITION DE LA CLASSE FLAMELIGHT ********************************************/
/**********************************************************************************************************************/

/** La classe FlameLight d�fini une flamme comme une source de lumi�re pour la sc�ne.
 * Elle permet de sp�cifier les m�thodes d'�clairage et d'ombrage. En revanche, la flamme
 * en elle-m�me n'est pas repr�sent�e ici.
 */
class FlameLight
{
public:
  /** Constructeur de source lumineuse de type flamme.
   * @param scene pointeur sur la sc�ne.
   * @param index Indice de la lumi�re pour OpenGL. Compris entre [0;7].
   * @param program pointeur sur le program charg� de la construction des shadow volumes.
   * @param IESFilename nom du fichier IES � utiliser
   */
  FlameLight (const Scene* const scene, uint index, const GLSLProgram* const program, const char* const IESFilename);
  
  /** Destructeur */
  virtual ~FlameLight();
  
  /** D�place la lumi�re � la position. Ceci d�place en r�alit� la lumi�re OpenGL.
   * @param pos Position de la lumi�re.
   */
  virtual void setLightPosition (const Point& pos)
  {
    m_lightPosition[0] = pos.x;
    m_lightPosition[1] = pos.y;
    m_lightPosition[2] = pos.z;
    m_centreSP = pos;
  }
  
  /** Dessine les shadow volumes � partir de la source. */
  void drawShadowVolume (GLfloat fatness[4], GLfloat extrudeDist[4]);  
  
  /** Active la source de lumi�re. */
  void switchOn ();  
  /** Eteint la source */
  void switchOff ();
  
  /** Active la source de lumi�re. */
  virtual void switchOnMulti (){};  
  /** Eteint la source */
  virtual void switchOffMulti (){};
  
  /** R�cup�ration du tableau des intensit�s du solide photom�trique.
   * @return Pointeur sur les intensit�s.
   */
  float *getIntensities(void) const { return m_iesFile->getIntensities(); };
  
  /** Retourne l'intensit� globale de la source.
   * @return Coefficient de l'intensit�.
   */
  const float getIntensity(void) const { return m_intensity; };
  
  /** R�cup�ration du centre du solide photom�trique.
   * @param x Coordonn�e x.
   * @param y Coordonn�e y.
   * @param z Coordonn�e z.
   */
  void getCenterSP(GLfloat& x, GLfloat& y, GLfloat& z) const { 
    x = (GLfloat)m_centreSP.x; y = (GLfloat)m_centreSP.y; z = (GLfloat)m_centreSP.z;
  };
  
  /** R�cup�ration du centre.
   */
  Point getCenterSP(void) const { return m_centreSP; };
  
  const float getLazimut() const {return m_iesFile->getLazimut();};
  
  const float getLzenith() const {return m_iesFile->getLzenith();};
  
  const float getLazimutTEX() const {return m_iesFile->getLazimutTEX();};
  
  const float getLzenithTEX() const {return m_iesFile->getLzenithTEX();};
  
  /** Retourne le nombre de valeurs en z�nithal. */
  const uint getIESZenithSize() const {return m_iesFile->getNbzenith();};
  
  /** Retourne le nombre de valeurs en azimuthal. */
  const uint getIESAzimuthSize() const {return m_iesFile->getNbazimut();};
  
  /** Supprime le fichier IES courant et en utilise un autre. */
  void useNewIESFile(const char* const IESFilename) { delete m_iesFile; m_iesFile = new IES(IESFilename); };
  
  /** Calcul de l'intensit� du centre et de l'orientation du solide photom�trique. */
  virtual void computeIntensityPositionAndDirection() = 0;
  
  /** Modifie le coefficient pond�rateur de l'intensit�. */
  virtual void setIntensityCoef(float coef) { m_intensityCoef = coef; };
  
protected:
  /** Centre du solide photom�trique dans l'espace. */
  Point m_centreSP;
  /** Orientation du solide photom�trique, utilis�e pour la rotation. */
  float m_orientationSPtheta;
  /** Axe de rotation. */
  Vector m_axeRotation;
  /** Valeur de l'intensit� du solide. */
  float m_intensity;
  /** Coefficient pond�rateur de l'intensit� de la source. */
  float m_intensityCoef;
  /** Indice de la lumi�re pour OpenGL. */
  short m_light;
  /** Position de la lumi�re ponctuelle OpenGL dans l'espace. */
  GLfloat m_lightPosition[4];
private:
  
  /** Pointeur sur la sc�ne. */
  const Scene *m_scene;
  
  /** Pointeur sur le program g�n�rateur de volumes d'ombres. */
  const GLSLProgram *m_SVProgram;
  
  
  /** Fichier IES utilis� pour le solide photom�trique de la source. */
  IES *m_iesFile;
};


/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE FIRESOURCE ****************************************/
/**********************************************************************************************************************/

/** La classe FireSource d�signe un objet qui produit du feu. Il se compose d'un luminaire (classe Object), de
 * flammes (classe RealFlame) et d'une source de lumi�re (classe FlameLight). Cette classe est abstraite.<br>
 * Pour obtenir la position absolue dans le rep�re du monde, il faut passer par la m�thode
 * getPosition() qui additionne la position relative de la flamme � la position absolue du solveur dans le monde.
 * Toute la construction de la flamme est faite dans le rep�re (0,0,0). Lors du dessin de la flamme dans la m�thode
 * drawFlame(), une translation est effectu�e en r�cup�rant la position avec getPosition() pour placer la flamme
 * dans le rep�re du monde.
 *
 * @author	Flavien Bridault
 */
class FireSource : public FlameLight
{
public:
  /** Constructeur d'une source de flammes. La position de la source est donn�e dans le rep�re du solveur.
   * @param flameConfig Configuration de la flamme.
   * @param s Pointeur sur le solveur de fluides.
   * @param nbFlames Nombre de flammes, si = 0 alors le tableau contenant les flammes n'est pas allou�.
   * et ceci doit alors �tre r�alis� par la classe fille.
   * @param scene Pointeur sur la sc�ne.
   * @param texname Nom du fichier contenant le luminaire.
   * @param index Indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL).
   * @param program Pointeur sur le program charg� de la construction des shadow volumes.
   */
  FireSource (const FlameConfig& flameConfig, Field3D* const s, uint nbFlames, Scene* const scene,
	      const wxString &texname, uint index, const GLSLProgram* const program);
  /** Destructeur */
  virtual ~FireSource ();

  /** Ajuste le niveau de d�tail de la NURBS.
   * @param value Valeur comprise entre 0 et LOD_VALUES. 
   */
  virtual void setLOD(u_char value)
  { 
    switch(value)
      {
      case 5:
	for (uint i = 0; i < m_nbFlames; i++)
	  {
	    m_flames[i]->setSamplingTolerance(4);
	    m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
	    m_flames[i]->setFlatFlame(false);
	  }
	break;
      case 4:
	for (uint i = 0; i < m_nbFlames; i++)
	  {
	    m_flames[i]->setSamplingTolerance(3);
	    m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
	    m_flames[i]->setFlatFlame(false);
	  }
	break;
      case 3:
	for (uint i = 0; i < m_nbFlames; i++)
	  {
	    m_flames[i]->setSamplingTolerance(2);
	    m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
	    m_flames[i]->setFlatFlame(false);
	  }
	break;
      case 2:
	for (uint i = 0; i < m_nbFlames; i++)
	  {
	    m_flames[i]->setSamplingTolerance(2);
	    m_flames[i]->setSkeletonsLOD(HALF_SKELETON);
	    m_flames[i]->setFlatFlame(false);
	  }
	break;
      case 1:
	for (uint i = 0; i < m_nbFlames; i++)
	  {
	    m_flames[i]->setSamplingTolerance(1);
	    m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
	    m_flames[i]->setFlatFlame(true);
	  }
	break;
      case 0:
	for (uint i = 0; i < m_nbFlames; i++)
	  {
	    m_flames[i]->setSamplingTolerance(1);
	    m_flames[i]->setSkeletonsLOD(HALF_SKELETON);
	    m_flames[i]->setFlatFlame(true);
	  }
	break;
      default:
	cerr << "Bad NURBS LOD parameter" << endl;
      }
  };
  
  /** Retourne la position absolue dans le rep�re du monde.
   * @return Position absolue dans le rep�re du monde.
   */
  Point getPosition () const
  {
    return Point(m_solver->getPosition());
  }
  
  /** Fonction charg�e de construire les flammes composant la source de feu. Elle se charge �galement 
   * de d�terminer la position de la source de lumi�re.
   */
  virtual void build();
  
  /** Dessine la m�che de la flamme. Les m�ches des RealFlame sont d�finies en (0,0,0), une translation
   * est donc effectu�e pour tenir compte du placement du feu dans le monde.
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
  
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture. La flamme d'une RealFlame est d�finie dans le rep�re du solveur,
   * donc seule une translation correspondant � la position du solveur est effectu�e.
   *
   * @param display affiche ou non la flamme
   * @param displayParticle affiche ou non les particules des squelettes
   * @parma boundingVolume Optionel, volume englobant � afficher: 0 aucun, 1 sph�re, 2 bo�te
   */
  virtual void drawFlame(bool display, bool displayParticle, u_char boundingVolume=0) const
  {
#ifdef COUNT_NURBS_POLYGONS
    g_count=0;
#endif
    switch(boundingVolume){
    case BOUNDING_SPHERE : drawBoundingSphere(); break;
    case IMPOSTOR : drawImpostor(); break;
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
	  // Trace une sph�re indiquant le centre
	  // 	    glPushMatrix();
	  // 	    glTranslatef (m_centreSP.x, m_centreSP.y, m_centreSP.z);
	  // 	    GraphicsFn::SolidSphere (.05, 10, 10);
	  // 	    glPopMatrix();
	}
      break;
    }
#ifdef COUNT_NURBS_POLYGONS
    cerr << g_count << endl;
#endif
  }
  
  virtual void drawBoundingSphere() const { if(m_visibility) m_boundingSphere.draw(); };
  
  virtual void drawImpostor() const;
  
  /** Fonction appel�e par le solveur de fluides pour ajouter l'�l�vation thermique de la flamme.
   */
  virtual void addForces ()
  {
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->addForces(m_fdf, m_innerForce, m_perturbate);
  }
  
  /** Affectation du coefficient multiplicateur de la FDF.
   * @param value Coefficient.
   */
  virtual void setInnerForce(float value) { m_innerForce=value; };
  
  /** Affectation de la FDF.
   * @param value FDF.
   */
  virtual void setFDF(int value) { m_fdf=value; };
  
  /** Affectation de la m�thode de perturbation.
   * @param value Perturbation.
   */
  virtual void setPerturbateMode(char value) { m_perturbate=value; };
  
  /** Modifie le type de flickering sur les flammes. */
  char getPerturbateMode() const { return m_perturbate; };
  
  /** Affectation de la dur�e de vie des squelettes guides.
   * @param value Dur�e de vie en it�rations.
   */
  virtual void setLeadLifeSpan(uint value) {
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->setLeadLifeSpan(value);
  };
  
  /** Affectation de la dur�e de vie des squelettes p�riph�riques.
   * @param value Dur�e de vie en it�rations.
   */
  virtual void setPeriLifeSpan(uint value) { 
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->setPeriLifeSpan(value); 
  };
  
  /** Active ou d�sactive l'affichage textur� sur la flamme. */
  virtual void setSmoothShading (bool state)
  {
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->setSmoothShading(state);
  }
  
  /** Fonction permettant de r�cup�rer le centre de la flamme dans le rep�re local */
  virtual Point getCenter() const { return m_center; };
  
  /** Fonction permettant de r�cup�rer l'orientation principale de la flamme
   */
  virtual Vector getMainDirection() const { return m_direction; };
  
  void computeIntensityPositionAndDirection(void);
  
  /** Construction de la sph�re englobante de l'objet. */
  void buildBoundingSphere ();
  
  /** Calcul de la visibilit� de la source. La m�thode cr�e d'abord une sph�re englobante 
   * et teste ensuite la visibilit� de celle-ci. */
  virtual void computeVisibility(const Camera &view, bool forceSpheresBuild=false);
  
  bool isVisible() { return m_visibility; };
  float getDistance() { return m_dist; };

  virtual bool operator<(const FireSource& other) const;

  /** Op�rateur de comparaison des flammes bas�es sur leur distance par rapport
   * � la cam�ra. On est oblig� de d�finir cet op�rateur car on utilise des
   * vecteurs de pointeurs pour stocker les flammes. */
  static bool cmp( const FireSource* a, const FireSource* b ) {
    return a->m_dist < b->m_dist;
  }
protected:
  /** Nombre de flammes */
  uint m_nbFlames;
  /** Tableau contenant les flammes */
  RealFlame **m_flames;
  
  /** Pointeur sur le solveur de fluides */
  Field3D *m_solver;
  
  /** Texture utilis�e pour les flammes */
  Texture m_texture;
  
  /** Sph�re englobante utilis�e pour v�rifier la visibilit� de la source. */
  BoundingSphere m_boundingSphere;
  /** Visibilit� de la flamme. */
  bool m_visibility;
  /** Distance par rapport � la cam�ra */
  float m_dist;

  /** Derni�re valeur du flickering connue avant que la gestion du LOD ne la modifie */
  char m_flickSave;
  
  /** Coefficient de force appliqu� sur la FDF. */
  float m_innerForce;
  /** M�thode de perturbation appliqu�e sur la FDF. */
  char m_perturbate;
  /** Fonction de distribution de carburant. */
  int m_fdf;
  
  /** Centre de la flamme, recalcul� � chaque it�ration dans build() */
  Point m_center;
  /** Direction principale de la flamme, recalcul�e � chaque it�ration dans build() */
  Vector m_direction;
  
  /** Sauvegardes du niveau de d�tail pr�c�dent, permet de d�terminer s'il y a un changement. */
  int m_fluidsLODSave, m_nurbsLODSave;
};

/** La classe Firesource ajoute la notion de flammes d�tach�es.
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
   * @param scene Pointeur sur la sc�ne.
   * @param texname Nom du fichier image de la texture.
   * @param index Indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL).
   * @param program Pointeur sur le program charg� de la construction des shadow volumes.
   * @param objName Nom du luminaire � charger dans le fichier filename.
   */
  DetachableFireSource (const FlameConfig& flameConfig, Field3D* const s, uint nbFlames, Scene* const scene,
			const wxString &texname, uint index, const GLSLProgram* const program);
  virtual ~DetachableFireSource();
  
  virtual void build();
  virtual void drawFlame(bool display, bool displayParticle, u_char boundingVolume=0) const;
   
  virtual void setLOD(u_char value)
  { 
    u_char tolerance=4;
    switch(value)
      {
      case 5:
	for (uint i = 0; i < m_nbFlames; i++)
	  {
	    m_flames[i]->setSamplingTolerance(4);
	    m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
	    m_flames[i]->setFlatFlame(false);
	    tolerance = 4;
	  }
	break;
      case 4:
	for (uint i = 0; i < m_nbFlames; i++)
	  {
	    m_flames[i]->setSamplingTolerance(3);
	    m_flames[i]->setSkeletonsLOD(FULL_SKELETON);
	    m_flames[i]->setFlatFlame(false);
	    tolerance = 3;
	  }
	break;
      case 3:
	for (uint i = 0; i < m_nbFlames; i++)
	  {
	    m_flames[i]->setSamplingTolerance(2);
	    m_flames[i]->setSkeletonsLOD(HALF_SKELETON);
	    m_flames[i]->setFlatFlame(false);
	  }
	tolerance = 2;
	break;
      case 2:
	for (uint i = 0; i < m_nbFlames; i++)
	  {
	    m_flames[i]->setSamplingTolerance(1);
	    m_flames[i]->setSkeletonsLOD(HALF_SKELETON);
	    m_flames[i]->setFlatFlame(false);
	  }
	tolerance = 1;
	break;
      case 1:
      case 0:
	for (uint i = 0; i < m_nbFlames; i++)
	  {
	    m_flames[i]->setSamplingTolerance(1);
	    m_flames[i]->setSkeletonsLOD(HALF_SKELETON);
	    m_flames[i]->setFlatFlame(true);
	  }
	tolerance = 1;
	break;
      default:
	cerr << "Bad NURBS LOD parameter" << endl;
      }
    for (list < DetachedFlame* >::const_iterator flamesIterator = m_detachedFlamesList.begin ();
	 flamesIterator != m_detachedFlamesList.end();  flamesIterator++)
      (*flamesIterator)->setSamplingTolerance(tolerance);
  };
//   virtual void drawImpostor () const;

  /** Ajoute une flamme d�tach�e � la source.
   * @param detachedFlame Pointeur sur la nouvelle flamme d�tach�e � ajouter.
   */
  virtual void addDetachedFlame(DetachedFlame* detachedFlame)
  {
    m_detachedFlamesList.push_back(detachedFlame);
  }
  
  /** Supprime une flamme d�tach�e � la source.
   * @param detachedFlame Pointeur sur la flamme d�tach�e � enlever.
   */
  virtual void removeDetachedFlame(DetachedFlame* detachedFlame)
  {
    m_detachedFlamesList.remove(detachedFlame);
  }
  
  /** Active ou d�sactive l'affichage textur� sur la flamme. */
  virtual void setSmoothShading (bool state);
  
  virtual void computeVisibility(const Camera &view, bool forceSpheresBuild=false);
  
  void computeIntensityPositionAndDirection(void);
private:
  /** Construit la bounding box utilis�e pour l'affichage */
  virtual void buildBoundingBox ();
  
  /** Liste des flammes d�tach�es */
  list<DetachedFlame *> m_detachedFlamesList;
  Point m_BBmin, m_BBmax;
};

#endif

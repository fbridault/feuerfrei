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
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes.
   * @param IESFilename nom du fichier IES � utiliser
   */
  FlameLight (Scene *scene, uint index, CgSVShader* shader, const char* const IESFilename);
  
  /** Destructeur */
  virtual ~FlameLight();
  
  /** D�place la lumi�re � la position. Ceci d�place en r�alit� la lumi�re OpenGL.
   * @param pos Position de la lumi�re.
   */
  void setLightPosition (Point& pos)
  {
    m_lightPosition[0] = pos.x;
    m_lightPosition[1] = pos.y;
    m_lightPosition[2] = pos.z;

    m_centreSP = pos;
  }
  
  /** Dessine les shadow volumes � partir de la source. */
  void drawShadowVolume ();  
  
  /** Active la source de lumi�re. */
  void switchOn ();
  
  /** Eteint la source */
  void switchOff ();
  
  /** R�cup�ration du tableau des intensit�s du solide photom�trique.
   * @return Pointeur sur les intensit�s.
   */
  float *getIntensities(void) const { return m_iesFile->getIntensities(); };
  
  /** Retourne l'intensit� globale de la source.
   * @return Coefficient de l'intensit�.
   */
  const double getIntensity(void) const { return m_intensity; };
  
  /** R�cup�ration du centre du solide photom�trique.
   * @param x Coordonn�e x.
   * @param y Coordonn�e y.
   * @param z Coordonn�e z.
   */
  const void getCenterSP(double& x, double& y, double& z) const { x = m_centreSP.x; y = m_centreSP.y; z = m_centreSP.z; };
  
  const double getLazimut() const {return m_iesFile->getLazimut();};
  
  const double getLzenith() const {return m_iesFile->getLzenith();};
  
  const double getLazimutTEX() const {return m_iesFile->getLazimutTEX();};
  
  const double getLzenithTEX() const {return m_iesFile->getLzenithTEX();};
  
  /** Retourne le nombre de valeurs en z�nithal. */
  const uint getIESZenithSize() const {return m_iesFile->getNbzenith();};
  
  /** Retourne le nombre de valeurs en azimuthal. */
  const uint getIESAzimuthSize() const {return m_iesFile->getNbazimut();};
  
  /** Supprime le fichier IES courant et en utilise un autre. */
  void useNewIESFile(const char* const IESFilename) { delete m_iesFile; m_iesFile = new IES(IESFilename); };

  /** Calcul de l'intensit� du centre et de l'orientation du solide photom�trique. */
  virtual void computeIntensityPositionAndDirection() = 0;
  
  /** Modifie le coefficient pond�rateur de l'intensit�. */
  virtual void setIntensityCoef(double coef) { m_intensityCoef = coef; };
  
protected:
  /** Centre du solide photom�trique dans l'espace. */
  Point m_centreSP;
  /** Orientation du solide photom�trique, utilis�e pour la rotation. */
  double m_orientationSPtheta;
  /** Axe de rotation. */
  Vector m_axeRotation;
  /** Valeur de l'intensit� du solide. */
  double m_intensity;
  /** Coefficient pond�rateur de l'intensit� de la source. */
  double m_intensityCoef;
private:
  /** Indice de la lumi�re pour OpenGL. */
  short m_light;
  
  /** Pointeur sur la sc�ne. */
  Scene *m_scene;

  /** Pointeur sur le shader g�n�rateur de volumes d'ombres. */
  CgSVShader *m_cgShader;
  
  /** Position de la lumi�re ponctuelle OpenGL dans l'espace. */
  GLfloat m_lightPosition[4];
  
  /** Fichier IES utilis� pour le solide photom�trique de la source. */
  IES *m_iesFile;
};


/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE FIRESOURCE ****************************************/
/**********************************************************************************************************************/

/** La classe FireSource d�signe un objet qui produit du feu. Il se compose d'un luminaire (classe Object), de
 * flammes (classe RealFlame) et d'une source de lumi�re (classe FlameLight). Cette classe est abstraite.<br>
 * La position de la flamme stock�e dans m_position est une position relative par rapport au solveur � laquelle
 * elle appartient. Pour obtenir la position absolue dans le rep�re du monde, il faut passer par la m�thode
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
   * @param filename Nom du fichier contenant le luminaire.
   * @param texname Nom du fichier contenant le luminaire.
   * @param index Indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL).
   * @param shader Pointeur sur le shader charg� de la construction des shadow volumes.
   * @param objName Nom du luminaire � charger dans le fichier filename.
   */
  FireSource (FlameConfig* flameConfig, Solver * s, uint nbFlames, Scene *scene, const char *filename,
	      const wxString &texname, uint index, CgSVShader * shader, const char *objName=NULL);
  /** Destructeur */
  virtual ~FireSource ();

  /** Ajuste la valeur d'�chantillonnage de la NURBS.
   * @param value Valeur de sampling, g�n�ralement compris dans un intervalle [1;1000]. 
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

  /** Retourne la position absolue dans le rep�re du monde.
   * @return Position absolue dans le rep�re du monde.
   */
  Point getPosition ()
  {
    return Point(m_position+m_solver->getPosition());
  }
  
  /** Fonction charg�e de construire les flammes composant la source de feu. Elle se charge �galement 
   * de d�terminer la position de la source de lumi�re.
   */
  virtual void build();
  
  /** Dessine la m�che de la flamme. Les m�ches des RealFlame sont d�finies en (0,0,0), une translation
   * est donc effectu�e pour tenir compte du placement du feu dans le monde.
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
  
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture. La flamme d'une RealFlame est d�finie dans le rep�re du solveur,
   * donc seule une translation correspondant � la position du solveur est effectu�e.
   *
   * @param displayParticle affiche ou non les particules des squelettes
   */
  virtual void drawFlame(bool display, bool displayParticle)
  {
    Point pt(m_solver->getPosition());
    glPushMatrix();
    glTranslatef (pt.x, pt.y, pt.z);
    glScalef (m_solver->getDimX(), m_solver->getDimY(), m_solver->getDimZ());
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->drawFlame(display, displayParticle);  
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
  
  /** Dessine la flamme et sa m�che.
   * @param displayParticle affiche ou non les particules des squelettes.
   * @param displayBoxes Affiche ou non le partitionnement de la m�che.
   */
  void draw(bool display, bool displayParticle, bool displayBoxes)
  {
    glScalef (m_solver->getDimX(), m_solver->getDimY(), m_solver->getDimZ());
    drawWick(displayBoxes);
    drawFlame(display, displayParticle);
  }
  
  /** Dessine le luminaire de la flamme. Les luminaires sont d�finis en (0,0,0), une translation
   * est donc effectu�e pour tenir compte du placement du feu dans le monde.
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
    
  /** Dessine le luminaire de la flamme. Les luminaires sont d�finis en (0,0,0), une translation
   * est donc effectu�e pour tenir compte du placement du feu dans le monde.
   *
   * @param shader vertex program utilis� pour le solide photom�trique 
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
  
  /** Fonction appel�e par le solveur de fluides pour ajouter l'�l�vation thermique de la flamme.
   * @param perturbate Type de perturbation parmi FLICKERING_VERTICAL, FLICKERING_RANDOM, etc...
   * @param fdf Type de fonction de distribution de carburant parmi FDF_LINEAR, FDF_BILINEAR, 
   * FDF_EXPONENTIAL, FDF_GAUSS, FDF_RANDOM.
   */
  virtual void addForces (char perturbate, char fdf=0)
  {
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->addForces(perturbate,fdf);
  }
  
  /** Affectation de la v�locit� induite par la flamme.
   * @param value V�locit� de la flamme.
   */
  virtual void setForces(double value)
  {
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->setForces(value);
  }
  
  /** Active ou d�sactive l'affichage textur� sur la flamme. */
  virtual void toggleSmoothShading ()
  {
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->toggleSmoothShading();
  }
  
  /** Fonction permettant de r�cup�rer l'orientation principale de la flamme
   * pour orienter le solide photom�trique.
   */
  virtual Vector getMainDirection();
  
  virtual void locateInSolver(){
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->locateInSolver();
  }
  
  /** Calcul de l'intensit� du centre et de l'orientation du solide photom�trique */
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

  /** Est-ce que la source poss�de un luminaire */
  bool m_hasLuminary;

  /** Indique si la source produit des flammes d�tach�es */
  bool m_breakable;

  /** Texture utilis�e pour les flammes */
  Texture m_texture;
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
   * @param shader Pointeur sur le shader charg� de la construction des shadow volumes.
   * @param objName Nom du luminaire � charger dans le fichier filename.
   */
  DetachableFireSource (FlameConfig* flameConfig, Solver * s, uint nbFlames, Scene *scene, const char *filename,
			const wxString &texname, uint index, CgSVShader * shader, const char *objName=NULL);
  virtual ~DetachableFireSource();
  
  virtual void build();
  virtual void drawFlame(bool display, bool displayParticle);
  
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
  
private:
  /** Liste des flammes d�tach�es */
  list<DetachedFlame *> m_detachedFlamesList;
};

#endif
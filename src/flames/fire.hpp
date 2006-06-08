#if !defined(FIRE_H)
#define FIRE_H

class FlameLight;
class FireSource;

#include "flames.hpp"

#include "basicFlames.hpp"
#include "../solvers/solver.hpp"
#include "../scene/texture.hpp"
#include "../scene/object.hpp"
#include "../shaders/CgSVShader.hpp"

class BasicFlame;
class Solver;
class Object;
class Scene;


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
  /** Constructeur de flamme simple.
   * @param scene pointeur sur la sc�ne
   * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL)
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes
   */
  FlameLight (Scene *scene, uint index, CgSVShader* shader);
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
  
private:
  short m_light;
  
  Scene *m_scene;
  CgSVShader *m_cgShader;
  
  GLfloat m_lightPosition[4];  
};


/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE FIRESOURCE ****************************************/
/**********************************************************************************************************************/

/** La classe FireSource d�signe un objet qui produit du feu. Il se compose d'un luminaire (classe Object), de
 * flammes (classe BasicFlame) et d'une source de lumi�re (classe FlameLight). Cette classe est abstraite.<br>
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
   * @param s pointeur sur le solveur de fluides
   * @param nbFlames nombre de flammes, si = 0 alors le tableau contenant les flammes n'est pas allou�
   * et ceci doit alors �tre r�alis� par la classe fille
   * @param scene pointeur sur la sc�ne
   * @param innerForce force int�rieure de la flamme
   * @param filename nom du fichier contenant le luminaire
   * @param index indice de la flamme dans la sc�ne (pour attribution d'une lumi�re OpenGL)
   * @param shader pointeur sur le shader charg� de la construction des shadow volumes
   * @param objname nom du luminaire � charger dans le fichier filename
   */
  FireSource (FlameConfig* flameConfig, Solver * s, uint nbFlames, Scene *scene, const char *filename, uint index, 
	      CgSVShader * shader, const char *objName=NULL);
  virtual ~FireSource ();

  virtual void setSamplingTolerance(double value){ 
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->setSamplingTolerance(value);
  };
  /** Retourne la position absolue dans le rep�re du monde .
   * @return position absolue dans le rep�re du monde
   */
  Point getPosition ()
  {
    return Point(m_position+m_solver->getPosition());
  }
  
  /** Fonction charg�e de construire les flammes composant la source de feu. Elle se charge �galement 
   * de d�terminer la position de la source de lumi�re.
   */
  virtual void build();
  
  /** Dessine la m�che de la flamme. Les m�ches des BasicFlame sont d�finies en (0,0,0), une translation
   * est donc effectu�e pour tenir compte du placement du feu dans le monde.
   */
  virtual void drawWick(bool displayBoxes)
  {      
    Point pt(getPosition());
    glPushMatrix();
    glTranslatef (pt.x, pt.y, pt.z);
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->drawWick(displayBoxes);
    glPopMatrix();
  }
  
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture. La flamme d'une BasicFlame est d�finie dans le rep�re du solveur,
   * donc seule une translation correspondant � la position du solveur est effectu�e.
   *
   * @param displayParticle affiche ou non les particules des squelettes
   */
  virtual void drawFlame(bool displayParticle)
  {
    Point pt(m_solver->getPosition());
    glPushMatrix();
    glTranslatef (pt.x, pt.y, pt.z);
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->drawFlame(displayParticle);  
    glPopMatrix();
  }  
  
  /** Dessine la flamme et sa m�che
   *
   * @param displayParticle affiche ou non les particules des squelettes
   */
  void draw(bool displayParticle, bool displayBoxes)
  {
    drawWick(displayBoxes);
    drawFlame(displayParticle);
  }
  
  /** Dessine le luminaire de la flamme. Les luminaires sont d�finis en (0,0,0), une translation
   * est donc effectu�e pour tenir compte du placement du feu dans le monde.
   */
  void drawLuminary()
  {
    if(hasLuminary){
      Point position(getPosition());
      glPushMatrix();
      glTranslatef (position.x, position.y, position.z);
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
    if(hasLuminary){
      Point position(getPosition());
      glPushMatrix();
      glTranslatef (position.x, position.y, position.z);
      shader.setModelViewProjectionMatrix();
      glCallList(m_luminaryDL);
      glPopMatrix();
    }
  }
  
  /** Fonction appel�e par le solveur de fluides pour ajouter l'�l�vation thermique de la flamme.
   * Elle demande � chacune des flammes composant le feu d'ajouter ses forces.
   *
   * @param perturbate type de perturbation dans l'�l�vation thermique
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
  
  /** Fonction permettant de r�cup�rer l'orientation principale de la flamme
   * pour orienter le solide photom�trique.
   */
  virtual Vector getMainDirection();
  
  virtual void locateInSolver(){
    for (uint i = 0; i < m_nbFlames; i++)
      m_flames[i]->locateInSolver();
  }
  
protected:
  /** Nombre de flammes */
  uint m_nbFlames;
  /** Tableau contenant les flammes */
  BasicFlame **m_flames;
  
  /** Luminaire */
  Object *m_luminary;
  /** Index de la display list contenant le luminaire */
  GLuint m_luminaryDL;
  
  /** Pointeur sur le solveur de fluides */
  Solver *m_solver;

  /** Position relative de la source dans le solveur auquel elle appartient */
  Point m_position;

  /** Est-ce que la source poss�de un luminaire */
  bool hasLuminary;
};

#endif

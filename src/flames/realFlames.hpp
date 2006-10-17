#ifndef REALFLAMES_HPP
#define REALFLAMES_HPP

class LineFlame;
class PointFlame;
class DetachedFlame;
class DetachableFireSource;

#include "abstractFlames.hpp"

#include <list>

#include "wick.hpp"

/**********************************************************************************************************************/
/**************************************** DEFINITION DE LA CLASSE LINEFLAME **************************************/
/**********************************************************************************************************************/

/** La classe LineFlame implémente une flamme qui provient d'une mèche "linéaire".<br>
 * L'objet Wick appartient à la classe LineFlame, il est donc précisé dans le constructeur
 * de Wick que l'objet doit être importé dans la scène dans l'état "detached", de sorte que 
 * le constructeur de la scène ne cherche pas à le référencer ni à le détruire.
 *
 * @author	Flavien Bridault
 */
class LineFlame : public RealFlame
{
public:
  /** Constructeur.
   * @param flameConfig Pointeur vers la configuration de la flamme.
   * @param scene Pointeur vers la scène.
   * @param tex Pointeur vers la texture à utiliser.
   * @param s Pointeur vers le solveur.
   * @param wickFileName Chaîne de caractère contenant le nom du fichier contenant la mèche.
   * @param parentFire Pointeur sur le feu auquel appartient la flamme.
   * @param detachedFlamesWidth Largeur des flammes détachées.
   * @param wickName Chaîne de caractère contenant le nom de la mèche dans le fichier OBJ.
   */
  LineFlame (FlameConfig* flameConfig, Scene *scene, Texture* const tex, Solver3D *s,
	     const char *wickFileName, DetachableFireSource *parentFire, double detachedFlamesWidth, 
	     const char *wickName=NULL);
  virtual ~LineFlame();
  
  virtual void drawFlame(bool display, bool displayParticle) { 
    /* Affichage des particules */
    if(displayParticle) drawParticles();
    if(display) drawLineFlame(); 
  };
  
  virtual void drawWick(bool displayBoxes);
  
  Vector getMainDirection();
  virtual Point getCenter ();
  
  void addForces(u_char perturbate, u_char fdf);
  
  Point* getTop();  
  Point* getBottom();
  
  void breakCheck();
  
  /** Méthode permettant de générer des étincelles dans le feu.
   * @todo Cette méthode n'est pas encore terminée.
   */
  virtual void generateAndDrawSparks();
private:
  /** Mèche de la flamme */
  Wick m_wick;

  /** Liste des particules utilisées pour afficher des étincelles */
  list<Particle *> m_sparksList;

  /** Pointeur sur le feu auquel appartient la flamme */
  DetachableFireSource *m_parentFire;
  
  /** Largeur des flammes détachées */
  double m_detachedFlamesWidth;
};

/**********************************************************************************************************************/
/*************************************** DEFINITION DE LA CLASSE POINTFLAME **************************************/
/**********************************************************************************************************************/

/** La classe PointFlame implémente une flamme qui provient d'une mèche verticale droite.<br>
 * Elle génère ses squelettes à partir du maillage de la mèche dont le nom est fourni au
 * constructeur.
 *
 * @author	Flavien Bridault
 */
class PointFlame : public RealFlame
{
public:
  /** Constructeur.
   * @param flameConfig Pointeur sur le configuration de la flamme.
   * @param tex Pointeur vers la texture à utiliser.
   * @param s Pointeur vers le solveur.
   * @param rayon Valeur du rayon du cercle formé par les racines des squelettes.
   */
  PointFlame ( FlameConfig* flameConfig, Texture* const tex, Solver3D * s, double rayon);
  
  /** Destructeur*/
  virtual ~PointFlame();
  
  virtual void drawFlame(bool display, bool displayParticle) { 
    /* Affichage des particules */
    if(displayParticle) drawParticles();
    if(display) drawPointFlame(); 
  };
  
  virtual void drawWick(bool displayBoxes);
  
  Vector getMainDirection(){
    return(*(m_leadSkeletons[0]->getMiddleParticle()));
  };
  virtual Point getCenter (){    
    return (*m_leadSkeletons[0]->getMiddleParticle () + m_position);
  }; 
  
  void addForces(u_char perturbate, u_char fdf=0);
  
  Point* getTop(){ return m_leadSkeletons[0]->getParticle(0); };
  Point* getBottom() { return m_leadSkeletons[0]->getRoot(); };
  
  /** Fonction testant si les squelettes doivent se briser. Si c'est le cas, elle effectue la division.
   * Dans le cas d'une PointFlame, cette méthode ne fait rien du tout pour l'instant.
   */
  void breakCheck() {};
};


/**********************************************************************************************************************/
/********************************************* DEFINITION DE LA CLASSE DETACHEDFLAME **********************************/
/**********************************************************************************************************************/

/** La classe DetachedFlame défini une flamme détachée, autrement dit une flamme éphémère qui n'est pas attachée à un
 * quelconque support. Pour l'instant seule une classe héritée de RealFlame peut produire une flamme détachée.
 *
 * @author	Flavien Bridault
 */
class DetachedFlame : public NurbsFlame
{
public:
  /** Constructeur de flamme détachée. Il est appelée lors de la division d'une flamme dans la méthode breackCheck().
   * @param source Flamme qui a générée la flamme détachée.
   * @param nbLeadSkeletons Nombre de squelettes guides.
   * @param leadSkeletons Tableaux contenant les squelettes guides.
   * @param nbSkeletons Nombre de squelettes périphériques.
   * @param periSkeletons Tableaux contenant les squelettes périphériques.
   * @param tex Pointeur vers la texture à utiliser.
   * @param solver Pointeur vers le solveur.
   */
  DetachedFlame(RealFlame *source, uint nbLeadSkeletons, FreeLeadSkeleton **leadSkeletons, 
		 uint nbSkeletons, FreePeriSkeleton **periSkeletons, Texture* const tex, Solver3D *solver);
  
  /** Destructeur*/
  virtual ~DetachedFlame ();
  
  virtual bool build();
  
  virtual void drawFlame(bool display, bool displayParticle) { 
    /* Affichage des particules */
    if(displayParticle) drawParticles();
    if(display) drawLineFlame(); 
  };
  
  void drawParticles()
  {
      uint i;
      /* Déplacement et détermination du maximum */
      for (i = 0; i < m_nbSkeletons; i++)
	m_periSkeletons[i]->draw();
      for (i = 0; i < m_nbLeadSkeletons; i++)
	m_leadSkeletons[i]->draw();
  };
  
  /** Méthode permettant d'informer à la flamme de se localiser dans le solveur
   * Elle doit être appelée dès qu'un changement de résolution de la grille intervient.
   */  
  virtual void locateInSolver3D(){ m_solver->findPointPosition(m_position, m_x, m_y, m_z); };
  
protected:
  /** Pointeur vers les squelettes guide. */
  FreeLeadSkeleton **m_leadSkeletons;
  /** Nombres de squelettes guides. */
  uint m_nbLeadSkeletons;
  /** Tableau contenant les pointeurs vers les squelettes périphériques. */
  FreePeriSkeleton **m_periSkeletons;
  /** Pointeur sur le solveur de fluides. */
  Solver3D *m_solver;  
  
  /** Tableau temporaire contenant les distances entre les particules d'un squelette. */
  double *m_distances;
  
  /** Tableau temporaire utilisé pour classer les indices des distances entre points de contrôle
   * lors de l'ajout de points de contrôle supplémentaires dans la NURBS.  Alloué une seule fois 
   * en début de programme à la taille maximale pour des raisons évidentes d'optimisation du temps 
   * d'exécution.
   */
  int *m_maxDistancesIndexes;
};

#endif

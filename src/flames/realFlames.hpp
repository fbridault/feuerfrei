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
   * @param flameConfig Pointeur sur le configuration de la flamme
   * @param scene Pointeur vers la scène
   * @param textureName Nom de la texture à utiliser
   * @param s Pointeur vers le solveur
   * @param wickFileName Chaîne de caractère contenant le nom du fichier contenant la mèche
   * @param parentFire Pointeur sur le feu auquel appartient la flamme
   * @param wickName Chaîne de caractère contenant le nom de la mèche dans le fichier OBJ
   */
  LineFlame (FlameConfig* flameConfig, Scene *scene, Texture* const tex, Solver *s,
	     const char *wickFileName, DetachableFireSource *parentFire, const char *wickName=NULL);
  virtual ~LineFlame();
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
   * build() avec le placage de texture
   */
  virtual void drawFlame(bool displayParticle) { drawParticles(displayParticle); drawLineFlame(); };
  
  /** Dessine la mèche de la flamme 
   * @param displayBoxes affiche ou non le partitionnement de la mèche
   */
  virtual void drawWick(bool displayBoxes);
    
  /** Retourne la direction de la base de la flamme vers la derniere particule
   * pour orienter le solide photométrique.
   */
  Vector getMainDirection();
  
  virtual void generateAndDrawSparks();
  
  virtual Point getCenter ();
  
  /** Fonction appelée par le solveur de fluides pour ajouter l'élévation thermique de la flamme.
   */
  void addForces(u_char perturbate, u_char fdf);
  
  Point* getTop();
  Point* getBottom();
  
  /** Fonction testant si les squelettes doivent se briser. Si c'est le cas, elle effectue la division.
   * @param 
   */
  void breakCheck();
  
private:
  /** Mèche de la flamme */
  Wick m_wick;

  list<Particle *> m_sparksList;

  /** Pointeur sur le feu auquel appartient la flamme */
  DetachableFireSource *m_parentFire;
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
   * @param flameConfig Pointeur sur le configuration de la flamme
   * @param s Pointeur vers le solveur
   * @param rayon Valeur du rayon du cercle formé par les racines des squelettes
   */
  PointFlame ( FlameConfig* flameConfig, Texture* const tex, Solver * s, double rayon);
  virtual ~PointFlame();
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
   * build() avec le placage de texture
   */
  virtual void drawFlame(bool displayParticle){ drawParticles(displayParticle); drawPointFlame(); };
  
  /** Dessine la mèche de la flamme 
   * @param displayBoxes affiche ou non le partitionnement de la mèche. Ce paramètre
   * est ignoré pour ce type de flamme puisque la mèche n'est pas découpé. 
   */
  virtual void drawWick(bool displayBoxes);
  
  /** Retourne la direction de la base de la flamme vers la derniere particule
   * pour orienter le solide photométrique.
   */
  Vector getMainDirection(){
    return(*(m_leadSkeletons[0]->getMiddleParticle()));
  };
  
  virtual Point getCenter (){    
    return (*m_leadSkeletons[0]->getMiddleParticle () + m_position);
  }; 

  /** Fonction appelée par le solveur de fluides pour ajouter l'élévation thermique de la flamme.
   */
  void addForces(u_char perturbate, u_char fdf=0);
  
  Point* getTop(){ return m_leadSkeletons[0]->getParticle(0); };
  Point* getBottom() { return m_leadSkeletons[0]->getRoot(); };

  /** Fonction testant si les squelettes doivent se briser. Si c'est le cas, elle effectue la division.
   * @param 
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
  /** Constructeur de flamme. La position de la flamme est définie dans le repère du solveur.
   * @param source Flamme qui a générée la flamme détachée.
   * @param nbSkeletons Nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage.
   * @param 
   */
  DetachedFlame(RealFlame *source, uint nbLeadSkeletons, FreeLeadSkeleton **leadSkeletons, 
		 uint nbSkeletons, FreePeriSkeleton **periSkeletons, Texture* const tex, Solver *solver);
  virtual ~DetachedFlame ();
    
  /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
   * des vecteurs de noeuds.
   * @return false si la flamme est morte
   */
  virtual bool build();
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
   * build() avec le placage de texture
   */
  virtual void drawFlame(bool displayParticle) { drawParticles(displayParticle); drawLineFlame(); };
    
  void drawParticles(bool displayParticle)
  {
    /* Affichage des particules */
    if(displayParticle){
      uint i;
      /* Déplacement et détermination du maximum */
      for (i = 0; i < m_nbSkeletons; i++)
	m_periSkeletons[i]->draw();
      for (i = 0; i < m_nbLeadSkeletons; i++)
	m_leadSkeletons[i]->draw();
    }
  };
  
  /** Méthode permettant d'informer à la flamme de se localiser dans le solveur */
  /* Elle doit être appelée dès qu'un changement de résolution de la grille intervient */
  virtual void locateInSolver(){ m_solver->findPointPosition(m_position, m_x, m_y, m_z); };
  
protected:
  /** Pointeur vers les squelettes guide. */
  FreeLeadSkeleton **m_leadSkeletons;
  /** Nombres de squelettes guides */
  uint m_nbLeadSkeletons;
  /** Tableau contenant les pointeurs vers les squelettes périphériques. */
  FreePeriSkeleton **m_periSkeletons;
  /** Pointeur sur le solveur de fluides */
  Solver *m_solver;  
  
  double *m_distances;
  /** Tableau temporaire utilisé pour classer les indices des distances entre points de contrôle
   * lors de l'ajout de points de contrôle supplémentaires dans la NURBS.  Alloué une seule fois 
   * en début de programme à la taille maximale pour des raisons évidentes d'optimisation du temps 
   * d'exécution.
   */
  int *m_maxDistancesIndexes;
};

#endif

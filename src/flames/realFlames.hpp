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

/** La classe LineFlame impl�mente une flamme qui provient d'une m�che "lin�aire".<br>
 * L'objet Wick appartient � la classe LineFlame, il est donc pr�cis� dans le constructeur
 * de Wick que l'objet doit �tre import� dans la sc�ne dans l'�tat "detached", de sorte que 
 * le constructeur de la sc�ne ne cherche pas � le r�f�rencer ni � le d�truire.
 *
 * @author	Flavien Bridault
 */
class LineFlame : public RealFlame
{
public:
  /** Constructeur.
   * @param flameConfig Pointeur sur le configuration de la flamme
   * @param scene Pointeur vers la sc�ne
   * @param textureName Nom de la texture � utiliser
   * @param s Pointeur vers le solveur
   * @param wickFileName Cha�ne de caract�re contenant le nom du fichier contenant la m�che
   * @param parentFire Pointeur sur le feu auquel appartient la flamme
   * @param wickName Cha�ne de caract�re contenant le nom de la m�che dans le fichier OBJ
   */
  LineFlame (FlameConfig* flameConfig, Scene *scene, Texture* const tex, Solver *s,
	     const char *wickFileName, DetachableFireSource *parentFire, const char *wickName=NULL);
  virtual ~LineFlame();
  
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture
   */
  virtual void drawFlame(bool displayParticle) { drawParticles(displayParticle); drawLineFlame(); };
  
  /** Dessine la m�che de la flamme 
   * @param displayBoxes affiche ou non le partitionnement de la m�che
   */
  virtual void drawWick(bool displayBoxes);
    
  /** Retourne la direction de la base de la flamme vers la derniere particule
   * pour orienter le solide photom�trique.
   */
  Vector getMainDirection();
  
  virtual void generateAndDrawSparks();
  
  virtual Point getCenter ();
  
  /** Fonction appel�e par le solveur de fluides pour ajouter l'�l�vation thermique de la flamme.
   */
  void addForces(u_char perturbate, u_char fdf);
  
  Point* getTop();
  Point* getBottom();
  
  /** Fonction testant si les squelettes doivent se briser. Si c'est le cas, elle effectue la division.
   * @param 
   */
  void breakCheck();
  
private:
  /** M�che de la flamme */
  Wick m_wick;

  list<Particle *> m_sparksList;

  /** Pointeur sur le feu auquel appartient la flamme */
  DetachableFireSource *m_parentFire;
};

/**********************************************************************************************************************/
/*************************************** DEFINITION DE LA CLASSE POINTFLAME **************************************/
/**********************************************************************************************************************/

/** La classe PointFlame impl�mente une flamme qui provient d'une m�che verticale droite.<br>
 * Elle g�n�re ses squelettes � partir du maillage de la m�che dont le nom est fourni au
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
   * @param rayon Valeur du rayon du cercle form� par les racines des squelettes
   */
  PointFlame ( FlameConfig* flameConfig, Texture* const tex, Solver * s, double rayon);
  virtual ~PointFlame();
  
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture
   */
  virtual void drawFlame(bool displayParticle){ drawParticles(displayParticle); drawPointFlame(); };
  
  /** Dessine la m�che de la flamme 
   * @param displayBoxes affiche ou non le partitionnement de la m�che. Ce param�tre
   * est ignor� pour ce type de flamme puisque la m�che n'est pas d�coup�. 
   */
  virtual void drawWick(bool displayBoxes);
  
  /** Retourne la direction de la base de la flamme vers la derniere particule
   * pour orienter le solide photom�trique.
   */
  Vector getMainDirection(){
    return(*(m_leadSkeletons[0]->getMiddleParticle()));
  };
  
  virtual Point getCenter (){    
    return (*m_leadSkeletons[0]->getMiddleParticle () + m_position);
  }; 

  /** Fonction appel�e par le solveur de fluides pour ajouter l'�l�vation thermique de la flamme.
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

/** La classe DetachedFlame d�fini une flamme d�tach�e, autrement dit une flamme �ph�m�re qui n'est pas attach�e � un
 * quelconque support. Pour l'instant seule une classe h�rit�e de RealFlame peut produire une flamme d�tach�e.
 *
 * @author	Flavien Bridault
 */
class DetachedFlame : public NurbsFlame
{
public:
  /** Constructeur de flamme. La position de la flamme est d�finie dans le rep�re du solveur.
   * @param source Flamme qui a g�n�r�e la flamme d�tach�e.
   * @param nbSkeletons Nombre de squelettes. Pour le moment nbSkeletons doit �tre pair en raison de l'affichage.
   * @param 
   */
  DetachedFlame(RealFlame *source, uint nbLeadSkeletons, FreeLeadSkeleton **leadSkeletons, 
		 uint nbSkeletons, FreePeriSkeleton **periSkeletons, Texture* const tex, Solver *solver);
  virtual ~DetachedFlame ();
    
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle commence par d�placer les particules 
   * des squelettes p�riph�riques. Ensuite, elle d�finit la matrice de points de contr�le de la NURBS,
   * des vecteurs de noeuds.
   * @return false si la flamme est morte
   */
  virtual bool build();
  
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture
   */
  virtual void drawFlame(bool displayParticle) { drawParticles(displayParticle); drawLineFlame(); };
    
  void drawParticles(bool displayParticle)
  {
    /* Affichage des particules */
    if(displayParticle){
      uint i;
      /* D�placement et d�termination du maximum */
      for (i = 0; i < m_nbSkeletons; i++)
	m_periSkeletons[i]->draw();
      for (i = 0; i < m_nbLeadSkeletons; i++)
	m_leadSkeletons[i]->draw();
    }
  };
  
  /** M�thode permettant d'informer � la flamme de se localiser dans le solveur */
  /* Elle doit �tre appel�e d�s qu'un changement de r�solution de la grille intervient */
  virtual void locateInSolver(){ m_solver->findPointPosition(m_position, m_x, m_y, m_z); };
  
protected:
  /** Pointeur vers les squelettes guide. */
  FreeLeadSkeleton **m_leadSkeletons;
  /** Nombres de squelettes guides */
  uint m_nbLeadSkeletons;
  /** Tableau contenant les pointeurs vers les squelettes p�riph�riques. */
  FreePeriSkeleton **m_periSkeletons;
  /** Pointeur sur le solveur de fluides */
  Solver *m_solver;  
  
  double *m_distances;
  /** Tableau temporaire utilis� pour classer les indices des distances entre points de contr�le
   * lors de l'ajout de points de contr�le suppl�mentaires dans la NURBS.  Allou� une seule fois 
   * en d�but de programme � la taille maximale pour des raisons �videntes d'optimisation du temps 
   * d'ex�cution.
   */
  int *m_maxDistancesIndexes;
};

#endif

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
   * @param flameConfig Pointeur vers la configuration de la flamme.
   * @param scene Pointeur vers la sc�ne.
   * @param tex Pointeur vers la texture � utiliser.
   * @param s Pointeur vers le solveur.
   * @param wickFileName Cha�ne de caract�re contenant le nom du fichier contenant la m�che.
   * @param parentFire Pointeur sur le feu auquel appartient la flamme.
   * @param detachedFlamesWidth Largeur des flammes d�tach�es.
   * @param wickName Cha�ne de caract�re contenant le nom de la m�che dans le fichier OBJ.
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
  
  /** M�thode permettant de g�n�rer des �tincelles dans le feu.
   * @todo Cette m�thode n'est pas encore termin�e.
   */
  virtual void generateAndDrawSparks();
private:
  /** M�che de la flamme */
  Wick m_wick;

  /** Liste des particules utilis�es pour afficher des �tincelles */
  list<Particle *> m_sparksList;

  /** Pointeur sur le feu auquel appartient la flamme */
  DetachableFireSource *m_parentFire;
  
  /** Largeur des flammes d�tach�es */
  double m_detachedFlamesWidth;
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
   * @param flameConfig Pointeur sur le configuration de la flamme.
   * @param tex Pointeur vers la texture � utiliser.
   * @param s Pointeur vers le solveur.
   * @param rayon Valeur du rayon du cercle form� par les racines des squelettes.
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
   * Dans le cas d'une PointFlame, cette m�thode ne fait rien du tout pour l'instant.
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
  /** Constructeur de flamme d�tach�e. Il est appel�e lors de la division d'une flamme dans la m�thode breackCheck().
   * @param source Flamme qui a g�n�r�e la flamme d�tach�e.
   * @param nbLeadSkeletons Nombre de squelettes guides.
   * @param leadSkeletons Tableaux contenant les squelettes guides.
   * @param nbSkeletons Nombre de squelettes p�riph�riques.
   * @param periSkeletons Tableaux contenant les squelettes p�riph�riques.
   * @param tex Pointeur vers la texture � utiliser.
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
      /* D�placement et d�termination du maximum */
      for (i = 0; i < m_nbSkeletons; i++)
	m_periSkeletons[i]->draw();
      for (i = 0; i < m_nbLeadSkeletons; i++)
	m_leadSkeletons[i]->draw();
  };
  
  /** M�thode permettant d'informer � la flamme de se localiser dans le solveur
   * Elle doit �tre appel�e d�s qu'un changement de r�solution de la grille intervient.
   */  
  virtual void locateInSolver3D(){ m_solver->findPointPosition(m_position, m_x, m_y, m_z); };
  
protected:
  /** Pointeur vers les squelettes guide. */
  FreeLeadSkeleton **m_leadSkeletons;
  /** Nombres de squelettes guides. */
  uint m_nbLeadSkeletons;
  /** Tableau contenant les pointeurs vers les squelettes p�riph�riques. */
  FreePeriSkeleton **m_periSkeletons;
  /** Pointeur sur le solveur de fluides. */
  Solver3D *m_solver;  
  
  /** Tableau temporaire contenant les distances entre les particules d'un squelette. */
  double *m_distances;
  
  /** Tableau temporaire utilis� pour classer les indices des distances entre points de contr�le
   * lors de l'ajout de points de contr�le suppl�mentaires dans la NURBS.  Allou� une seule fois 
   * en d�but de programme � la taille maximale pour des raisons �videntes d'optimisation du temps 
   * d'ex�cution.
   */
  int *m_maxDistancesIndexes;
};

#endif

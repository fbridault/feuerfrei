#if !defined(BASICFLAMES_H)
#define BASICFLAMES_H

class BasicFlame;
class LineFlame;
class PointFlame;

#include "GL/glu.h"
#include "periSkeleton.hpp"
#include "leadSkeleton.hpp"
#include "../solvers/solver.hpp"
#include "../scene/texture.hpp"

#include "wick.hpp"

#ifndef CALLBACK
#define CALLBACK
#endif

class PeriSkeleton;
class LeadSkeleton;
class Solver;
class Scene;

/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE BASICFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe BasicFlame fournit un objet simple englobant des squelettes de flamme.
 * Cette classe est abstraite et est h�rit�e par les classes LineFlame et PointFlame.
 * Elle permet de d�finir une primitive g�om�trique pour une flamme, mais ne permet pas de construire une flamme
 * en tant que source de lumi�re d'une sc�ne. C'est la classe FireSource qui permet de d�finir ceci, en utilisant
 * les classes h�rit�es de BasicFlame comme �l�ment de base.<br>
 * Une BasicFlame est construite � sa position relative d�finie dans m_position. Lors du dessin, la translation dans
 * le rep�re du feu est donc d�j� effectu�e. C'est la classe FireSource qui s'occupe de placer correctement dans
 * le rep�re du monde. En revanche, la m�che en d�finie en (0,0,0) et il faut donc la translater dans le rep�re du
 * feu � chaque op�ration de dessin.
 *
 * @author	Flavien Bridault
 */
class BasicFlame
{
public:
  /** Constructeur de flamme. La position de la flamme est d�finie dans le rep�re du solveur.
   * @param s pointeur sur le solveur de fluides
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit �tre pair en raison de l'affichage.
   * @param posRel position du centre de la flamme
   * @param innerForce force int�rieure de la flamme
   * @param scene pointeur sur la sc�ne
   * @param texname Nom du fichier image � charger
   * @param wrap_s param�tre de r�p�tition de la texture dans la direction s {GL_WRAP,GL_REPEAT}
   * @param wrap_t param�tre de r�p�tition de la texture dans la direction t {GL_WRAP,GL_REPEAT}
   */
  BasicFlame (Solver * s, int nbSkeletons, int nbFixedPoints, Point& posRel, double innerForce, Scene *scene, 
	      const wxString& texname, GLint wrap_s, GLint wrap_t);
  virtual ~BasicFlame ();
  
    /** Fonction appel�e par la fonction de dessin OpenGL. Elle commence par d�placer les particules 
   * des squelettes p�riph�riques. Ensuite, elle d�finit la matrice de points de contr�le de la NURBS,
   * des vecteurs de noeuds.
   */
  virtual void build() = 0;
  
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture. 
   */
  virtual void drawFlame(bool displayParticle) = 0;
  
  /** Dessine la m�che de la flamme 
   * @param displayBoxes affiche ou non le partitionnement de la m�che
   */
  virtual void drawWick(bool displayBoxes) = 0; 

  /** Dessine la flamme et sa m�che 
   * @param displayBoxes affiche ou non le partitionnement de la m�che
   */
  void draw(bool displayParticle, bool displayBoxes){
    drawWick(displayBoxes);
    drawFlame(displayParticle);
  };
  
  /** Fonction appel�e par le solveur de fluides pour ajouter l'�l�vation thermique de la flamme.
   */
  virtual void addForces (char perturbate, char fdf=0) = 0;
  
  virtual void setForces(double value){  m_innerForce=value; };
  
  virtual void toggleSmoothShading ();
  
    /** Retroune la direction de la base de la flamme vers la derniere particule
   * pour orienter le solide photom�trique.
   */
  virtual Vector getMainDirection() = 0;

  virtual Point getCenter () = 0;
  
protected:
  /** Fonction simplifiant l'affectation d'un point de contr�le.
   * @param u indice u du point de contr�le
   * @param v indice v du point de contr�le
   * @param pt position du point de contr�le dans l'espace
   */
  void setCtrlPoint (int u, int v, const Point * const pt)
  {
    m_ctrlPoints[(u * m_size + v) * 3] = pt->x;
    m_ctrlPoints[(u * m_size + v) * 3 + 1] = pt->y;
    m_ctrlPoints[(u * m_size + v) * 3 + 2] = pt->z;
    //    m_ctrlPoints[(u*m_size+v)*4+3] = 1.0;
  }
  
  /** Fonction simplifiant l'affectation d'un point de contr�le. 
   * @param u indice u du point de contr�le
   * @param v indice v du point de contr�le
   * @param pt position du point de contr�le dans l'espace
   * @param w coordonn�e homog�ne du point de contr�le, �quivalente au poids du point de contr�le
   */
  void setCtrlPoint (int u, int v, const Point * const pt, double w)
  {
    m_ctrlPoints[(u * m_size + v) * 3] = pt->x;
    m_ctrlPoints[(u * m_size + v) * 3 + 1] = pt->y;
    m_ctrlPoints[(u * m_size + v) * 3 + 2] = pt->z;
    //    m_ctrlPoints[(u*m_size+v)*3+3] = w;
  }
  
  static void CALLBACK nurbsError (GLenum errorCode);
  
  /** Nombre de squelettes de la flamme. */
  int m_nbSkeletons;
  /** Position en indices de la base de la flamme dans la grille de voxels du solveur. */
  int m_x, m_y, m_z;
  
  /** Ordre de la NURBS en u (�gal au degr� en u + 1). */
  int m_uorder;
  /** Ordre de la NURBS en v (�gal au degr� en v + 1). */
  int m_vorder;
  /** Tableau contenant les pointeurs vers les squelettes p�riph�riques. */
  PeriSkeleton **m_skeletons;
  /** Matrice de points de contr�le */
  GLfloat *m_ctrlPoints;
  /** Vecteur de noeuds en u */
  GLfloat *m_uknots;
  /** Vecteur de noeuds en v */
  GLfloat *m_vknots;
  /** Tableau temporaire utilis� pour stocker les distances entre chaque point de contr�le d'un
   * squelette. Allou� une seule fois en d�but de programme � la taille maximale pour des raisons
   * �videntes d'optimisation du temps d'ex�cution.
   */
  int m_uknotsCount, m_vknotsCount;
  int m_maxParticles;
  
  double *m_distances;
  /** Tableau temporaire utilis� pour classer les indices des distances entre points de contr�le
   * lors de l'ajout de points de contr�le suppl�mentaires dans la NURBS.  Allou� une seule fois 
   * en d�but de programme � la taille maximale pour des raisons �videntes d'optimisation du temps 
   * d'ex�cution.
   */
  int *m_maxDistancesIndexes;
  
  /** Objet OpenGL permettant de d�finir la NURBS */
  GLUnurbsObj *m_nurbs;
  
  /** Pointeur sur le solveur de fluides */
  Solver *m_solver;
  
  int m_size;
  bool m_toggle;
  
  int m_perturbateCount;
  
  short m_lifeSpanAtBirth;
  /** Nombre de points fixes pour chaque direction v = origine du squelette p�riph�rique + sommet du guide */
  short m_nbFixedPoints;
  
  double m_innerForce;
  
  Scene *m_scene;
  
  /** Identifiant de la texture. */
  Texture m_tex;
  
  /** Position relative de la flamme dans le feu auquel elle appartient */
  Point m_position;
};



/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE LINEFLAME *****************************************/
/**********************************************************************************************************************/

/** La classe LineFlame impl�mente une flamme qui provient d'une m�che "lin�aire".<br>
 * Elle g�n�re ses squelettes � partir du maillage de la m�che dont le nom est fourni au
 * constructeur.
 * L'objet Wick appartient � la classe LineFlame, il est donc pr�cis� dans le constructeur
 * de Wick que l'objet doit �tre import� dans la sc�ne dans l'�tat "detached", de sorte que 
 * le constructeur de la sc�ne ne cherche pas � le r�f�rencer ni � le d�truire.
 *
 * @author	Flavien Bridault
 */
class LineFlame : public BasicFlame
{
public:
  /** Constructeur.
   * @param s pointeur sur le solveur de fluides
   * @param nbSkeletons Nombre de squelettes guides
   * @param posRel Position relative du centre de la flamme
   * @param innerForce Force int�rieure de la flamme
   * @param scene Pointeur sur la sc�ne
   * @param wickName Cha�ne de caract�re contenant le nom du fichier contenant la m�che
   */
  LineFlame(Solver *s, int nbSkeletons, Point& posRel, double innerForce, Scene *scene, const wxString& textureName, const char *wickFileName, const char*wickName=NULL);
  virtual ~LineFlame();

      /** Fonction appel�e par la fonction de dessin OpenGL. Elle commence par d�placer les particules 
   * des squelettes p�riph�riques. Ensuite, elle d�finit la matrice de points de contr�le de la NURBS,
   * des vecteurs de noeuds.
   */
  virtual void build();
  
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture
   */
  virtual void drawFlame(bool displayParticle);
  
  /** Dessine la m�che de la flamme 
   * @param displayBoxes affiche ou non le partitionnement de la m�che
   */
  virtual void drawWick(bool displayBoxes);
  
  /** Fonction appel�e par le solveur de fluides pour ajouter l'�l�vation thermique de la flamme.
   */
  void addForces(char perturbate, char fdf);
  
  /** Retroune la direction de la base de la flamme vers la derniere particule
   * pour orienter le solide photom�trique.
   */
  Vector getMainDirection();
  
  virtual Point getCenter ();
  
private:
  /** M�che de la flamme */
  Wick m_wick;

  /** Pointeur vers les squelettes guide. */
  LeadSkeleton **m_leads;
  
  /** Nombres de squelettes guides */
  int m_nbLeadSkeletons;
};



/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE POINTFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe PointFlame impl�mente une flamme qui provient d'une m�che verticale droite.<br>
 * Elle g�n�re ses squelettes � partir du maillage de la m�che dont le nom est fourni au
 * constructeur.
 *
 * @author	Flavien Bridault
 */
class PointFlame : public BasicFlame
{
public:
  /** Constructeur.
   * @param s pointeur sur le solveur de fluides
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage. A VERIFIER
   * @param posRel Position relative du centre de la flamme
   * @param innerForce Force int�rieure de la flamme
   * @param scene Pointeur sur la sc�ne
   * @param rayon rayon de la flamme
   */
  PointFlame(Solver *s, int nbSkeletons, Point& posRel, double innerForce, Scene *scene, double rayon);
  virtual ~PointFlame();
  
      /** Fonction appel�e par la fonction de dessin OpenGL. Elle commence par d�placer les particules 
   * des squelettes p�riph�riques. Ensuite, elle d�finit la matrice de points de contr�le de la NURBS,
   * des vecteurs de noeuds.
   */
  virtual void build();
  
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture
   */
  virtual void drawFlame(bool displayParticle);
  
  /** Dessine la m�che de la flamme 
   * @param displayBoxes affiche ou non le partitionnement de la m�che. Ce param�tre
   * est ignor� pour ce type de flamme puisque la m�che n'est pas d�coup�. 
   */
  virtual void drawWick(bool displayBoxes);
  
  /** Fonction appel�e par le solveur de fluides pour ajouter l'�l�vation thermique de la flamme.
   */
  void addForces(char perturbate, char fdf=0);
  
  /** Retourne la direction de la base de la flamme vers la derniere particule
   * pour orienter le solide photom�trique.
   */
  Vector getMainDirection(){
    return(*(m_lead->getMiddleParticle()));
  };
  
  virtual Point getCenter (){    
    return (*m_lead->getMiddleParticle () + m_position);
  };
  
private:
  /** Pointeur vers le squelette guide. */
  LeadSkeleton *m_lead;
};

#endif
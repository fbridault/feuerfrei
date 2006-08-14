#ifndef BASICFLAMES_H
#define BASICFLAMES_H

class MetaFlame;
class BasicFlame;
class LineFlame;
class PointFlame;

#include "GL/glu.h"
#include "periSkeleton.hpp"
#include "leadSkeleton.hpp"
#include "../solvers/solver.hpp"
#include "../scene/texture.hpp"

#include <list>

#include "wick.hpp"

#ifndef CALLBACK
#define CALLBACK
#endif

class PeriSkeleton;
class LeadSkeleton;
class Solver;
/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE BASICFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe BasicFlame fournit un objet simple englobant des squelettes de flamme.
 * Cette classe est abstraite et est héritée par les classes LineFlame et PointFlame.
 * Elle permet de définir une primitive géométrique pour une flamme, mais ne permet pas de construire une flamme
 * en tant que source de lumière d'une scène. C'est la classe FireSource qui permet de définir ceci, en utilisant
 * les classes héritées de BasicFlame comme élément de base.<br>
 * Une BasicFlame est construite à sa position relative définie dans m_position. Lors du dessin, la translation dans
 * le repère du feu est donc déjà effectuée. C'est la classe FireSource qui s'occupe de placer correctement dans
 * le repère du monde. En revanche, la mèche en définie en (0,0,0) et il faut donc la translater dans le repère du
 * feu à chaque opération de dessin.
 *
 * @author	Flavien Bridault
 */
class MetaFlame
{
public:
  /** Constructeur de flamme. La position de la flamme est définie dans le repère du solveur.
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage.
   * @param posRel position du centre de la flamme
   * @param texname Nom du fichier image à charger
   * @param wrap_s paramètre de répétition de la texture dans la direction s {GL_WRAP,GL_REPEAT}
   * @param wrap_t paramètre de répétition de la texture dans la direction t {GL_WRAP,GL_REPEAT}
   */
  MetaFlame(FlameConfig* flameConfig, uint nbSkeletons, ushort nbFixedPoints, const wxString& texname, GLint wrap_s, GLint wrap_t);
  virtual ~MetaFlame ();
  
    /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
   * des vecteurs de noeuds.
   */
  virtual void build() = 0;
  
  void drawCachedFlame ()
  {
    glCallList(m_displayList);
  }
  
  void drawPointFlame();
  
  void drawLineFlame();  
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
   * build() avec le placage de texture. 
   */
  virtual void drawFlame(bool displayParticle) = 0;
  
  /** Dessine la mèche de la flamme 
   * @param displayBoxes affiche ou non le partitionnement de la mèche
   */
  virtual void drawWick(bool displayBoxes) = 0;
  
  /** Dessine la flamme et sa mèche 
   * @param displayBoxes affiche ou non le partitionnement de la mèche
   */
  void draw(bool displayParticle, bool displayBoxes){
    drawWick(displayBoxes);
    drawFlame(displayParticle);
  };
  
  virtual void setSamplingTolerance(double value){ gluNurbsProperty(m_nurbs, GLU_SAMPLING_TOLERANCE, value); };
  
  virtual void toggleSmoothShading ();
  
    /** Retroune la direction de la base de la flamme vers la derniere particule
   * pour orienter le solide photométrique.
   */
  virtual Vector getMainDirection() = 0;
  
  virtual Point getCenter () = 0;
  
  void cloneNURBSPropertiesFrom(MetaFlame& source)
  {
    m_maxParticles = source.m_maxParticles;
    memcpy(m_ctrlPoints, source.m_ctrlPoints,(m_maxParticles + m_nbFixedPoints) * (m_nbSkeletons + m_uorder) * 3 * sizeof(GLfloat));
    m_uknotsCount = source.m_uknotsCount;
    m_vknotsCount = source.m_vknotsCount;
    memcpy(m_uknots, source.m_uknots, m_uknotsCount * sizeof(GLfloat));
    memcpy(m_vknots, source.m_vknots, m_vknotsCount * sizeof(GLfloat));
    memcpy(m_texPoints, source.m_texPoints, (m_maxParticles + m_nbFixedPoints) * (m_nbSkeletons + m_uorder) * 2 * sizeof(GLfloat));
    memcpy(m_texTmp, source.m_texTmp, (m_maxParticles + m_nbFixedPoints) * sizeof(GLfloat));
    m_size = source.m_size;
  }
  Point getPosition(){ return m_position; };
  
  uint getNbSkeletons(){ return m_nbSkeletons; };
  
  unsigned short getNbFixedPoints(){ return m_nbFixedPoints; };
  
  virtual Point* getTop() = 0;
  virtual Point* getBottom() = 0;  
  
//   void setRenderMode() { gluNurbsProperty(m_nurbs,GLU_NURBS_MODE,GLU_NURBS_TESSELLATOR); };
//   void setTesselateMode() { gluNurbsProperty(m_nurbs,GLU_NURBS_MODE,GLU_NURBS_TESSELLATOR); };
  
protected:  
  /** Fonction simplifiant l'affectation d'un point de contrôle.
   * @param u indice u du point de contrôle
   * @param v indice v du point de contrôle
   * @param pt position du point de contrôle dans l'espace
   */
  void setCtrlPoint (const Point * const pt, GLfloat v)
  {
    *m_ctrlPoints++ = pt->x;
    *m_ctrlPoints++ = pt->y;
    *m_ctrlPoints++ = pt->z;
    *m_texPoints++ = *m_texTmp++;
    *m_texPoints++ = v;
  }
  /** Fonction simplifiant l'affectation d'un point de contrôle.
   * @param u indice u du point de contrôle
   * @param v indice v du point de contrôle
   * @param pt position du point de contrôle dans l'espace
   */
//   void setCtrlPoint (int u, int v, const Point * const pt)
//   {
//     m_ctrlPoints[(u * m_size + v) * 3] = pt->x;
//     m_ctrlPoints[(u * m_size + v) * 3 + 1] = pt->y;
//     m_ctrlPoints[(u * m_size + v) * 3 + 2] = pt->z;
//     //    m_ctrlPoints[(u*m_size+v)*4+3] = 1.0;
//   }
    
  static void CALLBACK nurbsError(GLenum errorCode)
  {
    const GLubyte *estring;
  
    estring = gluErrorString(errorCode);
    fprintf(stderr, "Erreur Nurbs : %s\n", estring);
    exit(0);
  }

  static void CALLBACK NurbsBegin(GLenum type, void *displayList)
  {
    glBegin(type);
  }

  static void CALLBACK NurbsEnd()
  {
    glEnd();
  }

  static void CALLBACK NurbsVertex ( GLfloat *vertex )
  {
    glVertex3fv(vertex);
  }

  static void CALLBACK NurbsNormal ( GLfloat *normal )
  {
    glNormal3fv(normal);
  }

  static void CALLBACK NurbsTexCoord ( GLfloat *texCoord )
  {
    glTexCoord2fv(texCoord);
  }

  /** Position en indices de la base de la flamme dans la grille de voxels du solveur. */
  uint m_x, m_y, m_z;
  
  /** Ordre de la NURBS en u (égal au degré en u + 1). */
  u_char m_uorder;
  /** Ordre de la NURBS en v (égal au degré en v + 1). */
  u_char m_vorder;
  /** Nombre de squelettes de la flamme. */
  uint m_nbSkeletons;
  /** Matrice de points de contrôle */
  GLfloat *m_ctrlPoints;
  /** Copie du pointeur vers le tableau de points de contrôle */
  GLfloat  *m_ctrlPointsSave;

  GLfloat *m_texPoints, *m_texPointsSave;
  GLfloat *m_texTmp, *m_texTmpSave;
  
  /** Vecteur de noeuds en u */
  GLfloat *m_uknots;
  /** Vecteur de noeuds en v */
  GLfloat *m_vknots;
  /** Tableau temporaire utilisé pour stocker les distances entre chaque point de contrôle d'un
   * squelette. Alloué une seule fois en début de programme à la taille maximale pour des raisons
   * évidentes d'optimisation du temps d'exécution.
   */
  uint m_uknotsCount, m_vknotsCount;
  uint m_maxParticles;
    
  /** Objet OpenGL permettant de définir la NURBS */
  GLUnurbsObj *m_nurbs;
  
  uint m_size;
  bool m_toggle;
  
  /** Nombre de points fixes pour chaque direction v = origine du squelette périphérique + sommet du guide */
  unsigned short m_nbFixedPoints;
  
  /** Texture de la flamme */
  Texture m_tex;
  
  /* Texture pour le halo */
  Texture m_halo;
  
  /** Position relative de la flamme dans le feu auquel elle appartient */
  Point m_position;
  
  GLuint m_displayList;
};

/**********************************************************************************************************************/
/********************************************* DEFINITION DE LA CLASSE BASICFLAME *************************************/
/**********************************************************************************************************************/

/** La classe FlameSolverInterface 
 *
 * @author	Flavien Bridault
 */
class BasicFlame : public MetaFlame
{
public:
  /** Constructeur de flamme. La position de la flamme est définie dans le repère du solveur.
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage.
   * @param posRel position du centre de la flamme
   * @param texname Nom du fichier image à charger
   * @param wrap_s paramètre de répétition de la texture dans la direction s {GL_WRAP,GL_REPEAT}
   * @param wrap_t paramètre de répétition de la texture dans la direction t {GL_WRAP,GL_REPEAT}
   */
  BasicFlame(FlameConfig* flameConfig, uint nbSkeletons, uint nbFixedPoints, const wxString& texname, 
	     GLint wrap_s, GLint wrap_t, Solver *s);
  virtual ~BasicFlame ();
  
  virtual void addForces (u_char perturbate, u_char fdf=0) = 0;
  
  virtual void setForces(double value){  m_innerForce=value; };
  
  void drawParticles(bool displayParticle)
  {
    /* Affichage des particules */
    if(displayParticle){
      uint i;
      /* Déplacement et détermination du maximum */
      for (i = 0; i < m_nbSkeletons; i++)
	m_skeletons[i]->draw();
      for (i = 0; i < m_nbLeadSkeletons; i++)
	m_leads[i]->draw();
    }
  };
  
  virtual Point* getTop() = 0;
  virtual Point* getBottom() = 0;
  
  /** Méthode permettant d'informer à la flamme de se localiser dans le solveur */
  /* Elle doit être appelée dès qu'un changement de résolution de la grille intervient */
  virtual void locateInSolver(){ m_solver->findPointPosition(m_position, m_x, m_y, m_z); };
  
protected:
  /** Pointeur vers les squelettes guide. */
  LeadSkeleton **m_leads;
  /** Nombres de squelettes guides */
  uint m_nbLeadSkeletons;
  /** Tableau contenant les pointeurs vers les squelettes périphériques. */
  PeriSkeleton **m_skeletons;
  /** Pointeur sur le solveur de fluides */
  Solver *m_solver;  
  
  double *m_distances;
  /** Tableau temporaire utilisé pour classer les indices des distances entre points de contrôle
   * lors de l'ajout de points de contrôle supplémentaires dans la NURBS.  Alloué une seule fois 
   * en début de programme à la taille maximale pour des raisons évidentes d'optimisation du temps 
   * d'exécution.
   */
  int *m_maxDistancesIndexes;

  
  double m_innerForce;
  
  uint m_perturbateCount;
};

/**********************************************************************************************************************/
/**************************************** DEFINITION DE LA CLASSE LINEFLAME **************************************/
/**********************************************************************************************************************/

/** La classe BasicLineFlame implémente une flamme qui provient d'une mèche "linéaire".<br>
 * L'objet Wick appartient à la classe LineFlame, il est donc précisé dans le constructeur
 * de Wick que l'objet doit être importé dans la scène dans l'état "detached", de sorte que 
 * le constructeur de la scène ne cherche pas à le référencer ni à le détruire.
 *
 * @author	Flavien Bridault
 */
class LineFlame : public BasicFlame
{
public:
  /** Constructeur.
   * @param nbSkeletons Nombre de squelettes guides
   * @param posRel Position relative du centre de la flamme
   * @param innerForce Force intérieure de la flamme
   * @param wickName Chaîne de caractère contenant le nom du fichier contenant la mèche
   */
  LineFlame (FlameConfig* flameConfig, Scene *scene, const wxString& textureName, Solver *s, 
	     const char *wickFileName, const char *wickName=NULL);
  virtual ~LineFlame();

      /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
   * des vecteurs de noeuds.
   */
  virtual void build();
  
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
private:
  /** Mèche de la flamme */
  Wick m_wick;

  list<Particle *> m_sparksList;
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
class PointFlame : public BasicFlame
{
public:
  /** Constructeur.
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit Ãªtre pair en raison de l'affichage. A VERIFIER
   * @param posRel Position relative du centre de la flamme
   * @param innerForce Force intérieure de la flamme
   * @param rayon rayon de la flamme
   */
  PointFlame ( FlameConfig* flameConfig, Solver * s, double rayon);
  virtual ~PointFlame();
  
      /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
   * des vecteurs de noeuds.
   */
  virtual void build();
  
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
    return(*(m_leads[0]->getMiddleParticle()));
  };
  
  virtual Point getCenter (){    
    return (*m_leads[0]->getMiddleParticle () + m_position);
  }; 

  /** Fonction appelée par le solveur de fluides pour ajouter l'élévation thermique de la flamme.
   */
  void addForces(u_char perturbate, u_char fdf=0);
  
  Point* getTop(){ return m_leads[0]->getParticle(0); };
  Point* getBottom() { return m_leads[0]->getRoot(); };
};

#endif

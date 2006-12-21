#ifndef BASICFLAMES_H
#define BASICFLAMES_H

class NurbsFlame;
class FixedFlame;
class RealFlame;

#include "GL/glu.h"
#include "periSkeleton.hpp"
#include "leadSkeleton.hpp"
#include "../solvers/solver3D.hpp"
#include "../scene/texture.hpp"

#include <list>

#ifndef CALLBACK
#define CALLBACK
#endif

class PeriSkeleton;
class LeadSkeleton;
class Field3D;

/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE NURBSFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe NurbsFlame est l'objet de plus bas niveau représentant une flamme. Elle défini les propriétés nécessaires
 * pour la construction d'une NURBS, ainsi que la localisation de la flamme dans l'espace. En revanche, la notion de 
 * squelette n'apparaît absolument pas ici.
 *
 * @author	Flavien Bridault
 */
class NurbsFlame
{
public:
  /** Constructeur de flamme par défaut n'effectuant pas l'allocation des tableaux utilisés pour la génération
   * des points de contrôle. L'allocation doit donc être effectuée par la classe fille, ce que fait par exemple
   * la classe FixedFlame.
   * @param flameConfig Pointeur sur le configuration de la flamme.
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage.
   * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
   * @param tex Pointeur sur la texture de la flamme.
   */
  NurbsFlame(FlameConfig* flameConfig, uint nbSkeletons, ushort nbFixedPoints, Texture* const tex);
  
  /** Constructeur de flamme. La position de la flamme est définie dans le repère du solveur.
   * @param source Pointeur sur la flamme qui a généré la flamme courante.
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage.
   * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
   * @param tex Pointeur sur la texture de la flamme.
   */
  NurbsFlame(NurbsFlame* const source, uint nbSkeletons, ushort nbFixedPoints, Texture* const tex);
  virtual ~NurbsFlame ();
  
    /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
   * des vecteurs de noeuds.
   * @return false si un problème dans la contruction est survenu (pas assez de particules par exemple)
   */
  virtual bool build() = 0;
      
  virtual void drawLineFlame();  
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
   * build() avec le placage de texture.
   * @param display Affiche ou non la flamme.
   * @param displayParticle Affiche ou non les particules.
   */
  virtual void drawFlame(bool display, bool displayParticle) = 0;
  
  /** Dessine la flamme et sa mèche.
   * @param display Affiche ou non la flamme.
   * @param displayParticle Affiche ou non les particules.
   * @param displayBoxes Affiche ou non le partitionnement de la mèche.
   */
  void draw(bool display, bool displayParticle, bool displayBoxes){
    drawFlame(display, displayParticle);
  };
  
  /** Ajuste la valeur d'échantillonnage de la NURBS.
   * @param value Valeur de sampling, généralement compris dans un intervalle [1;1000]. 
   */
  virtual void setSamplingTolerance(double value){ gluNurbsProperty(m_nurbs, GLU_SAMPLING_TOLERANCE, value); };
  
  /** Active ou désactive l'affichage texturé sur la flamme. */
  virtual void toggleSmoothShading ();
  
  Point getPosition(){ return m_position; };
  
  /** Retourne le nombre de squelettes */
  uint getNbSkeletons(){ return m_nbSkeletons; };
  
//   void setRenderMode() { gluNurbsProperty(m_nurbs,GLU_NURBS_MODE,GLU_NURBS_TESSELLATOR); };
//   void setTesselateMode() { gluNurbsProperty(m_nurbs,GLU_NURBS_MODE,GLU_NURBS_TESSELLATOR); };
  
  /** Retourne le nombre de points fixes, autrement dit le nombre de racines dans la flamme */
  unsigned short getNbFixedPoints(){ return m_nbFixedPoints; };
  
  /** Méthode permettant de cloner la flamme passée en paramètre
   * @param source Flamme à cloner
   */
  void cloneNURBSPropertiesFrom(NurbsFlame& source)
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
  
  Texture *getTexture() const { return m_tex; };

protected:
  /** Affiche la flamme sous forme de NURBS, à partir du tableau de points de contrôles et du tableau
   * de coordonnées de texture construits au préalable.
   */
  void drawNurbs ();
  
  /** Fonction simplifiant l'affectation d'un point de contrôle. L'algorithme de construction
   * de la NURBS parcours de façon séquentielle les squelettes, aucun indice dans le tableau
   * de points de contrôles n'est donc passé. En lieu et place, un pointeur est utilisé et
   * incrémenté après chaque affectation dans cette fonction. Le même système est employé
   * pour le tableau des coordonnées (u,v) de texture. La coordonnée u est précalculée dans
   * texTmp, tandis que la coordonnée v est passée en paramètre.
   *
   * @param pt point à affecter dans le tableau
   * @param v valeur de la coordonnée de texture t
   */
  void setCtrlPoint (const Point * const pt, GLfloat v)
  {
    *m_ctrlPoints++ = pt->x;
    *m_ctrlPoints++ = pt->y;
    *m_ctrlPoints++ = pt->z;
    *m_texPoints++ = *m_texTmp++;
    *m_texPoints++ = v;
  }
    
  static void CALLBACK nurbsError(GLenum errorCode)
  {
    const GLubyte *estring;
  
    estring = gluErrorString(errorCode);
    fprintf(stderr, "Erreur Nurbs : %s\n", estring);
    exit(0);
  }

  static void CALLBACK NurbsBegin(GLenum type, GLvoid *toggle)
  {
    if( * (bool *)toggle )
      glPolygonMode(GL_FRONT,GL_LINE);
    glBegin(type);
  }

  static void CALLBACK NurbsEnd(GLvoid *toggle)
  {
    glEnd();
    if( * (bool *)toggle )
      glPolygonMode(GL_FRONT,GL_FILL);
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
  
  /** Nombre de points fixes pour chaque direction v, par exemple origine des squelettes, sommet du guide */
  unsigned short m_nbFixedPoints;
    
  /** Objet OpenGL permettant de définir la NURBS */
  GLUnurbsObj *m_nurbs;
  
  uint m_size;
  bool m_toggle;  
  
  /** Texture de la flamme */
  Texture *m_tex;
    
  /** Position relative de la flamme dans le feu auquel elle appartient */
  Point m_position;
  
  /** Configuration de la flamme */
  FlameConfig *m_flameConfig;
};


/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE FIXEDFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe FixedFlame est l'objet de plus bas niveau représentant une flamme dont la base est fixe. Par rapport à la
 * classe de base NurbsFlame, elle rajoute la prise en compte des origines des squelettes, notamment pour l'allocation
 * des tableaux de points de contrôles. Elle défini également quelques méthodes abstraites pour obtenir des informations
 * sur la localisation de la flamme nécessaire pour l'éclairage et la construction d'un halo. Tout ceci n'est en effet
 * pas nécessaire pour la définition d'une flamme détachée.
 *
 * @author	Flavien Bridault
 */
class FixedFlame : public NurbsFlame
{
public:
  /** Constructeur de flamme par défaut.
   * @param flameConfig Pointeur sur le configuration de la flamme
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage.
   * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
   * @param tex Pointeur sur la texture de la flamme.
   */
  FixedFlame(FlameConfig* flameConfig, uint nbSkeletons, ushort nbFixedPoints, Texture* const tex);
  
  virtual ~FixedFlame ();
  
  /** Dessine la mèche de la flamme.
   * @param displayBoxes Affiche ou non le partitionnement de la mèche.
   */
  virtual void drawWick(bool displayBoxes) = 0;
  
  void draw(bool display, bool displayParticle, bool displayBoxes){
    drawWick(displayBoxes);
    drawFlame(display, displayParticle);
  };
  
  /** Affiche le halo.
   * @param angle Angle de rotation pour le quad texturé.
   */
  virtual void drawHalo (double angle);
  
  /** Dessine une flamme ponctuelle. La différence avec drawLineFlame() est que la texture est translatée
   * pour rester en face de l'observateur.
   */
  virtual void drawPointFlame();
  
  /** Retourne la direction de la base de la flamme vers la derniere particule
   * pour orienter le solide photométrique.
   * @return Direction.
   */
  virtual Vector getMainDirection() const = 0;
  
  /** Retourne le centre de la flamme.
   * @return Centre de la flamme.
   */
  virtual Point getCenter () const = 0;
  
  /** Renvoie un pointeur vers le sommet de la flamme.
   * @return Pointeur vers le sommet.
   */
  virtual Point* getTop() const = 0;
  
  /** Renvoie un pointeur vers le bas de la flamme.
   * @return Pointeur vers le bas.
   */
  virtual Point* getBottom() const = 0;
  
protected:
  /* Texture pour le halo */
  Texture m_halo;
};

class DetachedFlame;
/**********************************************************************************************************************/
/********************************************* DEFINITION DE LA CLASSE REALFLAME **************************************/
/**********************************************************************************************************************/

#include "noise.hpp"

/** La classe RealFlame, par rapport à la classe FixedFlame, ajoute la notion de squelettes de flamme ainsi que les
 * interactions avec un solveur de fluides.
 * Cette classe reste abstraite et est héritée par les classes LineFlame et PointFlame.
 * Elle permet de définir une primitive géométrique pour une flamme, mais ne permet pas de construire une flamme
 * en tant que source de lumière d'une scène. C'est la classe FireSource qui permet de définir ceci, en utilisant
 * les classes héritées de RealFlame comme élément de base.<br>
 * Une RealFlame est construite à sa position relative définie dans m_position. Lors du dessin, la translation dans
 * le repère du feu est donc déjà effectuée. C'est la classe FireSource qui s'occupe de placer correctement dans
 * le repère du monde. En revanche, la mèche en définie en (0,0,0) et il faut donc la translater dans le repère du
 * feu à chaque opération de dessin.
 *
 * @author	Flavien Bridault
 */
class RealFlame : public FixedFlame
{
public:
  /** Constructeur de flamme. La position de la flamme est définie dans le repère du solveur.
   * @param flameConfig Pointeur sur le configuration de la flamme.
   * @param nbSkeletons Nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage.
   * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
   * @param tex Pointeur sur la texture de la flamme.
   * @param s Pointeur vers le solveur.
   */
  RealFlame(FlameConfig* flameConfig, uint nbSkeletons, ushort nbFixedPoints, Texture* const tex, Field3D *s,
	    float noiseIncrement, float noiseMin, float noiseMax);
  virtual ~RealFlame ();
  
  /** Fonction appelée par le solveur de fluides pour ajouter l'élévation thermique de la flamme.
   * @param perturbate Type de perturbation parmi FLICKERING_VERTICAL, FLICKERING_RANDOM, etc...
   * @param fdf Type de fonction de distribution de carburant parmi FDF_LINEAR, FDF_BILINEAR, 
   * FDF_EXPONENTIAL, FDF_GAUSS, FDF_RANDOM.
   */
  virtual void addForces (u_char perturbate, u_char fdf=0) = 0;
  
  /** Affectation de la vélocité induite par la flamme.
   * @param value Vélocité de la flamme.
   */
  virtual void setForces(double value){  m_innerForce=value; };
  
  /** Affiche les particules de tous les squelettes composants la flamme. */
  void drawParticles()
  {
    uint i;
    /* Déplacement et détermination du maximum */
    for (i = 0; i < m_nbSkeletons; i++)
      m_periSkeletons[i]->draw();
    for (i = 0; i < m_nbLeadSkeletons; i++)
      m_leadSkeletons[i]->draw();
  };
  
  virtual bool build();
  
  virtual Vector getMainDirection() const = 0;  
  virtual Point getCenter () const = 0;
  
  virtual Point* getTop() const = 0;
  virtual Point* getBottom() const = 0;
  
  /** Méthode permettant d'informer à la flamme de se localiser dans le solveur */
  /* Elle doit être appelée dès qu'un changement de résolution de la grille intervient */
  virtual void locateInField(){ m_solver->findPointPosition(m_position, m_x, m_y, m_z); };
  
  /** Fonction testant si les squelettes doivent se briser. Si c'est le cas, elle effectue la division. */
  virtual void breakCheck() = 0;
protected:
  /** Pointeur vers les squelettes guide. */
  LeadSkeleton **m_leadSkeletons;
  /** Nombres de squelettes guides */
  uint m_nbLeadSkeletons;
  /** Tableau contenant les pointeurs vers les squelettes périphériques. */
  PeriSkeleton **m_periSkeletons;
  /** Pointeur sur le solveur de fluides */
  Field3D *m_solver;  
  
  double *m_distances;
  /** Tableau temporaire utilisé pour classer les indices des distances entre points de contrôle
   * lors de l'ajout de points de contrôle supplémentaires dans la NURBS.  Alloué une seule fois 
   * en début de programme à la taille maximale pour des raisons évidentes d'optimisation du temps 
   * d'exécution.
   */
  int *m_maxDistancesIndexes;
  
  double m_innerForce;
  
  uint m_perturbateCount;

  double m_seed;
  PerlinNoise1D m_noiseGenerator;
};

#endif

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
#include "../scene/object.hpp"

#include <list>

#ifndef CALLBACK
#define CALLBACK
#endif

class PeriSkeleton;
class LeadSkeleton;
class Field3D;

#ifdef COUNT_NURBS_POLYGONS
extern uint g_count;
#endif

/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE NURBSFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe NurbsFlame est l'objet de plus bas niveau représentant une flamme. Elle définit les propriétés nécessaires
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
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage.
   * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
   * @param tex Pointeur sur la texture de la flamme.
   */
  NurbsFlame(uint nbSkeletons, ushort nbFixedPoints, const Texture* const tex);
  
  /** Constructeur de flamme. La position de la flamme est définie dans le repère du solveur.
   * @param source Pointeur sur la flamme qui a généré la flamme courante.
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage.
   * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
   * @param tex Pointeur sur la texture de la flamme.
   */
  NurbsFlame(const NurbsFlame* const source, uint nbSkeletons, ushort nbFixedPoints, const Texture* const tex);
  virtual ~NurbsFlame ();
  
  void initNurbs(GLUnurbsObj** nurbs);
  /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
   * des vecteurs de noeuds.
   * @return false si un problème dans la contruction est survenu (pas assez de particules par exemple)
   */
  virtual bool build() = 0;
  
  virtual void drawLineFlame() const;  
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
   * build() avec le placage de texture.
   * @param display Affiche ou non la flamme.
   * @param displayParticle Affiche ou non les particules.
   */
  virtual void drawFlame(bool display, bool displayParticle) const = 0;
  
  /** Dessine la flamme et sa mèche.
   * @param display Affiche ou non la flamme.
   * @param displayParticle Affiche ou non les particules.
   * @param displayBoxes Affiche ou non le partitionnement de la mèche.
   */
  void draw(bool display, bool displayParticle, bool displayBoxes) const{
    drawFlame(display, displayParticle);
  };
  
  /** Ajuste la valeur d'échantillonnage de la NURBS.
   * @param value Valeur de sampling, généralement compris dans un intervalle [1;1000]. 
   */
  virtual void setSamplingTolerance(u_char value){
    switch(value){
    case 0:
      gluNurbsProperty(m_nurbs, GLU_U_STEP, 4);
      gluNurbsProperty(m_nurbs, GLU_V_STEP, 4);
      break;
    case 1:
//       gluNurbsProperty(m_nurbs, GLU_U_STEP, 2);
//       gluNurbsProperty(m_nurbs, GLU_V_STEP, 2);
      break;
    case 2:
//       gluNurbsProperty(m_nurbs, GLU_U_STEP, 1);
//       gluNurbsProperty(m_nurbs, GLU_V_STEP, 1);
      break;
    }
  };
  
  /** Active ou désactive l'affichage texturé sur la flamme. */
  virtual void setSmoothShading (bool state) { m_shadingType = (state) ? m_shadingType | 1 : m_shadingType & 2; };
  
  Point getPosition() const { return m_position; };
  
  /** Retourne le nombre de squelettes */
  uint getNbSkeletons() const { return m_nbSkeletons; };
    
  /** Retourne le nombre de points fixes, autrement dit le nombre de racines dans la flamme */
  unsigned short getNbFixedPoints() const{ return m_nbFixedPoints; };
  
  /** Méthode permettant de cloner la flamme passée en paramètre
   * @param source Flamme à cloner
   */
  void cloneNURBSPropertiesFrom(const NurbsFlame& source)
  {
    m_maxParticles = source.m_maxParticles;
    
    copy(source.m_ctrlPoints, &source.m_ctrlPoints[(m_maxParticles + m_nbFixedPoints) * (m_nbSkeletons + m_uorder) * 3], m_ctrlPoints);
    m_uknotsCount = source.m_uknotsCount;
    m_vknotsCount = source.m_vknotsCount;
    copy(source.m_uknots, &source.m_uknots[m_uknotsCount], m_uknots);
    copy(source.m_vknots, &source.m_vknots[m_vknotsCount], m_vknots);
    copy(source.m_texPoints, &source.m_texPoints[(m_maxParticles + m_nbFixedPoints) * (m_nbSkeletons + m_uorder) * 2], m_texPoints);
    copy(source.m_texTmp, &source.m_texTmp[m_maxParticles + m_nbFixedPoints], m_texTmp);
    m_vsize = source.m_vsize;
  }
  
  const Texture *getTexture() const { return m_tex; };

protected:
  /** Affiche la flamme sous forme de NURBS, à partir du tableau de points de contrôles et du tableau
   * de coordonnées de texture construits au préalable.
   */
  void drawNurbs () const;
  
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
  void setCtrlPoint (const Point * const pt, GLfloat u)
  {
    *m_ctrlPoints++ = pt->x;
    *m_ctrlPoints++ = pt->y;
    *m_ctrlPoints++ = pt->z;
    *m_texPoints++ = u;
    *m_texPoints++ = *m_texTmp++;
    m_count++;
  }
    
  static void CALLBACK nurbsError(GLenum errorCode)
  {
    const GLubyte *estring;
  
    estring = gluErrorString(errorCode);
    cerr << "Nurbs error : " << estring << endl;
    exit(0);
  }
  
  static void CALLBACK NurbsBegin(GLenum type, GLvoid *shadingType)
  {
#ifdef COUNT_NURBS_POLYGONS
    g_count++;
#endif
    /* Si on est en mode simplifié */
    if( *(u_char *)shadingType & 2)
      {
	glDisable(GL_CULL_FACE);
	if( (*(u_char *)shadingType & 1))
	  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	else
	  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      }
    else
      if( ! (*(u_char *)shadingType & 1))
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      
    glBegin(type);
  }

  static void CALLBACK NurbsEnd(GLvoid *shadingType)
  {
    glEnd();
    /* Si on est en mode simplifié */
    if( *(char *)shadingType & 2)
      {
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT,GL_FILL);
      }
    else
      if( ! (*(char *)shadingType & 1))
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
  uint m_count;
  /** Nombre de points fixes pour chaque direction v, par exemple origine des squelettes, sommet du guide */
  unsigned short m_nbFixedPoints;
  
  /** Nombre de points total dans la direction v de la NURBS, soit m_nbFixedPoints+m_maxParticles */
  uint m_vsize;
  /** Codage du type de shading : 1er bit à 1 si fil de fer; 2e bit à 1 si front and back */
  u_char m_shadingType;  
  
  /** Texture de la flamme */
  const Texture *m_tex;
  /* Incrément en u pour la coordonnée de texture */
  float m_utexInc;
    
  /** Position relative de la flamme dans le feu auquel elle appartient */
  Point m_position;
  
private:
  /** Objet OpenGL permettant de définir la NURBS */
  GLUnurbsObj *m_nurbs;
};


/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE FIXEDFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe FixedFlame est l'objet de plus bas niveau représentant une flamme dont la base est fixe. Par rapport à la
 * classe de base NurbsFlame, elle rajoute la prise en compte des origines des squelettes, notamment pour l'allocation
 * des tableaux de points de contrôles. Elle défini également quelques méthodes abstraites pour obtenir des informations
 * sur la localisation de la flamme nécessaire pour l'éclairage. Tout ceci n'est en effet
 * pas nécessaire pour la définition d'une flamme détachée.
 *
 * @author	Flavien Bridault
 */
class FixedFlame : public NurbsFlame
{
public:
  /** Constructeur de flamme par défaut.
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage.
   * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
   * @param tex Pointeur sur la texture de la flamme.
   */
  FixedFlame(uint nbSkeletons, ushort nbFixedPoints, const Texture* const tex);
  
  virtual ~FixedFlame ();
  
  /** Dessine la mèche de la flamme.
   * @param displayBoxes Affiche ou non le partitionnement de la mèche.
   */
  virtual void drawWick(bool displayBoxes) const = 0;
  
  void draw(bool display, bool displayParticle, bool displayBoxes) const{
    drawWick(displayBoxes);
    drawFlame(display, displayParticle);
  };
    
  /** Dessine une flamme ponctuelle. La différence avec drawLineFlame() est que la texture est translatée
   * pour rester en face de l'observateur.
   */
  virtual void drawPointFlame() const;
  
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
  virtual Point getTop() const = 0;
  
  /** Renvoie un pointeur vers le bas de la flamme.
   * @return Pointeur vers le bas.
   */
  virtual Point getBottom() const = 0;
  
};

class DetachedFlame;

#include <vector>
/**********************************************************************************************************************/
/********************************************* DEFINITION DE LA CLASSE REALFLAME **************************************/
/**********************************************************************************************************************/

/** La classe RealFlame, par rapport à la classe FixedFlame, ajoute la notion de squelettes de flamme ainsi que les
 * interactions avec un solveur de fluides. Elle est qualifiée de "Real" en comparaison avec les CloneFlame.
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
   * @param nbSkeletons Nombre de squelettes. Pour le moment nbSkeletons doit être pair en raison de l'affichage.
   * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
   * @param tex Pointeur sur la texture de la flamme.
   * @param s Pointeur vers le solveur.
   */
  RealFlame(uint nbSkeletons, ushort nbFixedPoints, const Texture* const tex, Field3D* const s);
  virtual ~RealFlame ();
  
  /** Fonction appelée par le solveur de fluides pour ajouter l'élévation thermique de la flamme.
   */
  virtual void addForces (int fdf, float innerForce, char perturbate){
    for (vector < LeadSkeleton * >::iterator skeletonsIterator = m_leadSkeletons.begin ();
	 skeletonsIterator != m_leadSkeletons.end (); skeletonsIterator++)
      (*skeletonsIterator)->addForces (fdf, innerForce, perturbate);
  }
  
  /** Affectation de la durée de vie des squelettes guides.
   * @param value Durée de vie en itérations.
   */
  virtual void setLeadLifeSpan(uint value) {
    for (vector < LeadSkeleton * >::iterator skeletonsIterator = m_leadSkeletons.begin ();
	 skeletonsIterator != m_leadSkeletons.end (); skeletonsIterator++)
      (*skeletonsIterator)->setLifeSpan(value);
  };
  
  /** Affectation de la durée de vie des squelettes périphériques.
   * @param value Durée de vie en itérations.
   */
  virtual void setPeriLifeSpan(uint value) { 
    for (uint i = 0; i < m_nbSkeletons; i++)
      m_periSkeletons[i]->setLifeSpan(value); 
  };
  
  /** Affiche les particules de tous les squelettes composants la flamme. */
  void drawParticles() const
  {
    uint i;
    /* Déplacement et détermination du maximum */
    for (i = 0; i < m_nbSkeletons; i++)
      m_periSkeletons[i]->draw();
    for (i = 0; i < m_nbLeadSkeletons; i++)
      m_leadSkeletons[i]->draw();
  };
  
  /** Affiche les particules de tous les squelettes composants la flamme. */
  void setSkeletonsLOD(u_char value) { m_lodSkel = value; m_lodSkelChanged = true; };
  
  /** Affiche les particules de tous les squelettes composants la flamme. */
  virtual void changeSkeletonsLOD()
  {
    uint i;
    for (i = 0; i < m_nbSkeletons; i++)
      m_periSkeletons[i]->setLOD(m_lodSkel);
    for (i = 0; i < m_nbLeadSkeletons; i++)
      m_leadSkeletons[i]->setLOD(m_lodSkel);
    m_lodSkelChanged = false;
  };
  
  /** Fonction chargée de remplir les coordonées de texture dans la direction v. Elles sont
   * en effet dépendantes du nombre de particules dans les squelettes et il est donc nécessaire
   * de les recalculer à chaque construction de la NURBS.
   */
  virtual void computeVTexCoords();
  virtual bool build();
  
  virtual Vector getMainDirection() const = 0;  
  virtual Point getCenter () const = 0;
  
  virtual Point getTop() const = 0;
  virtual Point getBottom() const = 0;
    
  /** Fonction testant si les squelettes doivent se briser. Si c'est le cas, elle effectue la division. */
  virtual void breakCheck() = 0;

protected:
  /** Vecteur contenant les squelettes guide. */
  vector < LeadSkeleton * > m_leadSkeletons;
  
  /** Nombres de squelettes guides */
  uint m_nbLeadSkeletons;
  /** Tableau contenant les pointeurs vers les squelettes périphériques. */
  PeriSkeleton **m_periSkeletons;
  /** Pointeur sur le solveur de fluides */
  Field3D *m_solver;  
  
  float *m_distances;
  /** Tableau temporaire utilisé pour classer les indices des distances entre points de contrôle
   * lors de l'ajout de points de contrôle supplémentaires dans la NURBS.  Alloué une seule fois 
   * en début de programme à la taille maximale pour des raisons évidentes d'optimisation du temps 
   * d'exécution.
   */
  int *m_maxDistancesIndexes;
  
  u_char m_lodSkel;
  /** Indique qu'un changement de niveau de détail sur les squelettes a été demandé. */
  bool m_lodSkelChanged;
};

#endif

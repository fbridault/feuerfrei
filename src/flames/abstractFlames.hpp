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

/** La classe NurbsFlame est l'objet de plus bas niveau repr�sentant une flamme. Elle d�finit les propri�t�s n�cessaires
 * pour la construction d'une NURBS, ainsi que la localisation de la flamme dans l'espace. En revanche, la notion de 
 * squelette n'appara�t absolument pas ici.
 *
 * @author	Flavien Bridault
 */
class NurbsFlame
{
public:
  /** Constructeur de flamme par d�faut n'effectuant pas l'allocation des tableaux utilis�s pour la g�n�ration
   * des points de contr�le. L'allocation doit donc �tre effectu�e par la classe fille, ce que fait par exemple
   * la classe FixedFlame.
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit �tre pair en raison de l'affichage.
   * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
   * @param tex Pointeur sur la texture de la flamme.
   */
  NurbsFlame(uint nbSkeletons, ushort nbFixedPoints, const Texture* const tex);
  
  /** Constructeur de flamme. La position de la flamme est d�finie dans le rep�re du solveur.
   * @param source Pointeur sur la flamme qui a g�n�r� la flamme courante.
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit �tre pair en raison de l'affichage.
   * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
   * @param tex Pointeur sur la texture de la flamme.
   */
  NurbsFlame(const NurbsFlame* const source, uint nbSkeletons, ushort nbFixedPoints, const Texture* const tex);
  virtual ~NurbsFlame ();
  
  void initNurbs(GLUnurbsObj** nurbs);
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle commence par d�placer les particules 
   * des squelettes p�riph�riques. Ensuite, elle d�finit la matrice de points de contr�le de la NURBS,
   * des vecteurs de noeuds.
   * @return false si un probl�me dans la contruction est survenu (pas assez de particules par exemple)
   */
  virtual bool build() = 0;
  
  virtual void drawLineFlame() const;  
  
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture.
   * @param display Affiche ou non la flamme.
   * @param displayParticle Affiche ou non les particules.
   */
  virtual void drawFlame(bool display, bool displayParticle) const = 0;
  
  /** Dessine la flamme et sa m�che.
   * @param display Affiche ou non la flamme.
   * @param displayParticle Affiche ou non les particules.
   * @param displayBoxes Affiche ou non le partitionnement de la m�che.
   */
  void draw(bool display, bool displayParticle, bool displayBoxes) const{
    drawFlame(display, displayParticle);
  };
  
  /** Ajuste la valeur d'�chantillonnage de la NURBS.
   * @param value Valeur de sampling, g�n�ralement compris dans un intervalle [1;1000]. 
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
  
  /** Active ou d�sactive l'affichage textur� sur la flamme. */
  virtual void setSmoothShading (bool state) { m_shadingType = (state) ? m_shadingType | 1 : m_shadingType & 2; };
  
  Point getPosition() const { return m_position; };
  
  /** Retourne le nombre de squelettes */
  uint getNbSkeletons() const { return m_nbSkeletons; };
    
  /** Retourne le nombre de points fixes, autrement dit le nombre de racines dans la flamme */
  unsigned short getNbFixedPoints() const{ return m_nbFixedPoints; };
  
  /** M�thode permettant de cloner la flamme pass�e en param�tre
   * @param source Flamme � cloner
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
  /** Affiche la flamme sous forme de NURBS, � partir du tableau de points de contr�les et du tableau
   * de coordonn�es de texture construits au pr�alable.
   */
  void drawNurbs () const;
  
  /** Fonction simplifiant l'affectation d'un point de contr�le. L'algorithme de construction
   * de la NURBS parcours de fa�on s�quentielle les squelettes, aucun indice dans le tableau
   * de points de contr�les n'est donc pass�. En lieu et place, un pointeur est utilis� et
   * incr�ment� apr�s chaque affectation dans cette fonction. Le m�me syst�me est employ�
   * pour le tableau des coordonn�es (u,v) de texture. La coordonn�e u est pr�calcul�e dans
   * texTmp, tandis que la coordonn�e v est pass�e en param�tre.
   *
   * @param pt point � affecter dans le tableau
   * @param v valeur de la coordonn�e de texture t
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
    /* Si on est en mode simplifi� */
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
    /* Si on est en mode simplifi� */
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
  
  /** Ordre de la NURBS en u (�gal au degr� en u + 1). */
  u_char m_uorder;
  /** Ordre de la NURBS en v (�gal au degr� en v + 1). */
  u_char m_vorder;
  /** Nombre de squelettes de la flamme. */
  uint m_nbSkeletons;
  /** Matrice de points de contr�le */
  GLfloat *m_ctrlPoints;
  /** Copie du pointeur vers le tableau de points de contr�le */
  GLfloat  *m_ctrlPointsSave;

  GLfloat *m_texPoints, *m_texPointsSave;
  GLfloat *m_texTmp, *m_texTmpSave;
  
  /** Vecteur de noeuds en u */
  GLfloat *m_uknots;
  /** Vecteur de noeuds en v */
  GLfloat *m_vknots;
  /** Tableau temporaire utilis� pour stocker les distances entre chaque point de contr�le d'un
   * squelette. Allou� une seule fois en d�but de programme � la taille maximale pour des raisons
   * �videntes d'optimisation du temps d'ex�cution.
   */
  uint m_uknotsCount, m_vknotsCount;
  uint m_maxParticles;
  uint m_count;
  /** Nombre de points fixes pour chaque direction v, par exemple origine des squelettes, sommet du guide */
  unsigned short m_nbFixedPoints;
  
  /** Nombre de points total dans la direction v de la NURBS, soit m_nbFixedPoints+m_maxParticles */
  uint m_vsize;
  /** Codage du type de shading : 1er bit � 1 si fil de fer; 2e bit � 1 si front and back */
  u_char m_shadingType;  
  
  /** Texture de la flamme */
  const Texture *m_tex;
  /* Incr�ment en u pour la coordonn�e de texture */
  float m_utexInc;
    
  /** Position relative de la flamme dans le feu auquel elle appartient */
  Point m_position;
  
private:
  /** Objet OpenGL permettant de d�finir la NURBS */
  GLUnurbsObj *m_nurbs;
};


/**********************************************************************************************************************/
/****************************************** DEFINITION DE LA CLASSE FIXEDFLAME ****************************************/
/**********************************************************************************************************************/

/** La classe FixedFlame est l'objet de plus bas niveau repr�sentant une flamme dont la base est fixe. Par rapport � la
 * classe de base NurbsFlame, elle rajoute la prise en compte des origines des squelettes, notamment pour l'allocation
 * des tableaux de points de contr�les. Elle d�fini �galement quelques m�thodes abstraites pour obtenir des informations
 * sur la localisation de la flamme n�cessaire pour l'�clairage. Tout ceci n'est en effet
 * pas n�cessaire pour la d�finition d'une flamme d�tach�e.
 *
 * @author	Flavien Bridault
 */
class FixedFlame : public NurbsFlame
{
public:
  /** Constructeur de flamme par d�faut.
   * @param nbSkeletons nombre de squelettes. Pour le moment nbSkeletons doit �tre pair en raison de l'affichage.
   * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
   * @param tex Pointeur sur la texture de la flamme.
   */
  FixedFlame(uint nbSkeletons, ushort nbFixedPoints, const Texture* const tex);
  
  virtual ~FixedFlame ();
  
  /** Dessine la m�che de la flamme.
   * @param displayBoxes Affiche ou non le partitionnement de la m�che.
   */
  virtual void drawWick(bool displayBoxes) const = 0;
  
  void draw(bool display, bool displayParticle, bool displayBoxes) const{
    drawWick(displayBoxes);
    drawFlame(display, displayParticle);
  };
    
  /** Dessine une flamme ponctuelle. La diff�rence avec drawLineFlame() est que la texture est translat�e
   * pour rester en face de l'observateur.
   */
  virtual void drawPointFlame() const;
  
  /** Retourne la direction de la base de la flamme vers la derniere particule
   * pour orienter le solide photom�trique.
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

/** La classe RealFlame, par rapport � la classe FixedFlame, ajoute la notion de squelettes de flamme ainsi que les
 * interactions avec un solveur de fluides. Elle est qualifi�e de "Real" en comparaison avec les CloneFlame.
 * Cette classe reste abstraite et est h�rit�e par les classes LineFlame et PointFlame.
 * Elle permet de d�finir une primitive g�om�trique pour une flamme, mais ne permet pas de construire une flamme
 * en tant que source de lumi�re d'une sc�ne. C'est la classe FireSource qui permet de d�finir ceci, en utilisant
 * les classes h�rit�es de RealFlame comme �l�ment de base.<br>
 * Une RealFlame est construite � sa position relative d�finie dans m_position. Lors du dessin, la translation dans
 * le rep�re du feu est donc d�j� effectu�e. C'est la classe FireSource qui s'occupe de placer correctement dans
 * le rep�re du monde. En revanche, la m�che en d�finie en (0,0,0) et il faut donc la translater dans le rep�re du
 * feu � chaque op�ration de dessin.
 *
 * @author	Flavien Bridault
 */
class RealFlame : public FixedFlame
{
public:
  /** Constructeur de flamme. La position de la flamme est d�finie dans le rep�re du solveur.
   * @param nbSkeletons Nombre de squelettes. Pour le moment nbSkeletons doit �tre pair en raison de l'affichage.
   * @param nbFixedPoints Nombre de points fixes, autrement dit les racines des squelettes de la flamme.
   * @param tex Pointeur sur la texture de la flamme.
   * @param s Pointeur vers le solveur.
   */
  RealFlame(uint nbSkeletons, ushort nbFixedPoints, const Texture* const tex, Field3D* const s);
  virtual ~RealFlame ();
  
  /** Fonction appel�e par le solveur de fluides pour ajouter l'�l�vation thermique de la flamme.
   */
  virtual void addForces (int fdf, float innerForce, char perturbate){
    for (vector < LeadSkeleton * >::iterator skeletonsIterator = m_leadSkeletons.begin ();
	 skeletonsIterator != m_leadSkeletons.end (); skeletonsIterator++)
      (*skeletonsIterator)->addForces (fdf, innerForce, perturbate);
  }
  
  /** Affectation de la dur�e de vie des squelettes guides.
   * @param value Dur�e de vie en it�rations.
   */
  virtual void setLeadLifeSpan(uint value) {
    for (vector < LeadSkeleton * >::iterator skeletonsIterator = m_leadSkeletons.begin ();
	 skeletonsIterator != m_leadSkeletons.end (); skeletonsIterator++)
      (*skeletonsIterator)->setLifeSpan(value);
  };
  
  /** Affectation de la dur�e de vie des squelettes p�riph�riques.
   * @param value Dur�e de vie en it�rations.
   */
  virtual void setPeriLifeSpan(uint value) { 
    for (uint i = 0; i < m_nbSkeletons; i++)
      m_periSkeletons[i]->setLifeSpan(value); 
  };
  
  /** Affiche les particules de tous les squelettes composants la flamme. */
  void drawParticles() const
  {
    uint i;
    /* D�placement et d�termination du maximum */
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
  
  /** Fonction charg�e de remplir les coordon�es de texture dans la direction v. Elles sont
   * en effet d�pendantes du nombre de particules dans les squelettes et il est donc n�cessaire
   * de les recalculer � chaque construction de la NURBS.
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
  /** Tableau contenant les pointeurs vers les squelettes p�riph�riques. */
  PeriSkeleton **m_periSkeletons;
  /** Pointeur sur le solveur de fluides */
  Field3D *m_solver;  
  
  float *m_distances;
  /** Tableau temporaire utilis� pour classer les indices des distances entre points de contr�le
   * lors de l'ajout de points de contr�le suppl�mentaires dans la NURBS.  Allou� une seule fois 
   * en d�but de programme � la taille maximale pour des raisons �videntes d'optimisation du temps 
   * d'ex�cution.
   */
  int *m_maxDistancesIndexes;
  
  u_char m_lodSkel;
  /** Indique qu'un changement de niveau de d�tail sur les squelettes a �t� demand�. */
  bool m_lodSkelChanged;
};

#endif

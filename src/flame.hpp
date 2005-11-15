#if !defined(FLAME_H)
#define FLAME_H

class Flame;

#ifndef CALLBACK
#define CALLBACK
#endif

#include "header.h"

#include "periSkeleton.hpp"
#include "solver.hpp"
#include "texture.hpp"
#include "eyeball.hpp"
#include "object.hpp"
#include "CgShader.hpp"

class PeriSkeleton;
class Solver;
class Object;
class CScene;

/** La classe Flame fournit un objet simple englobant les squelettes de flamme.
 * La fonction la plus importante et la plus complexe reste la fonction de dessin de la flamme dessine().
 * Pour le moment la classe est assez fig�e et ne permet
 * que la d�finition d'une flamme de bougie.
 *
 * @author	Flavien Bridault
 */
class Flame
{
public:
  /** Constructeur de flamme simple.
   * @param s pointeur sur le solveur de fluides
   * @param nb nombre de squelettes. Pour le moment nb doit �tre pair en raison de l'affichage. A VERIFIER
   * @param centre position du centre de la flamme.
   * @param pos position absolue du centre de la flamme dans l'espace
   * @param filename nom du fichier OBJ contenant le luminaire
   * @param pointeur sur la sc�ne
   */
  Flame (Solver * s, int nb, CPoint * centre, CPoint * pos, const char *filename, CScene *scene);
  Flame (Solver * s, CPoint * centre, CPoint * pos, const char *filename, CScene *scene);
  virtual ~Flame ();
  
    /** Fonction appel�e par la fonction de dessin OpenGL. Elle commence par d�placer les particules 
   * des squelettes p�riph�riques. Ensuite, elle d�finit la matrice de points de contr�le de la NURBS,
   * des vecteurs de noeuds.
   */
  virtual void build() = 0;
    
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture
   */
  virtual void drawFlame(bool displayParticle) = 0;

  /** Dessine la m�che de la flamme */
  virtual void drawWick() = 0;
  
  /** Dessine la m�che de la flamme */
  void drawLuminary() const
  {
    glPushMatrix();
    glTranslatef (m_position.x, m_position.y, m_position.z);
    glCallList(m_luminaryDL);
    glPopMatrix();
  };
  
  void drawLuminary(CgBasicVertexShader *shader) const
  {
    glPushMatrix();
    glTranslatef (m_position.x, m_position.y, m_position.z);
    shader->setModelViewProjectionMatrix();
    glCallList(m_luminaryDL);
    glPopMatrix();
  }
  
  /** Dessine la flamme et sa m�che */
  void draw(bool displayParticle){
    drawWick();
    drawFlame(displayParticle);
  };

  /** Fonction appel�e par le solveur de fluides pour ajouter l'�l�vation thermique de la flamme.
   */
  virtual void add_forces (bool perturbate) = 0;
  
  /** Fonction appel�e par la fonction de dessin OpenGL. Elle fournit l'�clairage d� � la flamme, 
   * au reste de la sc�ne via les particules du squelette guide. Elle s'occupe �galement de d�placer
   * les particules du squelette guide.
   */
  virtual void eclaire () = 0;
  
  CPoint *getPosition ()
  {
    return &m_position;
  };
  
  void setPosition (CPoint& position)
  {
    m_position=position;
  };

  void moveTo(CPoint& position);
  
  virtual void toggleSmoothShading ();
  
  virtual void draw_shadowVolumes () = 0;
  virtual void cast_shadows_double_multiple () = 0;
  virtual void cast_shadows_double () = 0;
  virtual void switch_on_lights ();

  /** Fonction permettant de r�cup�rer l'orientation principale de la flamme
   * pour orienter le solide photom�trique.
   */
  virtual CVector get_main_direction()=0;


protected:
  /** Ajoute une force p�riodique dans le solveur, pour donner une petite fluctuation sur la flamme */
  virtual void perturbate_forces () = 0;

  /** Fonction simplifiant l'affectation d'un point de contr�le.
   * @param u indice u du point de contr�le
   * @param v indice v du point de contr�le
   * @param pt position du point de contr�le dans l'espace
   */
  void setCtrlPoint (int u, int v, const CPoint * const pt)
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
  void setCtrlPoint (int u, int v, const CPoint * const pt, double w)
  {
    m_ctrlPoints[(u * m_size + v) * 3] = pt->x;
    m_ctrlPoints[(u * m_size + v) * 3 + 1] = pt->y;
    m_ctrlPoints[(u * m_size + v) * 3 + 2] = pt->z;
    //    m_ctrlPoints[(u*m_size+v)*3+3] = w;
  }
  
  virtual void switch_off_lights ();
  
  virtual void enable_only_ambient_light (int i);
  virtual void reset_diffuse_light (int i);
  
  virtual void draw_shadowVolume (int i) = 0;
  virtual void draw_shadowVolume2 (int i) = 0;
  
  static void CALLBACK nurbsError (GLenum errorCode);
  
  /** Nombre de squelettes de la flamme. */
  int m_nbSkeletons;
  /** Position en indices dans la grille de voxels du solveur. */
  int m_x, m_y, m_z;
  /** Position de la flamme dans la sc�ne. */
  CPoint m_position, m_startPosition;
  
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
  
  CObject *m_luminary;
  GLuint m_luminaryDL;
  
  int m_size;
  bool m_toggle;
  
  GLdouble m_lightPositions[8][4];
  short m_nbLights;
  
  int m_perturbateCount;

  short m_lifeSpanAtBirth;
  /** Nombre de points fixes pour chaque direction v = origine du squelette p�riph�rique + sommet du guide */
  short m_nbFixedPoints;

  CScene *m_scene;
};

#endif

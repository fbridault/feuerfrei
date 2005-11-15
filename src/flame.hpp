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
 * Pour le moment la classe est assez figée et ne permet
 * que la définition d'une flamme de bougie.
 *
 * @author	Flavien Bridault
 */
class Flame
{
public:
  /** Constructeur de flamme simple.
   * @param s pointeur sur le solveur de fluides
   * @param nb nombre de squelettes. Pour le moment nb doit être pair en raison de l'affichage. A VERIFIER
   * @param centre position du centre de la flamme.
   * @param pos position absolue du centre de la flamme dans l'espace
   * @param filename nom du fichier OBJ contenant le luminaire
   * @param pointeur sur la scène
   */
  Flame (Solver * s, int nb, CPoint * centre, CPoint * pos, const char *filename, CScene *scene);
  Flame (Solver * s, CPoint * centre, CPoint * pos, const char *filename, CScene *scene);
  virtual ~Flame ();
  
    /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
   * des vecteurs de noeuds.
   */
  virtual void build() = 0;
    
  /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
   * build() avec le placage de texture
   */
  virtual void drawFlame(bool displayParticle) = 0;

  /** Dessine la mèche de la flamme */
  virtual void drawWick() = 0;
  
  /** Dessine la mèche de la flamme */
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
  
  /** Dessine la flamme et sa mèche */
  void draw(bool displayParticle){
    drawWick();
    drawFlame(displayParticle);
  };

  /** Fonction appelée par le solveur de fluides pour ajouter l'élévation thermique de la flamme.
   */
  virtual void add_forces (bool perturbate) = 0;
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle fournit l'éclairage dû à la flamme, 
   * au reste de la scène via les particules du squelette guide. Elle s'occupe également de déplacer
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

  /** Fonction permettant de récupérer l'orientation principale de la flamme
   * pour orienter le solide photométrique.
   */
  virtual CVector get_main_direction()=0;


protected:
  /** Ajoute une force périodique dans le solveur, pour donner une petite fluctuation sur la flamme */
  virtual void perturbate_forces () = 0;

  /** Fonction simplifiant l'affectation d'un point de contrôle.
   * @param u indice u du point de contrôle
   * @param v indice v du point de contrôle
   * @param pt position du point de contrôle dans l'espace
   */
  void setCtrlPoint (int u, int v, const CPoint * const pt)
  {
    m_ctrlPoints[(u * m_size + v) * 3] = pt->x;
    m_ctrlPoints[(u * m_size + v) * 3 + 1] = pt->y;
    m_ctrlPoints[(u * m_size + v) * 3 + 2] = pt->z;
    //    m_ctrlPoints[(u*m_size+v)*4+3] = 1.0;
  }

  /** Fonction simplifiant l'affectation d'un point de contrôle. 
   * @param u indice u du point de contrôle
   * @param v indice v du point de contrôle
   * @param pt position du point de contrôle dans l'espace
   * @param w coordonnée homogène du point de contrôle, équivalente au poids du point de contrôle
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
  /** Position de la flamme dans la scène. */
  CPoint m_position, m_startPosition;
  
  /** Ordre de la NURBS en u (égal au degré en u + 1). */
  int m_uorder;
  /** Ordre de la NURBS en v (égal au degré en v + 1). */
  int m_vorder;
  /** Tableau contenant les pointeurs vers les squelettes périphériques. */
  PeriSkeleton **m_skeletons;
  /** Matrice de points de contrôle */
  GLfloat *m_ctrlPoints;
  /** Vecteur de noeuds en u */
  GLfloat *m_uknots;
  /** Vecteur de noeuds en v */
  GLfloat *m_vknots;
  /** Tableau temporaire utilisé pour stocker les distances entre chaque point de contrôle d'un
   * squelette. Alloué une seule fois en début de programme à la taille maximale pour des raisons
   * évidentes d'optimisation du temps d'exécution.
   */
  int m_uknotsCount, m_vknotsCount;
  int m_maxParticles;
  
  double *m_distances;
  /** Tableau temporaire utilisé pour classer les indices des distances entre points de contrôle
   * lors de l'ajout de points de contrôle supplémentaires dans la NURBS.  Alloué une seule fois 
   * en début de programme à la taille maximale pour des raisons évidentes d'optimisation du temps 
   * d'exécution.
   */
  int *m_maxDistancesIndexes;
  
  /** Objet OpenGL permettant de définir la NURBS */
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
  /** Nombre de points fixes pour chaque direction v = origine du squelette périphérique + sommet du guide */
  short m_nbFixedPoints;

  CScene *m_scene;
};

#endif

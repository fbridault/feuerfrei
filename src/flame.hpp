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
#include "scene.hpp"

class PeriSkeleton;
class Solver;
class Object;

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
   * @param rayon rayon de la flamme.
   */
  Flame (Solver * s, int nb, CPoint * centre, CPoint * pos, const char *filename, CScene *scene);
  Flame (Solver * s, CPoint * centre, CPoint * pos, const char *filename, CScene *scene);
  virtual ~Flame ();
  
    /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
   * des vecteurs de noeuds.
   */
  virtual void build() = 0;
    
  /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture
   */
  virtual void drawFlame(bool displayParticle) = 0;

  /** Dessine la m�che de la flamme */
  virtual void drawWick() = 0;

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
    return &position;
  };
  
  CObject *getLuminaire ()
  {
    return luminaire;
  };
  
  virtual void toggleSmoothShading ();

  virtual void draw_shadowVolumes (GLint objects_list_wsv) = 0;
  virtual void cast_shadows_double_multiple (GLint objects_list_wsv) = 0;
  virtual void cast_shadows_double (GLint objects_list_wsv) = 0;
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
    ctrlpoints[(u * size + v) * 3] = pt->getX ();
    ctrlpoints[(u * size + v) * 3 + 1] = pt->getY ();
    ctrlpoints[(u * size + v) * 3 + 2] = pt->getZ ();
    //    ctrlpoints[(u*size+v)*4+3] = 1.0;
  }

  /** Fonction simplifiant l'affectation d'un point de contrôle. 
   * @param u indice u du point de contrôle
   * @param v indice v du point de contrôle
   * @param pt position du point de contrôle dans l'espace
   * @param w coordonnée homogène du point de contrôle, équivalente au poids du point de contrôle
   */
  void setCtrlPoint (int u, int v, const CPoint * const pt, double w)
  {
    ctrlpoints[(u * size + v) * 3] = pt->getX ();
    ctrlpoints[(u * size + v) * 3 + 1] = pt->getY ();
    ctrlpoints[(u * size + v) * 3 + 2] = pt->getZ ();
    //    ctrlpoints[(u*size+v)*3+3] = w;
  }
  
  virtual void switch_off_lights ();
  
  virtual void enable_only_ambient_light (int i);
  virtual void reset_diffuse_light (int i);
  
  virtual void draw_shadowVolume (GLint objects_list_wsv, int i) = 0;
  virtual void draw_shadowVolume2 (GLint objects_list_wsv, int i) = 0;
  
  static void CALLBACK nurbsError (GLenum errorCode);
  
  /** Nombre de squelettes de la flamme. */
  int nb_squelettes;
  /** Position en indices dans la grille de voxels du solveur. */
  int x, y, z;
  /** Position de la flamme dans la scène. */
  CPoint position;
  
  /** Ordre de la NURBS en u (égal au degré en u + 1). */
  int uorder;
  /** Ordre de la NURBS en v (égal au degré en v + 1). */
  int vorder;
  /** Tableau contenant les pointeurs vers les squelettes périphériques. */
  PeriSkeleton **squelettes;
  /** Matrice de points de contrôle */
  GLfloat *ctrlpoints;
  /** Vecteur de noeuds en u */
  GLfloat *uknots;
  /** Vecteur de noeuds en v */
  GLfloat *vknots;
  /** Tableau temporaire utilisé pour stocker les distances entre chaque point de contrôle d'un
   * squelette. Alloué une seule fois en début de programme à la taille maximale pour des raisons
   * évidentes d'optimisation du temps d'exécution.
   */
  int uknotsCount, vknotsCount;
  int max_particles;
  
  float *distances;
  /** Tableau temporaire utilisé pour classer les indices des distances entre points de contrôle
   * lors de l'ajout de points de contrôle supplémentaires dans la NURBS.  Alloué une seule fois 
   * en début de programme à la taille maximale pour des raisons évidentes d'optimisation du temps 
   * d'exécution.
   */
  int *indices_distances_max;
  
  /** Objet OpenGL permettant de définir la NURBS */
  GLUnurbsObj *nurbs;
  
  /** Pointeur sur le solveur de fluides */
  Solver *solveur;
  
  CObject *luminaire;
  
  int size;
  bool toggle;
  
  GLfloat lightPositions[8][4];
  short nb_lights;
  
  int perturbate_count;

  short m_lifeSpanAtBirth;

  CScene *sc;
};

#endif

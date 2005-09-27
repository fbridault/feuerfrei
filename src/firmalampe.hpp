#if !defined(FIRMALAMPE_H)
#define FIRMALAMPE_H

class Firmalampe;

#include "flame.hpp"
#include "CgSVShader.hpp"
#include "leadSkeleton.hpp"
#include "wick.hpp"

class Flame;
class CgSVShader;

/** La classe Flamme fournit un objet simple englobant les squelettes de flamme.
 * La fonction la plus importante et la plus complexe reste la fonction de dessin de la flamme dessine().
 * Pour le moment la classe est assez fig�e et ne permet
 * que la d�finition d'une flamme de bougie.
 *
 * @author	Flavien Bridault
 */
class Firmalampe : public Flame
{
public:
  /** Constructeur de flamme simple.
   * @param s pointeur sur le solveur de fluides
   * @param nb Nombre de squelettes guides.
	 * @param centre Position du centre de la flamme.
   * @param pos position absolue du centre de la flamme dans l'espace
   * @param shader pointeur sur le shader qui g�n�re les shadow volumes
   */
  Firmalampe(Solver *s, int nb, CPoint *centre, CPoint *pos, CgSVShader *shader, char *meche_name, const char *filename, CScene *scene);
  ~Firmalampe();
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
   * des vecteurs de noeuds.
   */
  void build();
    
  /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS d�finie par la fonction
   * build() avec le placage de texture
   */
  void drawFlame(bool displayParticle);

  /** Dessine la m�che de la flamme */
  void drawWick();

  /** Fonction appel�e par le solveur de fluides pour ajouter l'�l�vation thermique de la flamme.
   */
  void add_forces(bool perturbate);

  /** Fonction appel�e par la fonction de dessin OpenGL. Elle fournit l'�clairage d� � la flamme, 
   * au reste de la sc�ne via les particules du squelette guide. Elle s'occupe �galement de d�placer
   * les particules du squelette guide.
   */
  void eclaire();

  void cast_shadows_double_multiple(GLint objects_list_wsv);
  void cast_shadows_double(GLint objects_list_wsv);
  void draw_shadowVolumes(GLint objects_list_wsv);

  /** Retroune la direction de la base de la flamme vers la derniere particule
   * pour orienter le solide photom�trique.
   */
  CVector get_main_direction();
private:
  /** Ajoute une force p�riodique dans le solveur, pour donner une petite fluctuation sur la flamme */
  void perturbate_forces();

  void draw_shadowVolume(GLint objects_list_wsv, int i);
  void draw_shadowVolume2(GLint objects_list_wsv, int i);

  /** M�che de la flamme */
  Wick meche;

  /** Pointeur vers les squelettes guide. */
  LeadSkeleton **guides;
  /** Nombres de squelettes guides */
  int nbLeadSkeletons;
  
  /** Identifiant de la texture. */
  Texture tex;

  /** Nombre de points fixes pour chaque direction v = origine du squelette p�riph�rique + sommet du guide */
  const static int nb_pts_fixes = 3;

  CgSVShader *cgShader;
};

#endif

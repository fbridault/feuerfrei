#if !defined(BOUGIE_H)
#define BOUGIE_H

class Bougie;

#include "flame.hpp"
#include "leadSkeleton.hpp"
#include "CgSVShader.hpp"
#include "CgSPVertexShader.hpp"
#include "CgBougieShaders.hpp"
	
class Flame;
class LeadSkeleton;
class CgSVShader;

/** La classe Flamme fournit un objet simple englobant les squelettes de flamme.
 * La fonction la plus importante et la plus complexe reste la fonction de dessin de la flamme dessine().
 * Pour le moment la classe est assez figée et ne permet
 * que la définition d'une flamme de bougie.
 *
 * @author	Flavien Bridault
 */
class Bougie : public Flame
{
public:
  /** Constructeur de flamme simple.
   * @param s pointeur sur le solveur de fluides
   * @param nb nombre de squelettes. Pour le moment nb doit être pair en raison de l'affichage. A VERIFIER
   * @param centre position du centre de la flamme, relativement à la grille
   * @param pos position absolue du centre de la flamme dans l'espace
   * @param rayon rayon de la flamme.
   * @param shader pointeur sur le shader qui génère les shadow volumes
   */
  Bougie(Solver *s, int nb, CPoint *centre, CPoint *pos, double rayon, 
	 CgSVShader *shader, const char *filename, CScene *scene, CGcontext *context);
  virtual ~Bougie();
  
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

  /** Fonction appelée par le solveur de fluides pour ajouter l'élévation thermique de la flamme.
   */
  void add_forces(bool perturbate);

  /** Fonction appelée par la fonction de dessin OpenGL. Elle fournit l'éclairage dû à la flamme, 
   * au reste de la scène via les particules du squelette guide. Elle s'occupe également de déplacer
   * les particules du squelette guide.
   */
  void eclaire();

  void cast_shadows_double_multiple(GLint objects_list_wsv);
  void cast_shadows_double(GLint objects_list_wsv);
  void draw_shadowVolumes(GLint objects_list_wsv);

  /** Retourne la direction de la base de la flamme vers la derniere particule
   * pour orienter le solide photomÃ©trique.
   */
  CVector get_main_direction(){
    return(*(guide->getElt(0)));
  };
  
private:
  /** Ajoute une force périodique dans le solveur, pour donner une petite fluctuation sur la flamme */
  void perturbate_forces();
  
  void draw_shadowVolume(GLint objects_list_wsv, int i);
  void draw_shadowVolume2(GLint objects_list_wsv, int i);

  /** Pointeur vers le squelette guide. */
  LeadSkeleton *guide;

  /** Nombre de points fixes pour chaque direction v = origine du guide + origine du squelette pÃ©riphÃ©rique + sommet du guide */
  const static int nb_pts_fixes = 3;
    
  /** Identifiant de la texture. */
  Texture tex;
  
  CgSVShader *cgShader;
//   CgBougieVertexShader cgBougieVertexShader;
//   CgBougieFragmentShader cgBougieFragmentShader;

  float gaussSigma;
};

#endif

#if !defined(BOUGIE_H)
#define BOUGIE_H

class Bougie;

#include "flame.hpp"
#include "leadSkeleton.hpp"
#include "../shaders/CgSVShader.hpp"
#include "../shaders/CgBougieShaders.hpp"

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
   * @param nb nombre de squelettes. Pour le moment nb doit Ãªtre pair en raison de l'affichage. A VERIFIER
   * @param centre position du centre de la flamme, relativement Ã  la grille
   * @param pos position absolue du centre de la flamme dans l'espace
   * @param rayon rayon de la flamme.
   * @param shader pointeur sur le shader qui génÃ¨re les shadow volumes
   */
  Bougie(Solver *s, int nb, CPoint& posRel, double rayon, double innerForce,
	 CgSVShader *shader, const char *filename, CScene *scene, CGcontext *context, int index);
  virtual ~Bougie();
  
  /** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
   * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrÃ´le de la NURBS,
   * des vecteurs de noeuds.
   */
  void build();
    
  /** Fonction appelée par la fonction de dessin OpenGL. Elle dessine la NURBS définie par la fonction
   * build() avec le placage de texture
   */
  void drawFlame(bool displayParticle);

  /** Dessine la mèche de la flamme */
  void drawWick();

  /** Fonction appelée par le solveur de fluides pour ajouter l'élévation thermique de la flamme.
   */
  void add_forces(char perturbate);

  /** Fonction appelée par la fonction de dessin OpenGL. Elle fournit l'éclairage dÃ» Ã  la flamme, 
   * au reste de la scÃ¨ne via les particules du squelette guide. Elle s'occupe également de déplacer
   * les particules du squelette guide.
   */
  void eclaire();

  void draw_shadowVolumes();

  /** Retourne la direction de la base de la flamme vers la derniere particule
   * pour orienter le solide photométrique.
   */
  CVector get_main_direction(){
    return(*(m_lead->getMiddleParticle()));
  };
  
private:
  
  void draw_shadowVolume(int i);
  void draw_shadowVolume2(int i);

  /** Pointeur vers le squelette guide. */
  LeadSkeleton *m_lead;
  
  /** Identifiant de la texture. */
  Texture m_tex;
  
  CgSVShader *m_cgShader;
//   CgBougieVertexShader cgBougieVertexShader;
//   CgBougieFragmentShader cgBougieFragmentShader;
};

#endif

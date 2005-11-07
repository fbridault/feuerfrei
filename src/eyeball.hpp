#if !defined(EYEBALL_H)
#define EYEBALL_H

#include "header.h"

/** Classe définissant un "eyeball" qui permet de tourner et de zoomer un point fixe dans
 * l'espace. Cette classe nécessite la librairie SDL ainsi que le module trackball.c
 * pour les opérations sur les quaternions. Elle fournit les fonctions pour le déplacement
 * de la souris void mouseButton(SDL_MouseButtonEvent *event) et du mouvement void mouseMotion(SDL_MouseMotionEvent *event).
 * Pour l'utiliser, il suffit de déclarer un objet de type Eyeball. Dans la fonction de dessin de la scène, il
 * ne reste alors plus qu'à appeler la fonction publique recalcModelView() avant de tracer l'objet à visualiser.
 * Cette classe peut donc être utilisée ainsi avec une application SDL quelconque.
 * A noter que tout le code est très fortement inspiré du source du soft de radiosité hiérarchique de François
 * Rousselle. Merci François ;-)
 *
 * @author	Flavien Bridault
 */
class Eyeball
{
public:
  /** Constructeur du eyeball.
  * param w largeur de la fenêtre de visualisation
  * param h hauteur de la fenêtre de visualisation
  * param clipping_value Distance de clipping
  */
  Eyeball(int w, int h, double clipping_value);
  virtual ~Eyeball() {};
  /** Calcul de la matrice de transformation. A appeler avant de tracer l'objet à visualiser dans la scène.*/
  void recalcModelView(void);

  /** Récupération de la coordonnée x du centre du eyeball. */
  GLdouble getEyex(){return eyex;};
  /** Récupération de la coordonnée y du centre du eyeball. */
  GLdouble getEyey(){return eyey;};
  /** Récupération de la coordonnée z du centre du eyeball. */
  GLdouble getEyez(){return eyez;};

  /** Récupération de la coordonnée x du centre du eyeball. */
  GLdouble getCenterx(){return centerx;};
  /** Récupération de la coordonnée y du centre du eyeball. */
  GLdouble getCentery(){return centery;};
  /** Récupération de la coordonnée z du centre du eyeball. */
  GLdouble getCenterz(){return centerz;};

  void OnMouseClick (wxMouseEvent& event);
  void OnMouseMotion (wxMouseEvent& event);
  
private:
  
  void recalculer_matrice_initiale(void);
  void deplacement(int bouton, int etat, int x, int y);
  
  /* Variables pour la visu */
  GLdouble eyex, eyey, eyez;
  
  GLdouble centerx, centery, centerz;
  GLdouble upx, upy, upz;
  GLdouble vectviseex,vectviseey,vectviseez;
  int bouton_active;
  GLdouble ouverture;
  float minx,miny,minz,maxx,maxy,maxz,taillex,zoomstep;
  long beginx, beginy;
  int depl,depl_rapide;
  int W,H;
  float curquat[4];
  float lastquat[4];

  double clipping_value;
};

#endif

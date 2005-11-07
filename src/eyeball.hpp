#if !defined(EYEBALL_H)
#define EYEBALL_H

#include "header.h"

/** Classe d�finissant un "eyeball" qui permet de tourner et de zoomer un point fixe dans
 * l'espace. Cette classe n�cessite la librairie SDL ainsi que le module trackball.c
 * pour les op�rations sur les quaternions. Elle fournit les fonctions pour le d�placement
 * de la souris void mouseButton(SDL_MouseButtonEvent *event) et du mouvement void mouseMotion(SDL_MouseMotionEvent *event).
 * Pour l'utiliser, il suffit de d�clarer un objet de type Eyeball. Dans la fonction de dessin de la sc�ne, il
 * ne reste alors plus qu'� appeler la fonction publique recalcModelView() avant de tracer l'objet � visualiser.
 * Cette classe peut donc �tre utilis�e ainsi avec une application SDL quelconque.
 * A noter que tout le code est tr�s fortement inspir� du source du soft de radiosit� hi�rarchique de Fran�ois
 * Rousselle. Merci Fran�ois ;-)
 *
 * @author	Flavien Bridault
 */
class Eyeball
{
public:
  /** Constructeur du eyeball.
  * param w largeur de la fen�tre de visualisation
  * param h hauteur de la fen�tre de visualisation
  * param clipping_value Distance de clipping
  */
  Eyeball(int w, int h, double clipping_value);
  virtual ~Eyeball() {};
  /** Calcul de la matrice de transformation. A appeler avant de tracer l'objet � visualiser dans la sc�ne.*/
  void recalcModelView(void);

  /** R�cup�ration de la coordonn�e x du centre du eyeball. */
  GLdouble getEyex(){return eyex;};
  /** R�cup�ration de la coordonn�e y du centre du eyeball. */
  GLdouble getEyey(){return eyey;};
  /** R�cup�ration de la coordonn�e z du centre du eyeball. */
  GLdouble getEyez(){return eyez;};

  /** R�cup�ration de la coordonn�e x du centre du eyeball. */
  GLdouble getCenterx(){return centerx;};
  /** R�cup�ration de la coordonn�e y du centre du eyeball. */
  GLdouble getCentery(){return centery;};
  /** R�cup�ration de la coordonn�e z du centre du eyeball. */
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

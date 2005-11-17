#if !defined(EYEBALL_H)
#define EYEBALL_H

#define COEFFICIENT_ROTATION_QUATERNION 1.0

#include "header.h"

class Quaternion
{
public:
  double x, y, z, w;
  
  Quaternion()
  {
    x = y = z = w = 0.0;
  };
  
  Quaternion(const Quaternion& q)
  {
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;      
  };
  
  ~Quaternion(){};
  
  double length()
  {
    return sqrt(x * x + y * y + z * z + w * w);
  };
  
  void normalize()
  {
    double L = length();
    
    x /= L;
    y /= L;
    z /= L;
    w /= L;
  };

  void conjugate()
  {
    x = -x;
    y = -y;
    z = -z;
  };
  
  Quaternion operator*(const Quaternion& B) const
  {
    Quaternion C;
    
    C.x = w*B.x + x*B.w + y*B.z - z*B.y;
    C.y = w*B.y - x*B.z + y*B.w + z*B.x;
    C.z = w*B.z + x*B.y - y*B.x + z*B.w;
    C.w = w*B.w - x*B.x - y*B.y - z*B.z;
    
    return C;
  };
};


/** Classe définissant un "eyeball" qui permet de tourner et de zoomer un point fixe dans
 * l'espace. Elle fournit les fonctions pour le déplacement
 * de la souris void mouseButton(SDL_MouseButtonEvent *event) et du mouvement void mouseMotion(SDL_MouseMotionEvent *event).
 * Pour l'utiliser, il suffit de déclarer un objet de type Eyeball. Dans la fonction de dessin de la scène, il
 * ne reste alors plus qu'à appeler la fonction publique recalcModelView() avant de tracer l'objet à visualiser.
 * Cette classe peut donc être utilisée ainsi avec une application SDL quelconque.
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

  // void addCenterX(double value){ centerx+=value; eyex+=value; 
//       recalculer_matrice_initiale (); };
//   void addCenterZ(double value){ centerz+=value; eyez+=value;
//       recalculer_matrice_initiale ();};
  void computeView(void);

  void setView(void){  
    gluLookAt(m_position.x, m_position.y, m_position.z,
	      m_view.x, m_view.y, m_view.z, m_up.x, m_up.y, m_up.z);
  };  

  void rotate(double angle, double x, double y, double z);
  
  void OnMouseClick (wxMouseEvent& event);
  void OnMouseMotion (wxMouseEvent& event);
  
private:
  /* Variables pour la visu */
  CVector m_up, m_view;
  CPoint m_position;
    
  int m_buttonPressed;
  GLdouble m_ouverture;
  int m_beginMouseX, m_beginMouseY;
  double m_clipping_value;
  double m_currentRotationX;  
  /** Angle maximal en radians */
  double maxAngle;
  bool m_move;
  double m_mouseSensitivity;
  int m_width, m_height;  
};

#endif

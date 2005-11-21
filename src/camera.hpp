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


/** Classe définissant un "camera" qui permet de tourner et de zoomer un point fixe dans
 * l'espace. Elle fournit les fonctions pour le déplacement
 * de la souris void mouseButton(SDL_MouseButtonEvent *event) et du mouvement void mouseMotion(SDL_MouseMotionEvent *event).
 * Pour l'utiliser, il suffit de déclarer un objet de type Camera. Dans la fonction de dessin de la scène, il
 * ne reste alors plus qu'à appeler la fonction publique recalcModelView() avant de tracer l'objet à visualiser.
 * Cette classe peut donc être utilisée ainsi avec une application SDL quelconque.
 *
 * @author	Flavien Bridault
 */
class Camera
{
public:
  /** Constructeur du camera.
  * @param w largeur de la fenêtre de visualisation
  * @param h hauteur de la fenêtre de visualisation
  * @param clipping_value Distance de clipping
  */
  Camera(int w, int h, double clipping_value);
  virtual ~Camera() {};

  // void addCenterX(double value){ centerx+=value; eyex+=value; 
//       recalculer_matrice_initiale (); };
//   void addCenterZ(double value){ centerz+=value; eyez+=value;
//       recalculer_matrice_initiale ();};
  void computeView(double x, double y);

  void setView(void){
    glPopMatrix ();
    glPushMatrix ();
    gluLookAt(0.0,0.0,0.0,
	      m_view.x, m_view.y, m_view.z, m_up.x, m_up.y, m_up.z);
    glTranslatef(m_position.x, m_position.y, m_position.z);
  };  

  void rotate(double angle, double x, double y, double z);
  
  void OnMouseClick (wxMouseEvent& event);
  void OnMouseMotion (wxMouseEvent& event);

  /** Déplacement de la caméra sur les côtés 
   * On construit simplement un vecteur orthogonal au vecteur up et au vecteur de vue
   * et on effectue ensuite une translation suivant ce vecteur
   * @param value valeur de la translation
   */
  void moveOnSides(double value){
    CVector axis = m_view ^ m_up;
    m_position = m_position + (axis * value);
  }; 
  /** Déplacement de la caméra vers l'avant ou vers l'arrière 
   * On effectue ensuite une translation suivant le vecteur de vue
   * @param value valeur de la translation
   */
  void moveOnFrontOrBehind(double value){
    m_position = m_position + (m_view * value);
  };  
  /** Déplacement de la caméra vers l'avant ou vers l'arrière 
   * On effectue ensuite une translation suivant le vecteur de vue
   * @param value valeur de la translation
   */
  void moveUpOrDown(double value){
    m_position = m_position + (m_up * value);
  };
  
private:
  /* Variables pour la visu */
  CPoint m_position;
  CVector m_up, m_view;
  
  int m_buttonPressed;
  GLdouble m_ouverture;
  int m_beginMouseX, m_beginMouseY;
  double m_clipping_value;
  double m_currentRotationX;  
  /** Angle maximal en radians */
  double m_maxAngleX;
  bool m_move;
  double m_mouseSensitivity;
  int m_width, m_height;  
};

#endif

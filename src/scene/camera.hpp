#if !defined(EYEBALL_H)
#define EYEBALL_H

#define COEFFICIENT_ROTATION_QUATERNION 1.0

#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "../vector.hpp"
#include <wx/event.h>

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


class Scene;

/** Classe d�finissant une cam�ra subjective � la premi�re personne, qui permet donc 
 * de tourner, de se d�placer et de zoomer autour d'un point quelconque dans
 * l'espace. Elle fournit les fonctions pour les clics de la souris void OnMouseClick (wxMouseEvent& event) 
 * et du mouvement void OnMouseMotion (wxMouseEvent& event)<br>
 * Pour l'utiliser, il suffit de d�clarer un objet de type Camera. Dans la fonction de dessin de la sc�ne, il
 * ne reste alors plus qu'� appeler la fonction publique setView() avant de tracer l'objet � visualiser.
 * Cette classe peut donc �tre utilis�e ainsi avec une application wxWidgets quelconque.
 *
 * @author	Flavien Bridault
 */
class Camera
{
public:
  /** Constructeur du camera.
  * @param w largeur de la fen�tre de visualisation
  * @param h hauteur de la fen�tre de visualisation
  * @param clipping_value Distance de clipping
  */
  Camera(int w, int h, double clipping_value, Scene* scene);
  virtual ~Camera() {};

  // void addCenterX(double value){ centerx+=value; eyex+=value; 
//       recalculer_matrice_initiale (); };
//   void addCenterZ(double value){ centerz+=value; eyez+=value;
//       recalculer_matrice_initiale ();};
  /** Calcul de la rotation de la cam�ra 
   * @param x position finale de la souris en x
   * @param y position finale de la souris en y
   */
  void computeView(double x, double y);

  /** Placement de la cam�ra. Fonction � appeler au d�but de chaque tour de la
   * boucle de dessin
   */
  void setView(void){
    glPopMatrix ();
    glPushMatrix ();
    gluLookAt(0.0,0.0,0.0, m_view.x, m_view.y, m_view.z, m_up.x, m_up.y, m_up.z);
    glTranslatef(m_position.x, m_position.y, m_position.z);
  };  

  /** Rotation de la cam�ra */
  void rotate(double angle, double x, double y, double z);
  
  void OnMouseClick (wxMouseEvent& event);
  void OnMouseMotion (wxMouseEvent& event);

  /** D�placement de la cam�ra sur les c�t�s 
   * On construit simplement un vecteur orthogonal au vecteur up et au vecteur de vue
   * et on effectue ensuite une translation suivant ce vecteur
   * @param value valeur de la translation
   */
  void moveOnSides(double value){
    Vector axis = m_view ^ m_up;
    m_position = m_position + (axis * value);
  }; 
  /** D�placement de la cam�ra vers l'avant ou vers l'arri�re 
   * On effectue ensuite une translation suivant le vecteur de vue
   * @param value valeur de la translation
   */
  void moveOnFrontOrBehind(double value){
    m_position = m_position + (m_view * value);
    computeFrustrum();
  };  
  /** D�placement de la cam�ra vers l'avant ou vers l'arri�re 
   * On effectue ensuite une translation suivant le vecteur de vue
   * @param value valeur de la translation
   */
  void moveUpOrDown(double value){
    m_position = m_position + (m_up * value);
  };
  
  void computeFrustrum();
  
  double *getFrustum(uint side){ return m_frustum[side]; };
  
private:
  /** Position de la sc�ne. La cam�ra reste toujours centr�e en (0,0,0) */
  Point m_position;
  /** Vecteur vers le haut et direction de vis�e */
  Vector m_up, m_view;
  /** Bouton de la souris actuellement appuy� */
  int m_buttonPressed;
  /** Angle d'ouverture de la cam�ra en degr�s */
  GLdouble m_ouverture;
  /** Variables temporaires pour savoir � partir de quel endroit le glissement de la
   * souris a commenc� 
   */
  int m_beginMouseX, m_beginMouseY;
  /** Valeur de clipping de la cam�ra */
  double m_clipping_value;
  /** Rotation actuelle autour de l'axe X */
  double m_currentRotationX;  
  /** Angle de rotation maximal autour de l'axe X en radians */
  double m_maxAngleX;
  /** Variable temporaire indiquant qu'un mouvement a lieu avec la souris */
  bool m_move;
  /** Sensibilit� de la souris - valeurs conseill�es entre 50 et 1000 */
  double m_mouseSensitivity;
  /** Largeur et hauteur de la vue de la cam�ra */
  int m_width, m_height;
  /** Plans du frustrum */
  double m_frustum[6][4];
  
  Scene *m_scene;
};

#endif

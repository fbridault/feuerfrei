#if !defined(EYEBALL_H)
#define EYEBALL_H

#define COEFFICIENT_ROTATION_QUATERNION 1.0

#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "../common.hpp"
#include "../vector.hpp"
#include <wx/event.h>

class Quaternion
{
public:
  float x, y, z, w;
  
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
  
  float length()
  {
    return sqrt(x * x + y * y + z * z + w * w);
  };
  
  void normalize()
  {
    float L = length();
    
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
  * @param scene Pointeur sur la sc�ne
  */
#ifdef RTFLAMES_BUILD
  Camera(int w, int h, float clipping_value, Scene* const scene);
#else
  Camera(int w, int h, float clipping_value);
#endif
  virtual ~Camera() {};

  // void addCenterX(float value){ centerx+=value; eyex+=value; 
//       recalculer_matrice_initiale (); };
//   void addCenterZ(float value){ centerz+=value; eyez+=value;
//       recalculer_matrice_initiale ();};
  /** Calcul de la rotation de la cam�ra 
   * @param x position finale de la souris en x
   * @param y position finale de la souris en y
   */
  void computeView(float x, float y);

  float computeAngles(float input) const
  {
    float angleW, angleH;
    
    angleW = (input * 180 / PI)/ m_ouverture;
    angleH = (input * 180 / PI)/ (m_ouverture * m_aspect);
    return (angleW * angleH);
  };
  
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
  void rotate(float angle, float x, float y, float z);
  
  void OnMouseClick (wxMouseEvent& event);
  void OnMouseMotion (wxMouseEvent& event);

  /** D�placement de la cam�ra sur les c�t�s 
   * On construit simplement un vecteur orthogonal au vecteur up et au vecteur de vue
   * et on effectue ensuite une translation suivant ce vecteur
   * @param value valeur de la translation
   */
  void moveOnSides(float value){
    Vector axis = m_view ^ m_up;
    m_position = m_position + (axis * value);
    setView();
  }; 
  /** D�placement de la cam�ra vers l'avant ou vers l'arri�re 
   * On effectue ensuite une translation suivant le vecteur de vue
   * @param value valeur de la translation
   */
  void moveOnFrontOrBehind(float value){
    m_position = m_position + (m_view * value);
    setView();
#ifdef RTFLAMES_BUILD
    computeFrustrum();
#endif
  };  
  /** D�placement de la cam�ra vers l'avant ou vers l'arri�re 
   * On effectue ensuite une translation suivant le vecteur de vue
   * @param value valeur de la translation
   */
  void moveUpOrDown(float value){
    m_position = m_position + (m_up * value);
    setView();
  };
  
#ifdef RTFLAMES_BUILD
  void computeFrustrum();
  
  const float* getFrustum(uint side) const { return m_frustum[side]; };
  
  /* R�cup�re les coordonn�es � l'�cran d'un point */
  void getScreenCoordinates(const Point& objPos, Point& screenPos) const;
  /* R�cup�re les coordonn�es d'une sph�re sur l'�cran */
  void getSphereCoordinates(const Point& objPos, float radius, Point& centerScreenPos, Point& periScreenPos ) const;
#endif
  
private:
  /** Position de la sc�ne. La cam�ra reste toujours centr�e en (0,0,0) */
  Point m_position;
  /** Vecteur vers le haut et direction de vis�e */
  Vector m_up, m_view;
  /** Bouton de la souris actuellement appuy� */
  int m_buttonPressed;
  /** Angle d'ouverture de la cam�ra en degr�s */
  float m_ouverture;
  /** Ratio de la largeur sur la hauteur de la projection */
  float m_aspect;
  
  /** Variables temporaires pour savoir � partir de quel endroit le glissement de la
   * souris a commenc� 
   */
  int m_beginMouseX, m_beginMouseY;
  /** Valeur de clipping de la cam�ra */
  float m_clipping_value;
  /** Rotation actuelle autour de l'axe X */
  float m_currentRotationX;  
  /** Angle de rotation maximal autour de l'axe X en radians */
  float m_maxAngleX;
  /** Variable temporaire indiquant qu'un mouvement a lieu avec la souris */
  bool m_move;
  /** Sensibilit� de la souris - valeurs conseill�es entre 50 et 1000 */
  float m_mouseSensitivity;
  /** Viewport */
  int m_viewPort[4];
#ifdef RTFLAMES_BUILD
  /** Plans du frustrum */
  float m_frustum[6][4];
  /** Matrice de projection */
  double m_projMatrix[16];
  /** Matrice de transformation */
  double m_modlMatrix[16];
  Scene *m_scene;
#endif
};

#endif

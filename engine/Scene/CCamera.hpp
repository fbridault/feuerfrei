#if !defined(CAMERA_H)
#define CAMERA_H

//#define COEFFICIENT_ROTATION_QUATERNION 1.0

#include <math.h>
#include "../Common.hpp"
#include "../Maths/CQuaternion.hpp"
#include "../Maths/CVector.hpp"

class CScene;

#include "../Utility/ISingleton.hpp"
#include "../Utility/Numerable.hpp"

struct _NMouseButtonState
{
	enum EValue
	{
		eButtonUp,
		eButtonDown,

		_NbValues
	};
};

DeclareNumerable(_NMouseButtonState, NMouseButtonState);

struct _NMouseButton
{
	enum EValue
	{
		eButtonNone,
		eButtonLeft,
		eButtonMiddle,
		eButtonRight,

		_NbValues
	};
};

DeclareNumerable(_NMouseButton, NMouseButton);

/** Classe définissant une caméra subjective à la première personne, qui permet donc
 * de tourner, de se déplacer et de zoomer autour d'un point quelconque dans
 * l'espace. Elle fournit les fonctions pour les clics de la souris void OnMouseClick (wxMouseEvent& event)
 * et du mouvement void OnMouseMotion (wxMouseEvent& event)<br>
 * Pour l'utiliser, il suffit de déclarer un objet de type CCamera. Dans la fonction de dessin de la scène, il
 * ne reste alors plus qu'à appeler la fonction publique setView() avant de tracer l'objet à visualiser.
 * Cette classe peut donc être utilisée ainsi avec une application wxWidgets quelconque.
 *
 * @author	Flavien Bridault
 */
class CCamera : public ISingleton<CCamera>
{
	friend class ISingleton<CCamera>;

private:
	/** Constructeur du camera.
	* @param w largeur de la fenêtre de visualisation
	* @param h hauteur de la fenêtre de visualisation
	* @param clipping_value Distance de clipping
	* @param scene Pointeur sur la scène
	*/
	CCamera();
	virtual ~CCamera() {};

public:
	void init(int w, int h, float clipping_value, CScene* const scene);
	// void addCenterX(float value){ centerx+=value; eyex+=value;
//       recalculer_matrice_initiale (); };
//   void addCenterZ(float value){ centerz+=value; eyez+=value;
//       recalculer_matrice_initiale ();};
	/** Calcul de la rotation de la caméra
	 * @param x position finale de la souris en x
	 * @param y position finale de la souris en y
	 */
	void computeView(float x, float y);

	float computeAngles(float input) const
	{
		float angleW, angleH;

		angleW = (input * 180 / M_PI)/ m_ouverture;
		angleH = (input * 180 / M_PI)/ (m_ouverture * m_aspect);
		return (angleW * angleH);
	};

	/** Placement de la caméra. Fonction à appeler au début de la boucle de dessin.
	 */
	void setView(void)
	{
		glPopMatrix ();
		glPushMatrix ();
		gluLookAt(0.0,0.0,0.0, m_view.x, m_view.y, m_view.z, m_up.x, m_up.y, m_up.z);
		glTranslatef(m_position.x, m_position.y, m_position.z);
		computeFrustrum();
	};

	/** Placement arbitraire de la caméra.
	 */
	void setFromViewPoint(const CPoint& pos, const CPoint& dir)
	{
		//    CPoint target = src->center+src->dir;
//
//    // Check if dir and up vectors are colinear for glutLookAt openGL call
//    // We compute the trapezoid area = norm of the cross product */
//    CVector area = src->dir^up;
//    if (area.length() < EPSILON)
//        up=CVector(0,1,0);
//    else
//        nup=up;
		CVector centre = pos+dir;
		glPopMatrix ();
		glPushMatrix ();
		gluLookAt(pos.x, pos.y, pos.z, centre.x, centre.y, centre.z, m_up.x, m_up.y, m_up.z);
		computeFrustrum();
	};

	/** Placement arbitraire de la caméra.
	*/
	void setFromViewPoint(const CPoint& pos, const CPoint& dir, const CPoint& up)
	{
		CVector centre = pos+dir;
		glPopMatrix ();
		glPushMatrix ();
		gluLookAt(pos.x, pos.y, pos.z, centre.x,centre.y,centre.z, up.x, up.y, up.z);
		computeFrustrum();
	};

	/** Rotation de la caméra */
	void rotate(float angle, float x, float y, float z);

	void OnMouseClick (NMouseButton a_nButton, NMouseButtonState a_nState, int x, int y);
	void OnMouseMotion (int x, int y);

	/** Déplacement de la caméra sur les côtés
	 * On construit simplement un vecteur orthogonal au vecteur up et au vecteur de vue
	 * et on effectue ensuite une translation suivant ce vecteur
	 * @param value valeur de la translation
	 */
	void moveOnSides(float value)
	{
		CVector axis = m_view ^ m_up;
		m_position = m_position + (axis * value);
		setView();
	};
	/** Déplacement de la caméra vers l'avant ou vers l'arrière
	 * On effectue ensuite une translation suivant le vecteur de vue
	 * @param value valeur de la translation
	 */
	void moveOnFrontOrBehind(float value)
	{
		m_position = m_position + (m_view * value);
		setView();
	};
	/** Déplacement de la caméra vers l'avant ou vers l'arrière
	 * On effectue ensuite une translation suivant le vecteur de vue
	 * @param value valeur de la translation
	 */
	void moveUpOrDown(float value)
	{
		m_position = m_position + (m_up * value);
		setView();
	};

	void setOrthographicProjection()
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, m_viewPort[2], 0, m_viewPort[3]);
		glMatrixMode(GL_MODELVIEW);
	};

	void resetPerspectiveProjection()
	{
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	};

	void computeFrustrum();

	const float* getFrustum(uint side) const
	{
		return m_frustum[side];
	};

	uint getWidth() const
	{
		return m_viewPort[2];
	};
	uint getHeight() const
	{
		return m_viewPort[3];
	};
	CPoint getPosition() const
	{
		return CPoint(0,0,0)-m_position;
	};

	void setSize(int width, int height);

//  void displayStringOnScreen(const string &time, uint x ,uint y);

	/* Récupère les coordonnées à l'écran d'un point */
	void getScreenCoordinates(const CPoint& objPos, CPoint& screenPos) const;
	/* Récupère les coordonnées d'une sphère sur l'écran */
	void getSphereCoordinates(const CPoint& objPos, float radius, CPoint& centerScreenPos, CPoint& periScreenPos ) const;

private:
	/** Position de la scène. La caméra reste toujours centrée en (0,0,0) */
	CPoint m_position;
	/** Vecteur vers le haut et direction de visée */
	CVector m_up, m_view;
	/** Bouton de la souris actuellement appuyé */
	NMouseButton m_buttonPressed;
	/** Angle d'ouverture de la caméra en degrés */
	float m_ouverture;
	/** Ratio de la largeur sur la hauteur de la projection */
	float m_aspect;

	/** Variables temporaires pour savoir à partir de quel endroit le glissement de la
	 * souris a commencé
	 */
	int m_beginMouseX, m_beginMouseY;
	/** Valeur de clipping de la caméra */
	float m_clipping_value;
	/** Rotation actuelle autour de l'axe X */
	float m_currentRotationX;
	/** Angle de rotation maximal autour de l'axe X en radians */
	float m_maxAngleX;
	/** Variable temporaire indiquant qu'un mouvement a lieu avec la souris */
	bool m_move;
	/** Sensibilité de la souris - valeurs conseillées entre 50 et 1000 */
	float m_mouseSensitivity;
	/** Viewport */
	int m_viewPort[4];
	/** Plans du frustrum */
	float m_frustum[6][4];
	/** Matrice de projection */
	double m_projMatrix[16];
	/** Matrice de transformation */
	double m_modlMatrix[16];
	CScene *m_scene;
};

#endif

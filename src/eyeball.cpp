#include "eyeball.hpp"

Eyeball::Eyeball (int width, int height, double clipping) : m_up(0.0,1.0,0.0), m_view(0.0,0.0,-1.0)
{
  m_width = width;
  m_height = height;

  m_currentRotationX = 0;
  m_mouseSensitivity = 1;
  maxAngle = 1;
  
  m_clipping_value = clipping;

  /* ouverture de la pyramide de vision */
  m_ouverture = 60.0;
  
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (m_ouverture, 1.0, 0.1, m_clipping_value);

  /* initialisation du deplacement trackball */
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  gluLookAt (m_position.x,m_position.y,m_position.z,m_view.x, m_view.y, m_view.z, m_up.x, m_up.y, m_up.z);
  glPushMatrix ();
  glLoadIdentity ();
}

void Eyeball::computeView(void)
{  
  int middleX = m_width/2;
  int middleY = m_height/2;

  // vector that describes mouseposition - center
  CVector mouseDirection;
  
  // get the distance and direction the mouse moved in x (in
  // pixels). We can't use the actual number of pixels in radians,
  // as only six pixels  would cause a full 360 degree rotation.
  // So we use a mousesensitivity variable that can be changed to
  // vary how many radians we want to turn in the x-direction for
  // a given mouse movement distance

  // We have to remember that positive rotation is counter-clockwise. 
  // Moving the mouse down is a negative rotation about the x axis
  // Moving the mouse right is a negative rotation about the y axis
  mouseDirection.x = (middleX - m_beginMouseX)/m_mouseSensitivity; 
  mouseDirection.y = (middleY - m_beginMouseY)/m_mouseSensitivity;

  m_currentRotationX += mouseDirection.y;
  
  // We don't want to rotate up more than one radian, so we cap it.
  if(m_currentRotationX > 1)
  {
    m_currentRotationX = 1;
    return;
  }else
    // We don't want to rotate down more than one radian, so we cap it.
    if(m_currentRotationX < -1)
      {
	m_currentRotationX = -1;
	return;
      }
    else
      {
	// get the axis to rotate around the x-axis. 
	CVector axis = (m_view - m_position) ^ m_up;
	// To be able to use the quaternion conjugate, the axis to
	// rotate around must be normalized.
	axis.normalize();
	
	// Rotate around the y axis
	rotate(mouseDirection.y, axis.x, axis.y, axis.z);
	// Rotate around the x axis
	rotate(mouseDirection.x, 0, 1, 0);
      }
}

void Eyeball::rotate(double angle, double x, double y, double z)
{
  Quaternion temp, quatView, result;

  temp.x = x * sin(angle/2);
  temp.y = y * sin(angle/2);
  temp.z = z * sin(angle/2);
  temp.w = cos(angle/2);

  quatView.x = m_view.x;
  quatView.y = m_view.y;
  quatView.z = m_view.z;
  quatView.w = 0;

  result = temp * quatView;
  temp.conjugate();
  result = result * temp;

  m_view.x = result.x;
  m_view.y = result.y;
  m_view.z = result.z;
}

void Eyeball::OnMouseClick (wxMouseEvent& event)
{
  m_buttonPressed = event.GetButton();
  m_beginMouseX = event.GetX();
  m_beginMouseY = event.GetY();

  if (event.ButtonDown())
    m_move = true;
  else
    m_move = false;
}

void Eyeball::OnMouseMotion (wxMouseEvent& event)
{  
  if(m_move){
    if (m_buttonPressed == wxMOUSE_BTN_LEFT)
      {
	computeView();
	//add_quats (lastquat, curquat, curquat);
      }
    m_beginMouseX = event.GetX();
    m_beginMouseY = event.GetY();
  }
}

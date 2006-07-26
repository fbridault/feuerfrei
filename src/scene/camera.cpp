#include "camera.hpp"

Camera::Camera (int width, int height, double clipping) : m_position(0.0,0.0,-2.0), m_up(0.0,1.0,0.0), m_view(0.0,0.0,-1.0)
{
  m_width = width;
  m_height = height;
  
  m_buttonPressed = 0;
  m_move = false;
  m_currentRotationX = 0;
  m_mouseSensitivity = 800;
  m_maxAngleX = 1.2;
  
  m_clipping_value = clipping;

  /* ouverture de la pyramide de vision */
  m_ouverture = 60.0;
  
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (m_ouverture, 4/3.0, 0.1, m_clipping_value);

  /* initialisation du deplacement trackball */
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  /* La caméra reste toujours centrée en (0,0,0) */
  gluLookAt (0.0,0.0,0.0,m_view.x, m_view.y, m_view.z, m_up.x, m_up.y, m_up.z);
  glPushMatrix ();
  glLoadIdentity ();
}

void Camera::computeView(double x, double y)
{  
  Vector mouseDirection;
  
  mouseDirection.x = (m_beginMouseX-x)/m_mouseSensitivity; 
  mouseDirection.y = (m_beginMouseY-y)/m_mouseSensitivity;

  m_currentRotationX += mouseDirection.y;
  
  if(m_currentRotationX > m_maxAngleX)
  {
    m_currentRotationX = m_maxAngleX;
    return;
  }else
    if(m_currentRotationX < -m_maxAngleX)
      {
	m_currentRotationX = -m_maxAngleX;
	return;
      }
    else
      {
	// Récupérer l'axe de rotation qui sera X
	Vector axis = m_view ^ m_up;
	
	axis.normalize();	
	// Rotation autour de l'axe Y
	rotate(mouseDirection.y, axis.x, axis.y, axis.z);
	// Rotation autour de l'axe X
	rotate(mouseDirection.x, 0, 1, 0);
      }
}

void Camera::rotate(double angle, double x, double y, double z)
{
  Quaternion temp, quatView, result;

  temp.x = x * sin(angle/2.0);
  temp.y = y * sin(angle/2.0);
  temp.z = z * sin(angle/2.0);
  temp.w = cos(angle/2.0);

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

void Camera::OnMouseClick (wxMouseEvent& event)
{
  m_buttonPressed = event.GetButton();
  m_beginMouseX = event.GetX();
  m_beginMouseY = event.GetY();
  
  if (event.ButtonDown())
    m_move = true;
  else
    m_move = false;
}

void Camera::OnMouseMotion (wxMouseEvent& event)
{  
  if(m_move){
    
    if (m_buttonPressed == wxMOUSE_BTN_RIGHT)
      {
	computeView(event.GetX(),event.GetY());
	//add_quats (lastquat, curquat, curquat);
      }
    else
      if (m_buttonPressed == wxMOUSE_BTN_MIDDLE)
	{
	  moveOnSides( (event.GetX() - m_beginMouseX)  / (m_mouseSensitivity/5) );
	  moveUpOrDown( (m_beginMouseY - event.GetY() ) / (m_mouseSensitivity/5) );
	}
    m_beginMouseX = event.GetX();
    m_beginMouseY = event.GetY();
  }
}

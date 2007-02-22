#include "camera.hpp"

#include "scene.hpp"

uint g_objectCount;

#ifdef RTFLAMES_BUILD
Camera::Camera (int width, int height, double clipping, Scene* const scene ) :
#else
Camera::Camera (int width, int height, double clipping) :
#endif
m_position(0.0,0.0,-2.0), m_up(0.0,1.0,0.0), m_view(0.0,0.0,-1.0)
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
  
#ifdef RTFLAMES_BUILD
  m_scene = scene;
  computeFrustrum();
#endif
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
#ifdef RTFLAMES_BUILD
    computeFrustrum();
#endif
  }
}

#ifdef RTFLAMES_BUILD
void Camera::computeFrustrum()
{
   double   proj[16];
   double   modl[16];
   double   clip[16];
   double   t;
   
   /* Get the current PROJECTION matrix from OpenGL */
   glGetDoublev( GL_PROJECTION_MATRIX, proj );
   
   /* Get the current MODELVIEW matrix from OpenGL */
   glGetDoublev( GL_MODELVIEW_MATRIX, modl );
   
   /* Combine the two matrices (multiply projection by modelview) */
   clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
   clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
   clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
   clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

   clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
   clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
   clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
   clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

   clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
   clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
   clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
   clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

   clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
   clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
   clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
   clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

   /* Extract the numbers for the RIGHT plane */
   m_frustum[0][0] = clip[ 3] - clip[ 0];
   m_frustum[0][1] = clip[ 7] - clip[ 4];
   m_frustum[0][2] = clip[11] - clip[ 8];
   m_frustum[0][3] = clip[15] - clip[12];

   /* Normalize the result */
   t = sqrt( m_frustum[0][0] * m_frustum[0][0] + m_frustum[0][1] * m_frustum[0][1] + m_frustum[0][2] * m_frustum[0][2] );
   m_frustum[0][0] /= t;
   m_frustum[0][1] /= t;
   m_frustum[0][2] /= t;
   m_frustum[0][3] /= t;

   /* Extract the numbers for the LEFT plane */
   m_frustum[1][0] = clip[ 3] + clip[ 0];
   m_frustum[1][1] = clip[ 7] + clip[ 4];
   m_frustum[1][2] = clip[11] + clip[ 8];
   m_frustum[1][3] = clip[15] + clip[12];

   /* Normalize the result */
   t = sqrt( m_frustum[1][0] * m_frustum[1][0] + m_frustum[1][1] * m_frustum[1][1] + m_frustum[1][2] * m_frustum[1][2] );
   m_frustum[1][0] /= t;
   m_frustum[1][1] /= t;
   m_frustum[1][2] /= t;
   m_frustum[1][3] /= t;

   /* Extract the BOTTOM plane */
   m_frustum[2][0] = clip[ 3] + clip[ 1];
   m_frustum[2][1] = clip[ 7] + clip[ 5];
   m_frustum[2][2] = clip[11] + clip[ 9];
   m_frustum[2][3] = clip[15] + clip[13];

   /* Normalize the result */
   t = sqrt( m_frustum[2][0] * m_frustum[2][0] + m_frustum[2][1] * m_frustum[2][1] + m_frustum[2][2] * m_frustum[2][2] );
   m_frustum[2][0] /= t;
   m_frustum[2][1] /= t;
   m_frustum[2][2] /= t;
   m_frustum[2][3] /= t;

   /* Extract the TOP plane */
   m_frustum[3][0] = clip[ 3] - clip[ 1];
   m_frustum[3][1] = clip[ 7] - clip[ 5];
   m_frustum[3][2] = clip[11] - clip[ 9];
   m_frustum[3][3] = clip[15] - clip[13];

   /* Normalize the result */
   t = sqrt( m_frustum[3][0] * m_frustum[3][0] + m_frustum[3][1] * m_frustum[3][1] + m_frustum[3][2] * m_frustum[3][2] );
   m_frustum[3][0] /= t;
   m_frustum[3][1] /= t;
   m_frustum[3][2] /= t;
   m_frustum[3][3] /= t;

   /* Extract the FAR plane */
   m_frustum[4][0] = clip[ 3] - clip[ 2];
   m_frustum[4][1] = clip[ 7] - clip[ 6];
   m_frustum[4][2] = clip[11] - clip[10];
   m_frustum[4][3] = clip[15] - clip[14];

   /* Normalize the result */
   t = sqrt( m_frustum[4][0] * m_frustum[4][0] + m_frustum[4][1] * m_frustum[4][1] + m_frustum[4][2] * m_frustum[4][2] );
   m_frustum[4][0] /= t;
   m_frustum[4][1] /= t;
   m_frustum[4][2] /= t;
   m_frustum[4][3] /= t;

   /* Extract the NEAR plane */
   m_frustum[5][0] = clip[ 3] + clip[ 2];
   m_frustum[5][1] = clip[ 7] + clip[ 6];
   m_frustum[5][2] = clip[11] + clip[10];
   m_frustum[5][3] = clip[15] + clip[14];

   /* Normalize the result */
   t = sqrt( m_frustum[5][0] * m_frustum[5][0] + m_frustum[5][1] * m_frustum[5][1] + m_frustum[5][2] * m_frustum[5][2] );
   m_frustum[5][0] /= t;
   m_frustum[5][1] /= t;
   m_frustum[5][2] /= t;
   m_frustum[5][3] /= t;
   
   /* Compute objects visibility */
//    g_objectCount=0;
   m_scene->computeVisibility(*this);
//    cerr << g_objectCount << " objects drawn" << endl;
}
#endif

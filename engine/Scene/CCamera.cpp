#include "CCamera.hpp"

#include "CScene.hpp"
#include "../Utility/GraphicsFn.hpp"
#include "../Common.hpp"

uint g_objectCount;

CCamera::CCamera () :
		m_position(0.0f,0.0f,-1.0f), m_up(0.0f,1.0f,0.0f), m_view(0.0f,0.0f,-1.0f)
{
	m_buttonPressed = NMouseButton::eButtonNone;
	m_move = false;
	m_currentRotationX = 0;
	m_mouseSensitivity = 800;
	m_maxAngleX = 1.2f;

	/* ouverture de la pyramide de vision */
	m_ouverture = 60.0f;
}

void CCamera::init (int width, int height, float clipping, CScene* const scene )
{
	m_scene = scene;

	m_clipping_value = clipping;

	m_viewPort[0] = m_viewPort[1] = 0;
	setSize(width,height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix ();

	setView();

	glGetFloatv (GL_MODELVIEW_MATRIX, g_modelViewMatrix);
}

void CCamera::computeView(float x, float y)
{
	CVector mouseDirection;

	mouseDirection.x = (m_beginMouseX-x)/m_mouseSensitivity;
	mouseDirection.y = (m_beginMouseY-y)/m_mouseSensitivity;

	m_currentRotationX += mouseDirection.y;

	if (m_currentRotationX > m_maxAngleX)
	{
		m_currentRotationX = m_maxAngleX;
		return;
	}
	else
		if (m_currentRotationX < -m_maxAngleX)
		{
			m_currentRotationX = -m_maxAngleX;
			return;
		}
		else
		{
			// Récupérer l'axe de rotation qui sera X
			CVector axis = m_view ^ m_up;

			axis.normalize();
			// Rotation autour de l'axe Y
			rotate(mouseDirection.y, axis.x, axis.y, axis.z);
			// Rotation autour de l'axe X
			rotate(mouseDirection.x, 0, 1, 0);
		}
}

void CCamera::rotate(float angle, float x, float y, float z)
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

void CCamera::OnMouseClick (NMouseButton a_nButton, NMouseButtonState a_nState, int x, int y)
{
	m_buttonPressed = a_nButton;
	m_beginMouseX = x;
	m_beginMouseY = y;

	if (a_nState == NMouseButtonState::eButtonDown)
		m_move = true;
	else
		m_move = false;
}

void CCamera::OnMouseMotion (int x, int y)
{
	if (m_move)
	{
		if (m_buttonPressed == NMouseButton::eButtonRight)
		{
			computeView(x,y);
		}
		else
			if (m_buttonPressed == NMouseButton::eButtonMiddle)
			{
				moveOnSides( (x - m_beginMouseX)  / (m_mouseSensitivity) );
				moveUpOrDown( (m_beginMouseY - y) / (m_mouseSensitivity) );
			}
		m_beginMouseX = x;
		m_beginMouseY = y;
		setView();
	}
}

void CCamera::computeFrustrum()
{
	float   clip[16];
	float   t;

	/* Get the current PROJECTION matrix from OpenGL */
	glGetDoublev( GL_PROJECTION_MATRIX, m_projMatrix );

	/* Get the current MODELVIEW matrix from OpenGL */
	glGetDoublev( GL_MODELVIEW_MATRIX, m_modlMatrix );

	/* Combine the two matrices (multiply projection by modelview) */
	clip[ 0] = m_modlMatrix[ 0] * m_projMatrix[ 0] + m_modlMatrix[ 1] * m_projMatrix[ 4] +
	           m_modlMatrix[ 2] * m_projMatrix[ 8] + m_modlMatrix[ 3] * m_projMatrix[12];
	clip[ 1] = m_modlMatrix[ 0] * m_projMatrix[ 1] + m_modlMatrix[ 1] * m_projMatrix[ 5] +
	           m_modlMatrix[ 2] * m_projMatrix[ 9] + m_modlMatrix[ 3] * m_projMatrix[13];
	clip[ 2] = m_modlMatrix[ 0] * m_projMatrix[ 2] + m_modlMatrix[ 1] * m_projMatrix[ 6] +
	           m_modlMatrix[ 2] * m_projMatrix[10] + m_modlMatrix[ 3] * m_projMatrix[14];
	clip[ 3] = m_modlMatrix[ 0] * m_projMatrix[ 3] + m_modlMatrix[ 1] * m_projMatrix[ 7] +
	           m_modlMatrix[ 2] * m_projMatrix[11] + m_modlMatrix[ 3] * m_projMatrix[15];

	clip[ 4] = m_modlMatrix[ 4] * m_projMatrix[ 0] + m_modlMatrix[ 5] * m_projMatrix[ 4] +
	           m_modlMatrix[ 6] * m_projMatrix[ 8] + m_modlMatrix[ 7] * m_projMatrix[12];
	clip[ 5] = m_modlMatrix[ 4] * m_projMatrix[ 1] + m_modlMatrix[ 5] * m_projMatrix[ 5] +
	           m_modlMatrix[ 6] * m_projMatrix[ 9] + m_modlMatrix[ 7] * m_projMatrix[13];
	clip[ 6] = m_modlMatrix[ 4] * m_projMatrix[ 2] + m_modlMatrix[ 5] * m_projMatrix[ 6] +
	           m_modlMatrix[ 6] * m_projMatrix[10] + m_modlMatrix[ 7] * m_projMatrix[14];
	clip[ 7] = m_modlMatrix[ 4] * m_projMatrix[ 3] + m_modlMatrix[ 5] * m_projMatrix[ 7] +
	           m_modlMatrix[ 6] * m_projMatrix[11] + m_modlMatrix[ 7] * m_projMatrix[15];

	clip[ 8] = m_modlMatrix[ 8] * m_projMatrix[ 0] + m_modlMatrix[ 9] * m_projMatrix[ 4] +
	           m_modlMatrix[10] * m_projMatrix[ 8] + m_modlMatrix[11] * m_projMatrix[12];
	clip[ 9] = m_modlMatrix[ 8] * m_projMatrix[ 1] + m_modlMatrix[ 9] * m_projMatrix[ 5] +
	           m_modlMatrix[10] * m_projMatrix[ 9] + m_modlMatrix[11] * m_projMatrix[13];
	clip[10] = m_modlMatrix[ 8] * m_projMatrix[ 2] + m_modlMatrix[ 9] * m_projMatrix[ 6] +
	           m_modlMatrix[10] * m_projMatrix[10] + m_modlMatrix[11] * m_projMatrix[14];
	clip[11] = m_modlMatrix[ 8] * m_projMatrix[ 3] + m_modlMatrix[ 9] * m_projMatrix[ 7] +
	           m_modlMatrix[10] * m_projMatrix[11] + m_modlMatrix[11] * m_projMatrix[15];

	clip[12] = m_modlMatrix[12] * m_projMatrix[ 0] + m_modlMatrix[13] * m_projMatrix[ 4] +
	           m_modlMatrix[14] * m_projMatrix[ 8] + m_modlMatrix[15] * m_projMatrix[12];
	clip[13] = m_modlMatrix[12] * m_projMatrix[ 1] + m_modlMatrix[13] * m_projMatrix[ 5] +
	           m_modlMatrix[14] * m_projMatrix[ 9] + m_modlMatrix[15] * m_projMatrix[13];
	clip[14] = m_modlMatrix[12] * m_projMatrix[ 2] + m_modlMatrix[13] * m_projMatrix[ 6] +
	           m_modlMatrix[14] * m_projMatrix[10] + m_modlMatrix[15] * m_projMatrix[14];
	clip[15] = m_modlMatrix[12] * m_projMatrix[ 3] + m_modlMatrix[13] * m_projMatrix[ 7] +
	           m_modlMatrix[14] * m_projMatrix[11] + m_modlMatrix[15] * m_projMatrix[15];

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

//void CCamera::displayStringOnScreen(const string &time, uint x, uint y)
//{
//  glColor3f(1.0f,1.0f,1.0f);
//  glPushMatrix();
//  glLoadIdentity();
//  setOrthographicProjection();
//  UGraphicsFn::renderBitmapString(x,y,m_viewPort[3],GLUT_BITMAP_HELVETICA_12,time.c_str());
//  glPopMatrix();
//  resetPerspectiveProjection();
//}

void CCamera::setSize(int width, int height)
{
	m_viewPort[2] = width;
	m_viewPort[3] = height;
	m_aspect = width/(float)height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective (m_ouverture, m_aspect, 0.01f, m_clipping_value);
	glViewport ( m_viewPort[0], m_viewPort[1], m_viewPort[2], m_viewPort[3]);
	glMatrixMode(GL_MODELVIEW);
}

void CCamera::getScreenCoordinates(const CPoint& objPos, CPoint& screenPos) const
{
	double pos[3];

	if ( gluProject( objPos.x, objPos.y, objPos.z, m_modlMatrix, m_projMatrix, m_viewPort,
	                 &pos[0], &pos[1], &pos[2]) == GL_FALSE )
		cerr << "GluProject failed" << endl;
	screenPos.x = (float)(pos[0]/m_viewPort[2]);
	screenPos.y = (float)(pos[1]/m_viewPort[3]);
	//  screenPos.z = (float)pos[2];
}

void CCamera::getSphereCoordinates(const CPoint& objPos, float radius, CPoint& centerScreenPos, CPoint& periScreenPos ) const
{
	CPoint up, tmp;

	/* Pour obtenir un point à la périphérie, on traite le problème un peu comme celui des billboards.
	 * On calcule le vecteur Up qui correspond au vecteur vertical local à la sphère. Il suffit ensuite
	 * de translater le centre de radius*up pour obtenir le point au zénith de la sphère.
	 */
	up.x = m_modlMatrix[1];
	up.y = m_modlMatrix[5];
	up.z = m_modlMatrix[9];

	getScreenCoordinates(objPos, centerScreenPos);
	tmp = objPos+(up*radius);
	getScreenCoordinates(tmp, periScreenPos);
}

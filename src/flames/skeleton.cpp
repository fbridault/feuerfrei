#include "skeleton.hpp"

#include <stdlib.h>
#include <math.h>
#include <engine/Utility/GraphicsFn.hpp>

#include "../solvers/solver3D.hpp"

#define LIFE_EXTEND 5

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FREESKELETON **************************************/
/**********************************************************************************************************************/
IFreeSkeleton::IFreeSkeleton(uint a_uiSize, Field3D& a_rField) :
	m_rField(a_rField)
{
	m_queue = new CParticle[a_uiSize];
	m_iHeadIndex = -1;
}

IFreeSkeleton::IFreeSkeleton(IFreeSkeleton const& a_rSrc, uint a_uiSplitHeight) :
	m_rField(a_rSrc.m_rField)
{
	m_queue = new CParticle[a_uiSplitHeight+1];

	/* Recopie des particules en fonction de la hauteur de coupe */
	for (uint i=0; i <= a_uiSplitHeight; i++)
	{
		m_queue[i] = a_rSrc.m_queue[i];
		/* Extension de la durée de vie !! */
		m_queue[i].m_lifespan +=LIFE_EXTEND;
	}
	m_iHeadIndex = a_uiSplitHeight;
	m_selfVelocity = a_rSrc.m_selfVelocity;
}

IFreeSkeleton::~IFreeSkeleton()
{
	delete [] m_queue;
}

void IFreeSkeleton::removeParticle(uint n)
{
	for (int i=n; i<m_iHeadIndex ; i++)
		m_queue[i] = m_queue[i+1];

	m_iHeadIndex--;
	assert(m_iHeadIndex>=0);
}

void IFreeSkeleton::swap(uint i, uint j)
{
	CParticle tmp(m_queue[i]);

	m_queue[i] = m_queue[j];
	m_queue[j] = tmp;
}

void IFreeSkeleton::move ()
{
	/* Boucle de parcours : du haut vers le bas */
	for (uint i = 0; i < getInternalSize (); i++)
	{
		CParticle& tmp = grabParticle (i);

		if (moveParticle (tmp))
			updateParticle (i, tmp);
		else
		{
			removeParticle (i);
			if (i < getInternalSize () - 1)
				i--;
		}
	}
}

bool IFreeSkeleton::moveParticle (CParticle& a_rParticle)
{
	CParticle copy(a_rParticle), copy2;

	if (a_rParticle.isDead ())
		return false;

	/* Si la particule sort de la grille, elle prend la vélocité du bord */
	if ( a_rParticle.x >= m_rField.getDimX() )
		a_rParticle.x = m_rField.getDimX() - EPSILON;
	if ( a_rParticle.y >= m_rField.getDimY() )
		a_rParticle.y = m_rField.getDimY() - EPSILON;
	if ( a_rParticle.z >= m_rField.getDimZ() )
		a_rParticle.z = m_rField.getDimZ() - EPSILON;
	if ( a_rParticle.x < 0.0f )
		a_rParticle.x = EPSILON;
	if ( a_rParticle.z < 0.0f )
		a_rParticle.z = EPSILON;
	/* Cas particulier en y, on supprime la particule si elle passe SOUS la grille
	 * (évite les problèmes de "stagnation" de la flamme) */
	if ( a_rParticle.y < 0.0f )
		return false;

	copy2 = a_rParticle;
	/* Calculer la nouvelle position ( Intégration d'Euler, on prend juste la dérivée première ) */
	m_rField.moveParticle(a_rParticle, m_selfVelocity);

	a_rParticle = a_rParticle - copy2 + copy;
	return true;
}

void IFreeSkeleton::draw () const
{
	for (uint i = 0; i < getSize (); i++)
		drawParticle( getParticle (i) ) ;
	glBegin(GL_LINE_STRIP);
	for (uint i = 0; i < getSize (); i++)
		glVertex3f(m_queue[i].x, m_queue[i].y, m_queue[i].z);
	glEnd();
}

void IFreeSkeleton::drawParticle (CParticle const& a_rParticle) const
{
	glColor4f (1.0f, 1.0f, 0.25f, 0.8f);
	glPushMatrix ();
	glTranslatef (a_rParticle.x, a_rParticle.y, a_rParticle.z);
	UGraphicsFn::SolidSphere (0.01f, 10, 10);
	glPopMatrix ();
}


/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE SKELETON ******************************************/
/**********************************************************************************************************************/

ISkeleton::ISkeleton(Field3D& a_rField, const CPoint& position, const CPoint& rootMoveFactor, uint pls) :
		IFreeSkeleton(NB_PARTICLES_MAX, a_rField),
		m_rootMoveFactor(rootMoveFactor)
{
	m_root = m_rootSave = position;
	m_selfVelocity=0.0f;
	m_lod=FULL_SKELETON;
	m_lifeSpan = pls;
}

void ISkeleton::draw () const
{
	drawRoot();
	for (uint i = 0; i < getSize (); i++)
		drawParticle( getParticle (i) ) ;
	glBegin(GL_LINE_STRIP);
	for (uint i = 0; i < getSize (); i++)
		glVertex3f(m_queue[i].x, m_queue[i].y, m_queue[i].z);
	glVertex3f(m_root.x, m_root.y, m_root.z);
	glEnd();
}

void ISkeleton::drawRoot () const
{
	glColor4f (1.0f, 0.0f, 0.25f, 0.8f);
	glPushMatrix ();
	glTranslatef (m_root.x, m_root.y, m_root.z);
	UGraphicsFn::SolidSphere (0.01f, 10, 10);
	glPopMatrix ();
}

void ISkeleton::moveRoot ()
{
	/* Calculer la nouvelle position ( Intégration d'Euler, on prend juste la dérivée première ) */
	m_root = m_rootSave + m_rootMoveFactor * m_rField.getUVW (m_rootSave, m_selfVelocity);

	/* Si l'origine sort de la grille, on la replace */
	if ( m_root.x >= m_rField.getDimX() )
		m_root.x = m_rField.getDimX() - EPSILON;
	if ( m_root.y >= m_rField.getDimY() )
		m_root.y = m_rField.getDimY() - EPSILON;
	if ( m_root.z >= m_rField.getDimZ() )
		m_root.z = m_rField.getDimZ() - EPSILON;
	if ( m_root.x < 0.0f )
		m_root.x = EPSILON;
	if ( m_root.y < 0.0f )
		m_root.y = EPSILON;
	if ( m_root.z < 0.0f )
		m_root.z = EPSILON;
}

void ISkeleton::move ()
{
	moveRoot ();

	if (getInternalSize () < NB_PARTICLES_MAX - 1)
		addParticle(m_root);

	IFreeSkeleton::move ();
}

bool ISkeleton::moveParticle (CParticle& a_rParticle)
{
	if (a_rParticle.isDead ())
		return false;

	m_rField.moveParticle(a_rParticle, m_selfVelocity);

	/* Si la particule sort de la grille, elle est éliminée */
	if (  	   a_rParticle.x < 0.0f || a_rParticle.x > m_rField.getDimX()
			|| a_rParticle.y < 0.0f || a_rParticle.y > m_rField.getDimY()
			|| a_rParticle.z < 0.0f || a_rParticle.z > m_rField.getDimZ() )
		return false;

	return true;
}

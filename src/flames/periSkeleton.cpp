#include "periSkeleton.hpp"

#include <stdlib.h>
#include <math.h>
#include <engine/Utility/GraphicsFn.hpp>

#include "../solvers/solver3D.hpp"
#include "leadSkeleton.hpp"


/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE PERISKELETON **************************************/
/**********************************************************************************************************************/
CPeriSkeleton::CPeriSkeleton (	Field3D& a_rField, const CPoint& a_rPosition, const CPoint& a_rRootMoveFactor,
															CLeadSkeleton& a_rLeadSkeleton, uint a_uiPls) :
		ISkeleton (a_rField, a_rPosition, a_rRootMoveFactor, a_uiPls),
		m_rLead(a_rLeadSkeleton)
{
	addParticle(m_root);
}

CPeriSkeleton::~CPeriSkeleton ()
{
}

CFreePeriSkeleton* CPeriSkeleton::split (uint a_uiSplitHeight, CFreeLeadSkeleton& a_rLeadSkeleton)
{
	CFreePeriSkeleton *skel = new CFreePeriSkeleton(rThis, a_rLeadSkeleton, a_uiSplitHeight);

	m_iHeadIndex = a_uiSplitHeight;

	assert(skel != NULL);
	return( skel );
}

void CPeriSkeleton::addForces ()
{
	float dummy;

	/* Applique une force à la base des squelettes (pour les bougies). Ceci n'a
	 * aucun effet pour les FakeFields, qui comptent eux sur la selfVelocity du
	 * squelette relatif pour transmettre cette force (voir moveParticle() plus
	 * bas). */
	m_rField.addVsrc( m_root, m_rLead.getLastAppliedForce(), dummy);
}

void CPeriSkeleton::addParticle(CPoint const& a_rParticle)
{
	if (m_iHeadIndex >= NB_PARTICLES_MAX-1)
	{
		puts("(EE) Too many particles in CPeriSkeleton::addParticle() !!!");
		return;
	}
	m_iHeadIndex++;

	m_queue[m_iHeadIndex] = a_rParticle;
	m_queue[m_iHeadIndex].birth(m_lifeSpan);
}

bool CPeriSkeleton::moveParticle (CParticle& a_rParticle)
{
	if (a_rParticle.isDead ())
		return false;

	/* Déplacement de la particule. Dans le cas présent, on utilise la
	 * selfVelocity du squelette relatif pour ajouter une force propre au
	 * squelette dans le cas des FakeFields. */
	m_rField.moveParticle(a_rParticle, m_rLead.getSelfVelocity());

	/* Si la particule sort de la grille, elle est éliminée */
	if (   	a_rParticle.x < 0.0f || a_rParticle.x > m_rField.getDimX()
	        || a_rParticle.y < 0.0f || a_rParticle.y > m_rField.getDimY()
	        || a_rParticle.z < 0.0f || a_rParticle.z > m_rField.getDimZ())
		return false;

	return true;
}

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FREEPERISKELETON **********************************/
/**********************************************************************************************************************/
CFreePeriSkeleton::CFreePeriSkeleton(IFreeSkeleton const& a_rSrc, CFreeLeadSkeleton& a_rLeadSkeleton, uint a_uiSplitHeight) :
		IFreeSkeleton(a_rSrc, a_uiSplitHeight),
		m_rLead(a_rLeadSkeleton)
{}

CFreePeriSkeleton::CFreePeriSkeleton(uint a_uiSize, Field3D& a_rField, CFreeLeadSkeleton& a_rLeadSkeleton) :
		IFreeSkeleton(a_uiSize, a_rField),
		m_rLead(a_rLeadSkeleton)
{}

CFreePeriSkeleton::~CFreePeriSkeleton ()
{
}

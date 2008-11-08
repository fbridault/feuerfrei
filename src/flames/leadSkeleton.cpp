#include "leadSkeleton.hpp"

#include <stdlib.h>
#include <math.h>
#include <engine/Utility/GraphicsFn.hpp>

#include "periSkeleton.hpp"
#include "../solvers/solver3D.hpp"

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE LEADSKELETON **************************************/
/**********************************************************************************************************************/
CLeadSkeleton::CLeadSkeleton (Field3D& a_rField, const CPoint& position, const CPoint& rootMoveFactor,
														uint a_uiPls, float u, float noiseIncrement, float noiseMin, float noiseMax) :
		ISkeleton (a_rField, position, rootMoveFactor,a_uiPls),
		m_noiseGenerator(noiseIncrement, noiseMin, noiseMax)
{
	m_u = u;
	m_perturbateCount=0;
	addParticle(m_root);
}

CLeadSkeleton::~CLeadSkeleton ()
{
}

void CLeadSkeleton::drawParticle (CParticle const& a_rParticle) const
{
	glColor4f (0.1f, 1.0f, 0.1f, 0.8f);
	glPushMatrix ();
	glTranslatef (a_rParticle.x, a_rParticle.y, a_rParticle.z);
	UGraphicsFn::SolidSphere (0.01f, 10, 10);
	glPopMatrix ();
}

void CLeadSkeleton::drawRoot () const
{
	glColor4f (0.0f, 0.4f, 0.0f, 0.8f);
	glPushMatrix ();
	glTranslatef (m_root.x, m_root.y, m_root.z);
	UGraphicsFn::SolidSphere (0.01f, 10, 10);
	glPopMatrix ();
}

void CLeadSkeleton::addForces (int fdf, float innerForce, char perturbate)
{
	m_selfVelocity = 0.0f;

	/* Applications des FDF */
	switch (fdf)
	{
		case FDF_LINEAR :
			m_lastAppliedForce = innerForce * (m_u + 1.0f);
			break;
		case FDF_BILINEAR :
			m_lastAppliedForce = innerForce * m_u * m_u;
			break;
		case FDF_EXPONENTIAL :
			m_lastAppliedForce = .1f * innerForce * 10 * exp(m_u);
			break;
		case FDF_GAUSS:
			m_lastAppliedForce = innerForce * 10 * exp(-m_u * m_u)/(9.0f);
			break;
		case FDF_RANDOM:
			m_lastAppliedForce = innerForce * rand()/((float)RAND_MAX);
			break;
		default:
			cerr << "(EE) FDF type error !!!" << endl;
	}

	switch (perturbate)
	{
		case FLICKERING_VERTICAL :
			if (m_perturbateCount >= 2)
			{
				m_lastAppliedForce += innerForce*5.0f;
				m_perturbateCount = 0;
			}
			else
				m_perturbateCount++;
			break;
		case FLICKERING_RANDOM1 :
			m_lastAppliedForce += (rand()/((float)RAND_MAX))/2.0f - .25f;
			break;
		case FLICKERING_RANDOM2 :
			m_lastAppliedForce += rand()/(10.0f*(float)RAND_MAX);
			break;
		case FLICKERING_NOISE :
			m_lastAppliedForce += m_noiseGenerator.getNextValue();
			break;
		case FLICKERING_NONE :
			break;
		default:
			cerr << "(EE) Flickering type error !!!" << endl;
	}
	m_rField.addVsrc( m_root, m_lastAppliedForce, m_selfVelocity);
}

void CLeadSkeleton::addParticle(CPoint const& a_rParticle)
{
	if (m_headIndex >= NB_PARTICLES_MAX-1)
	{
		puts("(EE) Too many particles in CLeadSkeleton::addParticle() !!!");
		return;
	}
	m_headIndex++;

	m_queue[m_headIndex] = a_rParticle;
	m_queue[m_headIndex].birth(m_lifeSpan);
}

CFreeLeadSkeleton* CLeadSkeleton::split (uint a_uiSplitHeight)
{
	CFreeLeadSkeleton *skel = new CFreeLeadSkeleton(rThis, a_uiSplitHeight);

	m_headIndex = a_uiSplitHeight;

	return( skel );
}


/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE FREELEADSKELETON **********************************/
/**********************************************************************************************************************/
CFreeLeadSkeleton::CFreeLeadSkeleton(IFreeSkeleton const& a_rSrc, uint a_uiSplitHeight) :
		IFreeSkeleton(a_rSrc, a_uiSplitHeight)
{
}

CFreeLeadSkeleton::~CFreeLeadSkeleton ()
{
}

CFreePeriSkeleton* CFreeLeadSkeleton::dup(const CPoint& offset)
{
	CFreePeriSkeleton *copy = new CFreePeriSkeleton(getInternalSize (), m_rField, rThis);

	for (uint i = 0; i < getInternalSize (); i++)
	{
		copy->m_queue[i] = m_queue[i] + offset;
		copy->m_queue[i].m_lifespan += m_queue[i].m_lifespan;
	}

	copy->m_headIndex = m_headIndex;
	copy->m_selfVelocity = m_selfVelocity;
	return copy;
}

void CFreeLeadSkeleton::drawParticle (CParticle const& a_rParticle) const
{
	glColor4f (0.1f, 1.0f, 0.1f, 0.8f);
	glPushMatrix ();
	glTranslatef (a_rParticle.x, a_rParticle.y, a_rParticle.z);
	UGraphicsFn::SolidSphere (0.01f, 10, 10);
	glPopMatrix ();
}

#include "cloneFlames.hpp"

#include <wx/intl.h>
/**********************************************************************************************************************/
/*************************************** IMPLEMENTATION DE LA CLASSE CLONEFLAME ***************************************/
/**********************************************************************************************************************/
CloneFlame::CloneFlame(const FlameConfig& flameConfig, const IRealFlame* const source, const CPoint& offset) :
		IFixedFlame (source->getNbSkeletons(), source->getNbFixedCPoints(), source->getTexture())
{
	m_source = source;
	m_position = offset;
}

CloneFlame::~CloneFlame()
{
}

bool CloneFlame::build()
{
	cloneNURBSPropertiesFrom(*m_source);

	m_ctrlPointsSave = m_ctrlPoints;
	for (uint i=0; i < (m_maxParticles + m_nbFixedPoints) * (m_nbSkeletons ) * 3; i++)
	{
		*m_ctrlPoints++ += rand()/(50*(float)RAND_MAX) - .025;
	}
	/* On recopie les m_uorder squelettes pour fermer la NURBS */
	GLfloat *startCtrlCPoints = m_ctrlPointsSave;
	for (uint i = 0; i < ((m_uorder-1)*m_vsize)*3; i++)
		*m_ctrlPoints++ = *startCtrlCPoints++;
	m_ctrlPoints = m_ctrlPointsSave;

	return true;
}

CloneLineFlame::CloneLineFlame(const FlameConfig& flameConfig, const CLineFlame* const source, const CPoint& offset) :
		CloneFlame (flameConfig, source, offset)
{
}

CloneLineFlame::~CloneLineFlame()
{
}

CloneCPointFlame::CloneCPointFlame(const FlameConfig& flameConfig, const CPointFlame* const source, const CPoint& offset) :
		CloneFlame (flameConfig, source, offset)
{
}

CloneCPointFlame::~CloneCPointFlame()
{
}

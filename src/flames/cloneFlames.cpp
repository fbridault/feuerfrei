#include "cloneFlames.hpp"

#include <wx/intl.h>
/**********************************************************************************************************************/
/*************************************** IMPLEMENTATION DE LA CLASSE CLONEFLAME ***************************************/
/**********************************************************************************************************************/
CloneFlame::CloneFlame(const FlameConfig& flameConfig, const RealFlame* const source, const CPoint& offset) :
  FixedFlame (source->getNbSkeletons(), source->getNbFixedCPoints(), source->getTexture())
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

  m_ctrlCPointsSave = m_ctrlCPoints;
  for(uint i=0; i < (m_maxParticles + m_nbFixedCPoints) * (m_nbSkeletons ) * 3; i++)
    {
      *m_ctrlCPoints++ += rand()/(50*(float)RAND_MAX) - .025;
    }
  /* On recopie les m_uorder squelettes pour fermer la NURBS */
  GLfloat *startCtrlCPoints = m_ctrlCPointsSave;
  for (uint i = 0; i < ((m_uorder-1)*m_vsize)*3; i++)
    *m_ctrlCPoints++ = *startCtrlCPoints++;
  m_ctrlCPoints = m_ctrlCPointsSave;

  return true;
}

CloneLineFlame::CloneLineFlame(const FlameConfig& flameConfig, const LineFlame* const source, const CPoint& offset) :
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

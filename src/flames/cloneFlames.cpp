#include "cloneFlames.hpp"

#include <wx/intl.h>
/**********************************************************************************************************************/
/*************************************** IMPLEMENTATION DE LA CLASSE CLONEFLAME ***************************************/
/**********************************************************************************************************************/
CloneFlame::CloneFlame(const FlameConfig& flameConfig, const RealFlame* const source, const Point& offset) :
  FixedFlame (source->getNbSkeletons(), source->getNbFixedPoints(), source->getTexture())
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
  for(uint i=0; i < (m_maxParticles + m_nbFixedPoints) * (m_nbSkeletons ) * 3; i++)
    {
      *m_ctrlPoints++ += rand()/(50*(double)RAND_MAX) - .025;
    }
  /* On recopie les m_uorder squelettes pour fermer la NURBS */
  GLfloat *startCtrlPoints = m_ctrlPointsSave;
  for (uint i = 0; i < ((m_uorder-1)*m_size)*3; i++)
    *m_ctrlPoints++ = *startCtrlPoints++;
  m_ctrlPoints = m_ctrlPointsSave;
  
  return true;
}

CloneLineFlame::CloneLineFlame(const FlameConfig& flameConfig, const LineFlame* const source, const Point& offset) :
  CloneFlame (flameConfig, source, offset)
{
}

CloneLineFlame::~CloneLineFlame()
{
}

ClonePointFlame::ClonePointFlame(const FlameConfig& flameConfig, const PointFlame* const source, const Point& offset) :
  CloneFlame (flameConfig, source, offset)
{
}

ClonePointFlame::~ClonePointFlame()
{
}

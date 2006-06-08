#include "cloneFlames.hpp"

#include <wx/intl.h>
/**********************************************************************************************************************/
/*************************************** IMPLEMENTATION DE LA CLASSE CLONEFLAME ***************************************/
/**********************************************************************************************************************/
CloneFlame::CloneFlame(FlameConfig* flameConfig, BasicFlame *source, Point offset, const wxString& texname, GLint wrap_s, GLint wrap_t) :
  MetaFlame (flameConfig, source->getNbSkeletons(), source->getNbFixedPoints(), texname, wrap_s, wrap_t)
{
  m_source = source;
  m_position = offset;
}

CloneFlame::~CloneFlame()
{
}

/** Fonction appelée par la fonction de dessin OpenGL. Elle commence par déplacer les particules 
 * des squelettes périphériques. Ensuite, elle définit la matrice de points de contrôle de la NURBS,
 * des vecteurs de noeuds.
 */
void CloneFlame::build()
{
  cloneNURBSPropertiesFrom(*m_source);
//   for(uint i=0; i < (m_maxParticles + m_nbFixedPoints) * (m_nbSkeletons + m_uorder) * 3; i++)
//     {
//       m_ctrlPoints[i] += rand()/(100*(double)RAND_MAX) - .005;
//     }
}

CloneLineFlame::CloneLineFlame(FlameConfig* flameConfig, LineFlame *source, Point offset) :
  CloneFlame (flameConfig, source, offset, _("textures/firmalampe.png"), GL_CLAMP, GL_REPEAT)
{
}

CloneLineFlame::~CloneLineFlame()
{
}

ClonePointFlame::ClonePointFlame(FlameConfig* flameConfig, PointFlame *source, Point offset) :
  CloneFlame (flameConfig, source, offset, _("textures/bougie2.png"), GL_CLAMP, GL_REPEAT)
{
}

ClonePointFlame::~ClonePointFlame()
{
}

#include "cloneFlames.hpp"

#include <wx/intl.h>
/**********************************************************************************************************************/
/*************************************** IMPLEMENTATION DE LA CLASSE CLONEFLAME ***************************************/
/**********************************************************************************************************************/
CloneFlame::CloneFlame(BasicFlame *source, Point& posRel, double samplingTolerance, const wxString& texname, 
		       GLint wrap_s, GLint wrap_t) :
  MetaFlame (source->getNbSkeletons(), source->getNbFixedPoints(), posRel, samplingTolerance, texname, wrap_s, wrap_t)
{
  m_source = source;
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
}


CloneLineFlame::CloneLineFlame(LineFlame *source, Point& posRel, double samplingTolerance) :
  CloneFlame (source, posRel, samplingTolerance, _("textures/firmalampe.png"), GL_CLAMP, GL_REPEAT)
{
}

CloneLineFlame::~CloneLineFlame()
{
}


ClonePointFlame::ClonePointFlame(PointFlame *source, Point& posRel, double samplingTolerance) :
  CloneFlame (source, posRel, samplingTolerance, _("textures/bougie2.png"), GL_CLAMP, GL_REPEAT)
{
}

ClonePointFlame::~ClonePointFlame()
{
}

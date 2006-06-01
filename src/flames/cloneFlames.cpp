#include "cloneFlames.hpp"

#include <wx/intl.h>
/**********************************************************************************************************************/
/*************************************** IMPLEMENTATION DE LA CLASSE CLONEFLAME ***************************************/
/**********************************************************************************************************************/
CloneFlame::CloneFlame(FlameConfig* flameConfig, BasicFlame *source, const wxString& texname, GLint wrap_s, GLint wrap_t) :
  MetaFlame (flameConfig, source->getNbSkeletons(), source->getNbFixedPoints(), texname, wrap_s, wrap_t)
{
  m_source = source;
}

CloneFlame::~CloneFlame()
{
}

/** Fonction appel�e par la fonction de dessin OpenGL. Elle commence par d�placer les particules 
 * des squelettes p�riph�riques. Ensuite, elle d�finit la matrice de points de contr�le de la NURBS,
 * des vecteurs de noeuds.
 */
void CloneFlame::build()
{
  cloneNURBSPropertiesFrom(*m_source);
}


CloneLineFlame::CloneLineFlame(FlameConfig* flameConfig, LineFlame *source) :
  CloneFlame (flameConfig, source, _("textures/firmalampe.png"), GL_CLAMP, GL_REPEAT)
{
}

CloneLineFlame::~CloneLineFlame()
{
}


ClonePointFlame::ClonePointFlame(FlameConfig* flameConfig, PointFlame *source) :
  CloneFlame (flameConfig, source, _("textures/bougie2.png"), GL_CLAMP, GL_REPEAT)
{
}

ClonePointFlame::~ClonePointFlame()
{
}

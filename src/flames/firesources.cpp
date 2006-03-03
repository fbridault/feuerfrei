#include "firesources.hpp"

#include "../scene/graphicsFn.hpp"
#include "../scene/scene.hpp"

Candle::Candle (Solver * s, CPoint& posRel, CScene *scene, double innerForce,  const char *filename, 
		int index, CgSVShader * shader, double rayon, int nbSkeletons):
  FireSource (s, 1, posRel, scene, innerForce, filename, index, shader)
	       //   cgCandleVertexShader (_("bougieShader.cg"),_("vertCandle"),context),
//   cgCandleFragmentShader (_("bougieShader.cg"),_("fragCandle"),context)
{
  m_flames[0] = new PointFlame(s, nbSkeletons, posRel, innerForce, scene, rayon);
}

Firmalampe::Firmalampe(Solver * s, CPoint& posRel, CScene *scene, double innerForce,  const char *filename, 
	   int index, CgSVShader * shader, int nbSkeletons, const char *wickName):
  FireSource (s, 1, posRel, scene, innerForce, filename, index, shader)
{
  m_flames[0] = new LineFlame(s, nbSkeletons, posRel, innerForce, scene, wickName);  
}

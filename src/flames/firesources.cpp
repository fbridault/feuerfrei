#include "firesources.hpp"

#include "../scene/graphicsFn.hpp"
#include "../scene/scene.hpp"

#include <vector>
#include <string>

#define WICK_NAME_PREFIX "Wick"
#define TORCH_NAME "Torch"

Candle::Candle (Solver * s, Point& posRel, Scene *scene, double innerForce,  const char *filename, 
		int index, CgSVShader * shader, double rayon, int nbSkeletons):
  FireSource (s, 1, posRel, scene, innerForce, filename, index, shader)
	       //   cgCandleVertexShader (_("bougieShader.cg"),_("vertCandle"),context),
//   cgCandleFragmentShader (_("bougieShader.cg"),_("fragCandle"),context)
{
  m_flames[0] = new PointFlame(s, nbSkeletons, posRel, innerForce, scene, rayon);
}

Firmalampe::Firmalampe(Solver * s, Point& posRel, Scene *scene, double innerForce,  const char *filename, 
		       int index, CgSVShader * shader, int nbSkeletons, const char *wickFileName):
  FireSource (s, 1, posRel, scene, innerForce, filename, index, shader)
{
  m_flames[0] = new LineFlame(s, nbSkeletons, posRel, innerForce, scene, _("textures/firmalampe.png"), wickFileName);
}

Torch::Torch(Solver * s, Point& posRel, Scene *scene, double innerForce, const char *torchName, 
	     int index, CgSVShader * shader, int nbSkeletons):
  FireSource (s, 0, posRel, scene, innerForce, torchName, index, shader, TORCH_NAME)
{
  vector<string> objList;
  int i=0;
  
  scene->getObjectsNameFromOBJ(torchName, objList, WICK_NAME_PREFIX);
  
  m_nbFlames = objList.size();
  m_flames = new BasicFlame* [m_nbFlames];
  
  for (vector < string >::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      m_flames[i] = new LineFlame(s, nbSkeletons, posRel, innerForce, scene, _("textures/torch2.png"), 
				  torchName, (*objListIterator).c_str());
    }
}

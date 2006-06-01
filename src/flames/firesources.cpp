#include "firesources.hpp"

#include "../scene/graphicsFn.hpp"
#include "../scene/scene.hpp"

#include <vector>
#include <string>

#define WICK_NAME_PREFIX "Wick"
#define TORCH_NAME "Torch"

Candle::Candle (FlameConfig *flameConfig, Solver * s, Scene *scene, const char *filename, uint index, 
		CgSVShader * shader, double rayon):
  FireSource (flameConfig, s, 1, scene, filename, index, shader)
	       //   cgCandleVertexShader (_("bougieShader.cg"),_("vertCandle"),context),
	       //   cgCandleFragmentShader (_("bougieShader.cg"),_("fragCandle"),context)
{
  m_flames[0] = new PointFlame(flameConfig, s, rayon);
}

Firmalampe::Firmalampe(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *filename, uint index,
		       CgSVShader * shader, const char *wickFileName):
  FireSource (flameConfig, s, 1, scene, filename, index, shader)
{
  m_flames[0] = new LineFlame( flameConfig, scene, _("textures/firmalampe.png"), s, wickFileName);
}

Torch::Torch(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *torchName, uint index, CgSVShader * shader):
  FireSource (flameConfig, s, 0, scene, torchName, index, shader, TORCH_NAME)
{
  vector<string> objList;
  int i=0;
  
  scene->getObjectsNameFromOBJ(torchName, objList, WICK_NAME_PREFIX);
  
  m_nbFlames = objList.size();
  m_flames = new BasicFlame* [m_nbFlames];
  
  for (vector < string >::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      m_flames[i] = new LineFlame( flameConfig, scene, _("textures/torch2.png"), s, torchName, (*objListIterator).c_str());
    }
}

CampFire::CampFire(FlameConfig *flameConfig, Solver * s, Scene *scene, const char *fireName, uint index, CgSVShader * shader ):
  FireSource (flameConfig, s, 0, scene, fireName, index, shader, TORCH_NAME)
{
  vector<string> objList;
  int i=0;
  
  scene->getObjectsNameFromOBJ(fireName, objList, WICK_NAME_PREFIX);
  
  m_nbFlames = objList.size();
  m_flames = new BasicFlame* [m_nbFlames];
  
  for (vector < string >::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      m_flames[i] = new LineFlame(flameConfig, scene, _("textures/torch2.png"), s, fireName, (*objListIterator).c_str());
    }
}

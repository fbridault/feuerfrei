#include "firesources.hpp"

#include "../scene/graphicsFn.hpp"
#include "../scene/scene.hpp"

#include <vector>
#include <string>

#define WICKPREFIX "Wick"

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
  m_flames[0] = new LineFlame(s, nbSkeletons, posRel, innerForce, scene, wickFileName);
}

Torch::Torch(Solver * s, Point& posRel, Scene *scene, double innerForce,  const char *filename, 
	     int index, CgSVShader * shader, int nbSkeletons, const char *torchName):
  FireSource (s, 0, posRel, scene, innerForce, filename, index, shader)
{
  char mtlName[255];
  vector<string> objList;
  int i=0;
  
  if(scene->getMTLFileNameFromOBJ(torchName, mtlName)){
    cerr << torchName << " utilise le fichier MTL " << mtlName << endl;
    AS_ERROR(chdir("./scenes"),"chdir scenes dans getMTLFileNameFromOBJ");
    scene->importMTL(mtlName);
    chdir("..");
  }
  scene->getObjectsNameFromOBJ(torchName, objList);
  
  m_nbFlames = objList.size();
  m_flames = new BasicFlame* [m_nbFlames];
  
  for (vector < string >::iterator objListIterator = objList.begin ();
       objListIterator != objList.end (); objListIterator++, i++)
    {
      int len = strlen(WICKPREFIX);
      if(!strncmp(WICKPREFIX,(*objListIterator).c_str(),len))
	m_flames[i] = new LineFlame(s, nbSkeletons, posRel, innerForce, scene, torchName, (*objListIterator).c_str());
    }
}

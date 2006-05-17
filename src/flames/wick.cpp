#include "wick.hpp"

#include "../scene/graphicsFn.hpp"
#include "../scene/scene.hpp"

Wick::Wick (const char *wickFileName, int nb_lead_skeletons, Scene *scene, Point& offset, const char*wickName) : Object(scene, offset)
{
  Point bounds[nb_lead_skeletons + 1];
  Point MinBound (DBL_MAX, DBL_MAX, DBL_MAX), MaxBound (DBL_MIN, DBL_MIN, DBL_MIN);
  Point midDist, cellSpan;
  vector < Point * >pointsPartitionsArray[nb_lead_skeletons];
  u_char max; /* 0 -> x, 1 -> y, 2 -> z */
  
  /* Chargement de la mèche */
  cerr << "Chargement de la mèche du fichier " << wickFileName << "...";
  scene->importOBJ(wickFileName, this, true, wickName);
  
  /*******************************/
  /* Création de la display list */
  /*******************************/
  m_wickDisplayList = glGenLists(1);
  glNewList (m_wickDisplayList, GL_COMPILE);
  //glEnable (GL_LIGHTING);
  draw (ALL,true);
  //glDisable (GL_LIGHTING);
  glEndList ();
  
  /*****************************************************************************/
  /* Création des points qui vont servir d'origines pour les squelettes guides */
  /*****************************************************************************/
  
  /* Parcours des points */
  /* La bounding box est délimitée par les points ptMax[nb_lead_skeletons] et ptMin[0] */
  getBoundingBox (bounds[nb_lead_skeletons], bounds[0]);
    
  /* Découpage de la bounding box en nb_lead_skeletons partitions */
  midDist = (bounds[nb_lead_skeletons] - bounds[0]) / (nb_lead_skeletons);
  cellSpan = bounds[nb_lead_skeletons] - bounds[0];
  
  if(midDist.x > midDist.y)
    if(midDist.x > midDist.z)
      /* Découpage en x */
      max=0;
    else
      max=2;
  else
    if(midDist.y > midDist.z)
      /* Découpage en y */
      max=1;
    else
      max=2;
  
  switch(max){
  case 0 : 
      for (int i = 1; i < nb_lead_skeletons; i++){
	bounds[i] = bounds[i-1];
	bounds[i].x += midDist.x;
      }
      cellSpan.x=midDist.x;
      
      for (vector < Point * >::iterator vertexIterator = m_vertexArray.begin ();
	   vertexIterator != m_vertexArray.end (); vertexIterator++)
	{
	  /* Calcul du max */
	  if ((*vertexIterator)->x >= MaxBound.x)
	    MaxBound = *(*vertexIterator);
	  /* Calcul du min */
	  if ((*vertexIterator)->x <= MinBound.x)
	    MinBound = *(*vertexIterator);
	}
      //       cerr << "Découpe en x" << endl;
      break;
 case 1 :
      /* Découpage en y */      
      for (int i = 1; i < nb_lead_skeletons; i++){
	bounds[i] = bounds[i-1];
	bounds[i].y += midDist.y;
      }
      cellSpan.y=midDist.y;
      
      for (vector < Point * >::iterator vertexIterator = m_vertexArray.begin ();
	   vertexIterator != m_vertexArray.end (); vertexIterator++)
	{
	  /* Calcul du max */
	  if ((*vertexIterator)->y >= MaxBound.y)
	    MaxBound = *(*vertexIterator);
	  /* Calcul du min */
	  if ((*vertexIterator)->y <= MinBound.y)
	    MinBound = *(*vertexIterator);
	}
      //       cerr << "Découpe en y" << endl;
      break;
  case 2 :
      /* Découpage en z */      
      for (int i = 1; i < nb_lead_skeletons; i++){
	bounds[i] = bounds[i-1];
	bounds[i].z += midDist.z;
      }
      cellSpan.z=midDist.z;

      for (vector < Point * >::iterator vertexIterator = m_vertexArray.begin ();
	   vertexIterator != m_vertexArray.end (); vertexIterator++)
	{
	  /* Calcul du max */
	  if ((*vertexIterator)->z >= MaxBound.z)
	    MaxBound = *(*vertexIterator);
	  /* Calcul du min */
	  if ((*vertexIterator)->z <= MinBound.z)
	    MinBound = *(*vertexIterator);
	}
//       cerr << "Découpe en z" << endl;
      break;
  }
  
//    cerr << nb_lead_skeletons << endl;
//    for (int i = 0; i <= nb_lead_skeletons; i++)
//      cerr << bounds[i] << endl;
//    cerr << "CellSpan " << cellSpan << endl;
  
  m_boxesDisplayList=glGenLists(1);
  glNewList (m_boxesDisplayList, GL_COMPILE);
  glColor3f(1.0,1.0,1.0);
//   glBegin(GL_LINE_LOOP);
//   glVertex3f(bounds[0].x,bounds[0].y,bounds[0].z);
//   glVertex3f(bounds[0].x,bounds[0].y,bounds[nb_lead_skeletons].z);
//   glVertex3f(bounds[0].x,bounds[nb_lead_skeletons].y,bounds[nb_lead_skeletons].z);
//   glVertex3f(bounds[0].x,bounds[nb_lead_skeletons].y,bounds[0].z);
//   glEnd();
//   glBegin(GL_LINE_LOOP);
//   glVertex3f(bounds[nb_lead_skeletons].x,bounds[0].y,bounds[0].z);
//   glVertex3f(bounds[nb_lead_skeletons].x,bounds[0].y,bounds[nb_lead_skeletons].z);
//   glVertex3f(bounds[nb_lead_skeletons].x,bounds[nb_lead_skeletons].y,bounds[nb_lead_skeletons].z);
//   glVertex3f(bounds[nb_lead_skeletons].x,bounds[nb_lead_skeletons].y,bounds[0].z);
//   glEnd();
//   glBegin(GL_LINES);
//   glVertex3f(bounds[0].x,bounds[0].y,bounds[0].z);
//   glVertex3f(bounds[nb_lead_skeletons].x,bounds[0].y,bounds[0].z);
//   glVertex3f(bounds[0].x,bounds[nb_lead_skeletons].y,bounds[0].z);
//   glVertex3f(bounds[nb_lead_skeletons].x,bounds[nb_lead_skeletons].y,bounds[0].z);
//   glVertex3f(bounds[0].x,bounds[0].y,bounds[nb_lead_skeletons].z);
//   glVertex3f(bounds[nb_lead_skeletons].x,bounds[0].y,bounds[nb_lead_skeletons].z);
//   glVertex3f(bounds[0].x,bounds[nb_lead_skeletons].y,bounds[nb_lead_skeletons].z);
//   glVertex3f(bounds[nb_lead_skeletons].x,bounds[nb_lead_skeletons].y,bounds[nb_lead_skeletons].z);
//   glEnd();
  for (int i = 0; i < nb_lead_skeletons; i++){
    glColor3f(0.0,i*1.0/(double)nb_lead_skeletons,1.0);
    Point bounds2 = bounds[i]+cellSpan;
    glBegin(GL_LINE_LOOP);
    glVertex3f(bounds[i].x,bounds[i].y,bounds[i].z);
    glVertex3f(bounds[i].x,bounds[i].y,bounds2.z);
    glVertex3f(bounds[i].x,bounds2.y,bounds2.z);
    glVertex3f(bounds[i].x,bounds2.y,bounds[i].z);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(bounds2.x,bounds[i].y,bounds[i].z);
    glVertex3f(bounds2.x,bounds[i].y,bounds2.z);
    glVertex3f(bounds2.x,bounds2.y,bounds2.z);
    glVertex3f(bounds2.x,bounds2.y,bounds[i].z);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(bounds[i].x,bounds[i].y,bounds[i].z);
    glVertex3f(bounds2.x,bounds[i].y,bounds[i].z);
    glVertex3f(bounds[i].x,bounds2.y,bounds[i].z);
    glVertex3f(bounds2.x,bounds2.y,bounds[i].z);
    glVertex3f(bounds[i].x,bounds[i].y,bounds2.z);
    glVertex3f(bounds2.x,bounds[i].y,bounds2.z);
    glVertex3f(bounds[i].x,bounds2.y,bounds2.z);
    glVertex3f(bounds2.x,bounds2.y,bounds2.z);
    glEnd();
  }
  glEndList();
  
  /* Tri des points dans les partitions */
  /* Il serait possible de faire un tri par dichotomie */
  /* pour aller un peu plus vite */
  for (vector < Point * >::iterator vertexIterator = m_vertexArray.begin ();
       vertexIterator != m_vertexArray.end (); vertexIterator++)
    for (int i = 0; i < nb_lead_skeletons; i++){
      Point bounds2 = bounds[i]+cellSpan;
      if ((*vertexIterator)->x > bounds[i].x &&
	  (*vertexIterator)->y > bounds[i].y &&
	  (*vertexIterator)->z > bounds[i].z &&
	  (*vertexIterator)->x < bounds2.x &&
	  (*vertexIterator)->y < bounds2.y &&
	  (*vertexIterator)->z < bounds2.z)
	pointsPartitionsArray[i].push_back (*vertexIterator);
    }
  /* Création des leadPoints */
  /* On prend simplement le barycentre de chaque partition */
  m_leadPointsArray.push_back (new WickPoint(MinBound,-1));
  
  for (int i = 0; i < nb_lead_skeletons; i++)
    {
      Point barycentre;
      int n;
      
      if (!pointsPartitionsArray[i].empty ())
 	{
	  barycentre.resetToNull ();
	  n = 0;
	  for (vector < Point * >::iterator pointsIterator = pointsPartitionsArray[i].begin ();
	       pointsIterator != pointsPartitionsArray[i].end (); pointsIterator++)
	    {
	      barycentre.x += (*pointsIterator)->x;
	      barycentre.y += (*pointsIterator)->y;
	      barycentre.z += (*pointsIterator)->z;
	      n++;
	    }
	  barycentre = barycentre / (double)n;
	  
	  m_leadPointsArray.push_back (new WickPoint (barycentre, 2*(i+1)/(double)(nb_lead_skeletons+1)-1));
// 	  cerr << 2*(i+1)/(double)(nb_lead_skeletons+1)-1 << endl;
 	}
      else
	 cerr << "Partition " << i << " vide" << endl;
    }
  m_leadPointsArray.push_back (new WickPoint(MaxBound,1));
  cerr << "Terminé" << endl;  
}

Wick::~Wick ()
{
  for (vector < WickPoint * >::iterator pointsIterator = m_leadPointsArray.begin ();
       pointsIterator != m_leadPointsArray.end (); pointsIterator++)
    delete (*pointsIterator);
  m_leadPointsArray.clear ();
}

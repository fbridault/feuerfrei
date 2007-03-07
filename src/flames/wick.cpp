#include "wick.hpp"

#include "../scene/graphicsFn.hpp"
#include "../scene/scene.hpp"

Wick::Wick (const char *wickFileName, const FlameConfig* const flameConfig, Scene* const scene, 
	    vector< LeadSkeleton * >& leadSkeletons, Field3D* const solver, const char*wickName) :
  Object(scene)
{
  Point bounds[flameConfig->skeletonsNumber + 1];
  Point MinBound (DBL_MAX, DBL_MAX, DBL_MAX), MaxBound (DBL_MIN, DBL_MIN, DBL_MIN);
  Point midDist, cellSpan;
  vector < Point * >pointsPartitionsArray[flameConfig->skeletonsNumber];
  u_char max; /* 0 -> x, 1 -> y, 2 -> z */
  
  /* Chargement de la mèche */
  cerr << "Chargement de la mèche du fichier " << wickFileName << "...";
  scene->importOBJ(wickFileName, this, wickName);
  
  /* Création du VBO */
  //glEnable (GL_LIGHTING);
  buildVBO();
  //glDisable (GL_LIGHTING);
  
  /*****************************************************************************/
  /* Création des points qui vont servir d'origines pour les squelettes guides */
  /*****************************************************************************/
  
  /* Parcours des points */
  /* La bounding box est délimitée par les points ptMax[flameConfig->skeletonsNumber] et ptMin[0] */
  getBoundingBox (bounds[flameConfig->skeletonsNumber], bounds[0]);
    
  /* Découpage de la bounding box en flameConfig->skeletonsNumber partitions */
  midDist = (bounds[flameConfig->skeletonsNumber] - bounds[0]) / (flameConfig->skeletonsNumber);
  cellSpan = bounds[flameConfig->skeletonsNumber] - bounds[0];
  
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
    for (int i = 1; i < flameConfig->skeletonsNumber; i++){
      bounds[i] = bounds[i-1];
      bounds[i].x += midDist.x;
    }
    cellSpan.x=midDist.x;
    
    for (vector < Vertex >::iterator vertexIterator = m_vertexArray.begin ();
	 vertexIterator != m_vertexArray.end (); vertexIterator++)
      {
	/* Calcul du max */
	if (vertexIterator->x >= MaxBound.x)
	  MaxBound = Point(vertexIterator->x, vertexIterator->y, vertexIterator->z);
	/* Calcul du min */
	if (vertexIterator->x <= MinBound.x)
	  MinBound = Point(vertexIterator->x, vertexIterator->y, vertexIterator->z);
      }
    //       cerr << "Découpe en x" << endl;
    break;
  case 1 :
    /* Découpage en y */      
    for (int i = 1; i < flameConfig->skeletonsNumber; i++){
      bounds[i] = bounds[i-1];
      bounds[i].y += midDist.y;
    }
    cellSpan.y=midDist.y;
    
    for (vector < Vertex >::iterator vertexIterator = m_vertexArray.begin ();
	 vertexIterator != m_vertexArray.end (); vertexIterator++)
      {
	/* Calcul du max */
	if (vertexIterator->y >= MaxBound.y)
	  MaxBound = Point(vertexIterator->x, vertexIterator->y, vertexIterator->z);
	/* Calcul du min */
	if (vertexIterator->y <= MinBound.y)
	  MinBound = Point(vertexIterator->x, vertexIterator->y, vertexIterator->z);
      }
    //       cerr << "Découpe en y" << endl;
    break;
  case 2 :
    /* Découpage en z */      
    for (int i = 1; i < flameConfig->skeletonsNumber; i++){
      bounds[i] = bounds[i-1];
      bounds[i].z += midDist.z;
    }
    cellSpan.z=midDist.z;

    for (vector < Vertex >::iterator vertexIterator = m_vertexArray.begin ();
	 vertexIterator != m_vertexArray.end (); vertexIterator++)
      {
	/* Calcul du max */
	if (vertexIterator->z >= MaxBound.z)
	  MaxBound = Point(vertexIterator->x, vertexIterator->y, vertexIterator->z);
	/* Calcul du min */
	if (vertexIterator->z <= MinBound.z)
	  MinBound = Point(vertexIterator->x, vertexIterator->y, vertexIterator->z);
      }
    //       cerr << "Découpe en z" << endl;
    break;
  }
  
  //    cerr << flameConfig->skeletonsNumber << endl;
  //    for (int i = 0; i <= flameConfig->skeletonsNumber; i++)
  //      cerr << bounds[i] << endl;
  //    cerr << "CellSpan " << cellSpan << endl;
  
  m_boxesDisplayList=glGenLists(1);
  glNewList (m_boxesDisplayList, GL_COMPILE);
  //   glColor3f(1.0,1.0,1.0);
  //   glBegin(GL_LINE_LOOP);
  //   glVertex3f(bounds[0].x,bounds[0].y,bounds[0].z);
  //   glVertex3f(bounds[0].x,bounds[0].y,bounds[flameConfig->skeletonsNumber].z);
  //   glVertex3f(bounds[0].x,bounds[flameConfig->skeletonsNumber].y,bounds[flameConfig->skeletonsNumber].z);
  //   glVertex3f(bounds[0].x,bounds[flameConfig->skeletonsNumber].y,bounds[0].z);
  //   glEnd();
  //   glBegin(GL_LINE_LOOP);
  //   glVertex3f(bounds[flameConfig->skeletonsNumber].x,bounds[0].y,bounds[0].z);
  //   glVertex3f(bounds[flameConfig->skeletonsNumber].x,bounds[0].y,bounds[flameConfig->skeletonsNumber].z);
  //   glVertex3f(bounds[flameConfig->skeletonsNumber].x,bounds[flameConfig->skeletonsNumber].y,bounds[flameConfig->skeletonsNumber].z);
  //   glVertex3f(bounds[flameConfig->skeletonsNumber].x,bounds[flameConfig->skeletonsNumber].y,bounds[0].z);
  //   glEnd();
  //   glBegin(GL_LINES);
  //   glVertex3f(bounds[0].x,bounds[0].y,bounds[0].z);
  //   glVertex3f(bounds[flameConfig->skeletonsNumber].x,bounds[0].y,bounds[0].z);
  //   glVertex3f(bounds[0].x,bounds[flameConfig->skeletonsNumber].y,bounds[0].z);
  //   glVertex3f(bounds[flameConfig->skeletonsNumber].x,bounds[flameConfig->skeletonsNumber].y,bounds[0].z);
  //   glVertex3f(bounds[0].x,bounds[0].y,bounds[flameConfig->skeletonsNumber].z);
  //   glVertex3f(bounds[flameConfig->skeletonsNumber].x,bounds[0].y,bounds[flameConfig->skeletonsNumber].z);
  //   glVertex3f(bounds[0].x,bounds[flameConfig->skeletonsNumber].y,bounds[flameConfig->skeletonsNumber].z);
  //   glVertex3f(bounds[flameConfig->skeletonsNumber].x,bounds[flameConfig->skeletonsNumber].y,bounds[flameConfig->skeletonsNumber].z);
  //   glEnd();
  for (int i = 0; i < flameConfig->skeletonsNumber; i++){
    glColor3f(0.0,i*1.0/(double)flameConfig->skeletonsNumber,1.0);
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
  
  /* Tri des points pour les ranger dans les partitions */
  /* Il serait possible de faire un tri par dichotomie */
  /* pour aller un peu plus vite */

  for (vector < Vertex >::iterator vertexIterator = m_vertexArray.begin ();
       vertexIterator != m_vertexArray.end (); vertexIterator++)
    for (int i = 0; i < flameConfig->skeletonsNumber; i++){
      Point bounds2 = bounds[i]+cellSpan;
      if (vertexIterator->x > bounds[i].x &&
	  vertexIterator->y > bounds[i].y &&
	  vertexIterator->z > bounds[i].z &&
	  vertexIterator->x < bounds2.x &&
	  vertexIterator->y < bounds2.y &&
	  vertexIterator->z < bounds2.z)
	pointsPartitionsArray[i].push_back (new Point(vertexIterator->x, vertexIterator->y, vertexIterator->z));
    }

  Point rootMoveFactorL(2,.1,1);
  /* Création des leadSkeletons */
  /* On prend simplement le barycentre de chaque partition */
  leadSkeletons.push_back (new LeadSkeleton(solver, MinBound, rootMoveFactorL, flameConfig, -1, .5, -.2, .3));
  
  for (int i = 0; i < flameConfig->skeletonsNumber; i++)
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
	  
	  leadSkeletons.push_back (new LeadSkeleton(solver, barycentre, rootMoveFactorL, flameConfig, 2*(i+1)/(double)(flameConfig->skeletonsNumber+1)-1, .5, -.2, .3));
 	}
      else
	cerr << "Warning ! Wick partition #" << i << " is empty" << endl;
    }
  leadSkeletons.push_back (new LeadSkeleton(solver, MaxBound, rootMoveFactorL, flameConfig, 1, .5, -.2, .3));

  /* Suppression des points */
  for (int i = 0; i < flameConfig->skeletonsNumber; i++)
    {
      for (vector < Point * >::iterator pointsIterator = pointsPartitionsArray[i].begin ();
	   pointsIterator != pointsPartitionsArray[i].end (); pointsIterator++)
	delete (*pointsIterator);
      pointsPartitionsArray[i].clear();
    }
  cerr << "Terminé" << endl;
}

Wick::~Wick ()
{
}

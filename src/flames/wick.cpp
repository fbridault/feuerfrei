#include "wick.hpp"

#include "../scene/graphicsFn.hpp"
#include "../scene/scene.hpp"

Wick::Wick (const char *wickFileName, FlameConfig *flameConfig, Scene *scene, 
	    vector< LeadSkeleton * >& leadSkeletons, Field3D *solver, const char*wickName) :
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
    
    for (uint i = 0; i < m_nbVertex; i+=3){
      /* Calcul du max */
      if (m_vertexArray[i] >= MaxBound.x)
	MaxBound = Point(m_vertexArray[i], m_vertexArray[i+1], m_vertexArray[i+2]);
	/* Calcul du min */
      if (m_vertexArray[i] <= MinBound.x)
	MinBound = Point(m_vertexArray[i], m_vertexArray[i+1], m_vertexArray[i+2]);
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
    
    for (uint i = 0; i < m_nbVertex; i+=3){
      /* Calcul du max */
      if (m_vertexArray[i+1] >= MaxBound.y)
	MaxBound = Point(m_vertexArray[i], m_vertexArray[i+1], m_vertexArray[i+2]);
	/* Calcul du min */
      if (m_vertexArray[i+1] <= MinBound.y)
	MinBound = Point(m_vertexArray[i], m_vertexArray[i+1], m_vertexArray[i+2]);
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

    for (uint i = 0; i < m_nbVertex; i+=3){
      /* Calcul du max */
      if (m_vertexArray[i+2] >= MaxBound.z)
	MaxBound = Point(m_vertexArray[i], m_vertexArray[i+1], m_vertexArray[i+2]);
	/* Calcul du min */
      if (m_vertexArray[i+2] <= MinBound.z)
	MinBound = Point(m_vertexArray[i], m_vertexArray[i+1], m_vertexArray[i+2]);
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
  for (uint j=0; j < m_nbVertex; j+=3)
    for (int i = 0; i < flameConfig->skeletonsNumber; i++){
      Point bounds2 = bounds[i]+cellSpan;
      if (m_vertexArray[j] > bounds[i].x &&
	  m_vertexArray[j+1] > bounds[i].y &&
	  m_vertexArray[j+2] > bounds[i].z &&
	  m_vertexArray[j] < bounds2.x &&
	  m_vertexArray[j+1] < bounds2.y &&
	  m_vertexArray[j+2] < bounds2.z)
	pointsPartitionsArray[i].push_back (new Point(m_vertexArray[j], m_vertexArray[j+1], m_vertexArray[j+2]));
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
  cerr << "Terminé" << endl;  
}

Wick::~Wick ()
{
}

#include "wick.hpp"

#include "../scene/graphicsFn.hpp"
#include "../scene/scene.hpp"

Wick::Wick (const char *wickFileName, int nb_lead_skeletons, Scene *scene, Point& offset, const char*wickName) : Object(scene, offset)
{
  Point ptMax[nb_lead_skeletons + 1], ptMin[nb_lead_skeletons + 1];
  Point ExtrGauche (DBL_MAX, 0, 0), ExtrDroite (DBL_MIN, 0, 0);
  vector < Point * >pointsPartitionsArray[nb_lead_skeletons];

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
  cerr << "Terminé" << endl;

  /*****************************************************************************/
  /* Création des points qui vont servir d'origines pour les squelettes guides */
  /*****************************************************************************/

  /* Parcours des points */
  /* La bounding box est délimitée par les points ptMax[nb_lead_skeletons] et ptMin[0] */
  getBoundingBox (ptMax[nb_lead_skeletons], ptMin[0]);

  for (vector < Point * >::iterator vertexIterator = m_vertexArray.begin ();
       vertexIterator != m_vertexArray.end (); vertexIterator++)
    {
      /* Calcul du max */
      if ((*vertexIterator)->x > ExtrDroite.x)
	ExtrDroite = *(*vertexIterator);
      /* Calcul du min */
      if ((*vertexIterator)->x < ExtrGauche.x)
	ExtrGauche = *(*vertexIterator);
    }
  
  /* Découpage de la bounding box en nb_lead_skeletons partitions selon l'axe X */
  for (int i = 1; i <= nb_lead_skeletons; i++)
    {
      ptMin[i].x = ptMin[0].x + i * (ptMax[nb_lead_skeletons].x - ptMin[0].x) / nb_lead_skeletons;
      ptMin[i].y = ptMin[0].y;
      ptMin[i].z = ptMin[0].z;
    }
  for (int i = 0; i < nb_lead_skeletons; i++)
    {
      ptMax[i].x = ptMin[0].x + i * (ptMax[nb_lead_skeletons].x - ptMin[0].x) / nb_lead_skeletons;
      ptMax[i].y = ptMax[nb_lead_skeletons].y;
      ptMax[i].z = ptMax[nb_lead_skeletons].z;
    }

  /* Tri des points dans les partitions */
  /* Il serait possible de faire un tri par dichotomie */
  /* pour aller un peu plus vite */
  for (vector < Point * >::iterator vertexIterator = m_vertexArray.begin ();
       vertexIterator != m_vertexArray.end (); vertexIterator++)
    for (int i = 1; i <= nb_lead_skeletons; i++)
      if ((*vertexIterator)->x > ptMin[i-1].x &&
	  (*vertexIterator)->y > ptMin[i-1].y &&
	  (*vertexIterator)->z > ptMin[i-1].z &&
	  (*vertexIterator)->x < ptMax[i].x &&
	  (*vertexIterator)->y < ptMax[i].y &&
	  (*vertexIterator)->z < ptMax[i].z)
	pointsPartitionsArray[i-1].push_back (*vertexIterator);
  
  /* Création des leadPoints */
  /* On prend simplement le barycentre de chaque partition */
  m_leadPointsArray.push_back (new Point(ExtrGauche));
  
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
	  
	  m_leadPointsArray.push_back (new Point (barycentre));
	}
    }
  m_leadPointsArray.push_back (new Point(ExtrDroite));  
}

Wick::~Wick ()
{
  for (vector < Point * >::iterator pointsIterator = m_leadPointsArray.begin ();
       pointsIterator != m_leadPointsArray.end (); pointsIterator++)
    delete (*pointsIterator);
  m_leadPointsArray.clear ();
}

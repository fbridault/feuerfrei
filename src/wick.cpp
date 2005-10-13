/***************************************************************************
 *            Wick.cpp
 *
 *  Tue Mar 29 16:22:08 2005
 *  Copyright  2005  Flavien Bridault
 *  bridault@lil.univ-littoral.fr
 ****************************************************************************/
#include "wick.hpp"
#include "graphicsFn.hpp"
#include "OBJReader.hpp"

Wick::Wick (char *filename, int nb_lead_squelettes)
{
  CPoint ptMax[nb_lead_squelettes + 1], ptMin[nb_lead_squelettes + 1];
  CPoint ExtrGauche (DBL_MAX, 0, 0), ExtrDroite (DBL_MIN, 0, 0);
  vector < CPoint * >pointsPartitionsArray[nb_lead_squelettes];

  /* Chargement de la mèche */
  cout << "Chargement de la mèche du fichier " << filename << "...";
  importOBJFile2Scene (filename, this);

  /*******************************/
  /* Création de la display list */
  /*******************************/
  glNewList (MECHE, GL_COMPILE);
  //glEnable (GL_LIGHTING);
  draw (ALL,true);
  //glDisable (GL_LIGHTING);
  glEndList ();
  cout << "Terminé" << endl;

  /*****************************************************************************/
  /* Création des points qui vont servir d'origines pour les squelettes guides */
  /*****************************************************************************/

  /* Parcours des points */
  /* La bouding box est délimitée par les points ptMax[nb_lead_squelettes] et ptMin[0] */
  getBoundingBox (ptMax[nb_lead_squelettes], ptMin[0]);

  for (vector < CPoint * >::iterator vertexIterator = vertexArray.begin ();
       vertexIterator != vertexArray.end (); vertexIterator++)
    {
      /* Calcul du max */
      if ((*vertexIterator)->getX () > ExtrDroite.getX ())
	ExtrDroite = *(*vertexIterator);
      /* Calcul du min */
      if ((*vertexIterator)->getX () < ExtrGauche.getX ())
	ExtrGauche = *(*vertexIterator);
    }
  
  /* Découpage de la bounding box en nb_lead_squelettes partitions selon l'axe X */
  for (int i = 1; i <= nb_lead_squelettes; i++)
    {
      ptMin[i].setX (ptMin[0].getX () +
		     i * (ptMax[nb_lead_squelettes].getX () -
			  ptMin[0].getX ()) / nb_lead_squelettes);
      ptMin[i].setY (ptMin[0].getY ());
      ptMin[i].setZ (ptMin[0].getZ ());
    }
  for (int i = 0; i < nb_lead_squelettes; i++)
    {
      ptMax[i].setX (ptMin[0].getX () +
		     i * (ptMax[nb_lead_squelettes].getX () -
			  ptMin[0].getX ()) / nb_lead_squelettes);
      ptMax[i].setY (ptMax[nb_lead_squelettes].getY ());
      ptMax[i].setZ (ptMax[nb_lead_squelettes].getZ ());
    }

  /* Tri des points dans les partitions */
  /* Il serait possible de faire un tri par dichotomie */
  /* pour aller un peu plus vite */
  for (vector < CPoint * >::iterator vertexIterator = vertexArray.begin ();
       vertexIterator != vertexArray.end (); vertexIterator++)
    for (int i = 1; i <= nb_lead_squelettes; i++)
      if ((*vertexIterator)->getX () > ptMin[i-1].getX () &&
	  (*vertexIterator)->getY () > ptMin[i-1].getY () &&
	  (*vertexIterator)->getZ () > ptMin[i-1].getZ () &&
	  (*vertexIterator)->getX () < ptMax[i].getX () &&
	  (*vertexIterator)->getY () < ptMax[i].getY () &&
	  (*vertexIterator)->getZ () < ptMax[i].getZ ())
	pointsPartitionsArray[i-1].push_back (*vertexIterator);
  
  /* Création des leadPoints */
  /* On prend simplement le barycentre de chaque partition */
  leadPointsArray.push_back (new CPoint(ExtrGauche));
			
  for (int i = 0; i < nb_lead_squelettes; i++)
    {
      CPoint barycentre;
      int n;

      if (!pointsPartitionsArray[i].empty ())
	{
	  barycentre.resetToNull ();
	  n = 0;
	  for (vector < CPoint * >::iterator pointsIterator =
		 pointsPartitionsArray[i].begin ();
	       pointsIterator !=
		 pointsPartitionsArray[i].end ();
	       pointsIterator++)
	    {
	      barycentre.addX ((*pointsIterator)->getX ());
	      barycentre.addY ((*pointsIterator)->getY ());
	      barycentre.addZ ((*pointsIterator)->getZ ());
	      n++;
	    }
	  barycentre = barycentre / (float)n;

	  leadPointsArray.push_back (new CPoint (barycentre));
	}
    }
  leadPointsArray.push_back (new CPoint(ExtrDroite));
}

Wick::~Wick ()
{
  for (vector < CPoint * >::iterator pointsIterator =
	 leadPointsArray.begin ();
       pointsIterator != leadPointsArray.end (); pointsIterator++)
    delete (*pointsIterator);
  leadPointsArray.clear ();
}


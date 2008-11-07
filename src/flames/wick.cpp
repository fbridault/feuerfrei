#include "wick.hpp"

#include <engine/Utility/GraphicsFn.hpp>


uint CWick::buildPointFDF (FlameConfig const& flameConfig, vector< LeadSkeleton * >& leadSkeletons, Field3D* const field )
{
	CPoint bounds[2], barycentre;

	/* Création du VBO */
	buildVBO();

	/* La bounding box est délimitée par les points ptMax[flameConfig.skeletonsNumber] et ptMin[0] */
	getBoundingBox (bounds[1], bounds[0]);

	m_boxesDisplayList=glGenLists(1);
	glNewList (m_boxesDisplayList, GL_COMPILE);
	glColor3f(0.0f,1.0f,1.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(bounds[0].x,bounds[0].y,bounds[0].z);
	glVertex3f(bounds[0].x,bounds[0].y,bounds[1].z);
	glVertex3f(bounds[0].x,bounds[1].y,bounds[1].z);
	glVertex3f(bounds[0].x,bounds[1].y,bounds[0].z);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3f(bounds[1].x,bounds[0].y,bounds[0].z);
	glVertex3f(bounds[1].x,bounds[0].y,bounds[1].z);
	glVertex3f(bounds[1].x,bounds[1].y,bounds[1].z);
	glVertex3f(bounds[1].x,bounds[1].y,bounds[0].z);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(bounds[0].x,bounds[0].y,bounds[0].z);
	glVertex3f(bounds[1].x,bounds[0].y,bounds[0].z);
	glVertex3f(bounds[0].x,bounds[1].y,bounds[0].z);
	glVertex3f(bounds[1].x,bounds[1].y,bounds[0].z);
	glVertex3f(bounds[0].x,bounds[0].y,bounds[1].z);
	glVertex3f(bounds[1].x,bounds[0].y,bounds[1].z);
	glVertex3f(bounds[0].x,bounds[1].y,bounds[1].z);
	glVertex3f(bounds[1].x,bounds[1].y,bounds[1].z);
	glEnd();
	glEndList();

	CPoint rootMoveFactorL(2.0f,.2f,2.0f);

	barycentre.resetToNull ();
	for (vector < Vertex >::iterator vertexIterator = m_vertexArray.begin ();
	        vertexIterator != m_vertexArray.end (); vertexIterator++)
	{
		barycentre.x += (*vertexIterator).x;
		barycentre.y += (*vertexIterator).y;
		barycentre.z += (*vertexIterator).z;
	}
	barycentre = barycentre / (float)m_vertexArray.size();

	leadSkeletons.push_back (new LeadSkeleton(field, barycentre, rootMoveFactorL, flameConfig.leadLifeSpan, 1,  .5f, 0.0f, .025f));

	return 0;
}

uint CWick::buildFDF (FlameConfig const& flameConfig, vector< LeadSkeleton * >& leadSkeletons, Field3D* const field )
{
	/* Création du VBO */
	buildVBO();

	/*****************************************************************************/
	/* Création des points qui vont servir d'origines pour les squelettes guides */
	/*****************************************************************************/

	/* Récupération de la bounding box */
	CPoint bounds[flameConfig.skeletonsNumber + 1];
	getBoundingBox (bounds[flameConfig.skeletonsNumber], bounds[0]);

	CPoint midDist = (bounds[flameConfig.skeletonsNumber] - bounds[0]) / (flameConfig.skeletonsNumber);
	CPoint cellSpan = bounds[flameConfig.skeletonsNumber] - bounds[0];

	/* Détermination de l'axe de découpe, on prend le plus grand */
	u_char max; /* 0 -> x, 1 -> y, 2 -> z */
	if (midDist.x > midDist.y)
		if (midDist.x > midDist.z)
			/* Découpage en x */
			max=0;
		else
			/* Découpage en z */
			max=2;
	else
		if (midDist.y > midDist.z)
			/* Découpage en y */
			max=1;
		else
			/* Découpage en z */
			max=2;

	/* Découpage de la bounding box en flameConfig.skeletonsNumber partitions */
	CPoint MinBound (FLT_MAX, FLT_MAX, FLT_MAX), MaxBound (-FLT_MAX, -FLT_MAX, -FLT_MAX);

	switch (max)
	{
		case 0 :
			/* Découpage en x */
			for (uint i = 1; i < flameConfig.skeletonsNumber; i++)
			{
				bounds[i] = bounds[i-1];
				bounds[i].x += midDist.x;
			}
			cellSpan.x=midDist.x;

			for (vector < Vertex >::iterator vertexIterator = m_vertexArray.begin ();
			        vertexIterator != m_vertexArray.end (); vertexIterator++)
			{
				/* Calcul du max */
				if (vertexIterator->x >= MaxBound.x)
					MaxBound = CPoint(vertexIterator->x, vertexIterator->y, vertexIterator->z);
				/* Calcul du min */
				if (vertexIterator->x <= MinBound.x)
					MinBound = CPoint(vertexIterator->x, vertexIterator->y, vertexIterator->z);
			}
			//       cerr << "Découpe en x" << endl;
			break;
		case 1 :
			/* Découpage en y */
			for (uint i = 1; i < flameConfig.skeletonsNumber; i++)
			{
				bounds[i] = bounds[i-1];
				bounds[i].y += midDist.y;
			}
			cellSpan.y=midDist.y;

			for (vector < Vertex >::iterator vertexIterator = m_vertexArray.begin ();
			        vertexIterator != m_vertexArray.end (); vertexIterator++)
			{
				/* Calcul du max */
				if (vertexIterator->y >= MaxBound.y)
					MaxBound = CPoint(vertexIterator->x, vertexIterator->y, vertexIterator->z);
				/* Calcul du min */
				if (vertexIterator->y <= MinBound.y)
					MinBound = CPoint(vertexIterator->x, vertexIterator->y, vertexIterator->z);
			}
			//       cerr << "Découpe en y" << endl;
			break;
		case 2 :
			/* Découpage en z */
			for (uint i = 1; i < flameConfig.skeletonsNumber; i++)
			{
				bounds[i] = bounds[i-1];
				bounds[i].z += midDist.z;
			}
			cellSpan.z=midDist.z;

			for (vector < Vertex >::iterator vertexIterator = m_vertexArray.begin ();
			        vertexIterator != m_vertexArray.end (); vertexIterator++)
			{
				/* Calcul du max */
				if (vertexIterator->z >= MaxBound.z)
					MaxBound = CPoint(vertexIterator->x, vertexIterator->y, vertexIterator->z);
				/* Calcul du min */
				if (vertexIterator->z <= MinBound.z)
					MinBound = CPoint(vertexIterator->x, vertexIterator->y, vertexIterator->z);
			}
			//       cerr << "Découpe en z" << endl;
			break;
	}

	//    cerr << flameConfig.skeletonsNumber << endl;
	//    for (int i = 0; i <= flameConfig.skeletonsNumber; i++)
	//      cerr << bounds[i] << endl;
	//    cerr << "CellSpan " << cellSpan << endl;

	m_boxesDisplayList=glGenLists(1);
	glNewList (m_boxesDisplayList, GL_COMPILE);
	for (uint i = 0; i < flameConfig.skeletonsNumber; i++)
	{
		glColor3f(0.0f,i*1.0f/(float)flameConfig.skeletonsNumber,1.0f);
		CPoint bounds2 = bounds[i]+cellSpan;
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
	vector < CPoint * >pointsPartitionsArray[flameConfig.skeletonsNumber];

	for (vector < Vertex >::iterator vertexIterator = m_vertexArray.begin ();
	        vertexIterator != m_vertexArray.end (); vertexIterator++)
		for (uint i = 0; i < flameConfig.skeletonsNumber; i++)
		{
			CPoint bounds2 = bounds[i]+cellSpan;
			if (vertexIterator->x > bounds[i].x &&
			        vertexIterator->y > bounds[i].y &&
			        vertexIterator->z > bounds[i].z &&
			        vertexIterator->x < bounds2.x &&
			        vertexIterator->y < bounds2.y &&
			        vertexIterator->z < bounds2.z)
				pointsPartitionsArray[i].push_back (new CPoint(vertexIterator->x, vertexIterator->y, vertexIterator->z));
		}

	CPoint rootMoveFactorL(2.0f,.1f,1.0f);
	float noiseMin=-.1f;
	float noiseMax=.1f;
	float noiseInc=.5f;
	/* Création des leadSkeletons */
	/* On prend simplement le barycentre de chaque partition */
	leadSkeletons.push_back (new LeadSkeleton(field, MinBound, rootMoveFactorL, flameConfig.leadLifeSpan, -1, noiseInc, noiseMin, noiseMax));

	for (uint i = 0; i < flameConfig.skeletonsNumber; i++)
	{
		CPoint barycentre;

		/** CWick partition should not be empty */
		assert (pointsPartitionsArray[i].empty() == false);

		barycentre.resetToNull ();
		for (vector < CPoint * >::iterator pointsIterator = pointsPartitionsArray[i].begin ();
				pointsIterator != pointsPartitionsArray[i].end (); pointsIterator++)
		{
			barycentre.x += (*pointsIterator)->x;
			barycentre.y += (*pointsIterator)->y;
			barycentre.z += (*pointsIterator)->z;
		}
		barycentre = barycentre / (float)pointsPartitionsArray[i].size();

		/** Calcul de la valeur d'entrée de la fonction de distribution de carburant F(u). */
		float fu = 2*(i+1)/(float)(flameConfig.skeletonsNumber+1)-1;

		LeadSkeleton *pLeadSkeleton =
			new LeadSkeleton(field, barycentre,rootMoveFactorL, flameConfig.leadLifeSpan, fu, noiseInc,noiseMin, noiseMax);
		leadSkeletons.push_back (pLeadSkeleton);
	}
	leadSkeletons.push_back (new LeadSkeleton(field, MaxBound, rootMoveFactorL, flameConfig.leadLifeSpan, 1, noiseInc, noiseMin, noiseMax));

	/* Suppression des points */
	for (uint i = 0; i < flameConfig.skeletonsNumber; i++)
	{
		for (vector < CPoint * >::iterator pointsIterator = pointsPartitionsArray[i].begin ();
		        pointsIterator != pointsPartitionsArray[i].end (); pointsIterator++)
			delete (*pointsIterator);
		pointsPartitionsArray[i].clear();
	}
	//  buildFDF(field);
	assert(leadSkeletons.size() == flameConfig.skeletonsNumber+2);

	return (max);
}

// void CWick::buildFDF(Field3D* const field)
// {
//   CPoint h=field->getDim();

//   h.x /= field->getXRes();
//   h.y /= field->getYRes();
//   h.z /= field->getZRes();

//   for (uint k = 1; k <= field->getZRes(); k++)
//     for (uint j = field->getYRes(); j >= 1; j--)
//       for (uint i = 1; i <= field->getXRes(); i++)
// 	if(checkCPointsInVoxel(h,i,j,k))
// 	  {
// 	    if(j==1)
// 	      {
// 		cerr << "Voxel " << i << "," << j << "," << k << endl;
// 		break;
// 	      }
// 	    else
// 	      if(!checkCPointsInVoxel(h,i,j-1,k))
// 		{
// 		  cerr << "Voxel " << i << "," << j-1 << "," << k << endl;
// 		  break;
// 		}
// 	  }
// }

CWick::~CWick ()
{
}

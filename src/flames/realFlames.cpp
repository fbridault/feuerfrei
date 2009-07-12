#include "realFlames.hpp"

#include <engine/Utility/GraphicsFn.hpp>

#include "abstractFires.hpp"

/**********************************************************************************************************************/
/*************************************** IMPLEMENTATION DE LA CLASSE LINEFLAME ****************************************/
/**********************************************************************************************************************/

CLineFlame::CLineFlame (const FlameConfig& flameConfig, ITexture const& a_rTex, Field3D& a_rField,
												CWick *wickObject, float width, float detachedFlamesWidth, IDetachableFireSource *parentFire ) :
		IRealFlame ((flameConfig.skeletonsNumber+2)*2, 3, a_rTex)
{
	CPoint pt;
	uint i,j;
	uint sliceDirection;

	CPoint rootMoveFactorP(.5f,.1f,.5f);

	m_wick = wickObject;
	sliceDirection = m_wick->buildFDF(flameConfig, m_leadSkeletons, a_rField);
	assert(m_nbSkeletons == m_leadSkeletons.size()*2);

	m_nbLeadSkeletons = m_leadSkeletons.size();

	/** Allocation des squelettes périphériques = deux par squelette périphérique */
	/* plus 2 aux extrémités pour fermer la NURBS */
	/* FAIT DANS BASICFLAME DESORMAIS */

	/* Génération d'un côté des squelettes périphériques */
	for (i = 0; i < m_nbLeadSkeletons; i++)
	{
		assert(m_leadSkeletons[i] != NULL);

		pt = m_leadSkeletons[i]->getRoot();
		switch (sliceDirection)
		{
			case 0 :
				pt.z -= (width / 2.0f);
			case 1 :
				pt.z -= (width / 2.0f);
			case 2 :
				pt.x += (width / 2.0f);
		}
		m_periSkeletons[i] = new CPeriSkeleton (a_rField, pt, rootMoveFactorP, *m_leadSkeletons[i], flameConfig.periLifeSpan);
	}

	/* Génération de l'autre côté des squelettes périphériques */
	for ( j = m_nbLeadSkeletons, i = m_nbLeadSkeletons; j > 0; j--, i++)
	{
		assert(m_leadSkeletons[j-1] != NULL);
		pt = m_leadSkeletons[j-1]->getRoot();
		switch (sliceDirection)
		{
			case 0 :
				pt.z += (width / 2.0f);
			case 1 :
				pt.z += (width / 2.0f);
			case 2 :
				pt.x -= (width / 2.0f);
		}
		m_periSkeletons[i] = new CPeriSkeleton (a_rField, pt, rootMoveFactorP, *m_leadSkeletons[j-1], flameConfig.periLifeSpan);
	}

	m_parentFire = parentFire;

	m_detachedFlamesWidth = detachedFlamesWidth;
	m_samplingMethod = 1;
	m_vTexInit = 0;

	m_utexInc = 2.0f/(m_nbSkeletons);
}

CLineFlame::~CLineFlame ()
{
}

void CLineFlame::breakCheck()
{
	float fSplit,fProba;
	uint const uiThreshold=3;
	float const fDetachThreshold=.9f;
	/* Indice de la particule à laquelle un squelette est découpé */
	uint uiSplitHeight;
	CFreeLeadSkeleton **leadSkeletonsArray;
	CFreePeriSkeleton **periSkeletonsArray;

	for ( uint i = 0; i < m_nbLeadSkeletons; i++)
	{
		if (m_leadSkeletons[i]->getInternalSize() < uiThreshold)
			return;

		fProba = (rand()/((float)RAND_MAX));

		/* Tirage aléatoire entre 0 et 1 */
		if ( fProba > fDetachThreshold)
		{
			CPoint offset;

			/* Tirage entre 0.5 et 1 pour la hauteur du squelette */
			fSplit = (rand()/(2.0f*(float)RAND_MAX))+.5f;

			leadSkeletonsArray = new CFreeLeadSkeleton* [1];

			/* Create first lead skeleton */
			uiSplitHeight = (uint)(fSplit * (m_leadSkeletons[i]->getInternalSize()-1) );
			leadSkeletonsArray[0] = m_leadSkeletons[i]->split(uiSplitHeight);

			assert( leadSkeletonsArray[0] != NULL);
			CFreeLeadSkeleton& rLeadSkeleton = *leadSkeletonsArray[0];

			/* Create then peri skeleton */
			periSkeletonsArray = new CFreePeriSkeleton* [4];

			offset.x=-m_detachedFlamesWidth * fSplit;
			periSkeletonsArray[0] = leadSkeletonsArray[0]->dup(offset);

			uiSplitHeight = (uint)(fSplit * (m_periSkeletons[i+1]->getInternalSize()-1));
			periSkeletonsArray[1] = m_periSkeletons[i+1]->split(uiSplitHeight, rLeadSkeleton);

			offset.x=m_detachedFlamesWidth * fSplit;
			periSkeletonsArray[2] = leadSkeletonsArray[0]->dup(offset);

			uiSplitHeight = (uint)(fSplit * (m_periSkeletons[m_nbSkeletons-i-1]->getInternalSize()-1));
			periSkeletonsArray[3] = m_periSkeletons[m_nbSkeletons-i-1]->split(uiSplitHeight, rLeadSkeleton);

			CDetachedFlame *pDetachedFlame =
				new CDetachedFlame(this, 1, leadSkeletonsArray, 4, periSkeletonsArray, m_rTexture, m_shadingType, m_samplingMethod);
			m_parentFire->addDetachedFlame(pDetachedFlame);
		}
	}
}

void CLineFlame::computeVTexCoords()
{
	float vinc, vtmp;
	uint i;

	/* Si la flamme a un parent, c'est une flamme détachable, on fait varier la texcoord V dans le temps */
	if (m_parentFire)
		m_vTexInit = (m_vTexInit >= 0.85f) ? 0.0f : m_vTexInit + 0.15f;
	else
		m_vTexInit = 0.0;
	if (m_lodSkel == FULL_SKELETON)
		vinc = 1.0f / (float)(m_vsize-1);
	else
		vinc = 1.0f / (float)(m_vsize-1);

	vtmp = m_vTexInit;
	for (i = 0; i < m_vsize; i++)
	{
		m_texTmp[i] = vtmp;
		vtmp += vinc;
	}
}

bool CLineFlame::buildFlat ()
{
	uint i, j, l;
	float utex;
	float dist_max;
	m_maxParticles = 0;
	m_count = 0;
	utex = 0.0f;

	if (m_lodSkelChanged) changeSkeletonsLOD();

	/* Déplacement des squelettes guides */
	for (i = 0; i < m_nbLeadSkeletons; i++)
	{
		m_leadSkeletons[i]->move ();
		if (m_leadSkeletons[i]->getSize () > m_maxParticles)
			m_maxParticles = m_leadSkeletons[i]->getSize ();
	}

	/* On rajoute des particules si il n'y en a pas assez pour construire la NURBS) */
	if (m_maxParticles < m_vorder)
		m_maxParticles = m_vorder;

	/* Déplacement des squelettes périphériques et détermination du maximum de particules par squelette */
	for (i = 0; i < m_nbSkeletons; i++)
	{
		m_periSkeletons[i]->move ();
	}

	m_vsize = m_maxParticles + m_nbFixedPoints;

	/* On connaît la taille maximale d'un squelette, on peut maintenant déterminer les coordonnées de texture en v */
	computeVTexCoords();

	/* Direction des u */
	for (i = 0; i < m_nbLeadSkeletons; i++)
	{
		/* Problème pour la direction des v, le nombre de particules par squelettes n'est pas garanti */
		/* La solution retenue va ajouter des points de contrôles là où les points de contrôles sont le plus éloignés */
		if (m_leadSkeletons[i]->getSize () < m_maxParticles)
		{
			// Nombre de points de contrôle supplémentaires
			uint nb_pts_supp = m_maxParticles - m_leadSkeletons[i]->getSize ();
			CPoint pt;

			/* On calcule les distances entre les particules successives */
			/* On prend également en compte l'origine du squelette */
			/* On laisse les distances au carré pour des raisons évidentes de coût de calcul */
			for (j = 0; j < m_leadSkeletons[i]->getSize () - 1; j++)
				m_distances[j] =
				  m_leadSkeletons[i]->getParticle (j).squaredDistanceFrom(m_leadSkeletons[i]->getParticle (j + 1));

			m_distances[m_leadSkeletons[i]->getSize () - 1] =
			  m_leadSkeletons[i]->getLastParticle ().squaredDistanceFrom (m_leadSkeletons[i]->getRoot ());

			/* On cherche les indices des distances max */
			/* On n'effectue pas un tri complet car on a seulement besoin de connaître les premiers */
			for (l = 0; l < nb_pts_supp; l++)
			{
				dist_max = 0.0f;
				for (j = 0; j < m_leadSkeletons[i]->getSize (); j++)
				{
					if (m_distances[j] > dist_max)
					{
						m_maxDistancesIndexes[l] = j;
						dist_max = m_distances[j];
					}
				}
				/* Il n'y a plus de place */
				if (dist_max == 0.0f)
					m_maxDistancesIndexes[l] = -1;
				else
					/* On met à la distance la plus grande pour ne plus la prendre en compte lors de la recherche suivante */
					m_distances[m_maxDistancesIndexes[l]] = 0;
			}
			/* Les particules les plus écartées sont maintenant connues, on peut passer à l'affichage */
			/* Remplissage des points de contrôle */

			for (j = 0; j < m_leadSkeletons[i]->getSize () - 1; j++)
			{
				SetCtrlPoint (m_leadSkeletons[i]->getParticle (j), utex);
				/* On regarde s'il ne faut pas ajouter un point */
				for (l = 0; l < nb_pts_supp; l++)
				{
					if (m_maxDistancesIndexes[l] == (int)j)
					{
						/* On peut référencer j+1 puisque m_maxDistancesIndexes[l] != j si j == m_leadSkeletons[i]->getSize()-1 */
						pt = CPoint::pointBetween(m_leadSkeletons[i]->getParticle (j), m_leadSkeletons[i]->getParticle (j + 1));
						SetCtrlPoint (pt, utex);
					}
				}
			}
			SetCtrlPoint (m_leadSkeletons[i]->getLastParticle (), utex);

			bool prec = false;
			for (l = 0; l < nb_pts_supp; l++)
				if (m_maxDistancesIndexes[l] == (int)m_leadSkeletons[i]->getSize ()-1)
				{
					pt = CPoint::pointBetween(m_leadSkeletons[i]->getRoot (), m_leadSkeletons[i]-> getLastParticle ());
					SetCtrlPoint (pt, utex);
					prec = true;
				}

			/* CPoints supplémentaires au cas où il n'y a "plus de place" ailleurs entre les particules */
			for (l = 0; l < nb_pts_supp; l++)
				if (m_maxDistancesIndexes[l] == -1)
				{
					if (!prec)
					{
						pt = m_leadSkeletons[i]-> getRoot ();
					}
					pt = CPoint::pointBetween (pt, m_leadSkeletons[i]->getLastParticle ());
					SetCtrlPoint (pt, utex);
					prec = true;
				}
			SetCtrlPoint (m_leadSkeletons[i]->getRoot (), utex);
		}
		else
		{
			/* Cas sans problème */
			/* Remplissage des points de contrôle */
			for (j = 0; j < m_leadSkeletons[i]->getSize (); j++)
			{
				SetCtrlPoint (m_leadSkeletons[i]->getParticle (j), utex);
			}
			SetCtrlPoint (m_leadSkeletons[i]->getRoot (), utex);
		}
		m_texTmp = m_texTmpSave;
		utex += m_utexInc;
	}

	m_ctrlPoints = m_ctrlPointsSave;
	m_texCPoints = m_texCPointsSave;

	/* Affichage en NURBS */
	m_uknotsCount = m_nbLeadSkeletons + m_uorder;
	m_vknotsCount = m_vsize + m_vorder;

	for (i = 0; i < m_uorder; i++)
		m_uknots[i] = 0;

	for (i = m_uorder; i < m_uknotsCount-m_uorder; i++)
		m_uknots[i] = m_uknots[i-1]+1;

	m_uknots[m_uknotsCount-m_uorder] =  m_uknots[m_uknotsCount-m_uorder-1]+1;
	for (i = m_uknotsCount-m_uorder+1; i < m_uknotsCount; i++)
		m_uknots[i] = m_uknots[i-1];

	for (j = 0; j < m_vorder; j++)
		m_vknots[j] = 0.0f;

	for (j = m_vorder; j < m_vknotsCount-m_vorder; j++)
		m_vknots[j] = m_vknots[j-1]+1;

	m_vknots[m_vknotsCount-m_vorder] =  m_vknots[m_vknotsCount-m_vorder-1]+1;
	for (j = m_vknotsCount-m_vorder+1; j < m_vknotsCount; j++)
		m_vknots[j] = m_vknots[j-1];

	if ( m_vsize*m_nbLeadSkeletons != m_count)
		cerr << "error " << m_vsize*m_nbLeadSkeletons << " " << m_count << endl;

	computeCenterAndExtremities();

	return true;
}

// void CLineFlame::generateAndDrawSparks()
// {
//   uint i, j, k;
//   uint life=30;

//   /* Ajout de particules */
//   if( (rand()/((float)RAND_MAX)) < .05){
//     CPoint pos;
//     float r = (rand()/((float)RAND_MAX));
//     pos = (m_wick.getLeadCPoint(m_wick.getLeadCPointsArraySize()-1)->m_pt);
//     pos = pos * r + m_wick.getLeadCPoint(0)->m_pt ;
//     CParticle *spark = new CParticle(pos, life);
//     m_sparksList.push_back(spark);
//   }

//   /* Déplacement et affichage des particules */
//   for (list < CParticle *>::iterator sparksListIterator = m_sparksList.begin ();
//        sparksListIterator != m_sparksList.end ();
//        sparksListIterator++){
//     CParticle *par = *sparksListIterator;
//     m_solver->findCPointPosition(*par, i, j, k);

//     (*sparksListIterator)->decreaseLife();

//     if ((*sparksListIterator)->isDead ())
//       {
// 	sparksListIterator =  m_sparksList.erase(sparksListIterator);
// 	delete par;
// 	continue;
//       }

//     if ( i >= m_solver->getXRes()  )
//       i = m_solver->getXRes()-1;
//     if ( j >= m_solver->getXRes()  )
//       j = m_solver->getYRes()-1;
//     if ( k >= m_solver->getXRes()  )
//       k = m_solver->getZRes()-1;

//     float div = 1/(float)life;
//     (*sparksListIterator)->x += m_solver->getU (i, j, k) * (*sparksListIterator)->m_lifespan*div;
//     (*sparksListIterator)->y += m_solver->getV (i, j, k) * (*sparksListIterator)->m_lifespan*div;
//     (*sparksListIterator)->z += m_solver->getW (i, j, k) * (*sparksListIterator)->m_lifespan*div;

//     glColor4f (1.0, 1.0, 0.45, 1.0);
//     glPushMatrix ();
//     glTranslatef ((*sparksListIterator)->x, (*sparksListIterator)->y, (*sparksListIterator)->z);
//     CUGraphicsFn::SolidSphere (0.01, 4, 4);
//     glPopMatrix ();
//   }
// }

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE POINTFLAME ****************************************/
/**********************************************************************************************************************/

CPointFlame::CPointFlame (const FlameConfig& flameConfig, ITexture const& a_rTex, Field3D& a_rField, float rayon, CWick *wick):
		IRealFlame ( flameConfig.skeletonsNumber, 3, a_rTex)
{
	uint i;
	float angle;

	assert(rayon > 0.0f);
	assert(wick != NULL);

	m_nbLeadSkeletons = 1;

	m_wick = wick;
	m_wick->buildPointFDF(flameConfig, m_leadSkeletons, a_rField);

	/* On créé les squelettes en cercle */
	angle = 0.0f;

	assert(m_leadSkeletons[0] != NULL);
	CPoint const& pt = m_leadSkeletons[0]->getRoot();

	for (i = 0; i < m_nbSkeletons; i++)
	{
		CPoint oRoot(cos (angle) * rayon + pt.x, pt.y, sin (angle) * rayon + pt.z);
		m_periSkeletons[i] =
			new CPeriSkeleton(a_rField, oRoot,	CPoint(1.0f,.75f,1.0f), *m_leadSkeletons[0], flameConfig.periLifeSpan);
		angle += 2.0f * PI / m_nbSkeletons;
	}

	m_utexInc = 2.0f/m_nbSkeletons;
}

CPointFlame::~CPointFlame ()
{
}


/**********************************************************************************************************************/
/*************************************** IMPLEMENTATION DE LA CLASSE DETACHEDFLAME ************************************/
/**********************************************************************************************************************/

CDetachedFlame::CDetachedFlame(const IRealFlame* const source, uint nbLeadSkeletons, CFreeLeadSkeleton **leadSkeletons,
                               uint nbSkeletons, CFreePeriSkeleton **periSkeletons, ITexture const& a_rTex, bool shadingType,
                               u_char samplingMethod) :
		INurbsFlame (source, nbSkeletons, 2, a_rTex)
{
	m_distances = new float[NB_PARTICLES_MAX - 1 + m_nbFixedPoints];
	m_maxDistancesIndexes = new int[NB_PARTICLES_MAX - 1 + m_nbFixedPoints];
	m_nbLeadSkeletons = nbLeadSkeletons;
	m_leadSkeletons = leadSkeletons;
	m_periSkeletons = periSkeletons;
	m_shadingType = shadingType;
	setSamplingTolerance(samplingMethod);
}

CDetachedFlame::~CDetachedFlame()
{
	assert(m_periSkeletons != NULL);
	for (uint i = 0; i < m_nbSkeletons; i++)
	{
		assert(m_periSkeletons[i] != NULL);
		delete m_periSkeletons[i];
	}
	delete[]m_periSkeletons;

	assert(m_leadSkeletons != NULL);
	for (uint i = 0; i < m_nbLeadSkeletons; i++)
	{
		assert(m_leadSkeletons[i] != NULL);
		delete m_leadSkeletons[i];
	}
	delete[]m_leadSkeletons;

	assert(m_distances != NULL);
	delete[]m_distances;
	assert(m_maxDistancesIndexes != NULL);
	delete[]m_maxDistancesIndexes;
}

void CDetachedFlame::computeVTexCoords()
{
	float vinc, vtmp;
	uint i;

	vinc = 1.0f / (float)(m_vsize-1);
	vtmp = 0.0;
	for (i = 0; i < m_vsize; i++)
	{
		m_texTmp[i] = vtmp;
		vtmp += vinc;
	}
}

bool CDetachedFlame::build()
{
	uint i, j, l;
	float utex;
	float dist_max;
	m_maxParticles = 0;
	utex = 0.0f;

	for (i = 0; i < m_nbLeadSkeletons; i++)
	{
		assert(m_leadSkeletons[i] != NULL);
		m_leadSkeletons[i]->move ();
	}

	/* Déplacement et détermination du maximum */
	for (i = 0; i < m_nbSkeletons; i++)
	{
		assert(m_periSkeletons[i] != NULL);
		m_periSkeletons[i]->move ();
		if (m_periSkeletons[i]->getSize () < 2)
			return false;
		if (m_periSkeletons[i]->getSize () > m_maxParticles)
			m_maxParticles = m_periSkeletons[i]->getSize ();
	}

	m_vsize = m_maxParticles+m_nbFixedPoints;

	/* On connaît la taille maximale d'un squelette, on peut maintenant déterminer les coordonnées de texture en v */
	computeVTexCoords();

	/* Direction des u */
	for (i = 0; i < m_nbSkeletons; i++)
	{
		assert(m_periSkeletons[i] != NULL);
		/* Problème pour la direction des v, le nombre de particules par squelettes n'est pas garanti */
		/* La solution retenue va ajouter des points de contrôles là où les points de contrôles sont le plus éloignés */
		if (m_periSkeletons[i]->getSize() < m_maxParticles)
		{
			// Nombre de points de contrôle supplémentaires
			uint nb_pts_supp = m_maxParticles - m_periSkeletons[i]->getSize();
			CPoint pt;
			/* On calcule les distances entre les particules successives */
			/* On prend également en compte l'origine du squelette ET les extrémités du guide */
			/* On laisse les distances au carré pour des raisons évidentes de coût de calcul */
			m_distances[0] =
			  m_periSkeletons[i]->getLeadSkeleton().getParticle(0).squaredDistanceFrom (m_periSkeletons[i]->getParticle(0));

			for (j = 0; j < m_periSkeletons[i]->getSize () - 1; j++)
				m_distances[j + 1] =
				  m_periSkeletons[i]->getParticle(j).squaredDistanceFrom(m_periSkeletons[i]->getParticle (j + 1));

			m_distances[m_periSkeletons[i]->getSize ()] =
			  m_periSkeletons[i]->getLastParticle().squaredDistanceFrom (m_periSkeletons[i]->getLeadSkeleton().getLastParticle ());

			/* On cherche les indices des distances max */
			/* On n'effectue pas un tri complet car on a seulement besoin de connaître les premiers */
			for (l = 0; l < nb_pts_supp; l++)
			{
				dist_max = -FLT_MAX;
				for (j = 0; j < m_periSkeletons[i]->getSize() - 1 + m_nbFixedPoints; j++)
				{
					if (m_distances[j] > dist_max)
					{
						m_maxDistancesIndexes[l] = j;
						dist_max = m_distances[j];
					}
				}
				/* Il n'y a plus de place */
				if (dist_max == -FLT_MAX)
					m_maxDistancesIndexes[l] = -1;
				else
					/* On met à la distance la plus grande pour ne plus la prendre en compte lors de la recherche suivante */
					m_distances[m_maxDistancesIndexes[l]] = 0;
			}
			/* Les particules les plus écartées sont maintenant connues, on peut passer à l'affichage */
			/* Remplissage des points de contrôle */
			SetCtrlPoint (m_periSkeletons[i]->getLeadSkeleton().getParticle(0), utex);

			for (l = 0; l < nb_pts_supp; l++)
				if (m_maxDistancesIndexes[l] == 0)
				{
					pt = CPoint::pointBetween(	m_periSkeletons[i]->getLeadSkeleton().getParticle(0),
												m_periSkeletons[i]->getParticle(0));
					SetCtrlPoint (pt, utex);
				}

			for (j = 0; j < m_periSkeletons[i]->getSize () - 1; j++)
			{
				/* On regarde s'il ne faut pas ajouter un point */
				for (l = 0; l < nb_pts_supp; l++)
				{
					if (m_maxDistancesIndexes[l] == (int)j + 1)
					{
						/* On peut référencer j+1 puisque normalement, m_maxDistancesIndexes[l] != j si j == m_periSkeletons[i]->getSize()-1 */
						pt = CPoint::pointBetween(	m_periSkeletons[i]->getParticle(j),
													m_periSkeletons[i]->getParticle(j + 1));
						SetCtrlPoint (pt, utex);
					}
				}
				SetCtrlPoint (m_periSkeletons[i]->getParticle(j), utex);
			}

			bool prec = false;

			SetCtrlPoint (m_periSkeletons[i]->getLastParticle(), utex);

			for (l = 0; l < nb_pts_supp; l++)
				if (m_maxDistancesIndexes[l] == (int)m_periSkeletons[i]->getSize())
				{
					pt = CPoint::pointBetween(m_periSkeletons[i]->getLastParticle(),
					                          m_periSkeletons[i]->getLeadSkeleton().getLastParticle());
					SetCtrlPoint (pt, utex);
					prec = true;
				}

			/* CPoints supplémentaires au cas où il n'y a "plus de place" ailleurs entre les particules */
			for (l = 0; l < nb_pts_supp; l++)
				if (m_maxDistancesIndexes[l] == -1)
				{
					if (!prec)
					{
						pt = m_periSkeletons[i]->getLastParticle();
					}
					pt = CPoint::pointBetween (pt, m_periSkeletons[i]->getLeadSkeleton().getLastParticle());
					SetCtrlPoint (pt, utex);
					prec = true;
				}
			SetCtrlPoint (m_periSkeletons[i]->getLeadSkeleton().getLastParticle(), utex);
		}
		else
		{
			/* Cas sans problème */
			/* Remplissage des points de contrôle */
			SetCtrlPoint (m_periSkeletons[i]->getLeadSkeleton().getParticle(0), utex);
			for (j = 0; j < m_periSkeletons[i]->getSize (); j++)
			{
				SetCtrlPoint (m_periSkeletons[i]->getParticle(j), utex);
			}
			SetCtrlPoint (m_periSkeletons[i]->getLeadSkeleton().getLastParticle(), utex);
		}
		m_texTmp = m_texTmpSave;
		utex += m_utexInc;
	}

	/* On recopie les m_uorder squelettes pour fermer la NURBS */
	GLfloat *startCtrlCPoints = m_ctrlPointsSave;
	for (i = 0; i < ((m_uorder-1)*m_vsize)*3; i++)
		*m_ctrlPoints++ = *startCtrlCPoints++;
	m_ctrlPoints = m_ctrlPointsSave;

	GLfloat *startTexCPoints = m_texCPointsSave;
	for (i = 0; i < ((m_uorder-1)*m_vsize)*2; i++)
		*m_texCPoints++ = *startTexCPoints++;
	m_texCPoints = m_texCPointsSave;

	/* Affichage en NURBS */
	m_uknotsCount = m_nbSkeletons + m_uorder + m_uorder - 1;
	m_vknotsCount = m_vsize + m_vorder;

	for (i = 0; i < m_uknotsCount; i++)
		m_uknots[i] = (float)i;

	for (j = 0; j < m_vorder; j++)
		m_vknots[j] = 0.0f;

	for (j = m_vorder; j < m_vknotsCount-m_vorder; j++)
		m_vknots[j] = m_vknots[j-1]+1;

	m_vknots[m_vknotsCount-m_vorder] =  m_vknots[m_vknotsCount-m_vorder-1]+1;
	for (j = m_vknotsCount-m_vorder+1; j < m_vknotsCount; j++)
		m_vknots[j] = m_vknots[j-1];

	return true;
}

// void CLineFlame::breakCheck()
// {
//   float split,proba=.5;
//   uint threshold=4;
//   uint splitHeight;
//   uint i,j;
//   CFreeLeadSkeleton **leadSkeletonsArray;
//   CFreePeriSkeleton **periSkeletonsArray;

//   for ( i = 0; i < m_nbLeadSkeletons; i++)
//     /* La division ne peut avoir lieu qu'au dessus du seuil */
//     if(m_leadSkeletons[i]->getSize() < threshold)
//       return;

//   split = (rand()/((float)RAND_MAX));
//   if( split > proba){
//     leadSkeletonsArray = new CFreeLeadSkeleton* [m_nbLeadSkeletons];
//     for ( i = 0; i < m_nbLeadSkeletons; i++){
//       /* Roulette russe : tirage aléatoire entre 0 et 1 */
//       splitHeight = (uint)(split * m_leadSkeletons[i]->getSize()) - 1;
//       leadSkeletonsArray[i] = m_leadSkeletons[i]->split(splitHeight);
//     }

//     periSkeletonsArray = new CFreePeriSkeleton* [m_nbSkeletons];

//     splitHeight = (uint)(split * m_periSkeletons[0]->getSize()) - 1;
//     periSkeletonsArray[0] = m_periSkeletons[0]->split(splitHeight, leadSkeletonsArray[0]);

//     /* Génération d'un côté des squelettes périphériques */
//     for (i = 1; i <= m_nbLeadSkeletons; i++)
//       {
// 	splitHeight = (uint)(split * m_periSkeletons[i]->getSize()) - 1;
// 	periSkeletonsArray[i] = m_periSkeletons[i]->split(splitHeight, leadSkeletonsArray[i-1]);
//       }

//     /* Génération de l'autre côté des squelettes périphériques */
//     for ( j = m_nbLeadSkeletons, i = m_nbLeadSkeletons + 2; j > 0; j--, i++)
//       {
// 	splitHeight = (uint)(split * m_periSkeletons[i]->getSize()) - 1;
// 	periSkeletonsArray[i] = m_periSkeletons[i]->split(splitHeight, leadSkeletonsArray[j-1]);
//       }

//     splitHeight = (uint)(split * m_periSkeletons[m_nbLeadSkeletons + 1]->getSize()) - 1;
//     periSkeletonsArray[m_nbLeadSkeletons + 1] =
//       m_periSkeletons[m_nbLeadSkeletons + 1]->split(splitHeight, leadSkeletonsArray[m_nbLeadSkeletons-1]);

//     m_parentFire->addDetachedFlame(new CDetachedFlame(this, m_nbLeadSkeletons, leadSkeletonsArray, m_nbSkeletons, periSkeletonsArray, m_tex, m_solver));
//   }
// }

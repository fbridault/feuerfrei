#include "realFlames.hpp"

#include <engine/graphicsFn.hpp>

#include "abstractFires.hpp"

/**********************************************************************************************************************/
/*************************************** IMPLEMENTATION DE LA CLASSE LINEFLAME ****************************************/
/**********************************************************************************************************************/

LineFlame::LineFlame (const FlameConfig& flameConfig, const ITexture* const tex, Field3D* const s,
		      Wick *wickObject, float width, float detachedFlamesWidth, DetachableFireSource *parentFire ) :
  RealFlame ((flameConfig.skeletonsNumber+2)*2, 3, tex, s)
{
  CPoint pt;
  uint i,j;
  uint sliceDirection;

  CPoint rootMoveFactorP(.5f,.1f,.5f);

  m_wick = wickObject;
  sliceDirection = m_wick->buildFDF(flameConfig, m_leadSkeletons, s);
  m_nbLeadSkeletons = m_leadSkeletons.size();

  /** Allocation des squelettes p�riph�riques = deux par squelette p�riph�rique */
  /* plus 2 aux extr�mit�s pour fermer la NURBS */
  /* FAIT DANS BASICFLAME DESORMAIS */

  /* G�n�ration d'un c�t� des squelettes p�riph�riques */
  for (i = 0; i < m_nbLeadSkeletons; i++)
    {
      pt = *m_leadSkeletons[i]->getRoot();
      switch(sliceDirection){
      case 0 : pt.z -= (width / 2.0f);
      case 1 : pt.z -= (width / 2.0f);
      case 2 : pt.x += (width / 2.0f);
      }
      m_periSkeletons[i] = new PeriSkeleton (m_solver, pt, rootMoveFactorP, m_leadSkeletons[i], flameConfig.periLifeSpan);
    }

  /* G�n�ration de l'autre c�t� des squelettes p�riph�riques */
  for ( j = m_nbLeadSkeletons, i = m_nbLeadSkeletons; j > 0; j--, i++)
    {
      pt = *m_leadSkeletons[j-1]->getRoot();
      switch(sliceDirection){
      case 0 : pt.z += (width / 2.0f);
      case 1 : pt.z += (width / 2.0f);
      case 2 : pt.x -= (width / 2.0f);
      }
      m_periSkeletons[i] = new PeriSkeleton (m_solver, pt, rootMoveFactorP, m_leadSkeletons[j-1], flameConfig.periLifeSpan);
    }

  m_parentFire = parentFire;

  m_detachedFlamesWidth = detachedFlamesWidth;
  m_samplingMethod = 1;
  m_vTexInit = 0;

  m_utexInc = 2.0f/(m_nbSkeletons);
}

LineFlame::~LineFlame ()
{
  delete m_wick;
}

void LineFlame::breakCheck()
{
  float split,proba;
  uint threshold=3;
  float detachThreshold=.9f;
  /* Indice de la particule � laquelle un squelette est d�coup� */
  uint splitHeight;
  uint i;
  FreeLeadSkeleton **leadSkeletonsArray;
  FreePeriSkeleton **periSkeletonsArray;

  for ( i = 0; i < m_nbLeadSkeletons; i++){
    if(m_leadSkeletons[i]->getInternalSize() < threshold)
      return;

    proba = (rand()/((float)RAND_MAX));

    /* Tirage al�atoire entre 0 et 1 */
    if( proba > detachThreshold){
      CPoint offset;

      /* Tirage entre 0.5 et 1 pour la hauteur du squelette */
      split = (rand()/(2.0f*(float)RAND_MAX))+.5f;

      leadSkeletonsArray = new FreeLeadSkeleton* [1];

      splitHeight = (uint)(split * (m_leadSkeletons[i]->getInternalSize()-1) );
      leadSkeletonsArray[0] = m_leadSkeletons[i]->split(splitHeight);

      periSkeletonsArray = new FreePeriSkeleton* [4];

      offset.x=-m_detachedFlamesWidth * split;
      periSkeletonsArray[0] = leadSkeletonsArray[0]->dup(offset);

      splitHeight = (uint)(split * (m_periSkeletons[i+1]->getInternalSize()-1));
      periSkeletonsArray[1] = m_periSkeletons[i+1]->split(splitHeight, leadSkeletonsArray[0]);

      offset.x=m_detachedFlamesWidth * split;
      periSkeletonsArray[2] = leadSkeletonsArray[0]->dup(offset);

      splitHeight = (uint)(split * (m_periSkeletons[m_nbSkeletons-i-1]->getInternalSize()-1));
      periSkeletonsArray[3] = m_periSkeletons[m_nbSkeletons-i-1]->split(splitHeight, leadSkeletonsArray[0]);

      m_parentFire->addDetachedFlame(new DetachedFlame(this, 1, leadSkeletonsArray, 4, periSkeletonsArray, m_tex, m_shadingType, m_samplingMethod));
    }
  }
}

void LineFlame::computeVTexCoords()
{
  float vinc, vtmp;
  uint i;

  /* Si la flamme a un parent, c'est une flamme d�tachable, on fait varier la texcoord V dans le temps */
  if(m_parentFire)
    m_vTexInit = (m_vTexInit >= 0.85f) ? 0.0f : m_vTexInit + 0.15f;
  else
    m_vTexInit = 0.0;
  if(m_lodSkel == FULL_SKELETON)
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

bool LineFlame::buildFlat ()
{
  uint i, j, l;
  float utex;
  float dist_max;
  m_maxParticles = 0;
  m_count = 0;
  utex = 0.0f;

  if(m_lodSkelChanged) changeSkeletonsLOD();

  /* D�placement des squelettes guides */
  for (i = 0; i < m_nbLeadSkeletons; i++){
    m_leadSkeletons[i]->move ();
    if (m_leadSkeletons[i]->getSize () > m_maxParticles)
      m_maxParticles = m_leadSkeletons[i]->getSize ();
  }

  /* On rajoute des particules si il n'y en a pas assez pour construire la NURBS) */
  if(m_maxParticles < m_vorder)
    m_maxParticles = m_vorder;

  /* D�placement des squelettes p�riph�riques et d�termination du maximum de particules par squelette */
  for (i = 0; i < m_nbSkeletons; i++)
    {
      m_periSkeletons[i]->move ();
    }

  m_vsize = m_maxParticles + m_nbFixedCPoints;

  /* On conna�t la taille maximale d'un squelette, on peut maintenant d�terminer les coordonn�es de texture en v */
  computeVTexCoords();

  /* Direction des u */
  for (i = 0; i < m_nbLeadSkeletons; i++)
    {
      /* Probl�me pour la direction des v, le nombre de particules par squelettes n'est pas garanti */
      /* La solution retenue va ajouter des points de contr�les l� o� les points de contr�les sont le plus �loign�s */
      if (m_leadSkeletons[i]->getSize () < m_maxParticles)
	{
	  // Nombre de points de contr�le suppl�mentaires
	  uint nb_pts_supp = m_maxParticles - m_leadSkeletons[i]->getSize ();
	  CPoint pt;

	  /* On calcule les distances entre les particules successives */
	  /* On prend �galement en compte l'origine du squelette */
	  /* On laisse les distances au carr� pour des raisons �videntes de co�t de calcul */
	  for (j = 0; j < m_leadSkeletons[i]->getSize () - 1; j++)
	    m_distances[j] =
	      m_leadSkeletons[i]->getParticle (j)->squaredDistanceFrom(*m_leadSkeletons[i]->getParticle (j + 1));

	  m_distances[m_leadSkeletons[i]->getSize () - 1] =
	    m_leadSkeletons[i]->getLastParticle ()->squaredDistanceFrom (*m_leadSkeletons[i]->getRoot ());

	  /* On cherche les indices des distances max */
	  /* On n'effectue pas un tri complet car on a seulement besoin de conna�tre les premiers */
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
		/* On met � la distance la plus grande pour ne plus la prendre en compte lors de la recherche suivante */
		m_distances[m_maxDistancesIndexes[l]] = 0;
	    }
	  /* Les particules les plus �cart�es sont maintenant connues, on peut passer � l'affichage */
	  /* Remplissage des points de contr�le */

	  for (j = 0; j < m_leadSkeletons[i]->getSize () - 1; j++)
	    {
	      setCtrlCPoint (m_leadSkeletons[i]->getParticle (j), utex);
	      /* On regarde s'il ne faut pas ajouter un point */
	      for (l = 0; l < nb_pts_supp; l++)
		{
		  if (m_maxDistancesIndexes[l] == (int)j)
		    {
		      /* On peut r�f�rencer j+1 puisque m_maxDistancesIndexes[l] != j si j == m_leadSkeletons[i]->getSize()-1 */
		      pt = CPoint::pointBetween(m_leadSkeletons[i]->getParticle (j), m_leadSkeletons[i]->getParticle (j + 1));
		      setCtrlCPoint (&pt, utex);
		    }
		}
	    }
	  setCtrlCPoint (m_leadSkeletons[i]->getLastParticle (), utex);

	  bool prec = false;
	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == (int)m_leadSkeletons[i]->getSize ()-1)
	      {
		pt = CPoint::pointBetween(m_leadSkeletons[i]->getRoot (), m_leadSkeletons[i]-> getLastParticle ());
		setCtrlCPoint (&pt, utex);
		prec = true;
	      }

	  /* CPoints suppl�mentaires au cas o� il n'y a "plus de place" ailleurs entre les particules */
	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == -1)
	      {
		if (!prec)
		  {
		    pt = *m_leadSkeletons[i]-> getRoot ();
		  }
		pt = CPoint::pointBetween (&pt, m_leadSkeletons[i]->getLastParticle ());
		setCtrlCPoint (&pt, utex);
		prec = true;
	      }
	  setCtrlCPoint (m_leadSkeletons[i]->getRoot (), utex);
	}
      else
	{
	  /* Cas sans probl�me */
	  /* Remplissage des points de contr�le */
	  for (j = 0; j < m_leadSkeletons[i]->getSize (); j++)
	    {
	      setCtrlCPoint (m_leadSkeletons[i]->getParticle (j), utex);
	    }
	  setCtrlCPoint (m_leadSkeletons[i]->getRoot (), utex);
	}
      m_texTmp = m_texTmpSave;
      utex += m_utexInc;
    }

  m_ctrlCPoints = m_ctrlCPointsSave;
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

  if( m_vsize*m_nbLeadSkeletons != m_count)
    cerr << "error " << m_vsize*m_nbLeadSkeletons << " " << m_count << endl;

  computeCenterAndExtremities();

  return true;
}

// void LineFlame::generateAndDrawSparks()
// {
//   uint i, j, k;
//   uint life=30;

//   /* Ajout de particules */
//   if( (rand()/((float)RAND_MAX)) < .05){
//     CPoint pos;
//     float r = (rand()/((float)RAND_MAX));
//     pos = (m_wick.getLeadCPoint(m_wick.getLeadCPointsArraySize()-1)->m_pt);
//     pos = pos * r + m_wick.getLeadCPoint(0)->m_pt ;
//     Particle *spark = new Particle(pos, life);
//     m_sparksList.push_back(spark);
//   }

//   /* D�placement et affichage des particules */
//   for (list < Particle *>::iterator sparksListIterator = m_sparksList.begin ();
//        sparksListIterator != m_sparksList.end ();
//        sparksListIterator++){
//     Particle *par = *sparksListIterator;
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

CPointFlame::CPointFlame (const FlameConfig& flameConfig, const ITexture* const tex, Field3D* const s, float rayon, Wick *wick):
  RealFlame ( flameConfig.skeletonsNumber, 3, tex, s)
{
  uint i;
  float angle;

  m_nbLeadSkeletons = 1;

  m_wick = wick;
  m_wick->buildCPointFDF(flameConfig, m_leadSkeletons, s);

  /* On cr�� les squelettes en cercle */
  angle = 0.0f;

  CPoint pt=*m_leadSkeletons[0]->getRoot();
  for (i = 0; i < m_nbSkeletons; i++)
    {
      m_periSkeletons[i] = new PeriSkeleton
	(m_solver,
	 CPoint (cos (angle) * rayon + pt.x, pt.y, sin (angle) * rayon + pt.z),
	 CPoint(1.0f,.75f,1.0f),
	 m_leadSkeletons[0], flameConfig.periLifeSpan);
      angle += 2.0f * PI / m_nbSkeletons;
    }

  m_utexInc = 2.0f/m_nbSkeletons;
}

CPointFlame::~CPointFlame ()
{
  delete m_wick;
}

void CPointFlame::getLightPositions (GLfloat lightPositions[8][4], uint& nbLights)
{
  Particle *tmp;

  nbLights=0;
  for (uint i = 0; i < m_leadSkeletons[0]->getSize () - 1; i++)
    {
      tmp = m_leadSkeletons[0]->getParticle (i);

      nbLights++;
      lightPositions[i][0] = tmp->x;
      lightPositions[i][1] = tmp->y;
      lightPositions[i][2] = tmp->z;
      lightPositions[i][3] = 1.0;
    }
}

/**********************************************************************************************************************/
/*************************************** IMPLEMENTATION DE LA CLASSE DETACHEDFLAME ************************************/
/**********************************************************************************************************************/

DetachedFlame::DetachedFlame(const RealFlame* const source, uint nbLeadSkeletons, FreeLeadSkeleton **leadSkeletons,
			     uint nbSkeletons, FreePeriSkeleton **periSkeletons, const ITexture* const tex, bool shadingType,
			     u_char samplingMethod) :
  NurbsFlame (source, nbSkeletons, 2, tex)
{
  m_distances = new float[NB_PARTICLES_MAX - 1 + m_nbFixedCPoints];
  m_maxDistancesIndexes = new int[NB_PARTICLES_MAX - 1 + m_nbFixedCPoints];
  m_nbLeadSkeletons = nbLeadSkeletons;
  m_leadSkeletons = leadSkeletons;
  m_periSkeletons = periSkeletons;
  m_shadingType = shadingType;
  setSamplingTolerance(samplingMethod);
}

DetachedFlame::~DetachedFlame()
{
  for (uint i = 0; i < m_nbSkeletons; i++)
    delete m_periSkeletons[i];
  delete[]m_periSkeletons;

  for (uint i = 0; i < m_nbLeadSkeletons; i++)
    delete m_leadSkeletons[i];
  delete[]m_leadSkeletons;

  delete[]m_distances;
  delete[]m_maxDistancesIndexes;
}

void DetachedFlame::computeVTexCoords()
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

bool DetachedFlame::build()
{
  uint i, j, l;
  float utex;
  float dist_max;
  m_maxParticles = 0;
  utex = 0.0f;

  for (i = 0; i < m_nbLeadSkeletons; i++)
    m_leadSkeletons[i]->move ();

  /* D�placement et d�termination du maximum */
  for (i = 0; i < m_nbSkeletons; i++)
    {
      m_periSkeletons[i]->move ();
      if(m_periSkeletons[i]->getSize () < 2)
	return false;
      if (m_periSkeletons[i]->getSize () > m_maxParticles)
	m_maxParticles = m_periSkeletons[i]->getSize ();
    }

  m_vsize = m_maxParticles+m_nbFixedCPoints;

  /* On conna�t la taille maximale d'un squelette, on peut maintenant d�terminer les coordonn�es de texture en v */
  computeVTexCoords();

  /* Direction des u */
  for (i = 0; i < m_nbSkeletons; i++)
    {
      /* Probl�me pour la direction des v, le nombre de particules par squelettes n'est pas garanti */
      /* La solution retenue va ajouter des points de contr�les l� o� les points de contr�les sont le plus �loign�s */
      if (m_periSkeletons[i]->getSize () < m_maxParticles)
	{
	  // Nombre de points de contr�le suppl�mentaires
	  uint nb_pts_supp = m_maxParticles - m_periSkeletons[i]->getSize ();
	  CPoint pt;
	  /* On calcule les distances entre les particules successives */
	  /* On prend �galement en compte l'origine du squelette ET les extr�mit�s du guide */
	  /* On laisse les distances au carr� pour des raisons �videntes de co�t de calcul */
	  m_distances[0] =
	    m_periSkeletons[i]->getLeadSkeleton ()->getParticle (0)->squaredDistanceFrom (*m_periSkeletons[i]->getParticle (0));

	  for (j = 0; j < m_periSkeletons[i]->getSize () - 1; j++)
	    m_distances[j + 1] =
	      m_periSkeletons[i]->getParticle (j)->squaredDistanceFrom(*m_periSkeletons[i]->getParticle (j + 1));

	  m_distances[m_periSkeletons[i]->getSize ()] =
	    m_periSkeletons[i]->getLastParticle ()->squaredDistanceFrom (*m_periSkeletons[i]->getLeadSkeleton ()->getLastParticle ());

	  /* On cherche les indices des distances max */
	  /* On n'effectue pas un tri complet car on a seulement besoin de conna�tre les premiers */
	  for (l = 0; l < nb_pts_supp; l++)
	    {
	      dist_max = -FLT_MAX;
	      for (j = 0; j < m_periSkeletons[i]->getSize () - 1 + m_nbFixedCPoints; j++)
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
		/* On met � la distance la plus grande pour ne plus la prendre en compte lors de la recherche suivante */
		m_distances[m_maxDistancesIndexes[l]] = 0;
	    }
	  /* Les particules les plus �cart�es sont maintenant connues, on peut passer � l'affichage */
	  /* Remplissage des points de contr�le */
	  setCtrlCPoint (m_periSkeletons[i]->getLeadSkeleton ()->getParticle (0), utex);

	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == 0)
	      {
		pt = CPoint::pointBetween(m_periSkeletons[i]->getLeadSkeleton ()->getParticle (0), m_periSkeletons[i]->getParticle (0));
		setCtrlCPoint (&pt, utex);
	      }

	  for (j = 0; j < m_periSkeletons[i]->getSize () - 1; j++)
	    {
	      /* On regarde s'il ne faut pas ajouter un point */
	      for (l = 0; l < nb_pts_supp; l++)
		{
		  if (m_maxDistancesIndexes[l] == (int)j + 1)
		    {
		      /* On peut r�f�rencer j+1 puisque normalement, m_maxDistancesIndexes[l] != j si j == m_periSkeletons[i]->getSize()-1 */
		      pt = CPoint::pointBetween(m_periSkeletons[i]->getParticle (j), m_periSkeletons[i]->getParticle (j + 1));
		      setCtrlCPoint (&pt, utex);
		    }
		}
	      setCtrlCPoint (m_periSkeletons[i]->getParticle (j), utex);
	    }

	  bool prec = false;

	  setCtrlCPoint (m_periSkeletons[i]->getLastParticle (), utex);

	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == (int)m_periSkeletons[i]->getSize ())
	      {
		pt = CPoint::pointBetween(m_periSkeletons[i]->getLastParticle (),
					 m_periSkeletons[i]->getLeadSkeleton()->getLastParticle ());
		setCtrlCPoint (&pt, utex);
		prec = true;
	      }

	  /* CPoints suppl�mentaires au cas o� il n'y a "plus de place" ailleurs entre les particules */
	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == -1)
	      {
		if (!prec)
		  {
		    pt = *m_periSkeletons[i]->getLastParticle ();
		  }
		pt = CPoint::pointBetween (&pt, m_periSkeletons[i]->getLeadSkeleton()->getLastParticle ());
		setCtrlCPoint (&pt, utex);
		prec = true;
	      }
	  setCtrlCPoint (m_periSkeletons[i]->getLeadSkeleton ()->getLastParticle (), utex);
	}
      else
	{
	  /* Cas sans probl�me */
	  /* Remplissage des points de contr�le */
	  setCtrlCPoint (m_periSkeletons[i]->getLeadSkeleton ()->getParticle (0), utex);
	  for (j = 0; j < m_periSkeletons[i]->getSize (); j++)
	    {
	      setCtrlCPoint (m_periSkeletons[i]->getParticle (j), utex);
	    }
	  setCtrlCPoint (m_periSkeletons[i]->getLeadSkeleton ()->getLastParticle (), utex);
	}
      m_texTmp = m_texTmpSave;
      utex += m_utexInc;
    }

  /* On recopie les m_uorder squelettes pour fermer la NURBS */
  GLfloat *startCtrlCPoints = m_ctrlCPointsSave;
  for (i = 0; i < ((m_uorder-1)*m_vsize)*3; i++)
    *m_ctrlCPoints++ = *startCtrlCPoints++;
  m_ctrlCPoints = m_ctrlCPointsSave;

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

// void LineFlame::breakCheck()
// {
//   float split,proba=.5;
//   uint threshold=4;
//   uint splitHeight;
//   uint i,j;
//   FreeLeadSkeleton **leadSkeletonsArray;
//   FreePeriSkeleton **periSkeletonsArray;

//   for ( i = 0; i < m_nbLeadSkeletons; i++)
//     /* La division ne peut avoir lieu qu'au dessus du seuil */
//     if(m_leadSkeletons[i]->getSize() < threshold)
//       return;

//   split = (rand()/((float)RAND_MAX));
//   if( split > proba){
//     leadSkeletonsArray = new FreeLeadSkeleton* [m_nbLeadSkeletons];
//     for ( i = 0; i < m_nbLeadSkeletons; i++){
//       /* Roulette russe : tirage al�atoire entre 0 et 1 */
//       splitHeight = (uint)(split * m_leadSkeletons[i]->getSize()) - 1;
//       leadSkeletonsArray[i] = m_leadSkeletons[i]->split(splitHeight);
//     }

//     periSkeletonsArray = new FreePeriSkeleton* [m_nbSkeletons];

//     splitHeight = (uint)(split * m_periSkeletons[0]->getSize()) - 1;
//     periSkeletonsArray[0] = m_periSkeletons[0]->split(splitHeight, leadSkeletonsArray[0]);

//     /* G�n�ration d'un c�t� des squelettes p�riph�riques */
//     for (i = 1; i <= m_nbLeadSkeletons; i++)
//       {
// 	splitHeight = (uint)(split * m_periSkeletons[i]->getSize()) - 1;
// 	periSkeletonsArray[i] = m_periSkeletons[i]->split(splitHeight, leadSkeletonsArray[i-1]);
//       }

//     /* G�n�ration de l'autre c�t� des squelettes p�riph�riques */
//     for ( j = m_nbLeadSkeletons, i = m_nbLeadSkeletons + 2; j > 0; j--, i++)
//       {
// 	splitHeight = (uint)(split * m_periSkeletons[i]->getSize()) - 1;
// 	periSkeletonsArray[i] = m_periSkeletons[i]->split(splitHeight, leadSkeletonsArray[j-1]);
//       }

//     splitHeight = (uint)(split * m_periSkeletons[m_nbLeadSkeletons + 1]->getSize()) - 1;
//     periSkeletonsArray[m_nbLeadSkeletons + 1] =
//       m_periSkeletons[m_nbLeadSkeletons + 1]->split(splitHeight, leadSkeletonsArray[m_nbLeadSkeletons-1]);

//     m_parentFire->addDetachedFlame(new DetachedFlame(this, m_nbLeadSkeletons, leadSkeletonsArray, m_nbSkeletons, periSkeletonsArray, m_tex, m_solver));
//   }
// }

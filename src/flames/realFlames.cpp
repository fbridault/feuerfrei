#include "realFlames.hpp"

#include "../scene/graphicsFn.hpp"
#include "abstractFires.hpp"

/**********************************************************************************************************************/
/*************************************** IMPLEMENTATION DE LA CLASSE LINEFLAME ****************************************/
/**********************************************************************************************************************/

LineFlame::LineFlame (const FlameConfig& flameConfig, const Texture* const tex, Field3D* const s, 
		      Wick *wickObject, float detachedFlamesWidth, DetachableFireSource *parentFire ) :
  RealFlame ((flameConfig.skeletonsNumber+2)*2, 3, tex, s)
{
  Point pt;
  float largeur = .03f;
  uint i,j;
  
  Point rootMoveFactorP(2.0f,.1f,.5f);
  
  m_wick = wickObject;
  m_wick->build(flameConfig, m_leadSkeletons, s);
  m_nbLeadSkeletons = m_leadSkeletons.size();
  
  /** Allocation des squelettes périphériques = deux par squelette périphérique */
  /* plus 2 aux extrémités pour fermer la NURBS */
  /* FAIT DANS BASICFLAME DESORMAIS */
  
  /* Génération d'un côté des squelettes périphériques */
  for (i = 0; i < m_nbLeadSkeletons; i++)
    {
      pt = *m_leadSkeletons[i]->getRoot();
      pt.z -= (largeur / 2.0f);
      m_periSkeletons[i] = new PeriSkeleton (m_solver, pt, rootMoveFactorP, m_leadSkeletons[i], flameConfig.periLifeSpan);
    }
  
  /* Génération de l'autre côté des squelettes périphériques */
  for ( j = m_nbLeadSkeletons, i = m_nbLeadSkeletons; j > 0; j--, i++)
    {
      pt = *m_leadSkeletons[j-1]->getRoot();
      pt.z += (largeur / 2.0f);
      m_periSkeletons[i] = new PeriSkeleton (m_solver, pt, rootMoveFactorP, m_leadSkeletons[j-1], flameConfig.periLifeSpan);
    }
  
  m_parentFire = parentFire;
  
  m_detachedFlamesWidth = detachedFlamesWidth;
  m_samplingMethod = 1;
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
  /* Indice de la particule à laquelle un squelette est découpé */
  uint splitHeight;
  uint i;
  FreeLeadSkeleton **leadSkeletonsArray;
  FreePeriSkeleton **periSkeletonsArray;
  
  for ( i = 0; i < m_nbLeadSkeletons; i++){
    if(m_leadSkeletons[i]->getInternalSize() < threshold)
      return;
    
    proba = (rand()/((float)RAND_MAX));
    
    /* Roulette russe : tirage aléatoire entre 0 et 1 */
    if( proba > detachThreshold){
      Point offset;
      
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

bool LineFlame::buildSimplified ()
{
  uint i, j, l;
  float vinc, vtmp, vtex;
  float dist_max;
  m_maxParticles = 0;
  m_count = 0;
  vtex = -0.5f;
  
  if(m_lodSkelChanged) changeSkeletonsLOD();
  
  /* Déplacement des squelettes guides */
  for (i = 0; i < m_nbLeadSkeletons; i++){
    m_leadSkeletons[i]->move ();
    if (m_leadSkeletons[i]->getSize () > m_maxParticles)
      m_maxParticles = m_leadSkeletons[i]->getSize ();
  }
  
  /* On rajoute des particules si il n'y en a pas assez pour construire la NURBS) */
  if(m_maxParticles < m_vorder)
    m_maxParticles = m_vorder;
  
  /* Déplacement des squelettes périphériques et détermination du maximum de particules par squelette */
  for (i = 0; i < m_nbSkeletons; i++)
    {
      m_periSkeletons[i]->move ();
    }
  
  m_vsize = m_maxParticles + m_nbFixedPoints;
  
  vinc = 1.0f / (float)(m_vsize-1);
  vtmp = 0.0f;
  for (i = 0; i < m_vsize; i++){
    m_texTmp[i] = vtmp;
    vtmp += vinc;
  }
  
  /* Direction des u */
  for (i = 0; i < m_nbLeadSkeletons; i++)
    {
      vtex += .5f;
      /* Problème pour la direction des v, le nombre de particules par squelettes n'est pas garanti */
      /* La solution retenue va ajouter des points de contrôles là où les points de contrôles sont le plus éloignés */
      if (m_leadSkeletons[i]->getSize () < m_maxParticles)
	{
	  // Nombre de points de contrôle supplémentaires
	  uint nb_pts_supp = m_maxParticles - m_leadSkeletons[i]->getSize ();
	  Point pt;
	  
	  /* On calcule les distances entre les particules successives */
	  /* On prend également en compte l'origine du squelette */
	  /* On laisse les distances au carré pour des raisons évidentes de coût de calcul */	  
	  for (j = 0; j < m_leadSkeletons[i]->getSize () - 1; j++)
	    m_distances[j] = 
	      m_leadSkeletons[i]->getParticle (j)->squaredDistanceFrom(*m_leadSkeletons[i]->getParticle (j + 1));
	  
	  m_distances[m_leadSkeletons[i]->getSize () - 1] = 
	    m_leadSkeletons[i]->getLastParticle ()->squaredDistanceFrom (*m_leadSkeletons[i]->getRoot ());
	  
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
	      setCtrlPoint (m_leadSkeletons[i]->getParticle (j), vtex);
	      /* On regarde s'il ne faut pas ajouter un point */
	      for (l = 0; l < nb_pts_supp; l++)
		{
		  if (m_maxDistancesIndexes[l] == (int)j)
		    {
		      /* On peut référencer j+1 puisque m_maxDistancesIndexes[l] != j si j == m_leadSkeletons[i]->getSize()-1 */
		      pt = Point::pointBetween(m_leadSkeletons[i]->getParticle (j), m_leadSkeletons[i]->getParticle (j + 1));
		      setCtrlPoint (&pt, vtex);
		    }
		}
	    }	  
	  setCtrlPoint (m_leadSkeletons[i]->getLastParticle (), vtex);
	  
	  bool prec = false;
	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == (int)m_leadSkeletons[i]->getSize ()-1)
	      {
		pt = Point::pointBetween(m_leadSkeletons[i]->getRoot (), m_leadSkeletons[i]-> getLastParticle ());
		setCtrlPoint (&pt, vtex);
		prec = true;
	      }
	  
	  /* Points supplémentaires au cas où il n'y a "plus de place" ailleurs entre les particules */
	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == -1)
	      {
		if (!prec)
		  {
		    pt = *m_leadSkeletons[i]-> getRoot ();
		  }
		pt = Point::pointBetween (&pt, m_leadSkeletons[i]->getLastParticle ());
		setCtrlPoint (&pt, vtex);
		prec = true;
	      }
	  setCtrlPoint (m_leadSkeletons[i]->getRoot (), vtex);
	}
      else
	{
	  /* Cas sans problème */
	  /* Remplissage des points de contrôle */
	  for (j = 0; j < m_leadSkeletons[i]->getSize (); j++)
	    {
	      setCtrlPoint (m_leadSkeletons[i]->getParticle (j), vtex);
	    }
	  setCtrlPoint (m_leadSkeletons[i]->getRoot (), vtex);
	}
      m_texTmp = m_texTmpSave;
    }
  
  m_ctrlPoints = m_ctrlPointsSave; 
  m_texPoints = m_texPointsSave;
  
  /* Affichage en NURBS */
  m_uknotsCount = m_nbLeadSkeletons + m_uorder;
  m_vknotsCount = m_vsize + m_vorder;

  for (i = 0; i < m_uknotsCount; i++)
    m_uknots[i] = (float)i;

//   for (i = 0; i < m_uorder; i++)
//     m_uknots[i] = 0;
  
//   for (i = m_uorder; i < m_uknotsCount-m_uorder; i++)
//     m_uknots[i] = m_uknots[i-1]+1;
  
//   m_uknots[m_uknotsCount-m_uorder] =  m_uknots[m_uknotsCount-m_uorder-1]+1;
//   for (i = m_uknotsCount-m_uorder+1; i < m_uknotsCount; i++)
//     m_uknots[i] = m_uknots[i-1];
  
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
//     Point pos;
//     float r = (rand()/((float)RAND_MAX));
//     pos = (m_wick.getLeadPoint(m_wick.getLeadPointsArraySize()-1)->m_pt);
//     pos = pos * r + m_wick.getLeadPoint(0)->m_pt ;
//     Particle *spark = new Particle(pos, life);
//     m_sparksList.push_back(spark);
//   }
  
//   /* Déplacement et affichage des particules */
//   for (list < Particle *>::iterator sparksListIterator = m_sparksList.begin (); 
//        sparksListIterator != m_sparksList.end ();
//        sparksListIterator++){
//     Particle *par = *sparksListIterator;
//     m_solver->findPointPosition(*par, i, j, k);

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
//     GraphicsFn::SolidSphere (0.01, 4, 4);
//     glPopMatrix ();
//   }
// }

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE POINTFLAME ****************************************/
/**********************************************************************************************************************/

PointFlame::PointFlame (const FlameConfig& flameConfig, const Texture* const tex, Field3D* const s, 
			float rayon, Object *wick):
  RealFlame ( flameConfig.skeletonsNumber, 3, tex, s)
{
  uint i;
  float angle;
  
  m_nbLeadSkeletons = 1;
  
  m_leadSkeletons.push_back (new LeadSkeleton (m_solver, m_position, Point(2.0f,0.2f,2.0f), 
					       flameConfig.leadLifeSpan, 0, .5f, 0.0f, .025f));
  
  /* On créé les squelettes en cercle */
  angle = 0.0f;
  for (i = 0; i < m_nbSkeletons; i++)
    {
      m_periSkeletons[i] = new PeriSkeleton
	(m_solver, 
	 Point (cos (angle) * rayon + m_position.x, m_position.y, sin (angle) * rayon + m_position.z),
	 Point(1.0f,.75f,1.0f),
	 m_leadSkeletons[0], flameConfig.periLifeSpan);
      angle += 2.0f * PI / m_nbSkeletons;
    }
  
  m_wick = wick;
}

PointFlame::~PointFlame ()
{  
  if(m_wick)
    delete m_wick;
}

void PointFlame::drawWick (bool displayBoxes) const
{
  if(!m_wick){
    float hauteur = 1 / 6.0f;
    float largeur = 1 / 60.0f;
    /* Affichage de la mèche */
    glPushMatrix ();
    glTranslatef (m_position.x, m_position.y-hauteur/2.0f, m_position.z);
    glRotatef (-90.0f, 1.0f, 0.0f, 0.0f);
    glColor3f (0.0f, 0.0f, 0.0f);
    GraphicsFn::SolidCylinder (largeur, hauteur, 10, 10);
    glTranslatef (0.0f, 0.0f, hauteur);
    GraphicsFn::SolidDisk (largeur, 10, 10);
    glPopMatrix ();
  }else{
    m_wick->draw();
  }
}

/**********************************************************************************************************************/
/*************************************** IMPLEMENTATION DE LA CLASSE DETACHEDFLAME ************************************/
/**********************************************************************************************************************/

DetachedFlame::DetachedFlame(const RealFlame* const source, uint nbLeadSkeletons, FreeLeadSkeleton **leadSkeletons, 
			     uint nbSkeletons, FreePeriSkeleton **periSkeletons, const Texture* const tex, bool shadingType,
			     u_char samplingMethod) :
  NurbsFlame (source, nbSkeletons, 2, tex)
{
  m_distances = new float[NB_PARTICLES_MAX - 1 + m_nbFixedPoints];
  m_maxDistancesIndexes = new int[NB_PARTICLES_MAX - 1 + m_nbFixedPoints];
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

bool DetachedFlame::build()
{
  uint i, j, l;
  float vinc, vtmp, vtex;
  float dist_max;
  m_maxParticles = 0;
  vtex = 0.0f;

  for (i = 0; i < m_nbLeadSkeletons; i++)
    m_leadSkeletons[i]->move ();
  
  /* Déplacement et détermination du maximum */
  for (i = 0; i < m_nbSkeletons; i++)
    {
      m_periSkeletons[i]->move ();
      if(m_periSkeletons[i]->getSize () < 2)
	return false;
      if (m_periSkeletons[i]->getSize () > m_maxParticles)
	m_maxParticles = m_periSkeletons[i]->getSize ();
    }
  
  m_vsize = m_maxParticles+m_nbFixedPoints;
  
  vinc = 1.0f / (float)(m_vsize-1);
  vtmp = 0.0f;
  for (i = 0; i < m_vsize; i++){
    m_texTmp[i] = vtmp;
    vtmp += vinc;
  }
  
  /* Direction des u */
  for (i = 0; i < m_nbSkeletons; i++)
    {
      vtex += .15f;
      /* Problème pour la direction des v, le nombre de particules par squelettes n'est pas garanti */
      /* La solution retenue va ajouter des points de contrôles là où les points de contrôles sont le plus éloignés */
      if (m_periSkeletons[i]->getSize () < m_maxParticles)
	{
	  // Nombre de points de contrôle supplémentaires
	  uint nb_pts_supp = m_maxParticles - m_periSkeletons[i]->getSize ();
	  Point pt;
	  /* On calcule les distances entre les particules successives */
	  /* On prend également en compte l'origine du squelette ET les extrémités du guide */
	  /* On laisse les distances au carré pour des raisons évidentes de coût de calcul */	  
	  m_distances[0] = 
	    m_periSkeletons[i]->getLeadSkeleton ()->getParticle (0)->squaredDistanceFrom (*m_periSkeletons[i]->getParticle (0));
	  
	  for (j = 0; j < m_periSkeletons[i]->getSize () - 1; j++)
	    m_distances[j + 1] = 
	      m_periSkeletons[i]->getParticle (j)->squaredDistanceFrom(*m_periSkeletons[i]->getParticle (j + 1));
	  
	  m_distances[m_periSkeletons[i]->getSize ()] = 
	    m_periSkeletons[i]->getLastParticle ()->squaredDistanceFrom (*m_periSkeletons[i]->getLeadSkeleton ()->getLastParticle ());
	  
	  /* On cherche les indices des distances max */
	  /* On n'effectue pas un tri complet car on a seulement besoin de connaître les premiers */
	  for (l = 0; l < nb_pts_supp; l++)
	    {
	      dist_max = FLT_MIN;
	      for (j = 0; j < m_periSkeletons[i]->getSize () - 1 + m_nbFixedPoints; j++)
		{
		  if (m_distances[j] > dist_max)
		    {
		      m_maxDistancesIndexes[l] = j;
		      dist_max = m_distances[j];
		    }
		}
	      /* Il n'y a plus de place */
	      if (dist_max == FLT_MIN)
		m_maxDistancesIndexes[l] = -1;
	      else
		/* On met à la distance la plus grande pour ne plus la prendre en compte lors de la recherche suivante */
		m_distances[m_maxDistancesIndexes[l]] = 0;
	    }
	  /* Les particules les plus écartées sont maintenant connues, on peut passer à l'affichage */
	  /* Remplissage des points de contrôle */
	  setCtrlPoint (m_periSkeletons[i]->getLeadSkeleton ()->getParticle (0), vtex);
	  
	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == 0)
	      {
		pt = Point::pointBetween(m_periSkeletons[i]->getLeadSkeleton ()->getParticle (0), m_periSkeletons[i]->getParticle (0));
		setCtrlPoint (&pt, vtex);
	      }
	  
	  for (j = 0; j < m_periSkeletons[i]->getSize () - 1; j++)
	    {
	      /* On regarde s'il ne faut pas ajouter un point */
	      for (l = 0; l < nb_pts_supp; l++)
		{
		  if (m_maxDistancesIndexes[l] == (int)j + 1)
		    {
		      /* On peut référencer j+1 puisque normalement, m_maxDistancesIndexes[l] != j si j == m_periSkeletons[i]->getSize()-1 */
		      pt = Point::pointBetween(m_periSkeletons[i]->getParticle (j), m_periSkeletons[i]->getParticle (j + 1));
		      setCtrlPoint (&pt, vtex);
		    }
		}
	      setCtrlPoint (m_periSkeletons[i]->getParticle (j), vtex);
	    }
	  
	  bool prec = false;
	  
	  setCtrlPoint (m_periSkeletons[i]->getLastParticle (), vtex);
	  
	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == (int)m_periSkeletons[i]->getSize ())
	      {
		pt = Point::pointBetween(m_periSkeletons[i]->getLastParticle (),
					 m_periSkeletons[i]->getLeadSkeleton()->getLastParticle ());
		setCtrlPoint (&pt, vtex);
		prec = true;
	      }
	  
	  /* Points supplémentaires au cas où il n'y a "plus de place" ailleurs entre les particules */
	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == -1)
	      {
		if (!prec)
		  {
		    pt = *m_periSkeletons[i]->getLastParticle ();
		  }
		pt = Point::pointBetween (&pt, m_periSkeletons[i]->getLeadSkeleton()->getLastParticle ());
		setCtrlPoint (&pt, vtex);
		prec = true;
	      }
	  setCtrlPoint (m_periSkeletons[i]->getLeadSkeleton ()->getLastParticle (), vtex);
	}
      else
	{
	  /* Cas sans problème */
	  /* Remplissage des points de contrôle */
	  setCtrlPoint (m_periSkeletons[i]->getLeadSkeleton ()->getParticle (0), vtex);
	  for (j = 0; j < m_periSkeletons[i]->getSize (); j++)
	    {
	      setCtrlPoint (m_periSkeletons[i]->getParticle (j), vtex);
	    }
	  setCtrlPoint (m_periSkeletons[i]->getLeadSkeleton ()->getLastParticle (), vtex);
	}
      m_texTmp = m_texTmpSave;
    }
  
  /* On recopie les m_uorder squelettes pour fermer la NURBS */
  GLfloat *startCtrlPoints = m_ctrlPointsSave;
  for (i = 0; i < ((m_uorder-1)*m_vsize)*3; i++)
    *m_ctrlPoints++ = *startCtrlPoints++;
  m_ctrlPoints = m_ctrlPointsSave;
  
  GLfloat *startTexPoints = m_texPointsSave;
  for (i = 0; i < ((m_uorder-1)*m_vsize)*2; i++)
    *m_texPoints++ = *startTexPoints++;  
  m_texPoints = m_texPointsSave;
  
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
//       /* Roulette russe : tirage aléatoire entre 0 et 1 */
//       splitHeight = (uint)(split * m_leadSkeletons[i]->getSize()) - 1; 
//       leadSkeletonsArray[i] = m_leadSkeletons[i]->split(splitHeight);
//     }
      
//     periSkeletonsArray = new FreePeriSkeleton* [m_nbSkeletons];

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
    
//     m_parentFire->addDetachedFlame(new DetachedFlame(this, m_nbLeadSkeletons, leadSkeletonsArray, m_nbSkeletons, periSkeletonsArray, m_tex, m_solver));
//   }
// }

#include "realFlames.hpp"

#include "../scene/graphicsFn.hpp"
#include "abstractFires.hpp"
/**********************************************************************************************************************/
/*************************************** IMPLEMENTATION DE LA CLASSE LINEFLAME ****************************************/
/**********************************************************************************************************************/

LineFlame::LineFlame (FlameConfig* flameConfig, Scene *scene, Texture* const tex, Solver *s, 
		      const char *wickFileName, DetachableFireSource *parentFire, 
		      double detachedFlamesWidth, const char *wickName ) :
  RealFlame (flameConfig, (flameConfig->skeletonsNumber+2)*2 + 2, 3, tex, s),
  m_wick (wickFileName, flameConfig->skeletonsNumber, scene, flameConfig->position, wickName)
{
  Point pt;
  double largeur = .03;
  uint i,j;
  
  Point rootMoveFactorP(2,.1,.5), rootMoveFactorL(2,.1,1);
  
  m_nbLeadSkeletons = m_wick.getLeadPointsArraySize ();
  m_leadSkeletons = new LeadSkeleton *[m_nbLeadSkeletons];
  
  /** Allocation des squelettes p�riph�riques = deux par squelette p�riph�rique */
  /* plus 2 aux extr�mit�s pour fermer la NURBS */
  /* FAIT DANS BASICFLAME DESORMAIS */
  
  /* G�n�ration d'un c�t� des squelettes p�riph�riques */
  for (i = 1; i <= m_nbLeadSkeletons; i++)
    {
      pt = m_wick.getLeadPoint (i - 1)->m_pt;
      m_leadSkeletons[i - 1] = new LeadSkeleton (m_solver, pt, rootMoveFactorL, &flameConfig->leadLifeSpan);
      pt.z += (-largeur / 2.0);
      m_periSkeletons[i] = new PeriSkeleton (m_solver, pt, rootMoveFactorP, m_leadSkeletons[i - 1], &flameConfig->periLifeSpan);
    }
  
  /* G�n�ration de l'autre c�t� des squelettes p�riph�riques */
  for ( j = m_nbLeadSkeletons, i = m_nbLeadSkeletons + 2; j > 0; j--, i++)
    {
      pt = m_wick.getLeadPoint (j - 1)->m_pt;
      pt.z += (largeur / 2.0);
      m_periSkeletons[i] = new PeriSkeleton (m_solver, pt, rootMoveFactorP, m_leadSkeletons[j - 1], &flameConfig->periLifeSpan);
    }
  
  /* Ajout des extr�mit�s */
  pt = m_wick.getLeadPoint (0)->m_pt;
  pt.x += (-largeur / 2.0);
  m_periSkeletons[0] = new PeriSkeleton (m_solver, pt, rootMoveFactorP, m_leadSkeletons[0], &flameConfig->periLifeSpan);
  
  pt = m_wick.getLeadPoint (m_nbLeadSkeletons - 1)->m_pt;
  pt.x += (largeur / 2.0);
  m_periSkeletons[m_nbLeadSkeletons + 1] = 
    new PeriSkeleton (m_solver, pt,rootMoveFactorP, m_leadSkeletons[m_nbLeadSkeletons - 1], &flameConfig->periLifeSpan);
  
  m_parentFire = parentFire;

  m_detachedFlamesWidth = detachedFlamesWidth;
}

LineFlame::~LineFlame ()
{
}

void LineFlame::addForces (u_char perturbate, u_char fdf)
{ 
  uint ptxPrev=MAXINT,ptyPrev=MAXINT,ptzPrev=MAXINT;
  uint ptx,pty,ptz,i=0;
  vector < WickPoint * >*wickLeadPointsArray = m_wick.getLeadPointsArray();
  
  for (vector < WickPoint * >::iterator pointsIterator = wickLeadPointsArray->begin ();
       pointsIterator != wickLeadPointsArray->end (); pointsIterator++)
    {
      m_solver->findPointPosition((*pointsIterator)->m_pt, ptx, pty, ptz);
      
      if(ptxPrev != ptx || ptyPrev != pty || ptzPrev != ptz){
	// m_solver->addVsrc (ptx, i, ptz, .0004* exp((*pointsIterator)->y)*exp((*pointsIterator)->getY ()) );
	// m_solver->addVsrc (ptx, i, ptz, m_innerForce * exp(((double)pty+(*pointsIterator)->y) ));
	// m_solver->addVsrc (ptx, pty, ptz, m_innerForce * exp(i));
	
	switch(fdf){
	case FDF_LINEAR :
	  m_solver->addVsrc (ptx, pty, ptz, m_innerForce * ((*pointsIterator)->m_u + 1));
	  break;
	case FDF_BILINEAR :
	  m_solver->addVsrc (ptx, pty, ptz, m_innerForce * (*pointsIterator)->m_u * (*pointsIterator)->m_u );
	  break;
	case FDF_EXPONENTIAL :
	  m_solver->addVsrc (ptx, pty, ptz, .1 * exp(m_innerForce * 14 * (*pointsIterator)->m_u));
	  break;
	case FDF_GAUSS:
// 	  m_solver->addVsrc (ptx, pty, ptz, -m_innerForce * (*pointsIterator)->m_u* (*pointsIterator)->m_u+m_innerForce);
	  m_solver->addVsrc (ptx, pty, ptz, m_innerForce*expf(m_innerForce * 30 -((*pointsIterator)->m_u) * (*pointsIterator)->m_u)/(9));
	  break;
	case FDF_RANDOM:
	  m_solver->addVsrc (ptx, pty, ptz, m_innerForce * rand()/((double)RAND_MAX));
	  break;
	}
	
	switch(perturbate){
	case FLICKERING_VERTICAL :
	  if (m_perturbateCount == 5)
	    {
	      m_solver->addVsrc (ptx, pty, ptz, m_innerForce*5);
	      m_perturbateCount = 0;
	    }
	  else
	    m_perturbateCount++;
	  break;
	case FLICKERING_RANDOM :
	  m_solver->addVsrc (ptx, pty, ptz, 1*rand()/((double)RAND_MAX)-.5);
	  break;
	}
	ptxPrev = ptx; ptyPrev = pty; ptzPrev = ptz;
	i++;
      }
    }
}

Point LineFlame::getCenter ()
{
  Point averagePos;
  Particle *tmp;
  
  for (uint i = 0; i < m_nbLeadSkeletons-2  ; i++){
    tmp = m_leadSkeletons[i+1]->getMiddleParticle ();
    
    averagePos = (averagePos + *tmp)/(i+1);
  }
  
  averagePos += m_position;
  
  return averagePos;
}

void LineFlame::breakCheck()
{
  double split,proba;
  uint threshold=4;
  double detachThreshold=.95;
  /* Indice de la particule � laquelle un squelette est d�coup� */
  uint splitHeight;
  uint i;
  FreeLeadSkeleton **leadSkeletonsArray;
  FreePeriSkeleton **periSkeletonsArray;
  
  for ( i = 0; i < m_nbLeadSkeletons; i++){
    if(m_leadSkeletons[i]->getSize() < threshold)
      return;
    
    proba = (rand()/((double)RAND_MAX));
    
    /* Roulette russe : tirage al�atoire entre 0 et 1 */
    if( proba > detachThreshold){
      Point offset;
      
      /* Tirage entre 0.5 et 1 pour la hauteur du squelette */
      split = (rand()/(2*(double)RAND_MAX))+.5;
      
      leadSkeletonsArray = new FreeLeadSkeleton* [1];
      
      splitHeight = (uint)(split * (m_leadSkeletons[i]->getSize()-1) );
      leadSkeletonsArray[0] = m_leadSkeletons[i]->split(splitHeight);
      
      periSkeletonsArray = new FreePeriSkeleton* [4];
      
      offset.x=-m_detachedFlamesWidth * split;
      periSkeletonsArray[0] = leadSkeletonsArray[0]->dup(offset);
      
      splitHeight = (uint)(split * (m_periSkeletons[i+1]->getSize()-1));
      periSkeletonsArray[1] = m_periSkeletons[i+1]->split(splitHeight, leadSkeletonsArray[0]);
      
      offset.x=m_detachedFlamesWidth * split;
      periSkeletonsArray[2] = leadSkeletonsArray[0]->dup(offset);
      
      splitHeight = (uint)(split * (m_periSkeletons[m_nbSkeletons-i-1]->getSize()-1));
      periSkeletonsArray[3] = m_periSkeletons[m_nbSkeletons-i-1]->split(splitHeight, leadSkeletonsArray[0]);
      
      m_parentFire->addDetachedFlame(new DetachedFlame(this, 1, leadSkeletonsArray, 4, periSkeletonsArray, m_tex, m_solver));
    }
  }
}

void LineFlame::drawWick(bool displayBoxes)
{
  glPushMatrix();
  glTranslatef (m_position.x, m_position.y, m_position.z);
  m_wick.drawWick(displayBoxes);
  glPopMatrix();
}

void LineFlame::generateAndDrawSparks()
{
  uint i, j, k;
  uint life=30;
  
  /* Ajout de particules */
  if( (rand()/((double)RAND_MAX)) < .05){
    Point pos;
    double r = (rand()/((double)RAND_MAX));
    pos = (m_wick.getLeadPoint(m_wick.getLeadPointsArraySize()-1)->m_pt);
    pos = pos * r + m_wick.getLeadPoint(0)->m_pt ;
    Particle *spark = new Particle(pos, life);
    m_sparksList.push_back(spark);
  }
  
  /* D�placement et affichage des particules */
  for (list < Particle *>::iterator sparksListIterator = m_sparksList.begin (); 
       sparksListIterator != m_sparksList.end ();
       sparksListIterator++){
    Particle *par = *sparksListIterator;
    m_solver->findPointPosition(*par, i, j, k);

    (*sparksListIterator)->decreaseLife();
    
    if ((*sparksListIterator)->isDead ())
      {
	sparksListIterator =  m_sparksList.erase(sparksListIterator); 
	continue;
      }
        
    if ( i >= m_solver->getXRes()  )
      i = m_solver->getXRes()-1;
    if ( j >= m_solver->getXRes()  )
      j = m_solver->getYRes()-1;
    if ( k >= m_solver->getXRes()  )
      k = m_solver->getZRes()-1;
    
    double div = 1/(double)life;
    (*sparksListIterator)->x += m_solver->getU (i, j, k) * (*sparksListIterator)->m_lifespan*div;
    (*sparksListIterator)->y += m_solver->getV (i, j, k) * (*sparksListIterator)->m_lifespan*div;
    (*sparksListIterator)->z += m_solver->getW (i, j, k) * (*sparksListIterator)->m_lifespan*div;
    
    glColor4f (1.0, 1.0, 0.45, 1.0);
    glPushMatrix ();
    glTranslatef ((*sparksListIterator)->x, (*sparksListIterator)->y, (*sparksListIterator)->z);
    GraphicsFn::SolidSphere (0.01, 4, 4);
    glPopMatrix ();
  }
}

Vector LineFlame::getMainDirection()
{
  Vector direction;
  for(uint i = 0; i < m_nbLeadSkeletons; i++){
    direction = direction + *(m_leadSkeletons[i]->getParticle(0));
  }
  direction = direction / m_nbLeadSkeletons;

  return direction;
}

Point* LineFlame::getTop()
{ 
  return m_leadSkeletons[0]->getParticle(0); 
}

Point* LineFlame::getBottom()
{
 return m_leadSkeletons[0]->getRoot(); 
}

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE POINTFLAME ****************************************/
/**********************************************************************************************************************/

PointFlame::PointFlame ( FlameConfig* flameConfig, Texture* const tex, Solver * s, double rayon):
  RealFlame ( flameConfig, flameConfig->skeletonsNumber, 3, tex, s)
{
  uint i;
  double angle;
  
  m_nbLeadSkeletons = 1;
  m_leadSkeletons = new LeadSkeleton *[m_nbLeadSkeletons];
  
  m_leadSkeletons[0] = new LeadSkeleton (m_solver, m_position, Point(4,0,4), &flameConfig->leadLifeSpan);
  
  /* On cr�� les squelettes en cercle */
  angle = 0;
  for (i = 0; i < m_nbSkeletons; i++)
    {
      m_periSkeletons[i] = new PeriSkeleton (m_solver, Point (cos (angle) * rayon + m_position.x, 
							   m_position.y, 
							   sin (angle) * rayon + m_position.z),
					 Point(4,.75,4),
					 m_leadSkeletons[0], &flameConfig->periLifeSpan);
      angle += 2 * PI / m_nbSkeletons;
    }
}

PointFlame::~PointFlame ()
{  
}

void PointFlame::addForces (u_char perturbate, u_char fdf)
{
  m_solver->addVsrc (m_x, 1, m_z, m_innerForce);
  
  switch(perturbate){
  case FLICKERING_VERTICAL :
    if(m_perturbateCount>=2){
      m_solver->setVsrc(m_x, 1, m_z, 0.0002);
      m_perturbateCount = 0;
    }else
      m_perturbateCount++;
    break;
  case FLICKERING_RIGHT :
    if(m_perturbateCount>=24)
      m_perturbateCount = 0;
    else{
      if(m_perturbateCount>=20){
	for (uint i = -m_solver->getZRes() / 4 - 1; i <= m_solver->getZRes () / 4 + 1; i++)
	  for (uint j = -2 * m_solver->getYRes() / 4; j < -m_solver->getYRes() / 4; j++)
	    m_solver->setUsrc (m_solver->getXRes(), 
			       ((uint) (ceil (m_solver->getYRes() / 2.0))) + j, 
			       ((uint) (ceil (m_solver->getZRes() / 2.0))) + i, -.1);
      }
      m_perturbateCount++;
    }
    break;
  case FLICKERING_RANDOM :
    m_solver->addVsrc(m_x, 1, m_z, rand()/(10*(double)RAND_MAX));
    m_solver->addVsrc(m_x+1, 1, m_z, rand()/(10*(double)RAND_MAX));
    m_solver->addVsrc(m_x-1, 1, m_z, rand()/(10*(double)RAND_MAX));
    m_solver->addVsrc(m_x, 1, m_z+1, rand()/(10*(double)RAND_MAX));
    m_solver->addVsrc(m_x, 1, m_z-1, rand()/(10*(double)RAND_MAX));
    break;
  }
}

void PointFlame::drawWick (bool displayBoxes)
{
  double hauteur = 1 / 6.0;
  double largeur = 1 / 60.0;
  /* Affichage de la m�che */
  glPushMatrix ();
  glTranslatef (m_position.x, m_position.y-hauteur/2.0, m_position.z);
  glRotatef (-90.0, 1.0, 0.0, 0.0);
  glColor3f (0.0, 0.0, 0.0);
  GraphicsFn::SolidCylinder (largeur, hauteur, 10, 10);
  glTranslatef (0.0, 0.0, hauteur);
  GraphicsFn::SolidDisk (largeur, 10, 10);
  glPopMatrix ();
}

/**********************************************************************************************************************/
/*************************************** IMPLEMENTATION DE LA CLASSE DETACHEDFLAME ************************************/
/**********************************************************************************************************************/

DetachedFlame::DetachedFlame(RealFlame *source, uint nbLeadSkeletons, FreeLeadSkeleton **leadSkeletons, 
			     uint nbSkeletons, FreePeriSkeleton **periSkeletons, Texture* const tex, Solver *solver) :
  NurbsFlame (source, nbSkeletons, 2, tex)
{  
  m_distances = new double[NB_PARTICLES_MAX - 1 + m_nbFixedPoints];
  m_maxDistancesIndexes = new int[NB_PARTICLES_MAX - 1 + m_nbFixedPoints];
  m_nbLeadSkeletons = nbLeadSkeletons;
  m_leadSkeletons = leadSkeletons;
  m_periSkeletons = periSkeletons;
  
  m_solver = solver;
  locateInSolver();
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
  double vinc, vtmp, vtex;
  m_maxParticles = 0;
  vtex = 0;
  
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
  
  m_size = m_maxParticles+m_nbFixedPoints;
  
  vinc = 1.0 / (double)(m_size-1);
  vtmp = 0;
  for (i = 0; i < m_size; i++){
    m_texTmp[i] = vtmp;
    vtmp += vinc;
  }
  
  /* Direction des u */
  for (i = 0; i < m_nbSkeletons; i++)
    {
      vtex += .15;
      /* Probl�me pour la direction des v, le nombre de particules par squelettes n'est pas garanti */
      /* La solution retenue va ajouter des points de contr�les l� o� les points de contr�les sont le plus �loign�s */
      if (m_periSkeletons[i]->getSize () < m_maxParticles)
	{
	  // Nombre de points de contr�le suppl�mentaires
	  uint nb_pts_supp = m_maxParticles - m_periSkeletons[i]->getSize ();
	  Point pt;
	  /* On calcule les distances entre les particules successives */
	  /* On prend �galement en compte l'origine du squelette ET les extr�mit�s du guide */
	  /* On laisse les distances au carr� pour des raisons �videntes de co�t de calcul */	  
	  m_distances[0] = 
	    m_periSkeletons[i]->getLeadSkeleton ()->getParticle (0)->squaredDistanceFrom (m_periSkeletons[i]->getParticle (0));
	  
	  for (j = 0; j < m_periSkeletons[i]->getSize () - 1; j++)
	    m_distances[j + 1] = 
	      m_periSkeletons[i]->getParticle (j)->squaredDistanceFrom(m_periSkeletons[i]->getParticle (j + 1));
	  
	  m_distances[m_periSkeletons[i]->getSize ()] = 
	    m_periSkeletons[i]->getLastParticle ()->squaredDistanceFrom (m_periSkeletons[i]->getLeadSkeleton ()->getLastParticle ());
	  
	  /* On cherche les indices des distances max */
	  /* On n'effectue pas un tri complet car on a seulement besoin de conna�tre les premiers */
	  for (l = 0; l < nb_pts_supp; l++)
	    {
	      double dist_max = FLT_MIN;
	      
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
		/* On met � la distance la plus grande pour ne plus la prendre en compte lors de la recherche suivante */
		m_distances[m_maxDistancesIndexes[l]] = 0;
	    }
	  /* Les particules les plus �cart�es sont maintenant connues, on peut passer � l'affichage */
	  /* Remplissage des points de contr�le */
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
		      /* On peut r�f�rencer j+1 puisque normalement, m_maxDistancesIndexes[l] != j si j == m_periSkeletons[i]->getSize()-1 */
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
	  
	  /* Points suppl�mentaires au cas o� il n'y a "plus de place" ailleurs entre les particules */
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
	  /* Cas sans probl�me */
	  /* Remplissage des points de contr�le */
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
  for (i = 0; i < ((m_uorder-1)*m_size)*3; i++)
    *m_ctrlPoints++ = *startCtrlPoints++;
  m_ctrlPoints = m_ctrlPointsSave;
  
  GLfloat *startTexPoints = m_texPointsSave;
  for (i = 0; i < ((m_uorder-1)*m_size)*2; i++)
    *m_texPoints++ = *startTexPoints++;  
  m_texPoints = m_texPointsSave;
  
  /* Affichage en NURBS */
  m_uknotsCount = m_nbSkeletons + m_uorder + m_uorder - 1;
  m_vknotsCount = m_maxParticles + m_vorder + m_nbFixedPoints;
  
  for (i = 0; i < m_uknotsCount; i++)
    m_uknots[i] = i;
  
  for (j = 0; j < m_vknotsCount; j++)
    m_vknots[j] = (j < m_vorder) ? 0 : (j - m_vorder + 1);
  
  for (j = m_vknotsCount - m_vorder; j < m_vknotsCount; j++)
    m_vknots[j] = m_vknots[m_vknotsCount - m_vorder];
  
  m_vknots[m_vorder] += 0.9;
  
  if (m_vknots[m_vorder] > m_vknots[m_vorder + 1])
    for (j = m_vorder + 1; j < m_vknotsCount; j++)
      m_vknots[j] += 1;
  return true;
}



// void LineFlame::breakCheck()
// {
//   double split,proba=.5;
//   uint threshold=4;
//   uint splitHeight;
//   uint i,j;
//   FreeLeadSkeleton **leadSkeletonsArray;
//   FreePeriSkeleton **periSkeletonsArray;
  
//   for ( i = 0; i < m_nbLeadSkeletons; i++)
//     /* La division ne peut avoir lieu qu'au dessus du seuil */
//     if(m_leadSkeletons[i]->getSize() < threshold)
//       return;
  
//   split = (rand()/((double)RAND_MAX));
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
#include "basicFlames.hpp"

#include "../scene/scene.hpp"
#include "../scene/graphicsFn.hpp"

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE BASICFLAME ****************************************/
/**********************************************************************************************************************/

BasicFlame::BasicFlame(Solver *s, int nbSkeletons, int nbFixedPoints, Point& posRel, double innerForce, Scene *scene, 
		       const wxString& texname, GLint wrap_s, GLint wrap_t) :
  m_tex (texname, wrap_s, wrap_t)
{  
  m_solver = s;
  m_scene = scene;
  
  m_nbSkeletons = nbSkeletons;
  m_nbFixedPoints = nbFixedPoints;
  
  m_skeletons = new PeriSkeleton* [m_nbSkeletons];
  for (int i = 0; i < m_nbSkeletons; i++)
    m_skeletons[i]=NULL;
  
  m_uorder = 4;
  m_vorder = 4;
  
  /* Allocation des tableaux à la taille maximale pour les NURBS, */
  /* ceci afin d'éviter des réallocations trop nombreuses */
  m_ctrlPoints =  new GLfloat[(NB_PARTICULES + m_nbFixedPoints) * (m_nbSkeletons + m_uorder) * 3];
  m_uknots = new GLfloat[m_uorder + m_nbSkeletons + m_uorder - 1];
  m_vknots = new GLfloat[m_vorder + NB_PARTICULES + m_nbFixedPoints];
  m_distances = new double[NB_PARTICULES - 1 + m_nbFixedPoints + m_vorder];
  m_maxDistancesIndexes = new int[NB_PARTICULES - 1 + m_nbFixedPoints + m_vorder];
  
  m_nurbs = gluNewNurbsRenderer();
  gluNurbsProperty(m_nurbs, GLU_SAMPLING_TOLERANCE, 50.0);
  gluNurbsProperty(m_nurbs, GLU_DISPLAY_MODE, GLU_FILL);
  /* Important : ne fait pas la tesselation si la NURBS n'est pas dans le volume de vision */
  gluNurbsProperty(m_nurbs, GLU_CULLING, GL_TRUE);
  gluNurbsCallback(m_nurbs, GLU_NURBS_ERROR, (void(*)())nurbsError);
  
  m_toggle=false;
  
  m_perturbateCount=0;
  m_innerForce = innerForce;
  m_position=posRel;
}

BasicFlame::~BasicFlame()
{
  gluDeleteNurbsRenderer(m_nurbs);
  
  for (int i = 0; i < m_nbSkeletons; i++)
    delete m_skeletons[i];
  delete[]m_skeletons;
  
  delete[]m_ctrlPoints;
  delete[]m_uknots;
  delete[]m_vknots;
  
  delete[]m_distances;
  delete[]m_maxDistancesIndexes;
}

void CALLBACK BasicFlame::nurbsError(GLenum errorCode)
{
  const GLubyte *estring;
  
  estring = gluErrorString(errorCode);
  fprintf(stderr, "Erreur Nurbs : %s\n", estring);
  exit(0);
}

void BasicFlame::toggleSmoothShading()
{
  m_toggle = !m_toggle;
  if(m_toggle)
    gluNurbsProperty(m_nurbs, GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON);
  else
    gluNurbsProperty(m_nurbs, GLU_DISPLAY_MODE, GLU_FILL);
}



/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE LINEFLAME ****************************************/
/**********************************************************************************************************************/
LineFlame::LineFlame (Solver *s, int nbSkeletons, Point& posRel, double innerForce, Scene *scene, const wxString& textureName, const char *wickFileName, const char *wickName) :
  BasicFlame (s, (nbSkeletons+2)*2 + 2, 2, posRel, innerForce, scene, textureName, GL_CLAMP, GL_REPEAT),
  m_wick (wickFileName, nbSkeletons, scene, posRel, wickName)
{
  Point pt;
  double largeur = .03;
  int i,j;
  
  m_lifeSpanAtBirth = 6;
  Point rootMoveFactorP(2,.1,.5), rootMoveFactorL(2,.1,1);
  
  m_nbLeadSkeletons = m_wick.getLeadPointsArraySize ();
  m_leads = new LeadSkeleton *[m_nbLeadSkeletons];
  
  /* Allocation des squelettes périphériques = deux par squelette périphérique */
  /* plus 2 aux extrémités pour fermer la NURBS */
  /* FAIT DANS BASICFLAME DESORMAIS */
  // m_nbSkeletons = (m_nbLeadSkeletons * 2 + 2);
  // m_skeletons = new PeriSkeleton *[m_nbSkeletons];
  
  /* Génération d'un côté des squelettes périphériques */
  for (i = 1; i <= m_nbLeadSkeletons; i++)
    {
      pt = m_wick.getLeadPoint (i - 1)->m_pt;
      m_leads[i - 1] = new LeadSkeleton (m_solver, pt, rootMoveFactorL,m_lifeSpanAtBirth);
      pt.z += (-largeur / 2.0);
      m_skeletons[i] = new PeriSkeleton (m_solver, pt, rootMoveFactorP, m_leads[i - 1], m_lifeSpanAtBirth - 2);
    }
  
  /* Génération de l'autre côté des squelettes périphériques */
  for ( j = m_nbLeadSkeletons, i = m_nbLeadSkeletons + 2; j > 0; j--, i++)
  {
	pt = m_wick.getLeadPoint (j - 1)->m_pt;
	pt.z += (largeur / 2.0);
	m_skeletons[i] = new PeriSkeleton (m_solver, pt, rootMoveFactorP, m_leads[j - 1], m_lifeSpanAtBirth - 2);
  }
  cerr << (nbSkeletons+2)*2 + 2 << " " << i << endl;
  
  /* Ajout des extrémités */
  pt = m_wick.getLeadPoint (0)->m_pt;
  pt.x += (-largeur / 2.0);
  m_skeletons[0] = new PeriSkeleton (m_solver, pt, rootMoveFactorP, m_leads[0], m_lifeSpanAtBirth - 2);
  
  pt = m_wick.getLeadPoint (m_nbLeadSkeletons - 1)->m_pt;
  pt.x += (largeur / 2.0);
  m_skeletons[m_nbLeadSkeletons + 1] = new PeriSkeleton (m_solver, pt,rootMoveFactorP, m_leads[m_nbLeadSkeletons - 1], m_lifeSpanAtBirth - 2);
  
  m_solver->findPointPosition(posRel, m_x, m_y, m_z);
}

LineFlame::~LineFlame ()
{
  for (int i = 0; i < m_nbLeadSkeletons; i++)
    delete m_leads[i];
  delete[]m_leads;
}

void LineFlame::addForces (char perturbate, char fdf)
{ 
  int ptxPrev=MININT,ptyPrev=MININT,ptzPrev=MININT;
  int ptx,pty,ptz,i=0;
  vector < WickPoint * >*wickLeadPointsArray = m_wick.getLeadPointsArray();
  
  for (vector < WickPoint * >::iterator pointsIterator = wickLeadPointsArray->begin ();
       pointsIterator != wickLeadPointsArray->end (); pointsIterator++)
    {
      m_solver->findPointPosition((*pointsIterator)->m_pt, ptx, pty, ptz);
      
      if(ptxPrev != ptx || ptyPrev != pty || ptzPrev != ptz){
	// m_solver->addVsrc (ptx, i, ptz, .0004* exp((*pointsIterator)->y)*exp((*pointsIterator)->getY ()) );
	// m_solver->addVsrc (ptx, i, ptz, m_innerForce * exp(((double)pty+(*pointsIterator)->y) ));
	// m_solver->addVsrc (ptx, pty, ptz, m_innerForce * exp(i));

	//
	switch(fdf){
	case FDF_LINEAR :
	  m_solver->addVsrc (ptx, pty, ptz, m_innerForce * (*pointsIterator)->m_u* (*pointsIterator)->m_u);
	  break;
	case FDF_EXPONENTIAL :
	  m_solver->addVsrc (ptx, pty, ptz, .1 * exp(m_innerForce * 14 * (*pointsIterator)->m_u));
	  break;
	case FDF_GAUSS:
	  m_solver->addVsrc (ptx, pty, ptz, m_innerForce*expf(m_innerForce * 30 -((*pointsIterator)->m_u) * (*pointsIterator)->m_u)/(9));
	  break;
	case FDF_RANDOM:
	  m_solver->addVsrc (ptx, pty, ptz, m_innerForce * rand()/((double)RAND_MAX));
	  break;
	}

	switch(perturbate){
	case FLICKERING_VERTICAL :
	  if (m_perturbateCount == 2)
	    {
	      m_solver->addVsrc (ptx, pty, ptz, m_innerForce);
	      m_perturbateCount = 0;
	    }
	  else
	    m_perturbateCount++;
	  break;
	case FLICKERING_RANDOM :
	  m_solver->addVsrc (ptx, pty, ptz, .4*rand()/((double)RAND_MAX)-.2);
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
  
  for (int i = 0; i < m_nbLeadSkeletons  ; i++){
    tmp = m_leads[i]->getMiddleParticle ();
    
    averagePos = (averagePos + *tmp)/(i+1);
  }
  
  averagePos += m_position;

  return averagePos;
}

void LineFlame::build ()
{
  int i, j, l;
  int count;
  m_maxParticles = INT_MIN;
  
  for (int i = 0; i < m_nbLeadSkeletons  ; i++)
    m_leads[i]->move ();
  
  /* Déplacement et détermination du maximum */
  for (i = 0; i < m_nbSkeletons; i++)
    {
      if(m_skeletons[i]==NULL)
	cerr << "buite" << endl;
      m_skeletons[i]->move ();
      if (m_skeletons[i]->getSize () > m_maxParticles)
	m_maxParticles = m_skeletons[i]->getSize ();
    }
  
  //  cout << m_maxParticles << endl;
  // m_ctrlPoints = new Matrix_HPoint3Df(m_nbSkeletons+m_uorder-1,m_maxParticles+m_nbFixedPoints);
  //   m_uknots = new Vector_FLOAT(m_uorder+m_nbSkeletons+m_uorder-1);
  //   m_vknots = new Vector_FLOAT(m_vorder+m_maxParticles+m_nbFixedPoints);
  m_size = m_maxParticles + m_nbFixedPoints;
  
  /* Direction des u */
  for (i = 0; i < m_nbSkeletons + m_uorder - 1; i++)
    {
      /* Petit souci d'optimisation : vu que l'on doit boucler, on va réaliser la vérification */
      /* et éventuellement l'ajout de points de contrôles alors qu'on les a déjà faits */
      /* Voir plus tard si cela ne peut pas être amélioré */
      int k = i % m_nbSkeletons;
      /* Problème pour la direction des v, le nombre de particules par squelettes n'est pas garanti */
      /* La solution retenue va ajouter des points de contrôles là où les points de contrôles sont le plus éloignés */
      if (m_skeletons[k]->getSize () < m_maxParticles)
	{
	  int nb_pts_supp = m_maxParticles - m_skeletons[k]->getSize ();;	// Nombre de points de contrÃ´le supplémentaires
	  Point pt;
	  
	  //cout << "cas à la con" << endl;
	  /* On calcule les distances entre les particules successives */
	  /* On prend également en compte l'origine du squelette ET les extrémités du guide */
	  /* On laisse les distances au carré pour des raisons évidentes de cot de calcul */
	  
	  m_distances[0] = 
	    m_skeletons[k]->getLeadSkeleton ()->getParticle (0)->squaredDistanceFrom (m_skeletons[k]->getParticle (0));
	  
	  for (j = 0; j < m_skeletons[k]->getSize () - 1; j++)
	    m_distances[j + 1] = 
	      m_skeletons[k]->getParticle (j)->squaredDistanceFrom(m_skeletons[k]->getParticle (j + 1));
	  
	  m_distances[m_skeletons[k]->getSize ()] = 
	    m_skeletons[k]->getLastParticle ()->squaredDistanceFrom (m_skeletons[k]->getRoot ());

	  /* On cherche les indices des distances max */
	  /* On n'effectue pas un tri complet car on a seulement besoin de connaître les premiers */
			
	  //cout << nb_pts_supp << " points supp. et " << squelettes[k]->getSize() << " particules dans le squelette" << endl;
	  //cout << "Distances : " << endl;
	  for (l = 0; l < nb_pts_supp; l++)
	    {
	      double dist_max = FLT_MIN;

	      for (j = 0;
		   j < m_skeletons[k]->getSize () + 2; j++)
		{
		  if (m_distances[j] > dist_max)
		    {
		      m_maxDistancesIndexes[l] = j;
		      dist_max = m_distances[j];
		    }
		  //cout << m_distances[j] << " ";
		}
	      /* Il n'y a plus de place */
	      if (dist_max == FLT_MIN)
		m_maxDistancesIndexes[l] = -1;
	      else
		/* On met Ã¯Â¿Å“0 la distance la plus grande pour ne plus la prendre en compte lors de la recherche suivante */
		m_distances[m_maxDistancesIndexes[l]] = 0;

	      //cout << FLT_MIN << endl << "dist max :" << dist_max << endl << m_maxDistancesIndexes[l] << endl;
	    }
	  //cout << "prout" << endl;
	  /* Les particules les plus Ã¯Â¿Å“artÃ¯Â¿Å“s sont maintenant connues, on peut passer Ã¯Â¿Å“l'affichage */
	  count = 0;

	  /* Remplissage des points de contrôle */
	  setCtrlPoint (i, count++, m_skeletons[k]->getLeadSkeleton ()->getParticle (0), 1.0);

	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == 0)
	      {
		pt = Point::pointBetween(m_skeletons[k]->getLeadSkeleton ()->getParticle (0), m_skeletons[k]->getParticle (0));
		setCtrlPoint (i, count++, &pt);
	      }

	  for (j = 0; j < m_skeletons[k]->getSize () - 1; j++)
	    {
	      /* On regarde s'il ne faut pas ajouter un point */
	      for (l = 0; l < nb_pts_supp; l++)
		{
		  if (m_maxDistancesIndexes[l] == j + 1)
		    {
		      /* On peut référencer j+1 puisque normalement, m_maxDistancesIndexes[l] != j si j == m_skeletons[k]->getSize()-1 */
		      pt = Point::pointBetween(m_skeletons[k]->getParticle (j), m_skeletons[k]->getParticle (j + 1));
		      setCtrlPoint (i, count++, &pt);
		    }
		}
	      setCtrlPoint (i, count++, m_skeletons[k]->getParticle (j));
	    }
	  
	  setCtrlPoint (i, count++, m_skeletons[k]->getLastParticle ());

	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == m_skeletons[k]->getSize ())
	      {
		pt = Point::pointBetween(m_skeletons[k]->getRoot (), m_skeletons[k]-> getLastParticle ());
		setCtrlPoint (i, count++, &pt);
	      }

	  setCtrlPoint (i, count++, m_skeletons[k]->getRoot ());

	  bool prec = false;

	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == m_skeletons[k]->getSize () + 1)
	      {
		pt = Point::pointBetween(m_skeletons[k]->getRoot (),
					  m_skeletons[k]->getLeadSkeleton()->getRoot ());
		setCtrlPoint (i, count++, &pt);
		prec = true;
	      }

	  /* Points supplémentaires au cas oÃ¹ il n'y a "plus de place" ailleurs entre les particules */
	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == -1)
	      {
		if (!prec)
		  {
		    pt = *m_skeletons[k]->
		      getRoot ();
		  }
		pt = Point::pointBetween (&pt, m_skeletons[k]->getLeadSkeleton()->getRoot ());
		setCtrlPoint (i, count++, &pt);
		prec = true;
	      }
	  setCtrlPoint (i, count++, m_skeletons[k]->getLeadSkeleton ()->getRoot ());
	}
      else
	{
	  /* Cas sans problÃ¨me */
	  count = 0;
	  /* Remplissage des points de contrÃ´le */
	  setCtrlPoint (i, count++, m_skeletons[k]->getLeadSkeleton ()->getParticle (0), 1.0);
	  for (j = 0; j < m_skeletons[k]->getSize (); j++)
	    {
	      setCtrlPoint (i, count++, m_skeletons[k]->getParticle (j));
	    }
	  setCtrlPoint (i, count++, m_skeletons[k]->getRoot ());
	  setCtrlPoint (i, count++, m_skeletons[k]->getLeadSkeleton ()->getRoot ());
	}
    }

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
}

void LineFlame::drawWick(bool displayBoxes)
{
  glPushMatrix();
  glTranslatef (m_position.x, m_position.y, m_position.z);
  m_wick.drawWick(displayBoxes);
  glPopMatrix();
}

void LineFlame::drawFlame (bool displayParticle)
{
  int i;
    
  /* Affichage des particules */
  if(displayParticle){
    /* Déplacement et détermination du maximum */
    for (i = 0; i < m_nbSkeletons; i++)
      m_skeletons[i]->draw();
    for (i = 0; i < m_nbLeadSkeletons; i++)
      m_leads[i]->draw();
  }
  
  if (m_toggle)
    {
      glColor3f (1.0, 1.0, 1.0);

      gluBeginSurface (m_nurbs);
      gluNurbsSurface (m_nurbs, m_uknotsCount, m_uknots,
		       m_vknotsCount, m_vknots,
		       (m_maxParticles + m_nbFixedPoints) * 3,
		       3, m_ctrlPoints, m_uorder, m_vorder,
		       GL_MAP2_VERTEX_3);
      gluEndSurface (m_nurbs);
    }
  else
    {
      /* Correction "à la grosse" pour les UVs -> à voir par la suite */
      double vtex = 1.1 / (double) (m_maxParticles);

      GLdouble texpts[2][2][2] =
	{ {{0.0, 0}, {0.0, 0.25}}, {{vtex, 0}, {vtex, 0.25}} };

      glEnable (GL_TEXTURE_2D);
      glMap2d (GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4,
	       2, &texpts[0][0][0]);
      glEnable (GL_MAP2_TEXTURE_COORD_2);
      glMapGrid2d (20, 0.0, 1.0, 20, 0.0, 1.0);
      glEvalMesh2 (GL_POINT, 0, 20, 0, 20);
      glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glBindTexture (GL_TEXTURE_2D, m_tex.getTexture ());

      gluBeginSurface (m_nurbs);
      gluNurbsSurface (m_nurbs, m_uknotsCount, m_uknots, m_vknotsCount, m_vknots, (m_maxParticles + m_nbFixedPoints) * 3,
		       3, m_ctrlPoints, m_uorder, m_vorder, GL_MAP2_VERTEX_3);
      gluEndSurface (m_nurbs);

      glDisable (GL_TEXTURE_2D);
    }
}

Vector LineFlame::getMainDirection()
{
  Vector direction;
  for(int i = 0; i < m_nbLeadSkeletons; i++){
    direction = direction + *(m_leads[i]->getParticle(0));
  }
  direction = direction / m_nbLeadSkeletons;

  return direction;
}

/**********************************************************************************************************************/
/************************************** IMPLEMENTATION DE LA CLASSE POINTFLAME ****************************************/
/**********************************************************************************************************************/

PointFlame::PointFlame (Solver * s, int nbSkeletons, Point& posRel, double innerForce, Scene *scene, double rayon):
  BasicFlame (s, nbSkeletons, 3, posRel, innerForce, scene, _("textures/bougie2.png"), GL_CLAMP, GL_REPEAT)
//   cgCandleVertexShader (_("bougieShader.cg"),_("vertCandle"),context),
//   cgCandleFragmentShader (_("bougieShader.cg"),_("fragCandle"),context)
{
  int i;
  double angle;
  
  m_lifeSpanAtBirth = 6;
  
  m_lead = new LeadSkeleton (m_solver, posRel, Point(4,.75,4),m_lifeSpanAtBirth);
  /* On créé les squelettes en cercle */
  angle = 0;
  for (i = 0; i < m_nbSkeletons; i++)
    {
      m_skeletons[i] = new PeriSkeleton (m_solver, Point (cos (angle) * rayon + posRel.x, 
							   posRel.y, 
							   sin (angle) * rayon + posRel.z),
					 Point(4,.75,4),
					 m_lead, m_lifeSpanAtBirth);
      angle += 2 * PI / m_nbSkeletons;
    }
  
  m_solver->findPointPosition(posRel, m_x, m_y, m_z);
}

PointFlame::~PointFlame ()
{
  delete m_lead;
}

void PointFlame::addForces (char perturbate, char fdf)
{
  m_solver->addVsrc (m_x, 1, m_z, m_innerForce);
  
  switch(perturbate){
  case FLICKERING_VERTICAL :
    if(m_perturbateCount>=2){
      m_solver->setVsrc(m_x, 1, m_z, 0.2);
      m_perturbateCount = 0;
    }else
      m_perturbateCount++;
    break;
  case FLICKERING_RIGHT :
    if(m_perturbateCount>=24)
      m_perturbateCount = 0;
    else{
      if(m_perturbateCount>=20){
	for (int i = -m_solver->getZRes() / 4 - 1; i <= m_solver->getZRes () / 4 + 1; i++)
	  for (int j = -2 * m_solver->getYRes() / 4; j < -m_solver->getYRes() / 4; j++)
	    m_solver->setUsrc (m_solver->getXRes(), 
			       ((int) (ceil (m_solver->getYRes() / 2.0))) + j, 
			       ((int) (ceil (m_solver->getZRes() / 2.0))) + i, -1);
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

void PointFlame::build ()
{
  int i, j, l;
  int count;
  m_maxParticles = INT_MIN;
  
  /* Déplacement du guide */
  m_lead->move ();

  /* Déplacement et détermination du maximum */
  for (i = 0; i < m_nbSkeletons; i++)
    {
      m_skeletons[i]->move ();
      
      if (m_skeletons[i]->getSize () > m_maxParticles)
	m_maxParticles = m_skeletons[i]->getSize ();
    }
  
  //  cout << m_maxParticles << endl;
  // m_ctrlPoints = new Matrix_HPoint3Df(m_nbSkeletons+m_uorder-1,m_maxParticles+m_nbFixedPoints);
  //   m_uknots = new Vector_FLOAT(m_uorder+m_nbSkeletons+m_uorder-1);
  //   m_vknots = new Vector_FLOAT(m_vorder+m_maxParticles+m_nbFixedPoints);
  m_size = m_maxParticles + m_nbFixedPoints;

  /* Direction des u */
  for (i = 0; i < m_nbSkeletons + m_uorder - 1; i++)
    {
      /* Petit souci d'optimisation : vu que l'on doit boucler, on va rÃ©aliser la vÃ©rification */
      /* et Ã©ventuellement l'ajout de points de contrÃ´les alors qu'on les a dÃ©jÃ  faits */
      /* Voir plus tard si cela ne peut pas Ãªtre amÃ©liorÃ© */
      int k = i % m_nbSkeletons;	// (m_nbSkeletons+m_uorder - i - 2) % m_nbSkeletons; // On prend "Ã  l'envers" pour que la gÃ©nÃ©ration des normales soit "Ã  l'extÃ©rieur"
      /* ProblÃ¨me pour la direction des v, le nombre de particules par skeletons n'est pas garanti */
      /* La solution retenue va ajouter des points de contrÃ´les lÃ  oÃ¹ les points de contrÃ´les sont le plus Ã©loignÃ©s */
      if (m_skeletons[k]->getSize () < m_maxParticles)
	{
	  int nb_pts_supp = m_maxParticles - m_skeletons[k]->getSize ();;	// Nombre de points de contrÃ´e supplÃ©mentaires
	  Point pt;

	  //cout << "cas Ã  la con" << endl;
	  /* On calcule les distances entre les particules successives */
	  /* On prend Ã©galement en compte l'origine du squelette ET les extrÃ©mitÃ©s du guide */
	  /* On laisse les distances au carrÃ© pour des raisons Ã©videntes de coÃ»t de calcul */

	  m_distances[0] = m_lead->getParticle(0)->squaredDistanceFrom(m_skeletons[k]->getParticle(0));

	  for (j = 0; j < m_skeletons[k]->getSize () - 1; j++)
	    m_distances[j + 1] = m_skeletons[k]->getParticle(j)->squaredDistanceFrom(m_skeletons[k]->getParticle(j + 1));
	  
	  m_distances[m_skeletons[k]->getSize ()] = 
	    m_skeletons[k]->getLastParticle()->squaredDistanceFrom(m_skeletons[k]->getRoot ());
	  m_distances[m_skeletons[k]->getSize () + 1] =
	    m_skeletons[k]->getRoot()->squaredDistanceFrom(m_lead->getRoot ());

	  /* On cherche les indices des distances max */
	  /* On n'effectue pas un tri complet car on a seulement besoin de connaÃ®tre les premiers */

	  //cout << nb_pts_supp << " points supp. et " << m_skeletons[k]->getSize() << " particules dans le squelette" << endl;
	  //cout << "Distances : " << endl;
	  for (l = 0; l < nb_pts_supp; l++)
	    {
	      double dist_max = FLT_MIN;

	      for (j = 0; j < m_skeletons[k]->getSize () + 2; j++){
		if (m_distances[j] > dist_max)
		  {
		  m_maxDistancesIndexes[l] = j;
		  dist_max = m_distances[j];
		  }
		//cout << distances[j] << " ";
	      }
	      /* Il n'y a plus de place */
	      if (dist_max == FLT_MIN)
		m_maxDistancesIndexes[l] = -1;
	      else
		/* On met Ã  la distance la plus grande pour ne plus la prendre en compte lors de la recherche suivante */
		m_distances[m_maxDistancesIndexes[l]] = 0;

	      //cout << FLT_MIN << endl << "dist max :" << dist_max << endl << m_maxDistancesIndexes[l] << endl;
	    }
	  //cout << "prout" << endl;
	  /* Les particules les plus Ã¯Â¿=artÃ¯Â¿=s sont maintenant connues, on peut passer Ã¯Â¿=l'affichage */
	  count = 0;
	  
	  /* Remplissage des points de contrle */
	  setCtrlPoint (i, count++, m_lead->getParticle (0), 1.0);

	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == 0)
	      {
		pt = Point::pointBetween (m_lead->getParticle (0), m_skeletons[k]->getParticle(0));
		setCtrlPoint (i, count++, &pt);
	      }
	  
	  for (j = 0; j < m_skeletons[k]->getSize () - 1; j++)
	    {
	      /* On regarde s'il ne faut pas ajouter un point */
	      for (l = 0; l < nb_pts_supp; l++)
		{
		  if (m_maxDistancesIndexes[l] == j + 1)
		    {
		      /* On peut rÃ©fÃ©rencer j+1 puisque normalement, m_maxDistancesIndexes[l] != j si j == m_skeletons[k]->getSize()-1 */
		      pt = Point::pointBetween(m_skeletons[k]->getParticle(j), m_skeletons[k]->getParticle(j + 1));
		      setCtrlPoint (i, count++, &pt);
		    }
		}
	      setCtrlPoint (i, count++, m_skeletons[k]->getParticle (j));
	    }

	  setCtrlPoint (i, count++, m_skeletons[k]->getLastParticle ());

	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == m_skeletons[k]->getSize ())
	      {
		pt = Point::pointBetween (m_skeletons[k]->getRoot (), m_skeletons[k]->getLastParticle());
		setCtrlPoint (i, count++, &pt);
	      }

	  setCtrlPoint (i, count++, m_skeletons[k]->getRoot ());

	  bool prec = false;

	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == m_skeletons[k]->getSize () + 1)
	      {
		pt = Point::pointBetween (m_skeletons[k]->getRoot (), m_lead->getRoot ());
		setCtrlPoint (i, count++, &pt);
		prec = true;
	      }

	  /* Points supplÃ©mentaires au cas oÃ¹ il n'y a "plus de place" ailleurs entre les particules */
	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == -1)
	      {
		if (!prec)
		  pt = *m_skeletons[k]-> getRoot ();
		
		pt = Point::pointBetween (&pt, m_lead->getRoot());
		setCtrlPoint (i, count++, &pt);
		prec = true;
	      }
	  setCtrlPoint (i, count++, m_lead->getRoot ());
	}
      else
	{
	  /* Cas sans problÃ¨me */
	  count = 0;
	  /* Remplissage des points de contrÃ´le */
	  setCtrlPoint (i, count++, m_lead->getParticle (0), 1.0);
	  for (j = 0; j < m_skeletons[k]->getSize (); j++)
	    {
	      setCtrlPoint (i, count++, m_skeletons[k]->getParticle (j));
	    }
	  setCtrlPoint (i, count++, m_skeletons[k]->getRoot ());
	  setCtrlPoint (i, count++, m_lead->getRoot ());
	}
    }

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
}

void PointFlame::drawWick (bool displayBoxes)
{
  double hauteur = m_solver->getDimY() / 6.0;
  double largeur = m_solver->getDimX() / 60.0;
  /* Affichage de la mèche */
  glPushMatrix ();
  glTranslatef (m_position.x, m_position.y-hauteur/2.0, m_position.z);
  glRotatef (-90.0, 1.0, 0.0, 0.0);
  glColor3f (0.0, 0.0, 0.0);
  GraphicsFn::SolidCylinder (largeur, hauteur, 10, 10);
  glTranslatef (0.0, 0.0, hauteur);
  GraphicsFn::SolidDisk (largeur, 10, 10);
  glPopMatrix ();
}

void PointFlame::drawFlame (bool displayParticle)
{
  int i;
  
  /* Affichage des particules */
  if(displayParticle){
    /* Déplacement et détermination du maximum */
    for (i = 0; i < m_nbSkeletons; i++)
      m_skeletons[i]->draw();
    m_lead->draw();
  }  
  
  if (m_toggle)
    {
      glColor3f (1.0, 1.0, 1.0);
      gluBeginSurface (m_nurbs);
      gluNurbsSurface (m_nurbs, m_uknotsCount, m_uknots, m_vknotsCount,
		       m_vknots, (m_maxParticles + m_nbFixedPoints) * 3,
		       3, m_ctrlPoints, m_uorder, m_vorder,
		       GL_MAP2_VERTEX_3);
      gluEndSurface (m_nurbs);
    }
  else
    {
      /* Correction "à la grosse" pour les UVs -> à voir par la suite */
      double vtex = 1.0 / (double) (m_maxParticles);

      GLdouble texpts[2][2][2] = { {{0.0, 0}, {0.0, .5}}, {{vtex, 0}, {vtex, .5}} };      
      double angle, angle2;
      
      /* Déplacement de la texture de maniÃ¨re Ã  ce qu'elle reste "en face" de l'observateur */
      GLdouble m[4][4];

      glGetDoublev (GL_MODELVIEW_MATRIX, &m[0][0]);

      /* Position de la bougie = translation dans la matrice courante */
      Vector bougiepos(m[3][0], m[3][1], m[3][2]);

      /* Position de l'axe de regard de bougie dans le repère du monde = axe initial * Matrice de rotation */
      /* Attention, ne pas prendre la translation en plus !!!! */
      Vector worldLookAt(m[2][0], m[2][1], m[2][2]);
      Vector worldLookX(m[0][0], m[0][1], m[0][2]);
      Vector direction(-bougiepos.x, 0.0, -bougiepos.z);

      direction.normalize ();
      /* Apparemment, pas besoin de le normaliser, on laisse pour le moment */
      worldLookAt.normalize ();
      worldLookX.normalize ();
      
      angle = -acos (direction * worldLookAt);
      angle2 = acos (direction * worldLookX);
            
      if (angle2 < PI / 2.0) angle = PI - angle;
      
      /****************************************************************************************/
      glMap2d (GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, **texpts);
      glEnable (GL_MAP2_TEXTURE_COORD_2);
      glMapGrid2d (20, 0.0, 1.0, 20, 0.0, 1.0);
      glEvalMesh2 (GL_POINT, 0, 20, 0, 20);
      
//       cgCandleVertexShader.setModelViewProjectionMatrix();
//       cgCandleVertexShader.setTexTranslation(angle / (double) (PI));
//       cgCandleVertexShader.setInverseModelViewMatrix();
//       cgCandleFragmentShader.setTexture(&tex);      
//       cgCandleVertexShader.enableShader();
//       cgCandleFragmentShader.enableShader();
      
      glEnable (GL_TEXTURE_2D);

      glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glBindTexture (GL_TEXTURE_2D, m_tex.getTexture ());

      glMatrixMode (GL_TEXTURE);
      glPushMatrix ();
      glLoadIdentity ();

      glTranslatef (0.0, angle / (double) (PI), 0.0);

      gluBeginSurface (m_nurbs);
      gluNurbsSurface (m_nurbs, m_uknotsCount, m_uknots, m_vknotsCount,
		       m_vknots, (m_maxParticles + m_nbFixedPoints) * 3,
		       3, m_ctrlPoints, m_uorder, m_vorder,
		       GL_MAP2_VERTEX_3);
      
      gluEndSurface (m_nurbs);
      
      glPopMatrix();
      glDisable (GL_TEXTURE_2D);

      glMatrixMode (GL_MODELVIEW);
 //      cgCandleVertexShader.disableProfile();
//       cgCandleFragmentShader.disableProfile();
    }
}

#include "firmalampe.hpp"

#include "graphicsFn.hpp"
#include "scene.hpp"

#ifndef CALLBACK
#define CALLBACK
#endif

Firmalampe::Firmalampe (Solver * s, int nb, CPoint * centre, CPoint * pos,
			CgSVShader * shader, const char *meche_name, const char *filename, CScene *scene):
  Flame (s, centre, pos, filename,scene),
  m_wick (meche_name, nb, scene),
  m_tex (_("textures/firmalampe.png"), GL_CLAMP, GL_CLAMP)
{
  CPoint pt;
  double largeur = 0.03;
  m_lifeSpanAtBirth = 4;
  m_nbFixedPoints = 3;
  
  CPoint rootMoveFactorP(2,.1,.5), rootMoveFactorL(2,.1,1);
  
  m_nbLeadSkeletons = m_wick.getLeadPointsArraySize ();
  m_leads = new LeadSkeleton *[m_nbLeadSkeletons];

  /* Squelettes périphériques = deux par squelette périphérique */
  /* plus 2 aux extrémités pour fermer la NURBS */
  m_nbSkeletons = (m_nbLeadSkeletons * 2 + 2);
  m_skeletons = new PeriSkeleton *[m_nbSkeletons];

  /* Génération d'un côté des squelettes périphériques */
  for (int i = 1; i <= m_nbLeadSkeletons; i++)
    {
      pt = *m_wick.getLeadPoint (i - 1);
      m_leads[i - 1] = new LeadSkeleton (m_solver, m_position, pt, rootMoveFactorL,m_lifeSpanAtBirth);
      pt.z += (-largeur / 2.0);
      m_skeletons[i] = new PeriSkeleton (m_solver, m_position, pt, rootMoveFactorP,
					m_leads[i - 1], m_lifeSpanAtBirth - 2);
    }

  /* Génération de l'autre cÃ´té des squelettes périphériques */
  for (int j = m_nbLeadSkeletons, i = m_nbLeadSkeletons + 2; j > 0; j--, i++)
  {
	pt = *m_wick.getLeadPoint (j - 1);
	pt.z += (largeur / 2.0);
	m_skeletons[i] = new PeriSkeleton (m_solver, m_position, pt, rootMoveFactorP,
					  m_leads[j - 1], m_lifeSpanAtBirth - 2);
  }
  
  /* Ajout des extrémités */
  pt = *m_wick.getLeadPoint (0);
  pt.x += (-largeur / 2.0);
  m_skeletons[0] = new PeriSkeleton (m_solver, m_position,  pt, rootMoveFactorP, 
				    m_leads[0], m_lifeSpanAtBirth - 2);
  pt = *m_wick.getLeadPoint (m_nbLeadSkeletons - 1);
  pt.x += (largeur / 2.0);
  m_skeletons[m_nbLeadSkeletons + 1] = new PeriSkeleton (m_solver, m_position, pt,rootMoveFactorP,
						      m_leads[m_nbLeadSkeletons - 1], m_lifeSpanAtBirth - 2);

  /* Allocation des tableaux à la taille maximale pour les NURBS, */
  /* ceci afin d'éviter des réallocations trop nombreuses */
  m_ctrlPoints =  new GLfloat[(NB_PARTICULES + m_nbFixedPoints) * (m_nbSkeletons + m_uorder) * 3];
  m_uknots = new GLfloat[m_uorder + m_nbSkeletons + m_uorder - 1];
  m_vknots = new GLfloat[m_vorder + NB_PARTICULES + m_nbFixedPoints];
  m_distances = new double[NB_PARTICULES - 1 + m_nbFixedPoints + m_vorder];
  m_maxDistancesIndexes = new int[NB_PARTICULES - 1 + m_nbFixedPoints + m_vorder];
  
  m_solver->findPointPosition(centre, &m_x, &m_y, &m_z);

  m_cgShader = shader;
}

Firmalampe::~Firmalampe ()
{
  delete[]m_ctrlPoints;
  delete[]m_uknots;
  delete[]m_vknots;

  delete[]m_distances;
  delete[]m_maxDistancesIndexes;

  delete[]m_leads;
}

void
Firmalampe::add_forces (bool perturbate)
{
  /* Cellule(s) génératrice(s) */
  for (int i = 1; i < m_solver->getXRes() + 1; i++)
    for (int j = 1; j < m_solver->getYRes() + 1; j++)
      for (int k = 1; k < m_solver->getZRes() + 1; k++)
	m_solver->setVsrc (i, j, k, .02 / (double) (j));

  // cout << x << " " << z << endl;
  //  m_solver->addVsrc(x,1,z,.3);

  //~ m_solver->addVsrc (x - 4, 1, z, .1);
  //~ m_solver->addVsrc (x + 4, 1, z, .1);
  //~ m_solver->addVsrc (x - 2, 1, z, .1);
  //m_solver->addVsrc (x+2, 1, z, .01);
  //m_solver->addVsrc (x+2, 1, z, -.05);

  //~ m_solver->addVsrc (x+1, 1, z, .01);
  //~ m_solver->addVsrc (x, 1, z, .11);
  //~ m_solver->addVsrc (x-1, 1, z, .12);
  //~ m_solver->addVsrc (x-2, 1, z, .10);
  
  int ptx,pty,ptz,ind_max=0,i=0;
  double ymax=DBL_MIN;
  vector < CPoint * >*wickLeadPointsArray = m_wick.getLeadPointsArray();

  for (vector < CPoint * >::iterator pointsIterator =
	 wickLeadPointsArray->begin ();
       pointsIterator != wickLeadPointsArray->end (); pointsIterator++)
    {
      ptx = (int) ((*pointsIterator)->x * m_solver->getDimX() * m_solver->getXRes()) + 1 + m_solver->getXRes() / 2;
      pty = (int) ((*pointsIterator)->y * m_solver->getDimY() * m_solver->getYRes()) + 1 ;
      ptz = (int) ((*pointsIterator)->z * m_solver->getDimZ() * m_solver->getZRes()) + 1 + m_solver->getZRes() / 2;
      //cout << ptx << " " << pty << " " << ptz << endl;
      //cout << (*pointsIterator)->y << endl;
      for (int i = pty ; i > 0 ; i--) 
	//m_solver->addVsrc (ptx, i, ptz, .0004* exp((*pointsIterator)->y)*exp((*pointsIterator)->getY ()) );
	m_solver->addVsrc (ptx, i, ptz, .005* exp(((double)pty+(*pointsIterator)->y) ));
    }
  
  /* Recherche d'un point maximum */
  /* ProblÃ¨me marche pas si il y a plus d'un pic dans la mÃ¨che */
//   for (vector < CPoint * >::iterator pointsIterator = wickLeadPointsArray->begin ();
//        pointsIterator != wickLeadPointsArray->end (); 
//        pointsIterator++)
//     {
//       if( ymax < (*pointsIterator)->y ){
// 	ymax = (*pointsIterator)->y;
// 	ind_max = i;
//       }
//       i++;
//     }
  
//   ptx = (int) ( ((*wickLeadPointsArray)[ind_max])->x * m_solver->getDimX() *
// 		m_solver->x) + 2 + m_solver->getX () / 2;
//   pty = (int) ( ((*wickLeadPointsArray)[ind_max])->y * m_solver->getDimY() *
// 		m_solver->y) + 1 ;
//   ptz = (int) ( ((*wickLeadPointsArray)[ind_max])->z * m_solver->getDimZ() *
// 		m_solver->z) + 1 + m_solver->getZ () / 2;
	
//   m_solver->addVsrc (ptx-1, pty, ptz, 3 * ((*wickLeadPointsArray)[ind_max])->y );
//   m_solver->addVsrc (ptx, pty, ptz, 4 * ((*wickLeadPointsArray)[ind_max])->y );
//   m_solver->addVsrc (ptx+1, pty, ptz, 3 * ((*wickLeadPointsArray)[ind_max])->y );
  
  if (perturbate)
    perturbate_forces ();
}

void
Firmalampe::perturbate_forces ()
{
  if (m_perturbateCount == 4)
    {
      m_solver->addVsrc (m_x, 1, m_z, .4);
      m_solver->addVsrc (m_x+1, 1, m_z, .3);
      m_solver->addVsrc (m_x-1, 1, m_z, .3);
      m_perturbateCount = 0;
    }
  else
    m_perturbateCount++;
}

void
Firmalampe::eclaire ()
{
  Particle *tmp;
  
  m_nbLights = 0;

  switch_off_lights ();
  /* Déplacement des m_leads + éclairage */
  
  for (int i = 0; i < m_nbLeadSkeletons  ; i++){
    m_leads[i]->move ();
    
    tmp = m_leads[i]->getLastParticle ();
    
    m_nbLights++;
    if( (i < 8 ) ){
      m_lightPositions[i][0] = tmp->x;
      m_lightPositions[i][1] = tmp->y;
      m_lightPositions[i][2] = tmp->z;
      m_lightPositions[i][3] = 1.0;
    }
  }
}

void
Firmalampe::build ()
{
  int i, j, l;
  int count;
  m_maxParticles = INT_MIN;
  
  eclaire();

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
      /* Petit souci d'optimisation : vu que l'on doit boucler, on va réaliser la vérification */
      /* et éventuellement l'ajout de points de contrÃ´les alors qu'on les a déjÃ  faits */
      /* Voir plus tard si cela ne peut pas Ãªtre amélioré */
      int k = i % m_nbSkeletons;
      /* ProblÃ¨me pour la direction des v, le nombre de particules par squelettes n'est pas garanti */
      /* La solution retenue va ajouter des points de contrÃ´les lÃ  oÃ¹ les points de contrÃ´les sont le plus éloignés */
      if (m_skeletons[k]->getSize () < m_maxParticles)
	{
	  int nb_pts_supp = m_maxParticles - m_skeletons[k]->getSize ();;	// Nombre de points de contrÃ´le supplémentaires
	  CPoint pt;

	  //cout << "cas Ã  la con" << endl;
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
	  /* On n'effectue pas un tri complet car on a seulement besoin de connaÃ¯Â¿Å“re les premiers */
			
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

	  /* Remplissage des points de contrle */
	  setCtrlPoint (i, count++, m_skeletons[k]->getLeadSkeleton ()->getParticle (0), 1.0);

	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == 0)
	      {
		pt = CPoint::pointBetween(m_skeletons[k]->getLeadSkeleton ()->getParticle (0), m_skeletons[k]->getParticle (0));
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
		      pt = CPoint::pointBetween(m_skeletons[k]->getParticle (j), m_skeletons[k]->getParticle (j + 1));
		      setCtrlPoint (i, count++, &pt);
		    }
		}
	      setCtrlPoint (i, count++, m_skeletons[k]->getParticle (j));
	    }
	  
	  setCtrlPoint (i, count++, m_skeletons[k]->getLastParticle ());

	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == m_skeletons[k]->getSize ())
	      {
		pt = CPoint::pointBetween(m_skeletons[k]->getRoot (), m_skeletons[k]-> getLastParticle ());
		setCtrlPoint (i, count++, &pt);
	      }

	  setCtrlPoint (i, count++, m_skeletons[k]->getRoot ());

	  bool prec = false;

	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == m_skeletons[k]->getSize () + 1)
	      {
		pt = CPoint::pointBetween(m_skeletons[k]->getRoot (),
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
		pt = CPoint::pointBetween (&pt, m_skeletons[k]->getLeadSkeleton()->getRoot ());
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

void 
Firmalampe::drawWick()
{
  glPushMatrix();
  glTranslatef (m_position.x, m_position.y, m_position.z);
  m_wick.drawWick();
  glPopMatrix();
}

void
Firmalampe::drawFlame (bool displayParticle)
{
  int i;

  glPushMatrix();
  glTranslatef (m_position.x, m_position.y, m_position.z);
  /* Affichage des particules */
  if(displayParticle){
    /* Déplacement et détermination du maximum */
    for (i = 0; i < m_nbSkeletons; i++)
      m_skeletons[i]->draw();
    for (i = 0; i < m_nbLeadSkeletons; i++)
      m_leads[i]->draw();
  }
  
  GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 0.9 };
  GLfloat mat_ambient2[] = { 1.0, 1.0, 1.0, 1.0 };

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
      double vtex = 1.0 / (double) (m_maxParticles);

      GLdouble texpts[2][2][2] =
	{ {{0.0, 0}, {0.0, 1.0}}, {{vtex, 0}, {vtex, 1.0}} };

      glEnable (GL_TEXTURE_2D);

      glMaterialfv (GL_FRONT, GL_AMBIENT, mat_ambient2);
      glMaterialfv (GL_FRONT, GL_DIFFUSE, mat_diffuse);

      glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
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
  glPopMatrix();
}

void
Firmalampe::draw_shadowVolumes ()
{
  //int nbVol = 8 - 
  for (int i = 0; i < m_nbLights - 3; i++)
    {
      m_cgShader->setLightPos (m_lightPositions[i]);

      m_cgShader->enableProfile ();
      m_cgShader->bindProgram ();

      glPushMatrix ();
      glLoadIdentity ();
      m_cgShader->setModelViewMatrixToInverse ();
      glPopMatrix ();
      m_cgShader->setModelViewProjectionMatrix ();

      m_scene->draw_sceneWSV();

      m_cgShader->disableProfile ();
    }
}

void
Firmalampe::draw_shadowVolume (int i)
{
  m_cgShader->setLightPos (m_lightPositions[i]);
  
  m_cgShader->enableProfile ();
  m_cgShader->bindProgram ();
  
  glPushMatrix ();
  glLoadIdentity ();
  m_cgShader->setModelViewMatrixToInverse ();
  glPopMatrix ();
  m_cgShader->setModelViewProjectionMatrix ();
  
  m_scene->draw_sceneWSV();
  
  m_cgShader->disableProfile ();
}

void
Firmalampe::draw_shadowVolume2 (int i)
{
  double pos[4];
  //  int ind = i / SHADOW_SAMPLE_PER_LIGHT;
  //  int modulo = i % SHADOW_SAMPLE_PER_LIGHT;
  
  //   pos[0] = m_lightPositions[ind][0]+modulo*(m_lightPositions[ind+1][0]-m_lightPositions[ind][0])/SHADOW_SAMPLE_PER_LIGHT;
  //   pos[1] = m_lightPositions[ind][1]+modulo*(m_lightPositions[ind+1][1]-m_lightPositions[ind][1])/SHADOW_SAMPLE_PER_LIGHT;
  //   pos[2] = m_lightPositions[ind][2]+modulo*(m_lightPositions[ind+1][2]-m_lightPositions[ind][2])/SHADOW_SAMPLE_PER_LIGHT;
  
  pos[0] = m_lightPositions[0][0] +
    i * (m_lightPositions[m_nbLights][0] -
	 m_lightPositions[0][0]) / 5.0;
  pos[1] = m_lightPositions[0][1] +
    i * (m_lightPositions[m_nbLights][1] -
	 m_lightPositions[0][1]) / 5.0;
  pos[2] = m_lightPositions[0][2] +
    i * (m_lightPositions[m_nbLights][2] -
	 m_lightPositions[0][2]) / 5.0;
  
  m_cgShader->setLightPos (pos);
  
  m_cgShader->enableProfile ();
  m_cgShader->bindProgram ();
  
  glPushMatrix ();
  glLoadIdentity ();
  m_cgShader->setModelViewMatrixToInverse ();
  glPopMatrix ();
  m_cgShader->setModelViewProjectionMatrix ();
  
  m_scene->draw_sceneWSV();
  
  m_cgShader->disableProfile ();
}

void
Firmalampe::cast_shadows_double_multiple ()
{
  switch_off_lights ();
  m_scene->draw_sceneWTEX ();

  glBlendFunc (GL_ONE, GL_ONE);
  for (int i = 0; i < 1 /*m_nbLights *//**SHADOW_SAMPLE_PER_LIGHT*/ ;
       i++)
    {
      enable_only_ambient_light (i);
      glClear (GL_STENCIL_BUFFER_BIT);
      glDepthFunc (GL_LESS);
      glPushAttrib (GL_COLOR_BUFFER_BIT |
		    GL_DEPTH_BUFFER_BIT | GL_POLYGON_BIT |
		    GL_STENCIL_BUFFER_BIT);

      glColorMask (0, 0, 0, 0);
      glDepthMask (0);

      glDisable (GL_CULL_FACE);
      glEnable (GL_STENCIL_TEST);
      glEnable (GL_STENCIL_TEST_TWO_SIDE_EXT);

      glActiveStencilFaceEXT (GL_BACK);
      glStencilOp (GL_KEEP,	// stencil test fail
		   GL_DECR_WRAP_EXT,	// depth test fail
		   GL_KEEP);	// depth test pass
      glStencilMask (~0);
      glStencilFunc (GL_ALWAYS, 0, ~0);

      glActiveStencilFaceEXT (GL_FRONT);
      glStencilOp (GL_KEEP,	// stencil test fail
		   GL_INCR_WRAP_EXT,	// depth test fail
		   GL_KEEP);	// depth test pass
      glStencilMask (~0);
      glStencilFunc (GL_ALWAYS, 0, ~0);

      draw_shadowVolume2 (i);

      glPopAttrib ();

      glDepthFunc (GL_EQUAL);

      glEnable (GL_STENCIL_TEST);
      glStencilFunc (GL_EQUAL, 0, ~0);
      glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);

      m_scene->draw_sceneWTEX ();

      reset_diffuse_light (i);
    }
  glDisable (GL_STENCIL_TEST);
  for (int i = 0; i < m_nbLights /**SHADOW_SAMPLE_PER_LIGHT*/ ;
       i++)
    {
      enable_only_ambient_light (i);
    }
  m_scene->draw_sceneWTEX ();
  for (int i = 0; i < m_nbLights /**SHADOW_SAMPLE_PER_LIGHT*/ ;
       i++)
    {
      reset_diffuse_light (i);
    }
  switch_on_lights ();
  glBlendFunc (GL_ZERO, GL_SRC_COLOR);
  m_scene->draw_scene ();
}

void
Firmalampe::cast_shadows_double ()
{
  switch_off_lights ();
  m_scene->draw_sceneWTEX ();
  switch_on_lights ();

  glPushAttrib (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
		GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT);

  glColorMask (0, 0, 0, 0);
  glDepthMask (0);

  glDisable (GL_CULL_FACE);
  glEnable (GL_STENCIL_TEST);
  glEnable (GL_STENCIL_TEST_TWO_SIDE_EXT);

  glActiveStencilFaceEXT (GL_BACK);
  glStencilOp (GL_KEEP,	// stencil test fail
	       GL_DECR_WRAP_EXT,	// depth test fail
	       GL_KEEP);	// depth test pass
  glStencilMask (~0);
  glStencilFunc (GL_ALWAYS, 0, ~0);

  glActiveStencilFaceEXT (GL_FRONT);
  glStencilOp (GL_KEEP,	// stencil test fail
	       GL_INCR_WRAP_EXT,	// depth test fail
	       GL_KEEP);	// depth test pass
  glStencilMask (~0);
  glStencilFunc (GL_ALWAYS, 0, ~0);

  draw_shadowVolumes ();

  glPopAttrib ();

  /* On teste ensuite à l'endroit où il faut dessiner */
  glDepthFunc (GL_EQUAL);

  glEnable (GL_STENCIL_TEST);
  glStencilFunc (GL_EQUAL, 0, ~0);
  glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
  glBlendFunc (GL_ONE, GL_ONE);
  m_scene->draw_sceneWTEX ();

  glDisable (GL_STENCIL_TEST);
  //  glDepthFunc(GL_LESS);

  switch_off_lights ();
  glDisable (GL_STENCIL_TEST);
  for (int i = 0; i < m_nbLights /**SHADOW_SAMPLE_PER_LIGHT*/ ; i++)
    {
      enable_only_ambient_light (i);
    }
  m_scene->draw_sceneWTEX ();
  for (int i = 0; i < m_nbLights /**SHADOW_SAMPLE_PER_LIGHT*/ ; i++)
    {
      reset_diffuse_light (i);
    }

  switch_on_lights ();
  glBlendFunc (GL_ZERO, GL_SRC_COLOR);
  m_scene->draw_scene ();
}

CVector Firmalampe::get_main_direction()
{
  CVector direction;
  for(int i = 0; i < m_nbLeadSkeletons; i++){
    direction = direction + *(m_leads[i]->getParticle(0));
  }
  direction = direction / m_nbLeadSkeletons;

  return direction;
}

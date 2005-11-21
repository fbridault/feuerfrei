#include "bougie.hpp"

#include "graphicsFn.hpp"
#include "scene.hpp"

#ifndef CALLBACK
#define CALLBACK
#endif

Bougie::Bougie (Solver * s, int nb, CPoint * centre, CPoint * pos, double rayon, 
		CgSVShader * shader, const char *filename, CScene *scene, CGcontext *context):
  Flame (s, nb, centre, pos, filename, scene),
  m_tex (_("textures/bougie2.png"), GL_CLAMP, GL_REPEAT)
//   cgBougieVertexShader (_("bougieShader.cg"),_("vertBougie"),context),
//   cgBougieFragmentShader (_("bougieShader.cg"),_("fragBougie"),context)
{
  int i;
  double angle;  
  m_lifeSpanAtBirth = 6;
  m_nbFixedPoints = 3;
  
  m_lead = new LeadSkeleton (m_solver, m_position, *centre, CPoint(4,.75,4),m_lifeSpanAtBirth);
  /* On créé les squelettes en cercle */
  angle = 0;
  for (i = 0; i < m_nbSkeletons; i++)
    {
      m_skeletons[i] =
	new PeriSkeleton (m_solver, m_position, CPoint (cos (angle) * rayon + centre->x, 
						     centre->y, 
						     sin (angle) * rayon + centre->z),
			  CPoint(4,.75,4),
			  m_lead, m_lifeSpanAtBirth);

      angle += 2 * PI / m_nbSkeletons;
    }
  
  /* Allocation des tableaux à la taille maximale pour les NURBS, */
  /* ceci afin d'éviter des réallocations trop nombreuses */
  /* Note : NB_PARTICULES + 3 pour prendre en compte l'origine du squelette plus les extrémités du guide central */
  m_ctrlPoints = new GLfloat[(NB_PARTICULES + m_nbFixedPoints) * (m_nbSkeletons + m_uorder) * 3];
  m_uknots = new GLfloat[m_uorder + m_nbSkeletons + m_uorder - 1];
  m_vknots = new GLfloat[m_vorder + NB_PARTICULES + m_nbFixedPoints];
  m_distances = new double[NB_PARTICULES - 1 + m_nbFixedPoints + m_vorder];
  m_maxDistancesIndexes = new int[NB_PARTICULES - 1 + m_nbFixedPoints + m_vorder];
  
  m_solver->findPointPosition(*centre, m_x, m_y, m_z);
  
  m_cgShader = shader;
}

Bougie::~Bougie ()
{
  for (int i = 0; i < m_nbSkeletons; i++)
    delete m_skeletons[i];
  delete[]m_skeletons;
  delete[]m_ctrlPoints;
  delete[]m_uknots;
  delete[]m_vknots;

  delete[]m_distances;
  delete[]m_maxDistancesIndexes;

  delete m_lead;
}

void
Bougie::add_forces (bool perturbate)
{
  /* Cellule(s) g�n�ratrice(s) */
  for (int i = 1; i < m_solver->getXRes() + 1; i++)
    for (int j = 1; j < m_solver->getYRes() + 1; j++)
      for (int k = 1; k < m_solver->getZRes() + 1; k++)
	m_solver->addVsrc (i, j, k, .08 / (double) (j));
  m_solver->addVsrc (m_x, 1, m_z, .04);

  if (perturbate)
    perturbate_forces ();
}

void
Bougie::perturbate_forces ()
{
  if(m_perturbateCount==4){
    m_solver->setVsrc(((int)(ceil(m_solver->getXRes()/2.0))),1,((int)(ceil(m_solver->getZRes()/2.0))),0.25);
    m_perturbateCount = 0;
  }else
    m_perturbateCount++;

  //  for (int i = -m_solver->z / 4 - 1; i <= m_solver->getZ () / 4 + 1; i++)
  //     for (int j = -2 * m_solver->y / 4;
  // 	 j < -m_solver->y / 4; j++)
  //       m_solver->setUsrc (m_solver->x,
  // 			((int)
  // 			 (ceil (m_solver->y / 2.0))) +
  // 			j,
  // 			((int)
  // 			 (ceil (m_solver->z / 2.0))) +
  // 			i, -1);
}

void
Bougie::eclaire ()
{
  m_nbLights = 0;
  Particle *tmp;
		
  switch_off_lights ();
  /* D�placement du guide */
  m_lead->move ();
	
  for (int i = 0; i < m_lead->getSize (); i++)
    {
      tmp = m_lead->getParticle (i);

      m_nbLights++;
      m_lightPositions[i][0] = tmp->x;
      m_lightPositions[i][1] = tmp->y;
      m_lightPositions[i][2] = tmp->z;
      m_lightPositions[i][3] = 1.0;
    }
}

void
Bougie::build ()
{
  int i, j, l;
  int count;
  m_maxParticles = INT_MIN;
  
  eclaire();

  /* D�placement et d�termination du maximum */
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
      /* et éventuellement l'ajout de points de contrôles alors qu'on les a déjà faits */
      /* Voir plus tard si cela ne peut pas être amélioré */
      int k = i % m_nbSkeletons;	// (m_nbSkeletons+m_uorder - i - 2) % m_nbSkeletons; // On prend "à l'envers" pour que la génération des normales soit "à l'extérieur"
      /* Problème pour la direction des v, le nombre de particules par skeletons n'est pas garanti */
      /* La solution retenue va ajouter des points de contrôles là où les points de contrôles sont le plus éloignés */
      if (m_skeletons[k]->getSize () < m_maxParticles)
	{
	  int nb_pts_supp = m_maxParticles - m_skeletons[k]->getSize ();;	// Nombre de points de contrôe supplémentaires
	  CPoint pt;

	  //cout << "cas à la con" << endl;
	  /* On calcule les distances entre les particules successives */
	  /* On prend également en compte l'origine du squelette ET les extrémités du guide */
	  /* On laisse les distances au carré pour des raisons évidentes de coût de calcul */

	  m_distances[0] = m_lead->getParticle(0)->squaredDistanceFrom(m_skeletons[k]->getParticle(0));

	  for (j = 0; j < m_skeletons[k]->getSize () - 1; j++)
	    m_distances[j + 1] = m_skeletons[k]->getParticle(j)->squaredDistanceFrom(m_skeletons[k]->getParticle(j + 1));
	  
	  m_distances[m_skeletons[k]->getSize ()] = 
	    m_skeletons[k]->getLastParticle()->squaredDistanceFrom(m_skeletons[k]->getRoot ());
	  m_distances[m_skeletons[k]->getSize () + 1] =
	    m_skeletons[k]->getRoot()->squaredDistanceFrom(m_lead->getRoot ());

	  /* On cherche les indices des distances max */
	  /* On n'effectue pas un tri complet car on a seulement besoin de connaître les premiers */

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
		/* On met à la distance la plus grande pour ne plus la prendre en compte lors de la recherche suivante */
		m_distances[m_maxDistancesIndexes[l]] = 0;

	      //cout << FLT_MIN << endl << "dist max :" << dist_max << endl << m_maxDistancesIndexes[l] << endl;
	    }
	  //cout << "prout" << endl;
	  /* Les particules les plus ï¿=artï¿=s sont maintenant connues, on peut passer ï¿=l'affichage */
	  count = 0;
	  
	  /* Remplissage des points de contrle */
	  setCtrlPoint (i, count++, m_lead->getParticle (0), 1.0);

	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == 0)
	      {
		pt = CPoint::pointBetween (m_lead->getParticle (0), m_skeletons[k]->getParticle(0));
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
		      pt = CPoint::pointBetween(m_skeletons[k]->getParticle(j), m_skeletons[k]->getParticle(j + 1));
		      setCtrlPoint (i, count++, &pt);
		    }
		}
	      setCtrlPoint (i, count++, m_skeletons[k]->getParticle (j));
	    }

	  setCtrlPoint (i, count++, m_skeletons[k]->getLastParticle ());

	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == m_skeletons[k]->getSize ())
	      {
		pt = CPoint::pointBetween (m_skeletons[k]->getRoot (), m_skeletons[k]->getLastParticle());
		setCtrlPoint (i, count++, &pt);
	      }

	  setCtrlPoint (i, count++, m_skeletons[k]->getRoot ());

	  bool prec = false;

	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == m_skeletons[k]->getSize () + 1)
	      {
		pt = CPoint::pointBetween (m_skeletons[k]->getRoot (), m_lead->getRoot ());
		setCtrlPoint (i, count++, &pt);
		prec = true;
	      }

	  /* Points supplémentaires au cas où il n'y a "plus de place" ailleurs entre les particules */
	  for (l = 0; l < nb_pts_supp; l++)
	    if (m_maxDistancesIndexes[l] == -1)
	      {
		if (!prec)
		  pt = *m_skeletons[k]-> getRoot ();
		
		pt = CPoint::pointBetween (&pt, m_lead->getRoot());
		setCtrlPoint (i, count++, &pt);
		prec = true;
	      }
	  setCtrlPoint (i, count++, m_lead->getRoot ());
	}
      else
	{
	  /* Cas sans problème */
	  count = 0;
	  /* Remplissage des points de contrôle */
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

void
Bougie::drawWick ()
{
  double hauteur = m_solver->getDimY() / 6.0;
  double largeur = m_solver->getDimX() / 60.0;
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

void
Bougie::drawFlame (bool displayParticle)
{
  int i;

  glPushMatrix();
  glTranslatef (m_position.x, m_position.y, m_position.z);
  
  /* Affichage des particules */
  if(displayParticle){
    /* D�placement et d�termination du maximum */
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
      /* Correction "� la grosse" pour les UVs -> � voir par la suite */
      double vtex = 1.0 / (double) (m_maxParticles);

      GLdouble texpts[2][2][2] = { {{0.0, 0}, {0.0, .5}}, {{vtex, 0}, {vtex, .5}} };      
      double angle, angle2;
      
      /* D�placement de la texture de manière à ce qu'elle reste "en face" de l'observateur */
      GLdouble m[4][4];

      glGetDoublev (GL_MODELVIEW_MATRIX, &m[0][0]);

      /* Position de la bougie = translation dans la matrice courante */
      CVector bougiepos(m[3][0], m[3][1], m[3][2]);

      /* Position de l'axe de regard de bougie dans le rep�re du monde = axe initial * Matrice de rotation */
      /* Attention, ne pas prendre la translation en plus !!!! */
      CVector worldLookAt(m[2][0], m[2][1], m[2][2]);
      CVector worldLookX(m[0][0], m[0][1], m[0][2]);
      CVector direction(-bougiepos.x, 0.0, -bougiepos.z);

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
      
//       cgBougieVertexShader.setModelViewProjectionMatrix();
//       cgBougieVertexShader.setTexTranslation(angle / (double) (PI));
//       cgBougieVertexShader.setInverseModelViewMatrix();
//       cgBougieFragmentShader.setTexture(&tex);      
//       cgBougieVertexShader.enableShader();
//       cgBougieFragmentShader.enableShader();
      
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
 //      cgBougieVertexShader.disableProfile();
//       cgBougieFragmentShader.disableProfile();
    }
  glPopMatrix ();
}

void
Bougie::draw_shadowVolumes ()
{
  m_cgShader->enableProfile ();
  m_cgShader->bindProgram ();

  for (int i = 0; i < m_nbLights - 3; i++)
    {
      m_cgShader->setLightPos (m_lightPositions[i]);

      glPushMatrix ();
      glLoadIdentity ();
      m_cgShader->setModelViewMatrixToInverse ();
      glPopMatrix ();
      m_cgShader->setModelViewProjectionMatrix ();

      m_scene->draw_sceneWSV();
    }
  m_cgShader->disableProfile ();
}

void
Bougie::draw_shadowVolume (int i)
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
Bougie::draw_shadowVolume2 (int i)
{
  double pos[4];
  //  int ind = i/SHADOW_SAMPLE_PER_LIGHT;
  //  int modulo = i%SHADOW_SAMPLE_PER_LIGHT;

  //   pos[0] = m_lightPositions[ind][0]+modulo*(m_lightPositions[ind+1][0]-m_lightPositions[ind][0])/SHADOW_SAMPLE_PER_LIGHT;
  //   pos[1] = m_lightPositions[ind][1]+modulo*(m_lightPositions[ind+1][1]-m_lightPositions[ind][1])/SHADOW_SAMPLE_PER_LIGHT;
  //   pos[2] = m_lightPositions[ind][2]+modulo*(m_lightPositions[ind+1][2]-m_lightPositions[ind][2])/SHADOW_SAMPLE_PER_LIGHT;

  pos[0] = m_lightPositions[0][0] + i * (m_lightPositions[m_nbLights][0] - m_lightPositions[0][0]) / 5.0;
  pos[1] = m_lightPositions[0][1] + i * (m_lightPositions[m_nbLights][1] - m_lightPositions[0][1]) / 5.0;
  pos[2] = m_lightPositions[0][2] + i * (m_lightPositions[m_nbLights][2] - m_lightPositions[0][2]) / 5.0;

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
Bougie::cast_shadows_double_multiple ()
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

void
Bougie::cast_shadows_double ()
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


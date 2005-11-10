#include "firmalampe.hpp"

#include <stdlib.h>

#include "graphicsFn.hpp"

#ifndef CALLBACK
#define CALLBACK
#endif

Firmalampe::Firmalampe (Solver * s, int nb, CPoint * centre, CPoint * pos,
			CgSVShader * shader, const char *meche_name, const char *filename, CScene *scene):
  Flame (s, centre, pos, filename,scene),
  meche (meche_name, nb, scene),
  tex (_("textures/firmalampe.png"), GL_CLAMP, GL_CLAMP)
{
  CPoint pt;
  float largeur = 0.03;
  m_lifeSpanAtBirth = 4;
  
  CPoint rootMoveFactorP(2,.1,.5), rootMoveFactorL(2,.1,1);
  
  nbLeadSkeletons = meche.getLeadPointsArraySize ();
  guides = new LeadSkeleton *[nbLeadSkeletons];

  /* Squelettes périphériques = deux par squelette périphérique */
  /* plus 2 aux extrémités pour fermer la NURBS */
  nb_squelettes = (nbLeadSkeletons * 2 + 2);
  squelettes = new PeriSkeleton *[nb_squelettes];

  /* Génération d'un côté des squelettes périphériques */
  for (int i = 1; i <= nbLeadSkeletons; i++)
    {
      pt = *meche.getLeadPoint (i - 1);
      guides[i - 1] = new LeadSkeleton (solveur, position, pt, rootMoveFactorL,m_lifeSpanAtBirth);
      pt.z += (-largeur / 2.0);
      squelettes[i] = new PeriSkeleton (solveur, position, pt, rootMoveFactorP,
					guides[i - 1], m_lifeSpanAtBirth - 2);
    }

  /* Génération de l'autre cÃ´té des squelettes périphériques */
  for (int j = nbLeadSkeletons, i = nbLeadSkeletons + 2; j > 0; j--, i++)
  {
	pt = *meche.getLeadPoint (j - 1);
	pt.z += (largeur / 2.0);
	squelettes[i] = new PeriSkeleton (solveur, position, pt, rootMoveFactorP,
					  guides[j - 1], m_lifeSpanAtBirth - 2);
  }
  
  /* Ajout des extrémités */
  pt = *meche.getLeadPoint (0);
  pt.x += (-largeur / 2.0);
  squelettes[0] = new PeriSkeleton (solveur, position,  pt, rootMoveFactorP, 
				    guides[0], m_lifeSpanAtBirth - 2);
  pt = *meche.getLeadPoint (nbLeadSkeletons - 1);
  pt.x += (largeur / 2.0);
  squelettes[nbLeadSkeletons + 1] = new PeriSkeleton (solveur, position, pt,rootMoveFactorP,
						      guides[nbLeadSkeletons - 1], m_lifeSpanAtBirth - 2);

  /* Allocation des tableaux à la taille maximale pour les NURBS, */
  /* ceci afin d'éviter des réallocations trop nombreuses */
  ctrlpoints =  new GLfloat[(NB_PARTICULES + nb_pts_fixes) * (nb_squelettes + uorder) * 3];
  uknots = new GLfloat[uorder + nb_squelettes + uorder - 1];
  vknots = new GLfloat[vorder + NB_PARTICULES + nb_pts_fixes];
  distances = new float[NB_PARTICULES - 1 + nb_pts_fixes + vorder];
  indices_distances_max = new int[NB_PARTICULES - 1 + nb_pts_fixes + vorder];

  x = (int) (centre->x * solveur->getDimX() * solveur->getXRes ()) + 1 + solveur->getXRes () / 2;
  y = (int) (centre->y * solveur->getDimY() * solveur->getYRes ()) + 1;
  z = (int) (centre->z * solveur->getDimZ() * solveur->getZRes ()) + 1 + solveur->getZRes () / 2;

  cgShader = shader;
}

Firmalampe::~Firmalampe ()
{
  delete[]ctrlpoints;
  delete[]uknots;
  delete[]vknots;

  delete[]distances;
  delete[]indices_distances_max;

  delete[]guides;
}

void
Firmalampe::add_forces (bool perturbate)
{
  /* Cellule(s) génératrice(s) */
  for (int i = 1; i < solveur->getXRes() + 1; i++)
    for (int j = 1; j < solveur->getYRes() + 1; j++)
      for (int k = 1; k < solveur->getZRes() + 1; k++)
	solveur->setVsrc (i, j, k, .02 / (float) (j));

  // cout << x << " " << z << endl;
  //  solveur->addVsrc(x,1,z,.3);

  //~ solveur->addVsrc (x - 4, 1, z, .1);
  //~ solveur->addVsrc (x + 4, 1, z, .1);
  //~ solveur->addVsrc (x - 2, 1, z, .1);
  //solveur->addVsrc (x+2, 1, z, .01);
  //solveur->addVsrc (x+2, 1, z, -.05);

  //~ solveur->addVsrc (x+1, 1, z, .01);
  //~ solveur->addVsrc (x, 1, z, .11);
  //~ solveur->addVsrc (x-1, 1, z, .12);
  //~ solveur->addVsrc (x-2, 1, z, .10);
  
  int ptx,pty,ptz,ind_max=0,i=0;
  double ymax=DBL_MIN;
  vector < CPoint * >*wickLeadPointsArray = meche.getLeadPointsArray();

  for (vector < CPoint * >::iterator pointsIterator =
	 wickLeadPointsArray->begin ();
       pointsIterator != wickLeadPointsArray->end (); pointsIterator++)
    {
      ptx = (int) ((*pointsIterator)->x * solveur->getDimX() * solveur->getXRes()) + 1 + solveur->getXRes() / 2;
      pty = (int) ((*pointsIterator)->y * solveur->getDimY() * solveur->getYRes()) + 1 ;
      ptz = (int) ((*pointsIterator)->z * solveur->getDimZ() * solveur->getZRes()) + 1 + solveur->getZRes() / 2;
      //cout << ptx << " " << pty << " " << ptz << endl;
      //cout << (*pointsIterator)->y << endl;
      for (int i = pty ; i > 0 ; i--) 
	//solveur->addVsrc (ptx, i, ptz, .0004* exp((*pointsIterator)->y)*exp((*pointsIterator)->getY ()) );
	solveur->addVsrc (ptx, i, ptz, .005* exp(((double)pty+(*pointsIterator)->y) ));
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
  
//   ptx = (int) ( ((*wickLeadPointsArray)[ind_max])->x * solveur->getDimX() *
// 		solveur->x) + 2 + solveur->getX () / 2;
//   pty = (int) ( ((*wickLeadPointsArray)[ind_max])->y * solveur->getDimY() *
// 		solveur->y) + 1 ;
//   ptz = (int) ( ((*wickLeadPointsArray)[ind_max])->z * solveur->getDimZ() *
// 		solveur->z) + 1 + solveur->getZ () / 2;
	
//   solveur->addVsrc (ptx-1, pty, ptz, 3 * ((*wickLeadPointsArray)[ind_max])->y );
//   solveur->addVsrc (ptx, pty, ptz, 4 * ((*wickLeadPointsArray)[ind_max])->y );
//   solveur->addVsrc (ptx+1, pty, ptz, 3 * ((*wickLeadPointsArray)[ind_max])->y );
  
  if (perturbate)
    perturbate_forces ();
}

void
Firmalampe::perturbate_forces ()
{
  if (perturbate_count == 4)
    {
      solveur->addVsrc (x, 1, z, .4);
      solveur->addVsrc (x+1, 1, z, .3);
      solveur->addVsrc (x-1, 1, z, .3);
      perturbate_count = 0;
    }
  else
    perturbate_count++;
}

void
Firmalampe::eclaire ()
{
  Particle *tmp;
  
  nb_lights = 0;

  switch_off_lights ();
  /* Déplacement des guides + éclairage */
  
  for (int i = 0; i < nbLeadSkeletons  ; i++){
    guides[i]->move ();
    
    tmp = guides[i]->getLastParticle ();
    
    nb_lights++;
    if( (i < 8 ) ){
      lightPositions[i][0] = tmp->x;
      lightPositions[i][1] = tmp->y;
      lightPositions[i][2] = tmp->z;
      lightPositions[i][3] = 1.0;
    }
  }
}

void
Firmalampe::build ()
{
  int i, j, l;
  int count;
  max_particles = INT_MIN;
  
  eclaire();

  /* Déplacement et détermination du maximum */
  for (i = 0; i < nb_squelettes; i++)
    {
      squelettes[i]->move ();
      if (squelettes[i]->getSize () > max_particles)
	max_particles = squelettes[i]->getSize ();
    }
  
  //  cout << max_particles << endl;
  // ctrlpoints = new Matrix_HPoint3Df(nb_squelettes+uorder-1,max_particles+nb_pts_fixes);
  //   uknots = new Vector_FLOAT(uorder+nb_squelettes+uorder-1);
  //   vknots = new Vector_FLOAT(vorder+max_particles+nb_pts_fixes);
  size = max_particles + nb_pts_fixes;

  /* Direction des u */
  for (i = 0; i < nb_squelettes + uorder - 1; i++)
    {
      /* Petit souci d'optimisation : vu que l'on doit boucler, on va réaliser la vérification */
      /* et éventuellement l'ajout de points de contrÃ´les alors qu'on les a déjÃ  faits */
      /* Voir plus tard si cela ne peut pas Ãªtre amélioré */
      int k = i % nb_squelettes;
      /* ProblÃ¨me pour la direction des v, le nombre de particules par squelettes n'est pas garanti */
      /* La solution retenue va ajouter des points de contrÃ´les lÃ  oÃ¹ les points de contrÃ´les sont le plus éloignés */
      if (squelettes[k]->getSize () < max_particles)
	{
	  int nb_pts_supp = max_particles - squelettes[k]->getSize ();;	// Nombre de points de contrÃ´le supplémentaires
	  CPoint pt;

	  //cout << "cas Ã  la con" << endl;
	  /* On calcule les distances entre les particules successives */
	  /* On prend également en compte l'origine du squelette ET les extrémités du guide */
	  /* On laisse les distances au carré pour des raisons évidentes de cot de calcul */

	  distances[0] = 
	    squelettes[k]->getLeadSkeleton ()->getParticle (0)->squaredDistanceFrom (squelettes[k]->getParticle (0));

	  for (j = 0; j < squelettes[k]->getSize () - 1; j++)
	    distances[j + 1] = 
	      squelettes[k]->getParticle (j)->squaredDistanceFrom(squelettes[k]->getParticle (j + 1));
	  
	  distances[squelettes[k]->getSize ()] = 
	    squelettes[k]->getLastParticle ()->squaredDistanceFrom (squelettes[k]->getRoot ());

	  /* On cherche les indices des distances max */
	  /* On n'effectue pas un tri complet car on a seulement besoin de connaÃ¯Â¿Å“re les premiers */
			
	  //cout << nb_pts_supp << " points supp. et " << squelettes[k]->getSize() << " particules dans le squelette" << endl;
	  //cout << "Distances : " << endl;
	  for (l = 0; l < nb_pts_supp; l++)
	    {
	      float dist_max = FLT_MIN;

	      for (j = 0;
		   j < squelettes[k]->getSize () + 2; j++)
		{
		  if (distances[j] > dist_max)
		    {
		      indices_distances_max[l] = j;
		      dist_max = distances[j];
		    }
		  //cout << distances[j] << " ";
		}
	      /* Il n'y a plus de place */
	      if (dist_max == FLT_MIN)
		indices_distances_max[l] = -1;
	      else
		/* On met Ã¯Â¿Å“0 la distance la plus grande pour ne plus la prendre en compte lors de la recherche suivante */
		distances[indices_distances_max[l]] = 0;

	      //cout << FLT_MIN << endl << "dist max :" << dist_max << endl << indices_distances_max[l] << endl;
	    }
	  //cout << "prout" << endl;
	  /* Les particules les plus Ã¯Â¿Å“artÃ¯Â¿Å“s sont maintenant connues, on peut passer Ã¯Â¿Å“l'affichage */
	  count = 0;

	  /* Remplissage des points de contrle */
	  setCtrlPoint (i, count++, squelettes[k]->getLeadSkeleton ()->getParticle (0), 1.0);

	  for (l = 0; l < nb_pts_supp; l++)
	    if (indices_distances_max[l] == 0)
	      {
		pt = CPoint::pointBetween(squelettes[k]->getLeadSkeleton ()->getParticle (0), squelettes[k]->getParticle (0));
		setCtrlPoint (i, count++, &pt);
	      }

	  for (j = 0; j < squelettes[k]->getSize () - 1; j++)
	    {
	      /* On regarde s'il ne faut pas ajouter un point */
	      for (l = 0; l < nb_pts_supp; l++)
		{
		  if (indices_distances_max[l] == j + 1)
		    {
		      /* On peut référencer j+1 puisque normalement, indices_distances_max[l] != j si j == squelettes[k]->getSize()-1 */
		      pt = CPoint::pointBetween(squelettes[k]->getParticle (j), squelettes[k]->getParticle (j + 1));
		      setCtrlPoint (i, count++, &pt);
		    }
		}
	      setCtrlPoint (i, count++, squelettes[k]->getParticle (j));
	    }
	  
	  setCtrlPoint (i, count++, squelettes[k]->getLastParticle ());

	  for (l = 0; l < nb_pts_supp; l++)
	    if (indices_distances_max[l] == squelettes[k]->getSize ())
	      {
		pt = CPoint::pointBetween(squelettes[k]->getRoot (), squelettes[k]-> getLastParticle ());
		setCtrlPoint (i, count++, &pt);
	      }

	  setCtrlPoint (i, count++, squelettes[k]->getRoot ());

	  bool prec = false;

	  for (l = 0; l < nb_pts_supp; l++)
	    if (indices_distances_max[l] == squelettes[k]->getSize () + 1)
	      {
		pt = CPoint::pointBetween(squelettes[k]->getRoot (),
					  squelettes[k]->getLeadSkeleton()->getRoot ());
		setCtrlPoint (i, count++, &pt);
		prec = true;
	      }

	  /* Points supplémentaires au cas oÃ¹ il n'y a "plus de place" ailleurs entre les particules */
	  for (l = 0; l < nb_pts_supp; l++)
	    if (indices_distances_max[l] == -1)
	      {
		if (!prec)
		  {
		    pt = *squelettes[k]->
		      getRoot ();
		  }
		pt = CPoint::pointBetween (&pt, squelettes[k]->getLeadSkeleton()->getRoot ());
		setCtrlPoint (i, count++, &pt);
		prec = true;
	      }
	  setCtrlPoint (i, count++, squelettes[k]->getLeadSkeleton ()->getRoot ());
	}
      else
	{
	  /* Cas sans problÃ¨me */
	  count = 0;
	  /* Remplissage des points de contrÃ´le */
	  setCtrlPoint (i, count++, squelettes[k]->getLeadSkeleton ()->getParticle (0), 1.0);
	  for (j = 0; j < squelettes[k]->getSize (); j++)
	    {
	      setCtrlPoint (i, count++, squelettes[k]->getParticle (j));
	    }
	  setCtrlPoint (i, count++, squelettes[k]->getRoot ());
	  setCtrlPoint (i, count++, squelettes[k]->getLeadSkeleton ()->getRoot ());
	}
    }

  /* Affichage en NURBS */
  uknotsCount = nb_squelettes + uorder + uorder - 1;
  vknotsCount = max_particles + vorder + nb_pts_fixes;

  for (i = 0; i < uknotsCount; i++)
    uknots[i] = i;

  for (j = 0; j < vknotsCount; j++)
    vknots[j] = (j < vorder) ? 0 : (j - vorder + 1);

  for (j = vknotsCount - vorder; j < vknotsCount; j++)
    vknots[j] = vknots[vknotsCount - vorder];

  vknots[vorder] += 0.9;

  if (vknots[vorder] > vknots[vorder + 1])
    for (j = vorder + 1; j < vknotsCount; j++)
      vknots[j] += 1;
}

void 
Firmalampe::drawWick()
{
  glPushMatrix();
  glTranslatef (position.x, position.y, position.z);
  glCallList (MECHE);
  glPopMatrix();
}

void
Firmalampe::drawFlame (bool displayParticle)
{
  int i;

  glPushMatrix();
  glTranslatef (position.x, position.y, position.z);
  /* Affichage des particules */
  if(displayParticle){
    /* Déplacement et détermination du maximum */
    for (i = 0; i < nb_squelettes; i++)
      squelettes[i]->draw();
    for (i = 0; i < nbLeadSkeletons; i++)
      guides[i]->draw();
  }
  
  GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 0.9 };
  GLfloat mat_ambient2[] = { 1.0, 1.0, 1.0, 1.0 };

  if (toggle)
    {
      glColor3f (1.0, 1.0, 1.0);

      gluBeginSurface (nurbs);
      gluNurbsSurface (nurbs, uknotsCount, uknots,
		       vknotsCount, vknots,
		       (max_particles + nb_pts_fixes) * 3,
		       3, ctrlpoints, uorder, vorder,
		       GL_MAP2_VERTEX_3);
      gluEndSurface (nurbs);
    }
  else
    {
      /* Correction "à la grosse" pour les UVs -> à voir par la suite */
      float vtex = 1.0 / (float) (max_particles);

      GLfloat texpts[2][2][2] =
	{ {{0.0, 0}, {0.0, 1.0}}, {{vtex, 0}, {vtex, 1.0}} };

      glEnable (GL_TEXTURE_2D);

      glMaterialfv (GL_FRONT, GL_AMBIENT, mat_ambient2);
      glMaterialfv (GL_FRONT, GL_DIFFUSE, mat_diffuse);

      glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
      glMap2f (GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4,
	       2, &texpts[0][0][0]);
      glEnable (GL_MAP2_TEXTURE_COORD_2);
      glMapGrid2f (20, 0.0, 1.0, 20, 0.0, 1.0);
      glEvalMesh2 (GL_POINT, 0, 20, 0, 20);
      glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glBindTexture (GL_TEXTURE_2D, tex.getTexture ());

      gluBeginSurface (nurbs);
      gluNurbsSurface (nurbs, uknotsCount, uknots,
		       vknotsCount, vknots,
		       (max_particles + nb_pts_fixes) * 3,
		       3, ctrlpoints, uorder, vorder,
		       GL_MAP2_VERTEX_3);
      gluEndSurface (nurbs);

      glDisable (GL_TEXTURE_2D);
    }
  glPopMatrix();
}

void
Firmalampe::draw_shadowVolumes (GLint objects_list_wsv)
{
  //int nbVol = 8 - 
  for (int i = 0; i < nb_lights - 3; i++)
    {
      cgShader->setLightPos (lightPositions[i]);

      cgShader->enableProfile ();
      cgShader->bindProgram ();

      glPushMatrix ();
      glLoadIdentity ();
      cgShader->setModelViewMatrixToInverse ();
      glPopMatrix ();
      cgShader->setWorldViewMatrixToIdentity ();

      glCallList (objects_list_wsv);

      cgShader->disableProfile ();
    }
}

void
Firmalampe::draw_shadowVolume (GLint objects_list_wsv, int i)
{
  cgShader->setLightPos (lightPositions[i]);
  
  cgShader->enableProfile ();
  cgShader->bindProgram ();
  
  glPushMatrix ();
  glLoadIdentity ();
  cgShader->setModelViewMatrixToInverse ();
  glPopMatrix ();
  cgShader->setWorldViewMatrixToIdentity ();
  
  glCallList (objects_list_wsv);
  
  cgShader->disableProfile ();
}

void
Firmalampe::draw_shadowVolume2 (GLint objects_list_wsv, int i)
{
  float pos[4];
  //  int ind = i / SHADOW_SAMPLE_PER_LIGHT;
  //  int modulo = i % SHADOW_SAMPLE_PER_LIGHT;
  
  //   pos[0] = lightPositions[ind][0]+modulo*(lightPositions[ind+1][0]-lightPositions[ind][0])/SHADOW_SAMPLE_PER_LIGHT;
  //   pos[1] = lightPositions[ind][1]+modulo*(lightPositions[ind+1][1]-lightPositions[ind][1])/SHADOW_SAMPLE_PER_LIGHT;
  //   pos[2] = lightPositions[ind][2]+modulo*(lightPositions[ind+1][2]-lightPositions[ind][2])/SHADOW_SAMPLE_PER_LIGHT;
  
  pos[0] = lightPositions[0][0] +
    i * (lightPositions[nb_lights][0] -
	 lightPositions[0][0]) / 5.0;
  pos[1] = lightPositions[0][1] +
    i * (lightPositions[nb_lights][1] -
	 lightPositions[0][1]) / 5.0;
  pos[2] = lightPositions[0][2] +
    i * (lightPositions[nb_lights][2] -
	 lightPositions[0][2]) / 5.0;
  
  cgShader->setLightPos (pos);
  
  cgShader->enableProfile ();
  cgShader->bindProgram ();
  
  glPushMatrix ();
  glLoadIdentity ();
  cgShader->setModelViewMatrixToInverse ();
  glPopMatrix ();
  cgShader->setWorldViewMatrixToIdentity ();
  
  glCallList (objects_list_wsv);
  
  cgShader->disableProfile ();
}

void
Firmalampe::cast_shadows_double_multiple (GLint objects_list_wsv)
{
  switch_off_lights ();
  sc->draw_sceneWTEX ();

  glBlendFunc (GL_ONE, GL_ONE);
  for (int i = 0; i < 1 /*nb_lights *//**SHADOW_SAMPLE_PER_LIGHT*/ ;
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

      draw_shadowVolume2 (objects_list_wsv, i);

      glPopAttrib ();

      glDepthFunc (GL_EQUAL);

      glEnable (GL_STENCIL_TEST);
      glStencilFunc (GL_EQUAL, 0, ~0);
      glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);

      sc->draw_sceneWTEX ();

      reset_diffuse_light (i);
    }
  glDisable (GL_STENCIL_TEST);
  for (int i = 0; i < nb_lights /**SHADOW_SAMPLE_PER_LIGHT*/ ;
       i++)
    {
      enable_only_ambient_light (i);
    }
  sc->draw_sceneWTEX ();
  for (int i = 0; i < nb_lights /**SHADOW_SAMPLE_PER_LIGHT*/ ;
       i++)
    {
      reset_diffuse_light (i);
    }
  switch_on_lights ();
  glBlendFunc (GL_ZERO, GL_SRC_COLOR);
  sc->draw_scene ();
}

void
Firmalampe::cast_shadows_double (GLint objects_list_wsv)
{
  switch_off_lights ();
  sc->draw_sceneWTEX ();
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

  draw_shadowVolumes (objects_list_wsv);

  glPopAttrib ();

  /* On teste ensuite à l'endroit où il faut dessiner */
  glDepthFunc (GL_EQUAL);

  glEnable (GL_STENCIL_TEST);
  glStencilFunc (GL_EQUAL, 0, ~0);
  glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
  glBlendFunc (GL_ONE, GL_ONE);
  sc->draw_sceneWTEX ();

  glDisable (GL_STENCIL_TEST);
  //  glDepthFunc(GL_LESS);

  switch_off_lights ();
  glDisable (GL_STENCIL_TEST);
  for (int i = 0; i < nb_lights /**SHADOW_SAMPLE_PER_LIGHT*/ ;
       i++)
    {
      enable_only_ambient_light (i);
    }
  sc->draw_sceneWTEX ();
  for (int i = 0; i < nb_lights /**SHADOW_SAMPLE_PER_LIGHT*/ ;
       i++)
    {
      reset_diffuse_light (i);
    }

  switch_on_lights ();
  glBlendFunc (GL_ZERO, GL_SRC_COLOR);
  sc->draw_scene ();
}

CVector Firmalampe::get_main_direction()
{
  CVector direction;
  for(int i = 0; i < nbLeadSkeletons; i++){
    direction = direction + *(guides[i]->getParticle(0));
  }
  direction = direction / nbLeadSkeletons;

  return direction;
}

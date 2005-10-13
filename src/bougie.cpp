#include "bougie.hpp"

#include <stdlib.h>

#include "graphicsFn.hpp"

#ifndef CALLBACK
#define CALLBACK
#endif

extern void draw_scene ();

Bougie::Bougie (Solver * s, int nb, CPoint * centre, CPoint * pos,
		double rayon, CgSVShader * shader, const char *filename, CScene *scene, CGcontext *context):
  Flame (s, nb, centre, pos, filename, scene),
  tex ("textures/bougie2.png", GL_CLAMP, GL_REPEAT),
  cgBougieVertexShader ("bougieShader.cg","vertBougie",context),
  cgBougieFragmentShader ("bougieShader.cg","fragBougie",context)
{
  int i;
  double angle;

  guide = new LeadSkeleton (solveur, position, *centre);
  /* On créé les squelettes en cercle */
  angle = 0;
  for (i = 0; i < nb_squelettes; i++)
    {
      squelettes[i] =
	new PeriSkeleton (solveur, position,
			  CPoint (cos (angle) * rayon +
				  centre->getX (),
				  centre->getY (),
				  sin (angle) * rayon +
				  centre->getZ ()), guide);

      angle += 2 * PI / nb_squelettes;
    }

  /* Allocation des tableaux à la taille maximale pour les NURBS, */
  /* ceci afin d'éviter des réallocations trop nombreuses */
  /* Note : NB_PARTICULES + 3 pour prendre en compte l'origine du squelette plus les extrémités du guide central */
  ctrlpoints = new GLfloat[(NB_PARTICULES + nb_pts_fixes) * (nb_squelettes + uorder) * 3];
  uknots = new GLfloat[uorder + nb_squelettes + uorder - 1];
  vknots = new GLfloat[vorder + NB_PARTICULES + nb_pts_fixes];
  distances = new float[NB_PARTICULES - 1 + nb_pts_fixes + vorder];
  indices_distances_max =
    new int[NB_PARTICULES - 1 + nb_pts_fixes + vorder];

  x = (int) (centre->getX () * dim_x * solveur->getX ()) + 1 + solveur->getX () / 2;
  y = (int) (centre->getY () * dim_y * solveur->getY ()) + 1;
  z = (int) (centre->getZ () * dim_z * solveur->getZ ()) + 1 + solveur->getZ () / 2;

  cgShader = shader;
}

Bougie::~Bougie ()
{
  delete[]ctrlpoints;
  delete[]uknots;
  delete[]vknots;

  delete[]distances;
  delete[]indices_distances_max;

  delete guide;
}

void
Bougie::add_forces (bool perturbate)
{
  /* Cellule(s) génératrice(s) */
  for (int i = 1; i < solveur->getX () + 1; i++)
    for (int j = 1; j < solveur->getY () + 1; j++)
      for (int k = 1; k < solveur->getZ () + 1; k++)
	solveur->setVsrc (i, j, k, .08 / (float) (j));
  solveur->addVsrc (x, 1, z, .04);

  if (perturbate)
    perturbate_forces ();
}

void
Bougie::perturbate_forces ()
{
  if(perturbate_count==4){
    solveur->setVsrc(((int)(ceil(solveur->getX()/2.0))),1,((int)(ceil(solveur->getZ()/2.0))),0.25);
    perturbate_count = 0;
  }else
    perturbate_count++;

  //  for (int i = -solveur->getZ () / 4 - 1; i <= solveur->getZ () / 4 + 1; i++)
  //     for (int j = -2 * solveur->getY () / 4;
  // 	 j < -solveur->getY () / 4; j++)
  //       solveur->setUsrc (solveur->getX (),
  // 			((int)
  // 			 (ceil (solveur->getY () / 2.0))) +
  // 			j,
  // 			((int)
  // 			 (ceil (solveur->getZ () / 2.0))) +
  // 			i, -1);
}

void
Bougie::eclaire ()
{
  nb_lights = 0;
  Particle *tmp;
		
  switch_off_lights ();
  /* D�placement du guide */
  guide->move ();
	
  for (int i = 0; i < guide->getSize (); i++)
    {
      tmp = guide->getElt (i);

      nb_lights++;
      lightPositions[i][0] = tmp->getX ();
      lightPositions[i][1] = tmp->getY ();
      lightPositions[i][2] = tmp->getZ ();
      lightPositions[i][3] = 1.0;
    }
}

void
Bougie::build ()
{
  int i, j, l;
  int count;
  max_particles = INT_MIN;
  
  eclaire();

  /* D�placement et d�termination du maximum */
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
      /* et éventuellement l'ajout de points de contrôles alors qu'on les a déjà faits */
      /* Voir plus tard si cela ne peut pas être amélioré */
      int k = i % nb_squelettes;	// (nb_squelettes+uorder - i - 2) % nb_squelettes; // On prend "à l'envers" pour que la génération des normales soit "à l'extérieur"
      /* Problème pour la direction des v, le nombre de particules par squelettes n'est pas garanti */
      /* La solution retenue va ajouter des points de contrôles là où les points de contrôles sont le plus éloignés */
      if (squelettes[k]->getSize () < max_particles)
	{
	  int nb_pts_supp = max_particles - squelettes[k]->getSize ();;	// Nombre de points de contrôe supplémentaires
	  CPoint pt;

	  //cout << "cas à la con" << endl;
	  /* On calcule les distances entre les particules successives */
	  /* On prend également en compte l'origine du squelette ET les extrémités du guide */
	  /* On laisse les distances au carré pour des raisons évidentes de coût de calcul */

	  distances[0] =
	    guide->getElt(0)->squaredDistanceFrom(squelettes[k]->getElt(0));

	  for (j = 0; j < squelettes[k]->getSize () - 1; j++)
	    distances[j + 1] =
	      squelettes[k]->getElt(j)->squaredDistanceFrom(squelettes[k]->getElt(j + 1));
	  
	  distances[squelettes[k]->getSize ()] = 
	    squelettes[k]->getLastElt()->squaredDistanceFrom(squelettes[k]->getOrigine ());
	  distances[squelettes[k]->getSize () + 1] =
	    squelettes[k]->getOrigine()->squaredDistanceFrom(guide->getOrigine ());

	  /* On cherche les indices des distances max */
	  /* On n'effectue pas un tri complet car on a seulement besoin de connaître les premiers */

	  //cout << nb_pts_supp << " points supp. et " << squelettes[k]->getSize() << " particules dans le squelette" << endl;
	  //cout << "Distances : " << endl;
	  for (l = 0; l < nb_pts_supp; l++)
	    {
	      float dist_max = FLT_MIN;

	      for (j = 0; j < squelettes[k]->getSize () + 2; j++){
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
		/* On met à la distance la plus grande pour ne plus la prendre en compte lors de la recherche suivante */
		distances[indices_distances_max[l]] = 0;

	      //cout << FLT_MIN << endl << "dist max :" << dist_max << endl << indices_distances_max[l] << endl;
	    }
	  //cout << "prout" << endl;
	  /* Les particules les plus ï¿=artï¿=s sont maintenant connues, on peut passer ï¿=l'affichage */
	  count = 0;
	  
	  /* Remplissage des points de contrle */
	  setCtrlPoint (i, count++, guide->getElt (0), 1.0);

	  for (l = 0; l < nb_pts_supp; l++)
	    if (indices_distances_max[l] == 0)
	      {
		pt = CPoint::pointBetween (guide->getElt (0), squelettes[k]->getElt(0));
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
		      pt = CPoint::pointBetween(squelettes[k]->getElt(j), squelettes[k]->getElt(j + 1));
		      setCtrlPoint (i, count++, &pt);
		    }
		}
	      setCtrlPoint (i, count++, squelettes[k]->getElt (j));
	    }

	  setCtrlPoint (i, count++, squelettes[k]->getLastElt ());

	  for (l = 0; l < nb_pts_supp; l++)
	    if (indices_distances_max[l] == squelettes[k]->getSize ())
	      {
		pt = CPoint::pointBetween (squelettes[k]->getOrigine (), squelettes[k]->getLastElt());
		setCtrlPoint (i, count++, &pt);
	      }

	  setCtrlPoint (i, count++, squelettes[k]->getOrigine ());

	  bool prec = false;

	  for (l = 0; l < nb_pts_supp; l++)
	    if (indices_distances_max[l] == squelettes[k]->getSize () + 1)
	      {
		pt = CPoint::pointBetween (squelettes[k]->getOrigine (), guide->getOrigine ());
		setCtrlPoint (i, count++, &pt);
		prec = true;
	      }

	  /* Points supplémentaires au cas où il n'y a "plus de place" ailleurs entre les particules */
	  for (l = 0; l < nb_pts_supp; l++)
	    if (indices_distances_max[l] == -1)
	      {
		if (!prec)
		  pt = *squelettes[k]-> getOrigine ();
		
		pt = CPoint::pointBetween (&pt, guide->getOrigine());
		setCtrlPoint (i, count++, &pt);
		prec = true;
	      }
	  setCtrlPoint (i, count++, guide->getOrigine ());
	}
      else
	{
	  /* Cas sans problème */
	  count = 0;
	  /* Remplissage des points de contrôle */
	  setCtrlPoint (i, count++, guide->getElt (0), 1.0);
	  for (j = 0; j < squelettes[k]->getSize (); j++)
	    {
	      setCtrlPoint (i, count++, squelettes[k]->getElt (j));
	    }
	  setCtrlPoint (i, count++, squelettes[k]->getOrigine ());
	  setCtrlPoint (i, count++, guide->getOrigine ());
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
Bougie::drawWick ()
{
  float hauteur = dim_y / 6.0;
  float largeur = dim_x / 60.0;
  /* Affichage de la m�che */
  glPushMatrix ();
  glTranslatef (position.getX (), position.getY()-hauteur/2.0, position.getZ ());
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
  glTranslatef (position.getX (), position.getY (), position.getZ ());
  /* Affichage des particules */
  if(displayParticle){
    /* D�placement et d�termination du maximum */
    for (i = 0; i < nb_squelettes; i++)
      squelettes[i]->draw();
    guide->draw();
  }  
  
  if (toggle)
    {
      glColor3f (1.0, 1.0, 1.0);
      gluBeginSurface (nurbs);
      gluNurbsSurface (nurbs, uknotsCount, uknots, vknotsCount,
		       vknots, (max_particles + nb_pts_fixes) * 3,
		       3, ctrlpoints, uorder, vorder,
		       GL_MAP2_VERTEX_3);
      gluEndSurface (nurbs);
    }
  else
    {
      /* Correction "� la grosse" pour les UVs -> à voir par la suite */
      float vtex = 1.0 / (float) (max_particles);

      GLfloat texpts[2][2][2] =	{ {{0.0, 0}, {0.0, .5}}, {{vtex, 0}, {vtex, .5}} };
      CPoint bougiepos;
      CVector worldLookAt, worldLookX, direction;
      double angle, angle2;

      /************* Déplacement de la texture de manière à ce qu'elle reste "en face" de l'observateur ********************/
      GLfloat m[4][4];

      glGetFloatv (GL_MODELVIEW_MATRIX, &m[0][0]);

      /* Position de la bougie = translation dans la matrice courante */
      bougiepos.setX (m[3][0]);
      bougiepos.setY (m[3][1]);
      bougiepos.setZ (m[3][2]);

      /* Position de l'axe de regard de bougie dans le repère du monde = axe initial * Matrice de rotation */
      /* Attention, ne pas prendre la translation en plus !!!! */
      worldLookAt.setX (m[2][0]);
      worldLookAt.setY (m[2][1]);
      worldLookAt.setZ (m[2][2]);

      worldLookX.setX (m[0][0]);
      worldLookX.setY (m[0][1]);
      worldLookX.setZ (m[0][2]);
      
      direction.setX (-bougiepos.getX ());
      direction.setY (0.0);
      direction.setZ (-bougiepos.getZ ());
      
      direction.normalize ();
      /* Apparemment, pas besoin de le normaliser, on laisse pour le moment */
      worldLookAt.normalize ();
      worldLookX.normalize ();
      
      angle = -acos (direction * worldLookAt);
      angle2 = acos (direction * worldLookX);
      
      if (angle2 < PI / 2.0) angle = PI - angle;
      
      /****************************************************************************************/
      glMap2f (GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, **texpts);
      glEnable (GL_MAP2_TEXTURE_COORD_2);
      glMapGrid2f (20, 0.0, 1.0, 20, 0.0, 1.0);
      glEvalMesh2 (GL_POINT, 0, 20, 0, 20);
      
//       cgBougieVertexShader.setModelViewProjectionMatrix();
//       cgBougieVertexShader.setTexTranslation(angle / (double) (PI));
//       cgBougieVertexShader.setInverseModelViewMatrix();
//       cgBougieFragmentShader.setTexture(&tex);      
//       cgBougieVertexShader.enableShader();
//       cgBougieFragmentShader.enableShader();
      
      glEnable (GL_TEXTURE_2D);

      glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glBindTexture (GL_TEXTURE_2D, tex.getTexture ());

      glMatrixMode (GL_TEXTURE);
      glPushMatrix ();
      glLoadIdentity ();
      glTranslatef (0.0, angle / (double) (PI), 0.0);

      gluBeginSurface (nurbs);
      gluNurbsSurface (nurbs, uknotsCount, uknots, vknotsCount,
		       vknots, (max_particles + nb_pts_fixes) * 3,
		       3, ctrlpoints, uorder, vorder,
		       GL_MAP2_VERTEX_3);
      gluEndSurface (nurbs);
      
      glPopMatrix();
      glDisable (GL_TEXTURE_2D);
 //      cgBougieVertexShader.disableProfile();
//       cgBougieFragmentShader.disableProfile();
    }
  glPopMatrix ();
}

void
Bougie::draw_shadowVolumes (GLint objects_list_wsv)
{
  cgShader->enableProfile ();
  cgShader->bindProgram ();

  for (int i = 0; i < nb_lights - 3; i++)
    {
      cgShader->setLightPos (lightPositions[i]);

      glPushMatrix ();
      glLoadIdentity ();
      cgShader->setModelViewMatrixToInverse ();
      glPopMatrix ();
      cgShader->setWorldViewMatrixToIdentity ();

      glCallList (objects_list_wsv);
    }
  cgShader->disableProfile ();
}

void
Bougie::draw_shadowVolume (GLint objects_list_wsv, int i)
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
Bougie::draw_shadowVolume2 (GLint objects_list_wsv, int i)
{
  float pos[4];
  //  int ind = i/SHADOW_SAMPLE_PER_LIGHT;
  //  int modulo = i%SHADOW_SAMPLE_PER_LIGHT;

  //   pos[0] = lightPositions[ind][0]+modulo*(lightPositions[ind+1][0]-lightPositions[ind][0])/SHADOW_SAMPLE_PER_LIGHT;
  //   pos[1] = lightPositions[ind][1]+modulo*(lightPositions[ind+1][1]-lightPositions[ind][1])/SHADOW_SAMPLE_PER_LIGHT;
  //   pos[2] = lightPositions[ind][2]+modulo*(lightPositions[ind+1][2]-lightPositions[ind][2])/SHADOW_SAMPLE_PER_LIGHT;

  pos[0] = lightPositions[0][0] + i * (lightPositions[nb_lights][0] -
				       lightPositions[0][0]) / 5.0;
  pos[1] = lightPositions[0][1] + i * (lightPositions[nb_lights][1] -
				       lightPositions[0][1]) / 5.0;
  pos[2] = lightPositions[0][2] + i * (lightPositions[nb_lights][2] -
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
Bougie::cast_shadows_double_multiple (GLint objects_list_wsv)
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
  for (int i = 0; i < nb_lights /**SHADOW_SAMPLE_PER_LIGHT*/ ; i++)
    {
      enable_only_ambient_light (i);
    }
  sc->draw_sceneWTEX ();
  for (int i = 0; i < nb_lights /**SHADOW_SAMPLE_PER_LIGHT*/ ; i++)
    {
      reset_diffuse_light (i);
    }
  switch_on_lights ();
  glBlendFunc (GL_ZERO, GL_SRC_COLOR);
  sc->draw_scene ();
}

void
Bougie::cast_shadows_double (GLint objects_list_wsv)
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

  switch_off_lights ();
  glDisable (GL_STENCIL_TEST);
  for (int i = 0; i < nb_lights /**SHADOW_SAMPLE_PER_LIGHT*/ ; i++)
    {
      enable_only_ambient_light (i);
    }
  sc->draw_sceneWTEX ();
  for (int i = 0; i < nb_lights /**SHADOW_SAMPLE_PER_LIGHT*/ ; i++)
    {
      reset_diffuse_light (i);
    }

  switch_on_lights ();
  glBlendFunc (GL_ZERO, GL_SRC_COLOR);

  sc->draw_scene ();
}

